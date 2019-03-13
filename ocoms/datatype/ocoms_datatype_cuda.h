/*
 * Copyright (c) 2011-2013 NVIDIA Corporation.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef _OCOMS_DATATYPE_CUDA_H
#define _OCOMS_DATATYPE_CUDA_H

/* Structure to hold CUDA support functions that gets filled in when the
 * common cuda code is initialized.  This removes any dependency on <cuda.h>
 * in the ocoms cuda datatype code. */
struct ocoms_common_cuda_function_table {
    int (*gpu_is_gpu_buffer)(const void*);
    int (*gpu_cu_memcpy_async)(void*, const void*, size_t, ocoms_convertor_t*);
    int (*gpu_cu_memcpy)(void*, const void*, size_t);
    int (*gpu_memmove)(void*, void*, size_t);
};
typedef struct ocoms_common_cuda_function_table ocoms_common_cuda_function_table_t;

void ocoms_cuda_convertor_init(ocoms_convertor_t* convertor, const void *pUserBuf);
bool ocoms_cuda_check_bufs(char *dest, char *src);
void* ocoms_cuda_memcpy(void * dest, const void * src, size_t size, ocoms_convertor_t* convertor);
void* ocoms_cuda_memcpy_sync(void * dest, void * src, size_t size);
void* ocoms_cuda_memmove(void * dest, void * src, size_t size);
void ocoms_cuda_add_initialization_function(int (*fptr)(ocoms_common_cuda_function_table_t *));
void ocoms_cuda_set_copy_function_async(ocoms_convertor_t* convertor, void *stream);

#endif
