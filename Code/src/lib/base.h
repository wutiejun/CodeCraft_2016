
#ifndef _BASE_H
#define _BASE_H


#include <sys/time.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <winsock2.h>
#if 0
#include <pwd.h>
#include <grp.h>
#endif

#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif /* HAVE_STROPTS_H */
#include <fcntl.h>
#ifdef HAVE_SYS_SELECT_H
#include <selectLib.h>
#endif /* HAVE_SYS_SELECT_H */
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/param.h>
#ifdef HAVE_SYS_SYSCTL_H
#ifdef GNU_LINUX
#include <linux/types.h>
#endif
#include <sys/sysctl.h>
#endif /* HAVE_SYS_SYSCTL_H */
//#include <sys/ioctl.h>
#ifdef HAVE_SYS_CONF_H
#include <sys/conf.h>
#endif /* HAVE_SYS_CONF_H */
#ifdef HAVE_SYS_KSYM_H
#include <sys/ksym.h>
#endif /* HAVE_SYS_KSYM_H */
//#include <syslog.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif /* TIME_WITH_SYS_TIME */
//#include <sys/uio.h>
//#include <sys/utsname.h>
#ifdef HAVE_RUSAGE
#include <sys/resource.h>
#endif /* HAVE_RUSAGE */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

/* machine dependent includes */
#ifdef SUNOS_5
#include <strings.h>
#endif /* SUNOS_5 */

/* machine dependent includes */
#ifdef HAVE_LINUX_VERSION_H
#include <linux/version.h>
#endif /* HAVE_LINUX_VERSION_H */

#ifdef HAVE_ASM_TYPES_H
#include <asm/types.h>
#endif /* HAVE_ASM_TYPES_H */

/* misc include group */
#include <stdarg.h>
#if !(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
/* Not C99; do we need to define va_copy? */
#ifndef va_copy
#ifdef __va_copy
#define va_copy(DST,SRC) __va_copy(DST,SRC)
#else
/* Now we are desperate; this should work on many typical platforms.
   But this is slightly dangerous, because the standard does not require
   va_copy to be a macro. */
#define va_copy(DST,SRC) memcpy(&(DST), &(SRC), sizeof(va_list))
#warning "Not C99 and no va_copy macro available, falling back to memcpy"
#endif /* __va_copy */
#endif /* !va_copy */
#endif /* !C99 */


#ifdef HAVE_LCAPS
#include <sys/capability.h>
#include <sys/prctl.h>
#endif /* HAVE_LCAPS */

#ifdef HAVE_SOLARIS_CAPABILITIES
#include <priv.h>
#endif /* HAVE_SOLARIS_CAPABILITIES */

/* network include group */

//#include <sys/socket.h>

#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif /* HAVE_SYS_SOCKIO_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
//#include <netinet/in_systm.h>
//#include <netinet/ip.h>
//#include <netinet/tcp.h>

#ifdef HAVE_NET_NETOPT_H
#include <net/netopt.h>
#endif /* HAVE_NET_NETOPT_H */

//#include <net/if.h>

#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif /* HAVE_NET_IF_DL_H */

#ifdef HAVE_NET_IF_VAR_H
#include <net/if_var.h>
#endif /* HAVE_NET_IF_VAR_H */

#ifdef HAVE_NET_ROUTE_H
#include <net/route.h>
#endif /* HAVE_NET_ROUTE_H */

#ifdef HAVE_NETLINK
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/filter.h>
#include <stddef.h>
#else
#define RT_TABLE_MAIN		0
#endif /* HAVE_NETLINK */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */

//#include <arpa/inet.h>

#ifdef HAVE_INET_ND_H
#include <inet/nd.h>
#endif /* HAVE_INET_ND_H */

#ifdef HAVE_NETINET_IN_VAR_H
#include <netinet/in_var.h>
#endif /* HAVE_NETINET_IN_VAR_H */

#ifdef HAVE_NETINET6_IN6_VAR_H
#include <netinet6/in6_var.h>
#endif /* HAVE_NETINET6_IN6_VAR_H */

#ifdef HAVE_NETINET_IN6_VAR_H
#include <netinet/in6_var.h>
#endif /* HAVE_NETINET_IN6_VAR_H */

#ifdef HAVE_NETINET6_IN_H
#include <netinet6/in.h>
#endif /* HAVE_NETINET6_IN_H */


