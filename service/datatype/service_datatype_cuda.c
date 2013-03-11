/*
 * Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "service/platform/ccs_config.h"

#include <errno.h>
#include <unistd.h>
#include <cuda.h>

#include "opal/align.h"
#include "service/mca/base/mca_base_param.h"
#include "service/util/output.h"
#include "orte/util/show_help.h"
#include "service/datatype/service_convertor.h"
#include "service/datatype/service_datatype_cuda.h"

static bool initialized = false;
static int service_cuda_verbose;
static int service_cuda_output = 0;
static void service_cuda_support_init(void);

void mca_cuda_convertor_init(service_convertor_t* convertor, const void *pUserBuf)
{   
    int res;
    CUmemorytype memType;
    CUdeviceptr dbuf = (CUdeviceptr)pUserBuf;

    if (!initialized) {
        service_cuda_support_init();
    }

    res = cuPointerGetAttribute(&memType,
                                CU_POINTER_ATTRIBUTE_MEMORY_TYPE, dbuf);
    if (res != CUDA_SUCCESS) {
        /* If we cannot determine it is device pointer,
         * just assume it is not. */
        return;
    } else if (memType == CU_MEMORYTYPE_HOST) {
        /* Host memory, nothing to do here */
        return;
    }
    /* Must be a device pointer */
    assert(memType == CU_MEMORYTYPE_DEVICE);

    convertor->cbmemcpy = (memcpy_fct_t)&service_cuda_memcpy;
    convertor->flags |= CONVERTOR_CUDA;
}

/* Checks the type of pointer
 *
 * @param dest   One pointer to check
 * @param source Another pointer to check
 */
bool service_cuda_check_bufs(char *dest, char *src)
{
    int res;
    CUmemorytype memType;

    res = cuPointerGetAttribute(&memType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, (CUdeviceptr)dest);
    if( memType == CU_MEMORYTYPE_DEVICE){
        return true;
    }
    res = cuPointerGetAttribute(&memType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, (CUdeviceptr)src);
    if( memType == CU_MEMORYTYPE_DEVICE){
        return true;
    }
    /* Assuming it is a host pointer for all other situations */
    return false;
}

/*
 * Need intermediate cuMemcpy function so we can check the return code
 * of the call.  If we see an error, abort as there is no recovery at
 * this point.
 */
void *service_cuda_memcpy(void *dest, void *src, size_t size)
{
    int res;
    res = cuMemcpy((CUdeviceptr)dest, (CUdeviceptr)src, size);
    if (res != CUDA_SUCCESS) {
        service_output(0, "CUDA: Error in cuMemcpy: res=%d, dest=%p, src=%p, size=%d",
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
void *service_cuda_memmove(void *dest, void *src, size_t size)
{
    CUdeviceptr tmp;
    int res;

    res = cuMemAlloc(&tmp,size);
    res = cuMemcpy(tmp, (CUdeviceptr) src, size);
    if(res != CUDA_SUCCESS){
        service_output(0, "CUDA: memmove-Error in cuMemcpy: res=%d, dest=%p, src=%p, size=%d",
                    res, (void *)tmp, src, (int)size);
        abort();
    }
    res = cuMemcpy((CUdeviceptr) dest, tmp, size);
    if(res != CUDA_SUCCESS){
        service_output(0, "CUDA: memmove-Error in cuMemcpy: res=%d, dest=%p, src=%p, size=%d",
                    res, dest, (void *)tmp, (int)size);
        abort();
    }
    cuMemFree(tmp);
    return dest;
}

/**
 * This function gets called once to check if the program is running in a cuda
 * environment. 
 */
static void service_cuda_support_init(void)
{
    int id;
    CUresult res;
    CUcontext cuContext;

    if (initialized) {
        return;
    }

    /* Set different levels of verbosity in the cuda related code. */
    id = ccs_mca_base_param_reg_int_name("opal", "cuda_verbose",
                                     "Set level of opal cuda verbosity",
                                     false, false, 0, &service_cuda_verbose);
    service_cuda_output = service_output_open(NULL);
    service_output_set_verbosity(service_cuda_output, service_cuda_verbose);

    /* Check to see if this process is running in a CUDA context.  If so,
     * all is good.  Currently, just print out a message in verbose mode
     * to help with debugging. */
    res = cuCtxGetCurrent(&cuContext);
    if (CUDA_SUCCESS != res) {
        service_output_verbose(10, service_cuda_output,
                            "CUDA: cuCtxGetCurrent failed, CUDA device pointers will not work");
    } else {
        service_output_verbose(10, service_cuda_output,
                            "CUDA: cuCtxGetCurrent succeeded, CUDA device pointers will work");
    }

    initialized = true;
}
