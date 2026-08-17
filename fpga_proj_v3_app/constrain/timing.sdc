create_clock -name clk_external -period 40 -waveform {0 20} [get_ports {clk_external}]
derive_clocks
