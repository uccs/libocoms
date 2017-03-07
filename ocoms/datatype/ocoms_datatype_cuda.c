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

#include "ocoms/platform/ocoms_config.h"

#include <errno.h>
#include <unistd.h>

#include "ocoms/primitives/align.h"
#include "ocoms/util/output.h"
#include "ocoms/datatype/ocoms_convertor.h"
#include "ocoms/datatype/ocoms_datatype_cuda.h"

static bool initialized = false;
int ocoms_cuda_verbose = 0;
static int ocoms_cuda_enabled = 0; /* Starts out disabled */
static int ocoms_cuda_output = 0;
static void ocoms_cuda_support_init(void);
static int (*common_cuda_initialization_function)(ocoms_common_cuda_function_table_t *) = NULL;
static ocoms_common_cuda_function_table_t ftable;

/* This function allows the common cuda code to register an
 * initialization function that gets called the first time an attempt
 * is made to send or receive a GPU pointer.  This allows us to delay
 * some CUDA initialization until after MPI_Init().
 */
void ocoms_cuda_add_initialization_function(int (*fptr)(ocoms_common_cuda_function_table_t *)) {
    common_cuda_initialization_function = fptr;
}

/**
 * This function is called when a convertor is instantiated.  It has to call
 * the ocoms_cuda_support_init() function once to figure out if CUDA support
 * is enabled or not.  If CUDA is not enabled, then short circuit out
 * for all future calls.
 */
void mca_cuda_convertor_init(ocoms_convertor_t* convertor, const void *pUserBuf)
{   
    /* Only do the initialization on the first GPU access */
    if (!initialized) {
        ocoms_cuda_support_init();
    }

    /* If not enabled, then nothing else to do */
    if (!ocoms_cuda_enabled) {
        return;
    }

    if (ftable.gpu_is_gpu_buffer(pUserBuf)) {
        convertor->cbmemcpy = (memcpy_fct_t)&ocoms_cuda_memcpy;
        convertor->flags |= CONVERTOR_CUDA;
    }
}

/* Checks the type of pointer
 *
 * @param dest   One pointer to check
 * @param source Another pointer to check
 */
bool ocoms_cuda_check_bufs(char *dest, char *src)
{
    /* Only do the initialization on the first GPU access */
    if (!initialized) {
        ocoms_cuda_support_init();
    }

    if (!ocoms_cuda_enabled) {
        return false;
    }

    if (ftable.gpu_is_gpu_buffer(dest) || ftable.gpu_is_gpu_buffer(src)) {
        return true;
    } else {
        return false;
    }
}

/*
 * With CUDA enabled, all contiguous copies will pass through this function.
 * Therefore, the first check is to see if the convertor is a GPU buffer.
 * Note that if there is an error with any of the CUDA calls, the program
 * aborts as there is no recovering.
 */
void *ocoms_cuda_memcpy(void *dest, const void *src, size_t size, ocoms_convertor_t* convertor)
{
    int res;

    if (!(convertor->flags & CONVERTOR_CUDA)) {
        return memcpy(dest, src, size);
    }
            
    if (convertor->flags & CONVERTOR_CUDA_ASYNC) {
        res = ftable.gpu_cu_memcpy_async(dest, (void *)src, size, convertor);
    } else {
        res = ftable.gpu_cu_memcpy(dest, (void *)src, size);
    }

    if (res != 0) {
        ocoms_output(0, "CUDA: Error in cuMemcpy: res=%d, dest=%p, src=%p, size=%d",
                    res, dest, src, (int)size);
        abort();
    } else {
        return dest;
    }
}

/*
 * This function is needed in cases where we do not have contiguous
 * datatypes.  The current code has macros that cannot handle a convertor
 * argument to the memcpy call.
 */
void *ocoms_cuda_memcpy_sync(void *dest, void *src, size_t size)
{
    int res;
    res = ftable.gpu_cu_memcpy(dest, src, size);
    if (res != 0) {
        ocoms_output(0, "CUDA: Error in cuMemcpy: res=%d, dest=%p, src=%p, size=%d",
                    res, dest, src, (int)size);
        abort();
    } else {
        return dest;
    }
}

/*
 * In some cases, need an implementation of memmove.  This is not fast, but
 * it is not often needed.
 */
void *ocoms_cuda_memmove(void *dest, void *src, size_t size)
{
    int res;

    res = ftable.gpu_memmove(dest, src, size);
    if(res != 0){
        ocoms_output(0, "CUDA: Error in gpu memmove: res=%d, dest=%p, src=%p, size=%d",
                    res, dest, src, (int)size);
        abort();
    }
    return dest;
}

/**
 * This function gets called once to check if the program is running in a cuda
 * environment. 
 */
static void ocoms_cuda_support_init(void)
{
    if (initialized) {
        return;
    }

    /* Set different levels of verbosity in the cuda related code. */
    ocoms_cuda_output = ocoms_output_open(NULL);
    ocoms_output_set_verbosity(ocoms_cuda_output, ocoms_cuda_verbose);

    /* Callback into the common cuda initialization routine. This is only
     * set if some work had been done already in the common cuda code.*/
    if (NULL != common_cuda_initialization_function) {
        if (0 == common_cuda_initialization_function(&ftable)) {
            ocoms_cuda_enabled = 1;
        }
    }

    if (1 == ocoms_cuda_enabled) {
        ocoms_output_verbose(10, ocoms_cuda_output,
                            "CUDA: enabled successfully, CUDA device pointers will work");
    } else {
        ocoms_output_verbose(10, ocoms_cuda_output,
                            "CUDA: not enabled, CUDA device pointers will not work");
    }

    initialized = true;
}

/**
 * Tell the convertor that copies will be asynchronous CUDA copies.  The
 * flags are cleared when the convertor is reinitialized.
 */
void ocoms_cuda_set_copy_function_async(ocoms_convertor_t* convertor, void *stream)
{
    convertor->flags |= CONVERTOR_CUDA_ASYNC;
    convertor->stream = stream;
}
