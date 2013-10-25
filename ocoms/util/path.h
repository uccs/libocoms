/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, LLC.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 * @file
 */

#ifndef OCOMS_PATH_H
#define OCOMS_PATH_H

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/platform/ocoms_constants.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

BEGIN_C_DECLS

/**
 *  Locates a file with certain permissions
 *  
 *  @param fname File name
 *  @param pathv Array of search directories
 *  @param mode  Permissions which must be satisfied (see access(2))
 *  @param envv  Pointer to string containing environment
 *
 *  @retval Full pathname of located file Success
 *  @retval NULL Failure
 *
 *  Environment variables can appear in the form $variable at the
 *  start of a prefix path and will be replaced by the environment
 *  value if it is defined; otherwise the whole prefix is ignored.
 *  Environment variables must be followed by a path delimiter or
 *  end-of-string.
 *
 * The caller is responsible for freeing the returned string.
 */
OCOMS_DECLSPEC char *ocoms_path_find(char *fname, char **pathv, int mode,
                                   char **envv) __ocoms_attribute_malloc__ __ocoms_attribute_warn_unused_result__;

/**
 *  Locates a file with certain permissions from a list of search
 *  paths
 *
 *  @param fname File name
 *  @param mode  Target permissions which must be satisfied (see access(2))
 *  @param envv  Pointer to environment list
 *  @param wrkdir Working directory
 *
 *  @retval Full pathname of located file Success
 *  @retval NULL Failure
 *
 *  Locates a file with certain permissions from the list of paths
 *  given by the $PATH environment variable.  Replaces "." in the
 *  path with the working dir.
 *
 * The caller is responsible for freeing the returned string.
 */
OCOMS_DECLSPEC char *ocoms_path_findv(char *fname, int mode, 
                                    char **envv, char *wrkdir) __ocoms_attribute_malloc__ __ocoms_attribute_warn_unused_result__;
/**
 *  Detect if the requested path is absolute or relative.
 *
 *  @param path  File name
 *
 *  @retval true if the path is absolute
 *  @retval false otherwise
 *
 *  Detect if a path is absolute or relative. Handle Windows
 *  with special care as an absolute path on Windows starts
 *  with [A-Za-z]: or \\ instead of the usual / on UNIX.
 */
OCOMS_DECLSPEC bool ocoms_path_is_absolute( const char *path );

/**
 * Find the absolute path for an executable and return it.
 *
 *  @param app_name  Executable name
 *
 *  @retval The absolute path if the application can be reached,
 *  @retval NULL otherwise.
 *
 * Try to figure out the absolute path based on the application name
 * (usually argv[0]). If the path is already absolute return a copy, if
 * it start with . look into the current directory, if not dig into
 * the $PATH.
 * In case of error or if executable was not found (as an example if
 * the application did a cwd between the start and this call), the
 * function will return NULL. Otherwise, an newly allocated string
 * will be returned.
 */
OCOMS_DECLSPEC char* ocoms_find_absolute_path( char* app_name ) __ocoms_attribute_warn_unused_result__;

/**
 * Forms a complete pathname and checks it for existance and
 * permissions
 *
 * @param fname File name
 * @param path Path prefix, if NULL then fname is an absolute path
 * @param mode Target permissions which must be satisfied (see access(2))
 *
 * @retval NULL Failure
 * @retval Full pathname of the located file on Success
 *
 * The caller is responsible for freeing the returned string.
 */
OCOMS_DECLSPEC char *ocoms_path_access(char *fname, char *path, int mode) __ocoms_attribute_malloc__ __ocoms_attribute_warn_unused_result__;


/**
 * @brief Figure out, whether fname is on network file system
 *
 * Try to figure out, whether the file name specified through fname is
 * on any network file system (currently NFS, Lustre and Panasas).
 *
 * If the file is not created, the parent directory is checked.
 * This allows checking for NFS prior to opening the file.
 *
 * @param[in]     fname        File name to check
 *
 * @retval true                If fname is on NFS, Lustre or Panasas
 * @retval false               otherwise
 */
OCOMS_DECLSPEC bool ocoms_path_nfs(char *fname) __ocoms_attribute_warn_unused_result__;

OCOMS_DECLSPEC int ocoms_path_df(const char *path, uint64_t *out_avail);

/**
 * @brief Returns the disk usage of path.
 *
 * @param[in] path       Path to check
 * @out_avail[out]       Amount of free space available on path (if successful)
 *
 * @retval OCOMS_SUCCESS  If the operation was successful
 * @retval OCOMS_ERROR    otherwise
 */
OCOMS_DECLSPEC int
ocoms_path_df(const char *path,
             uint64_t *out_avail)__ocoms_attribute_warn_unused_result__;

END_C_DECLS
#endif /* OCOMS_PATH_H */
