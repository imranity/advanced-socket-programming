//Get socket options- sample for using tcp socket options in C
#include <sys/types.h>
#include <sys/socket.h>
//#include <linux/in6.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/in.h>
#include <stdlib.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <netinet/tcp.h>       /* for TCP_xxx defines */
#include <linux/tcp.h>
#include <linux/udp.h>
//#include <linux/ip.h>

union val {
    int               i_val;
    long              l_val;
    struct linger     linger_val;
    struct timeval    timeval_val;
    struct tcp_info   info_val;
} val;

static char *sock_str_flag(union val *, int);
static char *sock_str_int(union val *, int);
static char *sock_str_linger(union val *, int);
static char *sock_str_timeval(union val *, int);
static char *sock_str_info(union val *, int);

struct sock_opts {
    const char  *opt_str;
    int         opt_level;
    int         opt_name;
    char        *(*opt_val_str) (union val *, int);
} sock_opts[] = {
    // SO_BINDTODEVICE: bind socket to a particular device like "eht0"
    // SO_BSDCOMPAT; enable BSD bug-to-bug compatiblity
    // SO_PEERCRED: return the credentials of foreign process
    // SO_PRIORITY: set the priority for all packets to be sent
    // SO_RCVBUFFORCE
    // SO_RCVLOWAT
    // SO_SNDLOWAT
    // SO_RCVTIMEO
    // SO_SNDTIMEO
    // SO_SNDBUFFORCE
    //
    { "SO_ACCEPTCONN",       SOL_SOCKET, SO_ACCEPTCONN,  sock_str_flag },
    { "SO_BROADCAST",        SOL_SOCKET, SO_BROADCAST,   sock_str_flag },
    { "SO_DEBUG",            SOL_SOCKET, SO_DEBUG,       sock_str_flag },
    { "SO_DOMAIN",           SOL_SOCKET, SO_DOMAIN,      sock_str_int },
    { "SO_DONTROUTE",        SOL_SOCKET, SO_DONTROUTE,   sock_str_flag },
    { "SO_ERROR",            SOL_SOCKET, SO_ERROR,       sock_str_int },
    { "SO_KEEPALIVE",        SOL_SOCKET, SO_KEEPALIVE,   sock_str_flag },
    { "SO_LINGER",           SOL_SOCKET, SO_LINGER,      sock_str_linger },
    { "SO_OOBINLINE",        SOL_SOCKET, SO_OOBINLINE,   sock_str_flag },
    { "SO_PASSCRED",         SOL_SOCKET, SO_PASSCRED,    sock_str_flag },
    { "SO_PROTOCOL",         SOL_SOCKET, SO_PROTOCOL,    sock_str_int },
    { "SO_RCVBUF",           SOL_SOCKET, SO_RCVBUF,      sock_str_int },
    { "SO_SNDBUF",           SOL_SOCKET, SO_SNDBUF,      sock_str_int },
    { "SO_RCVLOWAT",         SOL_SOCKET, SO_RCVLOWAT,    sock_str_int },
    { "SO_SNDLOWAT",         SOL_SOCKET, SO_SNDLOWAT,    sock_str_int },
    { "SO_RCVTIMEO",         SOL_SOCKET, SO_RCVTIMEO,    sock_str_timeval },
    { "SO_SNDTIMEO",         SOL_SOCKET, SO_SNDTIMEO,    sock_str_timeval },
    { "SO_REUSEADDR",        SOL_SOCKET, SO_REUSEADDR,   sock_str_flag },
#ifdef SO_REUSEPORT
    { "SO_REUSEPORT",        SOL_SOCKET, SO_REUSEPORT,   sock_str_flag },
#else
    { "SO_REUSEPORT",        0,          0,              NULL },
#endif
    { "SO_TIMESTAMP",        SOL_SOCKET, SO_TIMESTAMP,   sock_str_flag },
    { "SO_TYPE",             SOL_SOCKET, SO_TYPE,        sock_str_int },
//    { "SO_USELOOPBACK",      SOL_SOCKET, SO_USELOOPBACK, sock_str_flag },
    // IP related options
    // IP_ADD_MEMBERSHIP: join a multicast group
    // IP_DROP_MEMBERSHIP: leave a multicast group
    // IP_HDRINCL: user supplies an IP header
    // IP_MTU: retrieve the current known path MTU. only valid when connected
    // IP_MTU_DISCOVER: path mtu discovery
    // IP_MULTICAST_IF: set the local device for a multicast socket.
    // IP_OPTIONS: 40 bytes for IPv4
    // IP_PKTINFO: struct in_pktinfo
    // IP_RECVOPTS: pass all incoming IP options to the user
    // IP_RETOPTS: identical to IP_RECVOPTS, but return raw options
    { "IP_MULTICAST_LOOP",   IPPROTO_IP, IP_MULTICAST_LOOP, sock_str_flag },
    { "IP_MULTICAST_TTL",    IPPROTO_IP, IP_MULTICAST_TTL, sock_str_int },
    { "IP_NODEFRAG",         IPPROTO_IP, IP_NODEFRAG,    sock_str_flag },
    { "IP_RECVERR",          IPPROTO_IP, IP_RECVERR,     sock_str_flag },
    { "IP_RECVTOS",          IPPROTO_IP, IP_RECVTOS,     sock_str_flag },
    { "IP_RECVTTL",          IPPROTO_IP, IP_RECVTTL,     sock_str_int },
//    { "IP_ROUTER_ALERT",     IPPROTO_IP, IP_ROUTER_ALERT, sock_str_flag },
    { "IP_TOS",              IPPROTO_IP, IP_TOS,         sock_str_int },
    { "IP_TTL",              IPPROTO_IP, IP_TTL,         sock_str_int },
//    { "IPV6_DONTFRAG",       IPPROTO_IPV6,IPV6_DONTFRAG, sock_str_flag },
//    { "IPV6_UNICAST_HOPS",   IPPROTO_IPV6,IPV6_UNICAST_HOPS,sock_str_int },
//    { "IPV6_V6ONLY",         IPPROTO_IPV6,IPV6_V6ONLY,   sock_str_flag },
    // TCP related options
    { "TCP_CORK",           IPPROTO_TCP, TCP_CORK,      sock_str_flag },
    { "TCP_DEFER_ACCEPT", IPPROTO_TCP, TCP_DEFER_ACCEPT, sock_str_flag },
    { "TCP_INFO",           IPPROTO_TCP, TCP_INFO,      sock_str_info },
    { "TCP_KEEPCNT",        IPPROTO_TCP, TCP_KEEPCNT,   sock_str_int },
    { "TCP_KEEPIDLE",       IPPROTO_TCP, TCP_KEEPIDLE,  sock_str_int },
    { "TCP_KEEPINTVL",      IPPROTO_TCP, TCP_KEEPINTVL, sock_str_int },
    { "TCP_LINGER2",        IPPROTO_TCP, TCP_LINGER2,   sock_str_int },
    { "TCP_MAXSEG",         IPPROTO_TCP, TCP_MAXSEG,    sock_str_int },
    { "TCP_NODELAY",        IPPROTO_TCP, TCP_NODELAY,   sock_str_flag },
    { "TCP_QUICKACK",       IPPROTO_TCP, TCP_QUICKACK,  sock_str_flag },
    { "TCP_SYNCNT",         IPPROTO_TCP, TCP_SYNCNT,    sock_str_int },
    { "TCP_WINDOW_CLAMP",   IPPROTO_TCP, TCP_WINDOW_CLAMP,    sock_str_int },
    // UDP related options
    { "UDP_CORK",           IPPROTO_UDP, UDP_CORK,      sock_str_flag },
    { NULL,                 0,          0,              NULL }
};

