
#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "config.h"
/*Socket 端口选择，可按自己的习惯定义*/
#define SOCK_TCPS             0
#define SOCK_HUMTEM           0
#define SOCK_PING             0
#define SOCK_TCPC             1
#define SOCK_WEIBO            2
#define SOCK_DHCP             3
#define SOCK_HTTPS            4
#define SOCK_DNS              5
#define SOCK_SMTP             6
#define SOCK_NTP              7
#define SOCK_UDPS             7


extern Uint8 socket(SOCKET s, Uint8 protocol, Uint16 port, Uint8 flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close_socket(SOCKET s); // Close socket
extern Uint8 connect(SOCKET s, Uint8 * addr, Uint16 port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern Uint8 listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern Uint16 send(SOCKET s, const Uint8 * buf, Uint16 len); // Send data (TCP)
extern Uint16 recv(SOCKET s, Uint8 * buf, Uint16 len);	// Receive data (TCP)
extern Uint16 sendto(SOCKET s, const Uint8 * buf, Uint16 len, Uint8 * addr, Uint16 port); // Send data (UDP/IP RAW)
extern Uint16 recvfrom(SOCKET s, Uint8 * buf, Uint16 len, Uint8 * addr, Uint16  *port); // Receive data (UDP/IP RAW)
void send_ka(SOCKET s);

#ifdef __MACRAW__
void macraw_open(void);
Uint16 macraw_send( const Uint8 * buf, Uint16 len ); //Send data (MACRAW)
Uint16 macraw_recv( Uint8 * buf, Uint16 len ); //Recv data (MACRAW)
#endif

#endif
/* _SOCKET_H_ */

