#!/usr/bin/env python3
"""Receive and validate the raw W5500 digital-scope stream."""

from __future__ import annotations

import argparse
import math
import socket
import struct
import sys
import time


POINT_COUNT = 512
WIRE_BYTES = POINT_COUNT * 4


def receive_until_timeout(sock: socket.socket, timeout_s: float) -> bytes:
    chunks: list[bytes] = []
    sock.settimeout(timeout_s)
    while True:
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            break
        chunks.append(chunk)
    return b"".join(chunks)


def validate_ramp(payload: bytes) -> tuple[bool, str]:
    if len(payload) != WIRE_BYTES:
        return False, f"expected {WIRE_BYTES} bytes, received {len(payload)}"

    values = struct.unpack(f"<{POINT_COUNT}f", payload)
    for index, value in enumerate(values):
        if not math.isfinite(value):
            return False, f"sample[{index}] is not finite: {value!r}"

    for index in range(1, len(values)):
        delta = values[index] - values[index - 1]
        if delta != 1.0:
            return (
                False,
                f"sample delta mismatch at {index}: "
                f"prev={values[index - 1]}, current={values[index]}, delta={delta}",
            )

    return True, f"samples={len(values)}, first={values[0]}, last={values[-1]}"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="192.168.1.80")
    parser.add_argument("--port", type=int, default=502)
    parser.add_argument("--connect-timeout", type=float, default=2.0)
    parser.add_argument("--capture-wait", type=float, default=0.15)
    parser.add_argument("--receive-timeout", type=float, default=0.30)
    parser.add_argument("--output", help="optional raw payload output file")
    args = parser.parse_args()

    print(f"[SCOPE-PC] connect {args.host}:{args.port}")
    with socket.create_connection(
        (args.host, args.port), timeout=args.connect_timeout
    ) as sock:
        print("[SCOPE-PC] TX wave_trigger")
        sock.sendall(b"wave_trigger")
        time.sleep(args.capture_wait)

        print("[SCOPE-PC] TX wave_getcur")
        sock.sendall(b"wave_getcur")
        payload = receive_until_timeout(sock, args.receive_timeout)

    print(f"[SCOPE-PC] RX {len(payload)} bytes; timeout marks stream end")
    if args.output:
        with open(args.output, "wb") as output_file:
            output_file.write(payload)

    passed, detail = validate_ramp(payload)
    print(f"[SCOPE-PC] {'PASS' if passed else 'FAIL'}: {detail}")
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
