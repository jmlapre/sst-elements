// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#if !defined(__dv)
#if defined(__cplusplus)
#define __dv(v) \
		= v
#else /* __cplusplus */
#define __dv(v)
#endif /* __cplusplus */
#endif /* !__dv */
#include "balar_consts.h"

extern "C"{

uint64_t cudaMallocSST(void **devPtr, size_t size);

// `addr` is passed by value
__host__ cudaError_t CUDARTAPI cudaMallocHostSST(void *addr, size_t size);

unsigned CUDARTAPI __cudaRegisterFatBinarySST(char file_name[BALAR_CUDA_MAX_FILE_NAME]);

void CUDARTAPI __cudaRegisterFunctionSST(
    uint64_t fatCubinHandle,
    uint64_t hostFun,
    char deviceFun[BALAR_CUDA_MAX_KERNEL_NAME]
);

__host__ cudaError_t CUDARTAPI cudaMemcpySST(uint64_t dst, uint64_t src, size_t count, enum cudaMemcpyKind kind, uint8_t *payload);

__host__ cudaError_t CUDARTAPI cudaMemcpy(void * dst, const void * src, size_t count, enum cudaMemcpyKind kind);

__host__ cudaError_t CUDARTAPI cudaMemcpyToSymbol(const char *symbol, const void *src, size_t count, size_t offset, enum cudaMemcpyKind kind);

__host__ cudaError_t CUDARTAPI cudaMemcpyFromSymbol(void *dst, const char *symbol, size_t count, size_t offset, enum cudaMemcpyKind kind);

__host__ cudaError_t CUDARTAPI cudaConfigureCallSST(dim3 gridDim, dim3 blockDim, size_t sharedMem, cudaStream_t stream );

__host__ cudaError_t CUDARTAPI cudaConfigureCall(dim3 gridDim, dim3 blockDim, size_t sharedMem, cudaStream_t stream );

__host__ cudaError_t CUDARTAPI cudaSetupArgumentSST(uint64_t arg, uint8_t value[BALAR_CUDA_MAX_ARG_SIZE], size_t size, size_t offset);

__host__ cudaError_t CUDARTAPI cudaLaunchSST(uint64_t func);

__host__ cudaError_t CUDARTAPI cudaFree(void *devPtr);


__host__ __cudart_builtin__ cudaError_t CUDARTAPI cudaGetLastError(void);

void __cudaRegisterVar(
		void **fatCubinHandle,
		char *hostVar, //pointer to...something
		char *deviceAddress, //name of variable
		const char *deviceName, //name of variable (same as above)
		int ext,
		int size,
		int constant,
		int global );

cudaError_t CUDARTAPI cudaOccupancyMaxActiveBlocksPerMultiprocessorWithFlags(
        int* numBlocks,
        const char *hostFunc,
        int blockSize,
        size_t dynamicSMemSize,
        unsigned int flags);

__host__ cudaError_t CUDARTAPI cudaThreadSynchronizeSST(void);

__host__ cudaError_t CUDARTAPI cudaMemset(void *mem, int c, size_t count);

__host__ cudaError_t CUDARTAPI cudaGetDeviceCount(int *count);

__host__ cudaError_t CUDARTAPI cudaSetDevice(int device);

void __cudaRegisterTexture(
    void **fatCubinHandle, const struct textureReference *hostVar,
    const void **deviceAddress, const char *deviceName, int dim, int norm,
    int ext);

__host__ cudaError_t CUDARTAPI cudaBindTexture(
    size_t *offset, const struct textureReference *texref, const void *devPtr,
    const struct cudaChannelFormatDesc *desc, size_t size);

__host__ cudaError_t CUDARTAPI
cudaGetDeviceProperties(struct cudaDeviceProp *prop, int device);

cudaError_t CUDARTAPI cudaSetDeviceFlagsSST(unsigned int flags);

__host__ cudaError_t CUDARTAPI cudaStreamCreate(cudaStream_t *stream);

__host__ cudaError_t CUDARTAPI cudaEventCreate(cudaEvent_t *event);

cudaError_t CUDARTAPI cudaEventCreateWithFlags(cudaEvent_t *event, unsigned int flags);

__host__ cudaError_t CUDARTAPI cudaEventRecord(cudaEvent_t event, cudaStream_t stream);

__host__ cudaError_t CUDARTAPI cudaMemcpyAsync(void *dst, const void *src,
                                               size_t count,
                                               enum cudaMemcpyKind kind,
                                               cudaStream_t stream);


__host__ cudaError_t CUDARTAPI cudaEventSynchronizeSST(cudaEvent_t event);

__host__ cudaError_t CUDARTAPI cudaEventElapsedTime(float *ms,
                                                    cudaEvent_t start,
                                                    cudaEvent_t end);

__host__ cudaError_t CUDARTAPI cudaStreamDestroy(cudaStream_t stream);

__host__ cudaError_t CUDARTAPI cudaStreamSynchronizeSST(cudaStream_t stream);

__host__ cudaError_t CUDARTAPI cudaEventDestroy(cudaEvent_t event);

__host__ cudaError_t CUDARTAPI cudaDeviceGetAttribute(int *value,
                                                      enum cudaDeviceAttr attr,
                                                      int device);

void SST_receive_mem_reply(unsigned core_id,  void* mem_req);

bool SST_gpu_core_cycle();

void SST_gpgpusim_numcores_equal_check(unsigned sst_numcores);

bool SST_gpgpusim_launch_blocking();

}

std::tuple<cudaError_t, size_t, unsigned> SST_cudaGetParamConfig(uint64_t hostFun, unsigned index);

