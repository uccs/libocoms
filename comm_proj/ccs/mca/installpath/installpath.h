/*
 * Copyright (c) 2006-2007 Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef CCS_MCA_INSTALLDIRS_INSTALLDIRS_H
#define CCS_MCA_INSTALLDIRS_INSTALLDIRS_H

#include "ccs_config.h"

#include "service/mca/mca.h"
#include "service/mca/base/base.h"

BEGIN_C_DECLS

/*
 * Most of this file is just for ompi_info.  The only public interface
 * once service_init has been called is the service_install_dirs structure
 * and the service_install_dirs_expand() call */
struct service_install_dirs_t {
    char* prefix;
    char* exec_prefix;
    char* bindir;
    char* sbindir;
    char* libexecdir;
    char* datarootdir;
    char* datadir;
    char* sysconfdir;
    char* sharedstatedir;
    char* localstatedir;
    char* libdir;
    char* includedir;
    char* infodir;
    char* mandir;
    char* pkgdatadir;
    char* pkglibdir;
    char* pkgincludedir;
};
typedef struct service_install_dirs_t service_install_dirs_t;

/* Install directories.  Only available after service_init() */
CCS_DECLSPEC extern service_install_dirs_t service_install_dirs;

/**
 * Expand out path variables (such as ${prefix}) in the input string
 * using the current service_install_dirs structure */
CCS_DECLSPEC char * service_install_dirs_expand(const char* input);


/**
 * Structure for installpath components.
 */
struct service_installpath_base_component_2_0_0_t {
    /** MCA base component */
    ccs_mca_base_component_t component;
    /** MCA base data */
    ccs_mca_base_component_data_t component_data;
    /** install directories provided by the given component */
    service_install_dirs_t install_dirs_data;
};
/**
 * Convenience typedef
 */
typedef struct service_installpath_base_component_2_0_0_t service_installpath_base_component_t;

/*
 * Macro for use in components that are of type installpath
 */
#define CCS_INSTALLDIRS_BASE_VERSION_2_0_0 \
    MCA_BASE_VERSION_2_0_0, \
    "installpath", 2, 0, 0

END_C_DECLS

#endif /* CCS_MCA_INSTALLDIRS_INSTALLDIRS_H */