#ifdef HAVE_NETINET6_IP6_H
#include <netinet6/ip6.h>
#endif /* HAVE_NETINET6_IP6_H */

#ifdef HAVE_NETINET_ICMP6_H
#include <netinet/icmp6.h>
#endif /* HAVE_NETINET_ICMP6_H */

#ifdef HAVE_NETINET6_ND6_H
#include <netinet6/nd6.h>
#endif /* HAVE_NETINET6_ND6_H */

#include <ntlavll.h>


/* Some systems do not define UINT32_MAX, etc.. from inttypes.h
 * e.g. this makes life easier for FBSD 4.11 users.
 */
#ifndef INT8_MAX
#define INT8_MAX	(127)
#endif
#ifndef INT16_MAX
#define INT16_MAX	(32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX	(2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX	(255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX	(65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX	(4294967295U)
#endif

#ifdef HAVE_GLIBC_BACKTRACE
#include <execinfo.h>
#endif /* HAVE_GLIBC_BACKTRACE */

#ifdef BSDI_NRL

#ifdef HAVE_NETINET6_IN6_H
#include <netinet6/in6.h>
#endif /* HAVE_NETINET6_IN6_H */

#ifdef NRL
#include <netinet6/in6.h>
#endif /* NRL */

#define IN6_ARE_ADDR_EQUAL IN6_IS_ADDR_EQUAL

#endif /* BSDI_NRL */

/* Local includes: */
#if !(defined(__GNUC__) || defined(VTYSH_EXTRACT_PL))
#define __attribute__(x)
#endif  /* !__GNUC__ || VTYSH_EXTRACT_PL */





#ifdef HAVE_BROKEN_CMSG_FIRSTHDR
/* This bug is present in Solaris 8 and pre-patch Solaris 9 <sys/socket.h>;
   please refer to http://bugzilla.quagga.net/show_bug.cgi?id=142 */

/* Check that msg_controllen is large enough. */
#define ZCMSG_FIRSTHDR(mhdr) \
  (((size_t)((mhdr)->msg_controllen) >= sizeof(struct cmsghdr)) ? \
   CMSG_FIRSTHDR(mhdr) : (struct cmsghdr *)NULL)

#warning "CMSG_FIRSTHDR is broken on this platform, using a workaround"

#else /* HAVE_BROKEN_CMSG_FIRSTHDR */
#define ZCMSG_FIRSTHDR(M) CMSG_FIRSTHDR(M)
#endif /* HAVE_BROKEN_CMSG_FIRSTHDR */



/*
 * RFC 3542 defines several macros for using struct cmsghdr.
 * Here, we define those that are not present
 */

/*
 * Internal defines, for use only in this file.
 * These are likely wrong on other than ILP32 machines, so warn.
 */
#ifndef _CMSG_DATA_ALIGN
#define _CMSG_DATA_ALIGN(n)           (((n) + 3) & ~3)
#endif /* _CMSG_DATA_ALIGN */

#ifndef _CMSG_HDR_ALIGN
#define _CMSG_HDR_ALIGN(n)            (((n) + 3) & ~3)
#endif /* _CMSG_HDR_ALIGN */

/*
 * CMSG_SPACE and CMSG_LEN are required in RFC3542, but were new in that
 * version.
 */
#ifndef CMSG_SPACE
#define CMSG_SPACE(l)       (_CMSG_DATA_ALIGN(sizeof(struct cmsghdr)) + \
                              _CMSG_HDR_ALIGN(l))
//#warning "assuming 4-byte alignment for CMSG_SPACE"
#endif  /* CMSG_SPACE */


#ifndef CMSG_LEN
#define CMSG_LEN(l)         (_CMSG_DATA_ALIGN(sizeof(struct cmsghdr)) + (l))
//#warning "assuming 4-byte alignment for CMSG_LEN"
#endif /* CMSG_LEN */


/*  The definition of struct in_pktinfo is missing in old version of
    GLIBC 2.1 (Redhat 6.1).  */
#if defined (GNU_LINUX) && ! defined (HAVE_STRUCT_IN_PKTINFO)
struct in_pktinfo
{
    int ipi_ifindex;
    struct in_addr ipi_spec_dst;
    struct in_addr ipi_addr;
};
#endif

