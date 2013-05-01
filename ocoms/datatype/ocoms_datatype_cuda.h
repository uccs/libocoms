/*
 * Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
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

#include "cuda.h"

void mca_cuda_convertor_init(ocoms_convertor_t* convertor, const void *pUserBuf);
bool ocoms_cuda_check_bufs(char *dest, char *src);
void* ocoms_cuda_memcpy(void * dest, void * src, size_t size);
void* ocoms_cuda_memmove(void * dest, void * src, size_t size);

#endif
