/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OPAL_MCA_IF_IF_H
#define OPAL_MCA_IF_IF_H

#include "ccs_config.h"

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

#include "service/util/if.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"

BEGIN_C_DECLS

/*
 * Define INADDR_NONE if we don't have it.  Solaris is the only system
 * where I have found that it does not exist, and the man page for
 * inet_addr() says that it returns -1 upon failure.  On Linux and
 * other systems with INADDR_NONE, it's just a #define to -1 anyway.
 * So just #define it to -1 here if it doesn't already exist.
 */

#if !defined(INADDR_NONE)
#define INADDR_NONE -1
#endif

#define DEFAULT_NUMBER_INTERFACES 10
#define MAX_IFCONF_SIZE 10 * 1024 * 1024


typedef struct ccs_sysif_t {
    service_list_item_t     super;
    char                if_name[IF_NAMESIZE];
    int                 if_index;
    uint16_t            if_kernel_index;
#ifndef __WINDOWS__
    int                 if_flags;
#else
    u_long              if_flags;
#endif
    int                 if_speed;
    struct sockaddr_storage  if_addr;
   uint32_t             if_mask;
#ifdef __WINDOWS__
    struct sockaddr_in  if_bcast;
#endif
    uint32_t            if_bandwidth;
} ccs_sysif_t;
CCS_DECLSPEC OBJ_CLASS_DECLARATION(ccs_sysif_t);


/* "global" list of available interfaces */
CCS_DECLSPEC extern service_list_t ccs_sysif_list;

/* global flags */
CCS_DECLSPEC extern bool ccs_sysif_do_not_resolve;
CCS_DECLSPEC extern bool ccs_sysif_retain_loopback;

/**
 * Structure for if components.
 */
struct ccs_sysif_base_component_2_0_0_t {
    /** MCA base component */
    ccs_mca_base_component_t component;
    /** MCA base data */
    ccs_mca_base_component_data_t component_data;
};
/**
 * Convenience typedef
 */
typedef struct ccs_sysif_base_component_2_0_0_t ccs_sysif_base_component_t;

/*
 * Macro for use in components that are of type if
 */
#define OPAL_IF_BASE_VERSION_2_0_0 \
    MCA_BASE_VERSION_2_0_0, \
    "if", 2, 0, 0

END_C_DECLS

#endif /* OPAL_MCA_IF_IF_H */
