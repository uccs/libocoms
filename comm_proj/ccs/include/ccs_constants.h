#ifndef CCS_CONSTANTS_H
#define CCS_CONSTANTS_H

#define CCS_ERR_BASE             0 /* internal use only */

enum {
    CCS_SUCCESS                            = (CCS_ERR_BASE),

    CCS_ERROR                              = (CCS_ERR_BASE -  1),
    CCS_ERR_OUT_OF_RESOURCE                = (CCS_ERR_BASE -  2), /* fatal error */
    CCS_ERR_TEMP_OUT_OF_RESOURCE           = (CCS_ERR_BASE -  3), /* try again later */
    CCS_ERR_RESOURCE_BUSY                  = (CCS_ERR_BASE -  4),
    CCS_ERR_BAD_PARAM                      = (CCS_ERR_BASE -  5),  /* equivalent to MPI_ERR_ARG error code */
    CCS_ERR_FATAL                          = (CCS_ERR_BASE -  6),
    CCS_ERR_NOT_IMPLEMENTED                = (CCS_ERR_BASE -  7),
    CCS_ERR_NOT_SUPPORTED                  = (CCS_ERR_BASE -  8),
    CCS_ERR_INTERUPTED                     = (CCS_ERR_BASE -  9),
    CCS_ERR_WOULD_BLOCK                    = (CCS_ERR_BASE - 10),
    CCS_ERR_IN_ERRNO                       = (CCS_ERR_BASE - 11),
    CCS_ERR_UNREACH                        = (CCS_ERR_BASE - 12),
    CCS_ERR_NOT_FOUND                      = (CCS_ERR_BASE - 13),
    CCS_EXISTS                             = (CCS_ERR_BASE - 14), /* indicates that the specified object already exists */
    CCS_ERR_TIMEOUT                        = (CCS_ERR_BASE - 15),
    CCS_ERR_NOT_AVAILABLE                  = (CCS_ERR_BASE - 16),
    CCS_ERR_PERM                           = (CCS_ERR_BASE - 17), /* no permission */
    CCS_ERR_VALUE_OUT_OF_BOUNDS            = (CCS_ERR_BASE - 18),
    CCS_ERR_FILE_READ_FAILURE              = (CCS_ERR_BASE - 19),
    CCS_ERR_FILE_WRITE_FAILURE             = (CCS_ERR_BASE - 20),
    CCS_ERR_FILE_OPEN_FAILURE              = (CCS_ERR_BASE - 21),
    CCS_ERR_PACK_MISMATCH                  = (CCS_ERR_BASE - 22),
    CCS_ERR_PACK_FAILURE                   = (CCS_ERR_BASE - 23),
    CCS_ERR_UNPACK_FAILURE                 = (CCS_ERR_BASE - 24),
    CCS_ERR_UNPACK_INADEQUATE_SPACE        = (CCS_ERR_BASE - 25),
    CCS_ERR_UNPACK_READ_PAST_END_OF_BUFFER = (CCS_ERR_BASE - 26),
    CCS_ERR_TYPE_MISMATCH                  = (CCS_ERR_BASE - 27),
    CCS_ERR_OPERATION_UNSUPPORTED          = (CCS_ERR_BASE - 28),
    CCS_ERR_UNKNOWN_DATA_TYPE              = (CCS_ERR_BASE - 29),
    CCS_ERR_BUFFER                         = (CCS_ERR_BASE - 30),
    CCS_ERR_DATA_TYPE_REDEF                = (CCS_ERR_BASE - 31),
    CCS_ERR_DATA_OVERWRITE_ATTEMPT         = (CCS_ERR_BASE - 32),
    CCS_ERR_MODULE_NOT_FOUND               = (CCS_ERR_BASE - 33),
    CCS_ERR_TOPO_SLOT_LIST_NOT_SUPPORTED   = (CCS_ERR_BASE - 34),
    CCS_ERR_TOPO_SOCKET_NOT_SUPPORTED      = (CCS_ERR_BASE - 35),
    CCS_ERR_TOPO_CORE_NOT_SUPPORTED        = (CCS_ERR_BASE - 36),
    CCS_ERR_NOT_ENOUGH_SOCKETS             = (CCS_ERR_BASE - 37),
    CCS_ERR_NOT_ENOUGH_CORES               = (CCS_ERR_BASE - 38),
    CCS_ERR_INVALID_PHYS_CPU               = (CCS_ERR_BASE - 39),
    CCS_ERR_MULTIPLE_AFFINITIES            = (CCS_ERR_BASE - 40),
    CCS_ERR_SLOT_LIST_RANGE                = (CCS_ERR_BASE - 41),
    CCS_ERR_NETWORK_NOT_PARSEABLE          = (CCS_ERR_BASE - 42)
};

#define CCS_ERR_MAX                (CCS_ERR_BASE - 100)

/* Predefined RTE TAGS */
typedef enum {
    CCS_RTE_TAG_COMMON_SM_COMP_INDEX = 100,
    CCS_RTE_TAG_COMMON_SM_BACK_FILE_CREATED,
} ccs_rte_tags_t;

/* Pasha: dummy orte_show_help declaration */
#define orte_show_help(A...)
/* Pasha: dummy CCS_SOS_GET_ERROR_CODE */
#define CCS_SOS_GET_ERROR_CODE(rc) rc

enum {
    AM_TAG_OPENIB = 1,
};

#endif /* CCS_CONSTANTS_H */