/*
 * OSPF Fragmentation / fragmented writes
 *
 * ospfd can support writing fragmented packets, for cases where
 * kernel will not fragment IP_HDRINCL and/or multicast destined
 * packets (ie TTBOMK all kernels, BSD, SunOS, Linux). However,
 * SunOS, probably BSD too, clobber the user supplied IP ID and IP
 * flags fields, hence user-space fragmentation will not work.
 * Only Linux is known to leave IP header unmolested.
 * Further, fragmentation really should be done the kernel, which already
 * supports it, and which avoids nasty IP ID state problems.
 *
 * Fragmentation of OSPF packets can be required on networks with router
 * with many many interfaces active in one area, or on networks with links
 * with low MTUs.
 */
#ifdef GNU_LINUX
#define WANT_OSPF_WRITE_FRAGMENT
#endif
#define WANT_OSPF_WRITE_FRAGMENT

/*
 * IP_HDRINCL / struct ip byte order
 *
 * Linux: network byte order
 * *BSD: network, except for length and offset. (cf Stevens)
 * SunOS: nominally as per BSD. but bug: network order on LE.
 * OpenBSD: network byte order, apart from older versions which are as per
 *          *BSD
 */
#if defined(__NetBSD__) || defined(__FreeBSD__) \
   || (defined(__OpenBSD__) && (OpenBSD < 200311)) \
   || (defined(SUNOS_5) && defined(WORDS_BIGENDIAN))
#define HAVE_IP_HDRINCL_BSD_ORDER
#endif

/* Define BYTE_ORDER, if not defined. Useful for compiler conditional
 * code, rather than preprocessor conditional.
 * Not all the world has this BSD define.
 */
#ifndef BYTE_ORDER
#define BIG_ENDIAN	4321	/* least-significant byte first (vax, pc) */
#define LITTLE_ENDIAN	1234	/* most-significant byte first (IBM, net) */
#define PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */

//#if defined(WORDS_BIGENDIAN)
#define BYTE_ORDER	BIG_ENDIAN
//#else /* !WORDS_BIGENDIAN */
//#define BYTE_ORDER	LITTLE_ENDIAN
//#endif /* WORDS_BIGENDIAN */

#endif /* ndef BYTE_ORDER */

/* MAX / MIN are not commonly defined, but useful */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* For old definition. */
#ifndef IN6_ARE_ADDR_EQUAL
#define IN6_ARE_ADDR_EQUAL IN6_IS_ADDR_EQUAL
#endif /* IN6_ARE_ADDR_EQUAL */

/* default zebra TCP port for g_pzclient */
#define ZEBRA_PORT			2600

/* Zebra message types. */
#define ZEBRAD_INTERFACE_ADD                1
#define ZEBRAD_INTERFACE_DELETE             2
#define ZEBRAD_INTERFACE_ADDRESS_ADD        3
#define ZEBRAD_INTERFACE_ADDRESS_DELETE     4
#define ZEBRAD_INTERFACE_UP                 5
#define ZEBRAD_INTERFACE_DOWN               6
#define ZEBRAD_IPV4_ROUTE_ADD               7
#define ZEBRAD_IPV4_ROUTE_DELETE            8
#define ZEBRAD_IPV6_ROUTE_ADD               9
#define ZEBRAD_IPV6_ROUTE_DELETE           10
#define ZEBRAD_REDISTRIBUTE_ADD            11
#define ZEBRAD_REDISTRIBUTE_DELETE         12
#define ZEBRAD_REDISTRIBUTE_DEFAULT_ADD    13
#define ZEBRAD_REDISTRIBUTE_DEFAULT_DELETE 14
#define ZEBRAD_IPV4_NEXTHOP_LOOKUP         15
#define ZEBRAD_IPV6_NEXTHOP_LOOKUP         16
#define ZEBRAD_IPV4_IMPORT_LOOKUP          17
#define ZEBRAD_IPV6_IMPORT_LOOKUP          18
#define ZEBRAD_INTERFACE_RENAME            19
#define ZEBRAD_ROUTER_ID_ADD               20
#define ZEBRAD_ROUTER_ID_DELETE            21
#define ZEBRAD_ROUTER_ID_UPDATE            22
#define ZEBRAD_HELLO                       23
#define ZEBRAD_SUMA_L3_IF_MSG              24
#define ZEBRAD_SUMA_LINK_STA_MSG           25
#define ZEBRAD_SUMA_TRACK_OP               26
#define ZEBRAD_SUMA_TRACK_NOTIFY           27
#define ZEBRAD_MESSAGE_MAX                 28

