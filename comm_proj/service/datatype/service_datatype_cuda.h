/*
 * Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef _CCS_DATATYPE_CUDA_H
#define _CCS_DATATYPE_CUDA_H

#include "cuda.h"

void mca_cuda_convertor_init(service_convertor_t* convertor, const void *pUserBuf);
bool service_cuda_check_bufs(char *dest, char *src);
void* service_cuda_memcpy(void * dest, void * src, size_t size);
void* service_cuda_memmove(void * dest, void * src, size_t size);

#endif
