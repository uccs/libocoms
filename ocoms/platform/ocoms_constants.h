#ifndef OCOMS_CONSTANTS_H
#define OCOMS_CONSTANTS_H

#define OCOMS_ERR_BASE             0 /* internal use only */

enum {
    OCOMS_SUCCESS                            = (OCOMS_ERR_BASE),

    OCOMS_ERROR                              = (OCOMS_ERR_BASE -  1),
    OCOMS_ERR_OUT_OF_RESOURCE                = (OCOMS_ERR_BASE -  2), /* fatal error */
    OCOMS_ERR_TEMP_OUT_OF_RESOURCE           = (OCOMS_ERR_BASE -  3), /* try again later */
    OCOMS_ERR_RESOURCE_BUSY                  = (OCOMS_ERR_BASE -  4),
    OCOMS_ERR_BAD_PARAM                      = (OCOMS_ERR_BASE -  5),  /* equivalent to MPI_ERR_ARG error code */
    OCOMS_ERR_FATAL                          = (OCOMS_ERR_BASE -  6),
    OCOMS_ERR_NOT_IMPLEMENTED                = (OCOMS_ERR_BASE -  7),
    OCOMS_ERR_NOT_SUPPORTED                  = (OCOMS_ERR_BASE -  8),
    OCOMS_ERR_INTERUPTED                     = (OCOMS_ERR_BASE -  9),
    OCOMS_ERR_WOULD_BLOCK                    = (OCOMS_ERR_BASE - 10),
    OCOMS_ERR_IN_ERRNO                       = (OCOMS_ERR_BASE - 11),
    OCOMS_ERR_UNREACH                        = (OCOMS_ERR_BASE - 12),
    OCOMS_ERR_NOT_FOUND                      = (OCOMS_ERR_BASE - 13),
    OCOMS_EXISTS                             = (OCOMS_ERR_BASE - 14), /* indicates that the specified object already exists */
    OCOMS_ERR_TIMEOUT                        = (OCOMS_ERR_BASE - 15),
    OCOMS_ERR_NOT_AVAILABLE                  = (OCOMS_ERR_BASE - 16),
    OCOMS_ERR_PERM                           = (OCOMS_ERR_BASE - 17), /* no permission */
    OCOMS_ERR_VALUE_OUT_OF_BOUNDS            = (OCOMS_ERR_BASE - 18),
    OCOMS_ERR_FILE_READ_FAILURE              = (OCOMS_ERR_BASE - 19),
    OCOMS_ERR_FILE_WRITE_FAILURE             = (OCOMS_ERR_BASE - 20),
    OCOMS_ERR_FILE_OPEN_FAILURE              = (OCOMS_ERR_BASE - 21),
    OCOMS_ERR_PACK_MISMATCH                  = (OCOMS_ERR_BASE - 22),
    OCOMS_ERR_PACK_FAILURE                   = (OCOMS_ERR_BASE - 23),
    OCOMS_ERR_UNPACK_FAILURE                 = (OCOMS_ERR_BASE - 24),
    OCOMS_ERR_UNPACK_INADEQUATE_SPACE        = (OCOMS_ERR_BASE - 25),
    OCOMS_ERR_UNPACK_READ_PAST_END_OF_BUFFER = (OCOMS_ERR_BASE - 26),
    OCOMS_ERR_TYPE_MISMATCH                  = (OCOMS_ERR_BASE - 27),
    OCOMS_ERR_OPERATION_UNSUPPORTED          = (OCOMS_ERR_BASE - 28),
    OCOMS_ERR_UNKNOWN_DATA_TYPE              = (OCOMS_ERR_BASE - 29),
    OCOMS_ERR_BUFFER                         = (OCOMS_ERR_BASE - 30),
    OCOMS_ERR_DATA_TYPE_REDEF                = (OCOMS_ERR_BASE - 31),
    OCOMS_ERR_DATA_OVERWRITE_ATTEMPT         = (OCOMS_ERR_BASE - 32),
    OCOMS_ERR_MODULE_NOT_FOUND               = (OCOMS_ERR_BASE - 33),
    OCOMS_ERR_TOPO_SLOT_LIST_NOT_SUPPORTED   = (OCOMS_ERR_BASE - 34),
    OCOMS_ERR_TOPO_SOCKET_NOT_SUPPORTED      = (OCOMS_ERR_BASE - 35),
    OCOMS_ERR_TOPO_CORE_NOT_SUPPORTED        = (OCOMS_ERR_BASE - 36),
    OCOMS_ERR_NOT_ENOUGH_SOCKETS             = (OCOMS_ERR_BASE - 37),
    OCOMS_ERR_NOT_ENOUGH_CORES               = (OCOMS_ERR_BASE - 38),
    OCOMS_ERR_INVALID_PHYS_CPU               = (OCOMS_ERR_BASE - 39),
    OCOMS_ERR_MULTIPLE_AFFINITIES            = (OCOMS_ERR_BASE - 40),
    OCOMS_ERR_SLOT_LIST_RANGE                = (OCOMS_ERR_BASE - 41),
    OCOMS_ERR_NETWORK_NOT_PARSEABLE          = (OCOMS_ERR_BASE - 42),
    OCOMS_ERR_SILENT                         = (OCOMS_ERR_BASE - 43),
    OCOMS_ERR_NOT_INITIALIZED                = (OCOMS_ERR_BASE - 44),
    OCOMS_ERR_NOT_BOUND                      = (OCOMS_ERR_BASE - 45),
    OCOMS_ERR_TAKE_NEXT_OPTION               = (OCOMS_ERR_BASE - 46),
    OCOMS_ERR_PROC_ENTRY_NOT_FOUND           = (OCOMS_ERR_BASE - 47),
    OCOMS_ERR_DATA_VALUE_NOT_FOUND           = (OCOMS_ERR_BASE - 48)
};

#define OCOMS_ERR_MAX                (OCOMS_ERR_BASE - 100)

enum {
    AM_TAG_OPENIB = 1,
};

#define OCOMS_MODEX_TAG 100 

#endif /* OCOMS_CONSTANTS_H */