/* define for ospf and isis operate track */
typedef enum zebra_track_operate_e{
    ZEBRA_TRACK_INST_ADD,
    ZEBRA_TRACK_INST_DEL,
}zebra_track_operate_t;

/* track type define */
typedef enum zebra_track_type_e
{
    ZEBRA_TRACK_NONE = 0,
    ZEBRA_TRACK_BFD,
    ZEBRA_TRACK_ICMP,
    ZEBRA_TRACK_DEL,
    ZEBRA_TRACK_ALL,
}zebra_track_type_t;

#define track_static  1
#define track_isis    2
#define track_ospf    4
#define track_bgp     8
#define track_manual  16
#define track_max     17

/* Marker value used in new Zserv, in the byte location corresponding
 * the command value in the old zserv header. To allow old and new
 * Zserv headers to be distinguished from each other.
 */
#define ZEBRA_HEADER_MARKER              255

/* Zebra route's types are defined in route_types.h */


/* protocol packet receive parameter define */
#define PROTO_PKT_MTU 65535
#define PROTO_PKT_BUF_MAX 16
#define PROTO_PKT_BUF_MAX_SYS 20

typedef unsigned short u_short ;
typedef unsigned int u_int ;
typedef unsigned char u_char ;

typedef unsigned short u_int16_t ;
typedef unsigned char u_int8_t ;
typedef unsigned short  uint16_t;
typedef unsigned int size_t;
typedef unsigned long long uint64_t;
typedef unsigned int u_int32_t;
typedef u_int32_t in_addr_t;


typedef struct proto_pkt_s {
  u_short vlanid;
  u_short len;
  char buf[PROTO_PKT_MTU];
}proto_pkt_t;

typedef struct proto_pkt_port_s
{
  u_int slot;
  u_int port;
  u_int trunk;
}proto_pkt_port_t;

#pragma pack(1)
typedef struct eth_header_s
{
  u_char    szDstMac[6];
  u_char    szSrcMac[6];
  u_short   proto_type;
}eth_header_t;
/* TAKE care of order: _LITTLE_ENDIAN? _BIG_ENDIAN? */
typedef struct vlan_header_s
{
#if _BYTE_ORDER == _LITTLE_ENDIAN
  u_short   vlan_id:12,
            CFI:1,
            priority:3;
#else
  u_short   priority:3,
            CFI:1,
            vlan_id:12;
#endif
  u_short   type;
}vlan_header_t;
#if 0
struct ip_header_s
{
#if _BYTE_ORDER == _LITTLE_ENDIAN
	  u_int	ip_hl:4,		/* header length */
		ip_v:4,			/* version */
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
	  u_int	ip_v:4,			/* version */
		ip_hl:4,		/* header length */
#endif
		ip_tos:8,		/* type of service */
		ip_len:16;		/* total length */
	  u_short	ip_id;			/* identification */
	  short	ip_off;			/* fragment offset field */
	  u_char	ip_ttl;			/* time to live */
	  u_char	ip_p;			/* protocol */
	  u_short	ip_sum;			/* checksum */
	  in_addr_t ip_src,ip_dst;	/* source and dest address */
}ip_header_t;
#endif
#pragma pack()

/* Note: whenever a new route-type or zserv-command is added the
 * corresponding {command,route}_types[] table in lib/log.c MUST be
 * updated! */

/* Map a route type to a string.  For example, ZEBRA_ROUTE_RIPNG -> "ripng". */
extern const char *zebra_route_string(unsigned int route_type);
/* Map a route type to a char.  For example, ZEBRA_ROUTE_RIPNG -> 'R'. */
extern char zebra_route_char(unsigned int route_type);
/* Map a zserv command type to the same string,
 * e.g. ZEBRAD_INTERFACE_ADD -> "ZEBRAD_INTERFACE_ADD" */
/* Map a protocol name to its number. e.g. ZEBRA_ROUTE_BGP->9*/
extern int zebra_proto_name2num(const char *s);
/* Map redistribute X argument to protocol number.
 * unlike zebra_proto_name2num, this accepts shorthands and takes
 * an AFI value to restrict input */
extern int zebra_proto_redistnum(int afi, const char *s);

extern const char *zebra_zserv_command_string (unsigned int command);

/* Zebra's family types. */
#define ZEBRA_FAMILY_IPV4                1
#define ZEBRA_FAMILY_IPV6                2
#define ZEBRA_FAMILY_MAX                 3

