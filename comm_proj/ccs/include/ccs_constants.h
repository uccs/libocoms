#ifndef CCS_CONSTANTS_H
#define CCS_CONSTANTS_H

#define CCS_ERR_BASE             0 /* internal use only */

enum {
    CCS_SUCCESS                            = (CCS_ERR_BASE),

    CCS_ERROR                              = (CCS_ERR_BASE -  1),
    CCS_ERR_OUT_OF_RESOURCE                = (CCS_ERR_BASE -  2), /* fatal error */
    CCS_ERR_TEMP_OUT_OF_RESOURCE           = (CCS_ERR_BASE - 3), /* try again later */
    CCS_ERR_RESOURCE_BUSY                  = (CCS_ERR_BASE - 4),
    CCS_ERR_BAD_PARAM                      = (CCS_ERR_BASE -  5),  /* equivalent to MPI_ERR_ARG error code
                               */
    CCS_ERR_FATAL                          = (CCS_ERR_BASE -  6),
    CCS_ERR_NOT_IMPLEMENTED                = (CCS_ERR_BASE -  7),
    CCS_ERR_NOT_SUPPORTED                  = (CCS_ERR_BASE -  8)
};

#define CCS_ERR_MAX                (CCS_ERR_BASE - 100)


#endif /* CCS_CONSTANTS_H */
