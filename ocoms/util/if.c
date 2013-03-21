/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2009 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#if defined(__APPLE__) && defined(_LP64)
/* Apple engineering suggested using options align=power as a
   workaround for a bug in OS X 10.4 (Tiger) that prevented ioctl(...,
   SIOCGIFCONF, ...) from working properly in 64 bit mode on Power PC.
   It turns out that the underlying issue is the size of struct
   ifconf, which the kernel expects to be 12 and natural 64 bit
   alignment would make 16.  The same bug appears in 64 bit mode on
   Intel macs, but align=power is a no-op there, so instead, use the
   pack pragma to instruct the compiler to pack on 4 byte words, which
   has the same effect as align=power for our needs and works on both
   Intel and Power PC Macs. */
#pragma pack(push,4)
#endif
#include <net/if.h>
#if defined(__APPLE__) && defined(_LP64)
#pragma pack(pop)
#endif
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

#define DISABLE_OCOMS_IF_UTIL 1

#if defined(DISABLE_OCOMS_IF_UTIL) && !DISABLE_OCOMS_IF_UTIL

#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/if.h"
#include "ocoms/util/output.h"
#include "ocoms/util/argv.h"
#include "ocoms/platform/ocoms_constants.h"


#ifdef HAVE_STRUCT_SOCKADDR_IN

#ifndef MIN
#  define MIN(a,b)                ((a) < (b) ? (a) : (b))
#endif
        
/*
 *  Look for interface by name and returns its address 
 *  as a dotted decimal formatted string.
 */

int ocoms_ifnametoaddr(const char* if_name, struct sockaddr* addr, int length)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (strcmp(intf->if_name, if_name) == 0) {
            memcpy(addr, &intf->if_addr, length);
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}


/*
 *  Look for interface by name and returns its 
 *  corresponding ocoms_list index.
 */

int ocoms_ifnametoindex(const char* if_name)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return -1;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (strcmp(intf->if_name, if_name) == 0) {
            return intf->if_index;
        }
    }
    return -1;
}


/*
 *  Look for interface by name and returns its 
 *  corresponding kernel index.
 */

int16_t ocoms_ifnametokindex(const char* if_name)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return -1;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (strcmp(intf->if_name, if_name) == 0) {
            return intf->if_kernel_index;
        }
    }
    return -1;
}


/*
 *  Look for interface by ocoms_list index and returns its 
 *  corresponding kernel index.
 */

int ocoms_ifindextokindex(int if_index)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return -1;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (if_index == intf->if_index) {
            return intf->if_kernel_index;
        }
    }
    return -1;
}


/*
 *  Attempt to resolve the adddress (given as either IPv4/IPv6 string
 *  or hostname) and lookup corresponding interface.
 */

int ocoms_ifaddrtoname(const char* if_addr, char* if_name, int length)
{
    ocoms_sysif_t* intf;
#if OCOMS_WANT_IPV6
    int error;
    struct addrinfo hints, *res = NULL, *r;
#else
#ifndef __WINDOWS__
    in_addr_t inaddr;
#else 
    unsigned long inaddr;
#endif
    struct hostent *h;
#endif

    /* if the user asked us not to resolve interfaces, then just return */
    if (ocoms_sysif_do_not_resolve) {
        /* return not found so ifislocal will declare
         * the node to be non-local
         */
        return OCOMS_ERR_NOT_FOUND;
    }
    
    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

#if OCOMS_WANT_IPV6
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(if_addr, NULL, &hints, &res);

    if (error) {
        if (NULL != res) {
            freeaddrinfo (res);
        }
        return OCOMS_ERR_NOT_FOUND;
    }

    for (r = res; r != NULL; r = r->ai_next) {
        for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
            intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
            intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
            
            if (AF_INET == r->ai_family) {
                struct sockaddr_in ipv4;
                struct sockaddr_in *inaddr;

                inaddr = (struct sockaddr_in*) &intf->if_addr;
                memcpy (&ipv4, r->ai_addr, r->ai_addrlen);
                
                if (inaddr->sin_addr.s_addr == ipv4.sin_addr.s_addr) {
                    strncpy(if_name, intf->if_name, length);
                    return OCOMS_SUCCESS;
                }
            } else {
                if (IN6_ARE_ADDR_EQUAL(&((struct sockaddr_in6*) &intf->if_addr)->sin6_addr,
                    &((struct sockaddr_in6*) r->ai_addr)->sin6_addr)) {
                    strncpy(if_name, intf->if_name, length);
                    return OCOMS_SUCCESS;
                }
            }
        }
    }
    if (NULL != res) {
        freeaddrinfo (res);
    }