/* Error codes of zebra. */
#define ZEBRA_ERR_NOERROR                0
#define ZEBRA_ERR_RTEXIST               -1
#define ZEBRA_ERR_RTUNREACH             -2
#define ZEBRA_ERR_EPERM                 -3
#define ZEBRA_ERR_RTNOEXIST             -4
#define ZEBRA_ERR_KERNEL                -5
#define ZEBRA_ERR_RTMAX_NHOP            -6
#define ZEBRA_ERR_RTMAX_ECMP            -7
#define ZEBRA_ERR_FIBNOEXIST            -8
#define ZEBRA_ERR_SINGLE_MAX            -9
#define ZEBRA_ERR_TOTAL_MAX             -10

/* Zebra message flags */
#define ZEBRA_FLAG_INTERNAL           0x01
#define ZEBRA_FLAG_SELFROUTE          0x02
#define ZEBRA_FLAG_BLACKHOLE          0x04
#define ZEBRA_FLAG_IBGP               0x08
#define ZEBRA_FLAG_SELECTED           0x10
#define ZEBRA_FLAG_CHANGED            0x20
#define ZEBRA_FLAG_STATIC             0x40
#define ZEBRA_FLAG_REJECT             0x80

/* Zebra nexthop flags. */
#define ZEBRA_NEXTHOP_IFINDEX            1
#define ZEBRA_NEXTHOP_IFNAME             2
#define ZEBRA_NEXTHOP_IPV4               3
#define ZEBRA_NEXTHOP_IPV4_IFINDEX       4
#define ZEBRA_NEXTHOP_IPV4_IFNAME        5
#define ZEBRA_NEXTHOP_IPV6               6
#define ZEBRA_NEXTHOP_IPV6_IFINDEX       7
#define ZEBRA_NEXTHOP_IPV6_IFNAME        8
#define ZEBRA_NEXTHOP_BLACKHOLE          9

#ifndef INADDR_LOOPBACK
#define	INADDR_LOOPBACK	0x7f000001	/* Internet address 127.0.0.1.  */
#endif

/* Address family numbers from RFC1700. */
#define AFI_IP                    1
#define AFI_IP6                   2
#define AFI_MAX                   3

/* Subsequent Address Family Identifier. */
#define SAFI_UNICAST              1
#define SAFI_MULTICAST            2
#define SAFI_RESERVED_3           3
#define SAFI_MPLS_VPN             4
#define SAFI_MAX                  5

/* Filter direction.  */
#define FILTER_IN                 0
#define FILTER_OUT                1
#define FILTER_MAX                2

/* zebra route upper limit  */
#define ZEBRA_NEXT_HOP_LIMIT      16
#define ZEBRA_RT_UPPER_LIMIT      1024
#define OSPF_NEWORK_UPPER_LIMIT   500
#define OSPF_AREA_UPPER_LIMIT     50


/* Default Administrative Distance of each protocol. */
#define ZEBRA_KERNEL_DISTANCE_DEFAULT      0
#define ZEBRA_CONNECT_DISTANCE_DEFAULT     0
#define ZEBRA_STATIC_DISTANCE_DEFAULT      1
#define ZEBRA_RIP_DISTANCE_DEFAULT       120
#define ZEBRA_RIPNG_DISTANCE_DEFAULT     120
#define ZEBRA_OSPF_DISTANCE_DEFAULT      110
#define ZEBRA_OSPF6_DISTANCE_DEFAULT     110
#define ZEBRA_ISIS_DISTANCE_DEFAULT      115
#define ZEBRA_IBGP_DISTANCE_DEFAULT      200
#define ZEBRA_EBGP_DISTANCE_DEFAULT       20

/* Flag manipulation macros. */
#define CHECK_FLAG(V,F)      ((V) & (F))
#define SET_FLAG(V,F)        (V) |= (F)
#define UNSET_FLAG(V,F)      (V) &= ~(F)

/* AFI and SAFI type. */
typedef u_int16_t afi_t;
typedef u_int8_t safi_t;

/* Zebra types. Used in Zserv message header. */
typedef u_int16_t zebra_size_t;
typedef u_int16_t zebra_command_t;

/* FIFO -- first in first out structure and macros.  */
struct fifo
{
    struct fifo *next;
    struct fifo *prev;
};

#define FIFO_INIT(F)                                  \
  do {                                                \
    struct fifo *Xfifo = (struct fifo *)(F);          \
    Xfifo->next = Xfifo->prev = Xfifo;                \
  } while (0)

