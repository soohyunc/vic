/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 * 	This product includes software developed by the Network Research
 * 	Group at Lawrence Berkeley National Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) $Header$ (LBL)
 */

#ifndef vic_config_h
#define vic_config_h

#if defined(sgi) || defined(__bsdi__) || defined(__FreeBSD__) 
#include <sys/types.h>
#elif defined(sun)
#include <sys/types.h>
#define int8_t	char
#define int16_t	short
#define int32_t	long
typedef unsigned char  u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int   u_int32_t;
#elif defined(linux)
# include <sys/bitypes.h>
#else
# ifdef ultrix
#  include <sys/types.h>
# endif
# ifdef sco
   typedef char int8_t;
# else
   typedef signed char int8_t;
# endif
typedef short int16_t;
typedef int   int32_t;
typedef unsigned char	u_int8_t;
typedef unsigned short	u_int16_t;
typedef unsigned int	u_int32_t;
#endif

#if defined(sun) || defined(_AIX)
#if defined(__cplusplus)
extern "C" {
#endif
void srandom(unsigned);
long random(void);
#if defined(__cplusplus)
}
#endif
#endif

#ifdef sgi
#include <math.h>
#endif

#ifdef __linux__
#define DEFINED_ERRLIST
#endif

#include "debug.h"
#include <malloc.h>
#include <stdlib.h>
#include <time.h>		/* For clock_t */


#ifndef WIN32
#include <unistd.h>
#if defined(__cplusplus)
extern "C" {
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
int strcasecmp(const char *, const char *);
clock_t clock(void);
#if !defined(sco) && !defined(sgi) && !defined(__bsdi__) && !defined(__FreeBSD__) && !defined(sun) && !defined(__linux__)
int gethostid(void);
#endif
time_t time(time_t *);
char *ctime(const time_t *);
#if defined(__cplusplus)
}
#endif
#endif /*ndef WIN32*/

#if defined(NEED_SUNOS_PROTOS)
#if defined(__cplusplus)
extern "C" {
#endif
struct timeval;
struct timezone;
struct msghdr;
struct sockaddr;
int bind(int s, struct sockaddr*, int);
int close(int);
int connect(int s, struct sockaddr*, int);
int gethostid();
int gethostname(char*, int);
int getpid();
int getsockname(int, struct sockaddr*, int*);
int gettimeofday(struct timeval*, struct timezone*);
int ioctl(int fd, int request, ...);
int recv(int, void*, int len, int flags);
int recvfrom(int, void*, int len, int flags, struct sockaddr*, int*);
int send(int s, void*, int len, int flags);
int sendmsg(int, struct msghdr*, int);
int setsockopt(int s, int level, int optname, void* optval, int optlen);
int socket(int, int, int);
int strcasecmp(const char*, const char*);
#if defined(__cplusplus)
}
#endif
#endif

#ifdef WIN32
//#include <windef.h>
//#include <winbase.h>

#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>   /* abs() */
#include <string.h>

#ifdef MUSICA_IPV6
#include <winsock6.h>
#else
#include <winsock2.h>
//#include <ws2tcpip.h>
#endif

#ifdef HAVE_IPV6

#if !defined(MUSICA_IPV6) && defined(WIN32) /* MS_IPV6 */

#include <ws2ip6.h>
#include <ws2tcpip.h>

#define IN6_ARE_ADDR_EQUAL IN6_ADDR_EQUAL

#define IN6ADDR_ANY_INIT { (unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)0, \
                            (unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)0, \
                            (unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)0, \
                            (unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)0 } 
#define IN6_IS_ADDR_UNSPECIFIED(addr) \
        (((addr)->s6_addr[0] == 0) && \
        ((addr)->s6_addr[1] == 0) && \
        ((addr)->s6_addr[2] == 0) && \
        ((addr)->s6_addr[3] == 0) && \
        ((addr)->s6_addr[4] == 0) && \
        ((addr)->s6_addr[5] == 0) && \
        ((addr)->s6_addr[6] == 0) && \
        ((addr)->s6_addr[7] == 0) && \
        ((addr)->s6_addr[8] == 0) && \
        ((addr)->s6_addr[9] == 0) && \
        ((addr)->s6_addr[10] == 0) && \
        ((addr)->s6_addr[11] == 0) && \
        ((addr)->s6_addr[12] == 0) && \
        ((addr)->s6_addr[13] == 0) && \
        ((addr)->s6_addr[14] == 0) && \
        ((addr)->s6_addr[15] == 0))

#define IN6_IS_ADDR_MULTICAST(addr) \
        ((addr)->s6_addr[0] == 0xffU)

#if defined(__cplusplus)
extern "C" {
#endif
	const char * inet_ntop(int af, const void *src, char *dst, size_t size);
	int inet_pton(int af,const char *src,void *dst);
#if defined(__cplusplus)
}
#endif 

#define INET6_ADDRSTRLEN        46      /* max len of IPv6 addr in ascii */
                                        /* standard colon-hex notation. */ 
#define EAFNOSUPPORT -1
//#define IP_MULTICAST_LOOP       0x12    /* set/get IP multicast loopback   */

#endif /* MS_IPV6 */

#endif /* HAVE_IPV6 */

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	256
#endif

#define _SYS_NMLN	9
struct utsname {
	char sysname[_SYS_NMLN];
	char nodename[_SYS_NMLN];
	char release[_SYS_NMLN];
	char version[_SYS_NMLN];
	char machine[_SYS_NMLN];
};

typedef char *caddr_t;

struct iovec {
	caddr_t iov_base;
	int	    iov_len;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};
#ifndef MUSICA_IPV6
typedef int pid_t;
#endif
typedef int uid_t;
typedef int gid_t;
    
#if defined(__cplusplus)
extern "C" {
#endif

int uname(struct utsname *);
int getopt(int, char * const *, const char *);
int strcasecmp(const char *, const char *);
int srandom(int);
int random(void);
int gettimeofday(struct timeval *p, struct timezone *z);
int gethostid(void);
int getuid(void);
int getgid(void);
int getpid(void);
int nice(int);
int sendmsg(int, struct msghdr*, int);
time_t time(time_t *);

#if defined(__cplusplus)
}
#endif

#define ECONNREFUSED	WSAECONNREFUSED
#define ENETUNREACH		WSAENETUNREACH
#define EHOSTUNREACH	WSAEHOSTUNREACH
#define EWOULDBLOCK		WSAEWOULDBLOCK

#define M_PI		3.14159265358979323846

#include <windows.h>

#endif /* WIN32 */
  
#endif /* vic_config_h */