#else
    inaddr = inet_addr(if_addr);

    if (INADDR_NONE == inaddr) {
        h = gethostbyname(if_addr);
        if (0 == h) {
            return OCOMS_ERR_NOT_FOUND;
        }
        memcpy(&inaddr, h->h_addr, sizeof(inaddr));
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (((struct sockaddr_in*) &intf->if_addr)->sin_addr.s_addr == inaddr) {
            strncpy(if_name, intf->if_name, length);
            return OCOMS_SUCCESS;
        }
    }
#endif
    return OCOMS_ERR_NOT_FOUND;
}

/*
 *  Return the number of discovered interface.
 */

int ocoms_ifcount(void)
{
    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return 0;
    }

    return ocoms_list_get_size(&ocoms_sysif_list);
}


/*
 *  Return the ocoms_list interface index for the first
 *  interface in our list.
 */

int ocoms_ifbegin(void)
{
    ocoms_sysif_t *intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return -1;
    }

    intf = (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
    if (NULL != intf)
        return intf->if_index;
    return (-1);
}


/*
 *  Located the current position in the list by if_index and
 *  return the interface index of the next element in our list
 *  (if it exists).
 */

int ocoms_ifnext(int if_index)
{
    ocoms_sysif_t *intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return -1;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            do {
                ocoms_sysif_t* if_next = (ocoms_sysif_t*)ocoms_list_get_next(intf);
                ocoms_sysif_t* if_end =  (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
                if (if_next == if_end) {
                    return -1;
                }
                intf = if_next;
            } while(intf->if_index == if_index);
            return intf->if_index;
        }
    }
    return (-1);
}


/* 
 *  Lookup the interface by ocoms_list index and return the 
 *  primary address assigned to the interface.
 */

int ocoms_ifindextoaddr(int if_index, struct sockaddr* if_addr, unsigned int length)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            memcpy(if_addr, &intf->if_addr, MIN(length, sizeof (intf->if_addr)));
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}


/* 
 *  Lookup the interface by ocoms_list index and return the 
 *  network mask assigned to the interface.
 */

int ocoms_ifindextomask(int if_index, uint32_t* if_mask, int length)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            memcpy(if_mask, &intf->if_mask, length);
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}

/* 
 *  Lookup the interface by ocoms_list index and return the 
 *  flags assigned to the interface.
 *
 *  Bug: Make return type portable (compatible with Windows)
 */

int ocoms_ifindextoflags(int if_index, uint32_t* if_flags)
{
    ocoms_sysif_t* intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            memcpy(if_flags, &intf->if_flags, sizeof(uint32_t));
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}



/* 
 *  Lookup the interface by ocoms_list index and return
 *  the associated name.
 */

int ocoms_ifindextoname(int if_index, char* if_name, int length)
{
    ocoms_sysif_t *intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            strncpy(if_name, intf->if_name, length);
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}


/* 
 *  Lookup the interface by kernel index and return
 *  the associated name.
 */

int ocoms_ifkindextoname(int if_kindex, char* if_name, int length)
{
    ocoms_sysif_t *intf;

    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_kernel_index == if_kindex) {
            strncpy(if_name, intf->if_name, length);
            return OCOMS_SUCCESS;
        }
    }
    return OCOMS_ERROR;
}


#define ADDRLEN 100
bool
ocoms_ifislocal(const char *hostname)
{
    int ret;
#if OCOMS_WANT_IPV6
    char addrname[NI_MAXHOST]; /* should be larger than ADDRLEN, but I think
                                  they really mean IFNAMESIZE */
#else
    char addrname[ADDRLEN + 1];
#endif

    ret = ocoms_ifaddrtoname(hostname, addrname, ADDRLEN);

    return (OCOMS_SUCCESS == ret) ? true : false;
}

static uint32_t parse_dots(char *addr)
{
    char **tuple;
    uint32_t n[]={0,0,0,0};
    uint32_t net;
    int i;

    tuple = ocoms_argv_split(addr, '.');
    /* now assemble the address */
    for (i=0; NULL != tuple[i]; i++) {
        n[i] = strtoul(tuple[i], NULL, 10);
    }
    net = OCOMS_IF_ASSEMBLE_NETWORK(n[0], n[1], n[2], n[3]);
    ocoms_argv_free(tuple);
    return net;
}