#define FIFO_ADD(F,N)                                 \
  do {                                                \
    struct fifo *Xfifo = (struct fifo *)(F);          \
    struct fifo *Xnode = (struct fifo *)(N);          \
    Xnode->next = Xfifo;                              \
    Xnode->prev = Xfifo->prev;                        \
    Xfifo->prev = Xfifo->prev->next = Xnode;          \
  } while (0)

#define FIFO_DEL(N)                                   \
  do {                                                \
    struct fifo *Xnode = (struct fifo *)(N);          \
    Xnode->prev->next = Xnode->next;                  \
    Xnode->next->prev = Xnode->prev;                  \
  } while (0)

#define FIFO_HEAD(F)                                  \
  ((((struct fifo *)(F))->next == (struct fifo *)(F)) \
  ? NULL : (F)->next)

#define FIFO_EMPTY(F)                                 \
  (((struct fifo *)(F))->next == (struct fifo *)(F))

#define FIFO_TOP(F)                                   \
  (FIFO_EMPTY(F) ? NULL : ((struct fifo *)(F))->next)

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#define MAXPATHLEN 128

#define SYSCONFDIR "/tffs0/"

int zebra_quagga_gettimeofday (struct timeval *tv);

#define CTL_NET 3

int zebra_strcasecmp(const char *s1, const char *s2);
int zebra_strncasecmp(const char *s1, const char *s2, size_t n);

#define ZEBRA_MIN_VLAN_ID 0
#define ZEBRA_MAX_VLAN_ID 4094
#define ZEBRA_MIN_LPBCK_ID 4096
#define ZEBRA_MAX_LPBCK_ID 4103

#define VALID_VLAN_ID(id) ((id) <= ZEBRA_MAX_VLAN_ID)
#define VALID_LPBCK_ID(id) ((id) >= ZEBRA_MIN_LPBCK_ID && (id) <= ZEBRA_MAX_LPBCK_ID)

#define VALID_INTF_ID(id) (VALID_VLAN_ID(id) || VALID_LPBCK_ID(id))

/*vlan id加上64，得到vlanif的接口索引；1-64保留给vxworks使用*/
#define zebra_GetIfIndexByVlan(VID)          (64+VID)
#define zebra_GetVlanByIfIndex(INDEX)        (INDEX>=64?(INDEX-64):0)



typedef enum ZEBRA_IF_TYPE_S
{
    ZEBRA_IF_TYPE_SYS = 1,   /*vxorks内部保留接口，如loopback、mottsec0等*/
    ZEBRA_IF_TYPE_VLANIF,    /*vlanif接口*/

    ISIS_IF_TYPE_END,
} ZEBRA_IF_TYPE_ENUM_T;


#define array_size(ar) (sizeof(ar) / sizeof(ar[0]))

extern int route_if_GetTypeByIndex(u_int ulIfIndex);




#define ERR_LOG(ret,para1,para2,para3) \
		printf("\r\n *** err log. *** [%s.%d] %d,%d,%d,%d", __FILE__, __LINE__, ret, para1, para2, para3);

extern int dbg;
#define dbg_printf if (dbg) printf
#define NBB_TRC_ENTRY dbg_printf
#define NBB_ASSERT_FALSE(x) assert(!(x))
#define NBB_TRC_DETAIL(PARAMS)       
#define NBB_TRC_FLOW(PARAMS)         
#define NBB_TRC_EXIT()
#define NBB_MIN(X, Y)   (((X)<(Y)) ? (X) : (Y))
#define NBB_MAX(X, Y)   (((X)>(Y)) ? (X) : (Y))

#define OS_SPRINTF   sprintf

#define OS_STRCAT(STR1, STR2)          strcat((STR1), (STR2))
#define OS_STRNCAT(STR1, STR2, LEN)    strncat((STR1), (STR2), (size_t)(LEN))
#define OS_STRSTR(STR1, STR2)          strstr((STR1), (STR2))
#define OS_STRCPY(DEST, SRC)           strcpy((char *)(DEST), \
                                              (const char *)(SRC))
#define OS_STRNCPY(STR1, STR2, LEN)    strncpy((STR1), (STR2), (size_t)(LEN))
#define OS_STRCHR(STR, CHR)            strchr((STR), (CHR))
#define OS_STRRCHR(STR, CHR)           strrchr((STR), (CHR))


#endif /* _ZEBRA_H */