int main(int argc, char **argv) {
    int     fd;
    socklen_t len;
    struct sock_opts *ptr;

    for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
        printf("%s: ", ptr->opt_str);
        if (ptr->opt_val_str == NULL) {
            printf("(undefined)\n");
        } else {
            switch (ptr->opt_level) {
            case IPPROTO_IP:
//                fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
//                break;
            case IPPROTO_TCP:
            case SOL_SOCKET:
                fd = socket(AF_INET, SOCK_STREAM, 0);
                break;
#ifdef  IPV6
            case IPPROTO_IPV6:
                fd = socket(AF_INET6, SOCK_STREAM, 0);
                break;
#endif
#ifdef  IPPROTO_SCTP
            case IPPROTO_SCTP:
                fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
                break;
#endif
            case IPPROTO_UDP:
                fd = socket(AF_INET, SOCK_DGRAM, 0);
                break;

           default:
                printf("Can't create fd for level %d\n", ptr->opt_level);
                exit(1);
            }

            len = sizeof(val);
            if (getsockopt(fd, ptr->opt_level, ptr->opt_name, &val, &len) == -1) {
                 printf("getsockopt error: %s\n", strerror(errno));
                 return 1; 
             } else {
                 printf("default = %s\n", (*ptr->opt_val_str)  (&val, len));
            }
            close(fd);
        }
    }
    exit(0);
}

static char strres[128];

static char *sock_str_flag(union val *ptr, int len) {
    if (len != sizeof(int))
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
    else
        snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
        return(strres);
}

static char *sock_str_int(union val *ptr, int len) {
    if (len != sizeof(int))
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
    else
        snprintf(strres, sizeof(strres), "%d", ptr->i_val);
    return(strres);
}

static char *sock_str_linger(union val *ptr, int len) {
    struct linger   *lptr = &ptr->linger_val;

    if (len != sizeof(struct linger))
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", len);
    else
        snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d",
            lptr->l_onoff, lptr->l_linger);
    return(strres);
}

static char *sock_str_timeval(union val *ptr, int len) {
    struct timeval  *tvptr = &ptr->timeval_val;

    if (len != sizeof(struct timeval))
        snprintf(strres, sizeof(strres),
            "size (%d) not sizeof(struct timeval)", len);
    else
        snprintf(strres, sizeof(strres), "%d sec, %d usec",
            (int)tvptr->tv_sec, (int)tvptr->tv_usec);
    return(strres);
}

static char *sock_str_info(union val *ptr, int len) {
    struct tcp_info *tvptr = &ptr->info_val;

    if (len != sizeof(struct tcp_info)) {
        snprintf(strres, sizeof(strres),
            "size (%d) not sizeof(struct tcp_info)", len);
    } else {
        snprintf(strres, sizeof(strres), "sacked: %d, retrans: %d",
            (int)tvptr->tcpi_sacked, (int)tvptr->tcpi_retrans);
    }
    return(strres);
}