int
ocoms_iftupletoaddr(char *inaddr, uint32_t *net, uint32_t *mask)
{
    char *addr;
    char **tuple;
    int pval;
    char *msk, *ptr;
    
    /* if a mask was desired... */
    if (NULL != mask) {
        /* set default */
        *mask = 0xFFFFFFFF;
        /* protect the input */
        addr = strdup(inaddr);
        
        /* if entry includes mask, split that off */
        msk = NULL;
        if (NULL != (ptr = strchr(addr, '/'))) {
            *ptr = '\0';
            msk = ptr + 1;
            /* is the mask a tuple? */
            if (NULL != strchr(msk, '.')) {
                /* yes - extract mask from it */
                *mask = parse_dots(msk);
            } else {
                /* no - must be an int telling us how
                 * much of the addr to use: e.g., /16
                 */
                pval = strtol(msk, NULL, 10);
                if (24 == pval) {
                    *mask = 0xFFFFFF00;
                } else if (16 == pval) {
                    *mask = 0xFFFF0000;
                } else if (8 == pval) {
                    *mask = 0xFF000000;
                } else {
                    ocoms_output(0, "ocoms_iftupletoaddr: unknown mask");
                    free(addr);
                    return OCOMS_ERROR;
                }
            }
        } else {
            /* use the number of dots to determine it */
            tuple = ocoms_argv_split(addr, '.');
            pval = ocoms_argv_count(tuple);
            /* if we have three dots, then we have four
             * fields since it is a full address, so the
             * default netmask is fine
             */
            if (pval < 4) {
                if (3 == pval) {         /* 2 dots */
                    *mask = 0xFFFFFF00;
                } else if (2 == pval) {  /* 1 dot */
                    *mask = 0xFFFF0000;
                } else if (1 == pval) {  /* no dots */
                    *mask = 0xFF000000;
                } else {
                    ocoms_output(0, "ocoms_iftupletoaddr: unknown mask");
                    free(addr);
                    return OCOMS_ERROR;
                }
            }
            ocoms_argv_free(tuple);
        }
        free(addr);
    }
    
    /* if network addr is desired... */
    if (NULL != net) {
        /* set default */
        *net = 0;
        /* protect the input */
        addr = strdup(inaddr);
        
        /* if entry includes mask, split that off */
        if (NULL != (ptr = strchr(addr, '/'))) {
            *ptr = '\0';
        }
        /* now assemble the address */
        *net = parse_dots(addr);
        free(addr);
    }
    
    return OCOMS_SUCCESS;
}

/* 
 *  Determine if the specified interface is loopback
 */

bool ocoms_ifisloopback(int if_index)
{
    ocoms_sysif_t* intf;
    
    if (OCOMS_SUCCESS != ocoms_sysif_base_open()) {
        return OCOMS_ERROR;
    }

    for (intf =  (ocoms_sysif_t*)ocoms_list_get_first(&ocoms_sysif_list);
        intf != (ocoms_sysif_t*)ocoms_list_get_end(&ocoms_sysif_list);
        intf =  (ocoms_sysif_t*)ocoms_list_get_next(intf)) {
        if (intf->if_index == if_index) {
            if ((intf->if_flags & IFF_LOOPBACK) != 0) {
                return true;
            }
        }
    }
    return false;
}


#else /* HAVE_STRUCT_SOCKADDR_IN */

/* if we don't have struct sockaddr_in, we don't have traditional
   ethernet devices.  Just make everything a no-op error call */

int
ocoms_ifnametoaddr(const char* if_name, 
                  struct sockaddr* if_addr, int size)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifaddrtoname(const char* if_addr, 
                  char* if_name, int size)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifnametoindex(const char* if_name)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int16_t
ocoms_ifnametokindex(const char* if_name)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifindextokindex(int if_index)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifcount(void)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifbegin(void)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifnext(int if_index)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifindextoname(int if_index, char* if_name, int length)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifkindextoname(int kif_index, char* if_name, int length)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifindextoaddr(int if_index, struct sockaddr* if_addr, unsigned int length)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

int
ocoms_ifindextomask(int if_index, uint32_t* if_addr, int length)
{
    return OCOMS_ERR_NOT_SUPPORTED;
}

bool
ocoms_ifislocal(const char *hostname)
{
    return false;
}

int
ocoms_iftupletoaddr(char *inaddr, uint32_t *net, uint32_t *mask)
{
    return 0;
}

#endif /* HAVE_STRUCT_SOCKADDR_IN */
#endif /* DISABLE_OCOMS_IF_UTIL  */

