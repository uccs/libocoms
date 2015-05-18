/*
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved. 
 * Copyright (c) 2014      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
/** @file:
 *
 * The OCOMS Database Framework
 *
 */

#ifndef OCOMS_DSTORE_TYPES_H
#define OCOMS_DSTORE_TYPES_H

#include "ocoms_config.h"
//#include "ocoms/types.h"

#include "ocoms/datatype/ocoms_datatype.h"
//#include "ocoms/mca/pmix/pmix.h"

BEGIN_C_DECLS

/* some values are provided by an external entity such
 * as the resource manager. These values enter the
 * system via the PMIx interface at startup, but are
 * not explicitly retrieved by processes. Instead, procs
 * access them after RTE-init has stored them. For ease-of-use,
 * we define equivalent dstore names here. PMIx attributes
 * not listed here should be directly accessed via the
 * OCOMS pmix framework */
#define OCOMS_DSTORE_CPUSET        PMIX_CPUSET
#define OCOMS_DSTORE_CREDENTIAL    PMIX_CREDENTIAL
#define OCOMS_DSTORE_TMPDIR        PMIX_TMPDIR
#define OCOMS_DSTORE_JOBID         PMIX_JOBID
#define OCOMS_DSTORE_APPNUM        PMIX_APPNUM
#define OCOMS_DSTORE_RANK          PMIX_RANK
#define OCOMS_DSTORE_GLOBAL_RANK   PMIX_GLOBAL_RANK
#define OCOMS_DSTORE_LOCALRANK     PMIX_LOCAL_RANK
#define OCOMS_DSTORE_NODERANK      PMIX_NODE_RANK
#define OCOMS_DSTORE_LOCALLDR      PMIX_LOCALLDR
#define OCOMS_DSTORE_APPLDR        PMIX_APPLDR
#define OCOMS_DSTORE_LOCAL_PEERS   PMIX_LOCAL_PEERS
#define OCOMS_DSTORE_UNIV_SIZE     PMIX_UNIV_SIZE
#define OCOMS_DSTORE_JOB_SIZE      PMIX_JOB_SIZE
#define OCOMS_DSTORE_LOCAL_SIZE    PMIX_LOCAL_SIZE
#define OCOMS_DSTORE_NODE_SIZE     PMIX_NODE_SIZE
#define OCOMS_DSTORE_MAX_PROCS     PMIX_MAX_PROCS
#define OCOMS_DSTORE_NPROC_OFFSET  PMIX_NPROC_OFFSET
#define OCOMS_DSTORE_HOSTNAME      PMIX_HOSTNAME

/* some OCOMS-appropriate key definitions */
#define OCOMS_DSTORE_LOCALITY      "ocoms.locality"          // (uint16_t) relative locality of a peer
/* proc-specific scratch dirs */
#define OCOMS_DSTORE_JOB_SDIR      "ocoms.job.session.dir"  // (char*) job-level session dir
#define OCOMS_DSTORE_MY_SDIR       "ocoms.my.session.dir"   // (char*) session dir for this proc
#define OCOMS_DSTORE_URI           "ocoms.uri"              // (char*) uri of specified proc
#define OCOMS_DSTORE_ARCH          "ocoms.arch"             // (uint32_t) arch for specified proc
#define OCOMS_DSTORE_HOSTID        "ocoms.hostid"           // (uint32_t) hostid of specified proc
#define OCOMS_DSTORE_NODEID        "ocoms.nodeid"           // (uint32_t) nodeid of specified proc

END_C_DECLS

#endif
