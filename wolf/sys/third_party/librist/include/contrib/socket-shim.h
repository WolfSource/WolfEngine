/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 */

#ifndef __SOCKET_SHIM_H
#define __SOCKET_SHIM_H

#ifndef SOL_IP
#define SOL_IP 0x0
#define SOL_IPV6 0x29
#endif

#ifdef _WIN32
#include <winsock2.h>
#define _WINSOCKAPI_
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#define AF_LOCAL AF_UNSPEC
#define MSG_DONTWAIT (0)

typedef int socklen_t;
#ifndef __MINGW32__
#include <io.h>
#define dup(s) _dup(s)
#define write(fd, buf, size) _write(fd, buf, size)
#endif

#else /* Unix like OSes */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <net/if.h>

#endif

#endif
