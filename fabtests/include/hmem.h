/*
 * Copyright (c) 2020 Intel Corporation.  All rights reserved.
 *
 * This software is available to you under the BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 */

#ifndef _HMEM_H_
#define _HMEM_H_
#if HAVE_CONFIG_H
	#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_errno.h>

#if HAVE_ZE
#include <level_zero/ze_api.h>
extern struct libze_ops {
	ze_result_t (*zeInit)(ze_init_flags_t flags);
	ze_result_t (*zeDriverGet)(uint32_t *pCount,
				   ze_driver_handle_t *phDrivers);
	ze_result_t (*zeDriverGetExtensionFunctionAddress)(ze_driver_handle_t hDriver,
			const char *name, void **ppFunctionAddress);
	ze_result_t (*zeDeviceGet)(ze_driver_handle_t hDriver,
				   uint32_t *pCount,
				   ze_device_handle_t *phDevices);
	ze_result_t (*zeDeviceGetProperties)(ze_device_handle_t hDevice,
			ze_device_properties_t *pDeviceProperties);
	ze_result_t (*zeDeviceGetSubDevices)(ze_device_handle_t hDevice,
					     uint32_t *pCount,
					     ze_device_handle_t *phSubdevices);
	ze_result_t (*zeDeviceGetCommandQueueGroupProperties)(ze_device_handle_t hDevice,
			uint32_t *pCount,
			ze_command_queue_group_properties_t *pCommandQueueGroupProperties);
	ze_result_t (*zeDeviceCanAccessPeer)(ze_device_handle_t hDevice,
					     ze_device_handle_t hPeerDevice,
					     ze_bool_t *value);
	ze_result_t (*zeContextCreate)(ze_driver_handle_t hDriver,
				       const ze_context_desc_t *desc,
				       ze_context_handle_t *phContext);
	ze_result_t (*zeContextDestroy)(ze_context_handle_t hContext);
	ze_result_t (*zeCommandQueueCreate)(ze_context_handle_t hContext,
					    ze_device_handle_t hDevice,
					    const ze_command_queue_desc_t *desc,
					    ze_command_queue_handle_t *phCommandQueue);
	ze_result_t (*zeCommandQueueDestroy)(ze_command_queue_handle_t hCommandQueue);
	ze_result_t (*zeCommandQueueExecuteCommandLists)(
					ze_command_queue_handle_t hCommandQueue,
					uint32_t numCommandLists,
					ze_command_list_handle_t *phCommandLists,
					ze_fence_handle_t hFence);
	ze_result_t (*zeCommandQueueSynchronize)(ze_command_queue_handle_t hCommandQueue,
			uint64_t timeout);
	ze_result_t (*zeCommandListCreate)(ze_context_handle_t hContext,
					   ze_device_handle_t hDevice,
					   const ze_command_list_desc_t *desc,
					   ze_command_list_handle_t *phCommandList);
	ze_result_t (*zeCommandListCreateImmediate)(ze_context_handle_t hContext,
				ze_device_handle_t hDevice,
				const ze_command_queue_desc_t *altdesc,
				ze_command_list_handle_t *phCommandList);
	ze_result_t (*zeCommandListDestroy)(ze_command_list_handle_t hCommandList);
	ze_result_t (*zeCommandListClose)(ze_command_list_handle_t hCommandList);
	ze_result_t (*zeCommandListReset)(ze_command_list_handle_t hCommandList);
	ze_result_t (*zeCommandListAppendMemoryCopy)(
				ze_command_list_handle_t hCommandList,
				void *dstptr, const void *srcptr, size_t size,
				ze_event_handle_t hSignalEvent,
				uint32_t numWaitEvents,
				ze_event_handle_t *phWaitEvents);
	ze_result_t (*zeCommandListAppendMemoryFill)(
				ze_command_list_handle_t hCommandList,
				void *ptr, const void *pattern,
				size_t pattern_size, size_t size,
				ze_event_handle_t hSignalEvent,
				uint32_t numWaitEvents,
				ze_event_handle_t *phWaitEvents);
	ze_result_t (*zeMemAllocHost)(ze_context_handle_t hContext,
			const ze_host_mem_alloc_desc_t *host_desc,
			size_t size, size_t alignment, void **pptr);
	ze_result_t (*zeMemAllocDevice)(
				ze_context_handle_t hContext,
				const ze_device_mem_alloc_desc_t *device_desc,
				size_t size, size_t alignment, ze_device_handle_t hDevice,
				void *pptr);
	ze_result_t (*zeMemAllocShared)(ze_context_handle_t hContext,
			const ze_device_mem_alloc_desc_t *device_desc,
			const ze_host_mem_alloc_desc_t *host_desc,
			size_t size, size_t alignment,
			ze_device_handle_t hDevice, void **pptr);
	ze_result_t (*zeMemGetAllocProperties)(ze_context_handle_t hContext,
			const void *ptr,
			ze_memory_allocation_properties_t *pMemAllocProperties,
			ze_device_handle_t *phDevice);
	ze_result_t (*zeMemGetAddressRange)(ze_context_handle_t hContext,
			const void *ptr, void **pBase, size_t *pSize);
	ze_result_t (*zeMemGetIpcHandle)(ze_context_handle_t hContext,
			const void *ptr, ze_ipc_mem_handle_t *pIpcHandle);
	ze_result_t (*zeMemFree)(ze_context_handle_t hContext, void *ptr);
} libze_ops;

int init_libze_ops(void);
# endif /* HAVE_ZE */

#if HAVE_CUDA_RUNTIME_H
#include <cuda_runtime.h>
#include <cuda.h>
extern struct cuda_ops {
	CUresult (*cuInit)(unsigned int flags);
	cudaError_t (*cudaMemcpy)(void *dst, const void *src, size_t count,
				  enum cudaMemcpyKind kind);
	cudaError_t (*cudaMemcpyAsync)(void *dst, const void *src, size_t count,
				       enum cudaMemcpyKind kind,
				       cudaStream_t stream);
	cudaError_t (*cudaMalloc)(void **ptr, size_t size);
	cudaError_t (*cudaMallocHost)(void **ptr, size_t size);
	cudaError_t (*cudaFree)(void *ptr);
	cudaError_t (*cudaFreeHost)(void *ptr);
	cudaError_t (*cudaMemset)(void *ptr, int value, size_t count);
	CUresult (*cuMemGetInfo)(size_t* free, size_t* total);
	const char *(*cudaGetErrorName)(cudaError_t error);
	const char *(*cudaGetErrorString)(cudaError_t error);
	cudaError_t (*cudaSetDevice)(int device);
	cudaError_t (*cudaDeviceSynchronize)(void);
	// cudaError_t (*cudaGetDeviceProperties)(cudaDeviceProp* prop,
	// 				       int device);
	cudaError_t (*cudaDeviceCanAccessPeer)(int *canAccessPeer,
					       int deviceId,
					       int peerDeviceId);
	cudaError_t (*cudaDeviceEnablePeerAccess)(int peerDeviceId,
						  unsigned int flags);
	CUresult (*cuDeviceGetCount)(int *count);
	CUresult (*cuPointerSetAttribute)(void *data,
					  CUpointer_attribute attribute,
					  CUdeviceptr ptr);
	CUresult (*cuGetErrorName)(CUresult error, const char** pStr);
	CUresult (*cuGetErrorString)(CUresult error, const char** pStr);
#if HAVE_CUDA_DMABUF
	CUresult (*cuMemGetHandleForAddressRange)(void* handle,
						  CUdeviceptr dptr, size_t size,
						  CUmemRangeHandleType handleType,
						  unsigned long long flags);
#endif /* HAVE_CUDA_DMABUF */
	CUresult (*cuDeviceGetAttribute)(int* pi,
					 CUdevice_attribute attrib, CUdevice dev);
	CUresult (*cuDeviceGet)(CUdevice* device, int ordinal);
	CUresult (*cuMemGetAddressRange)( CUdeviceptr* pbase,
					  size_t* psize, CUdeviceptr dptr);
	CUresult (*cuIpcGetMemHandle)(CUipcMemHandle* pHandle,
				      CUdeviceptr dptr);
	cudaError_t (*cudaIpcCloseMemHandle)(void *devPtr);
} cuda_ops;
int ft_init_cuda(void);
# endif /* HAVE_CUDA_RUNTIME_H */

int ft_ze_init(void);
int ft_ze_cleanup(void);
int ft_ze_copy(uint64_t device, void *dst, const void *src, size_t size);
int ft_ze_alloc(uint64_t device, void **buf, size_t size);
int ft_ze_alloc_host(void **buf, size_t size);
int ft_ze_free(void *buf);
int ft_ze_memset(uint64_t device, void *buf, int value, size_t size);
int ft_ze_get_address_range(void *dev, void **buf, void **base_addr,
			    size_t *base_length);
int ft_ze_get_ipc_handle(void *buf, void *ipc_handle);

static inline int ft_host_init(void)
{
	return FI_SUCCESS;
}

static inline int ft_host_cleanup(void)
{
	return FI_SUCCESS;
}

static inline int ft_host_alloc(uint64_t device, void **buffer, size_t size)
{
	*buffer = malloc(size);
	return !*buffer ? -FI_ENOMEM : FI_SUCCESS;
}

static inline int ft_host_free(void *buf)
{
	free(buf);
	return FI_SUCCESS;
}

static inline int ft_host_memset(uint64_t device, void *buf, int value,
				 size_t size)
{
	memset(buf, value, size);
	return FI_SUCCESS;
}

static inline int ft_host_memcpy(uint64_t device, void *dst, const void *src,
				 size_t size)
{
	memcpy(dst, src, size);
	return FI_SUCCESS;
}

int ft_default_alloc_host(void **buf, size_t size);
int ft_default_free_host(void *buf);

int ft_cuda_init(void);
int ft_cuda_cleanup(void);
int ft_cuda_alloc(uint64_t device, void **buf, size_t size);
int ft_cuda_alloc_host(void **buf, size_t size);
int ft_cuda_free(void *buf);
int ft_cuda_free_host(void *buf);
int ft_cuda_memset(uint64_t device, void *buf, int value, size_t size);
int ft_cuda_copy_to_hmem(uint64_t device, void *dst, const void *src,
			 size_t size);
int ft_cuda_copy_from_hmem(uint64_t device, void *dst, const void *src,
			   size_t size);
int ft_cuda_get_dmabuf_fd(void *buf, size_t len,
			  int *fd, uint64_t *offset);
int ft_cuda_get_address_range(void *dev, void **buf, void **alloc_base,
			      size_t *size);
int ft_cuda_get_ipc_handle(void *buf, void *ipc_handle);
int ft_rocr_init(void);
int ft_rocr_cleanup(void);
int ft_rocr_alloc(uint64_t device, void **buf, size_t size);
int ft_rocr_free(void *buf);
int ft_rocr_memset(uint64_t device, void *buf, int value, size_t size);
int ft_rocr_memcpy(uint64_t device, void *dst, const void *src, size_t size);

int ft_neuron_init(void);
int ft_neuron_cleanup(void);
int ft_neuron_alloc(uint64_t device, void **buf, size_t size);
int ft_neuron_free(void *buf);
int ft_neuron_memset(uint64_t device, void *buf, int value, size_t size);
int ft_neuron_memcpy_to_hmem(uint64_t device, void *dst, const void *src, size_t size);
int ft_neuron_memcpy_from_hmem(uint64_t device, void *dst, const void *src, size_t size);

int ft_synapseai_init(void);
int ft_synapseai_cleanup(void);
int ft_synapseai_alloc(uint64_t device, void **buf, size_t size);
int ft_synapseai_alloc_host(void **buf, size_t size);
int ft_synapseai_free(void *buf);
int ft_synapseai_free_host(void *buf);
int ft_synapseai_memset(uint64_t device, void *buf, int value, size_t size);
int ft_synapseai_copy_to_hmem(uint64_t device, void *dst, const void *src, size_t size);
int ft_synapseai_copy_from_hmem(uint64_t device, void *dst, const void *src, size_t size);
int ft_synapseai_get_dmabuf_fd(void *buf, size_t len, int *dmabuf_fd, uint64_t *dmabuf_offset);

int ft_hmem_init(enum fi_hmem_iface iface);
int ft_hmem_cleanup(enum fi_hmem_iface iface);
int ft_hmem_alloc(enum fi_hmem_iface iface, uint64_t device, void **buf,
		  size_t size);
int ft_hmem_alloc_host(enum fi_hmem_iface iface, void **buf, size_t size);
int ft_hmem_free(enum fi_hmem_iface iface, void *buf);
int ft_hmem_free_host(enum fi_hmem_iface iface, void *buf);
int ft_hmem_memset(enum fi_hmem_iface iface, uint64_t device, void *buf,
		   int value, size_t size);
int ft_hmem_copy_to(enum fi_hmem_iface iface, uint64_t device, void *dst,
		    const void *src, size_t size);
int ft_hmem_copy_from(enum fi_hmem_iface iface, uint64_t device, void *dst,
		      const void *src, size_t size);
int ft_hmem_get_dmabuf_fd(enum fi_hmem_iface iface,
			  void *buf, size_t len,
			  int *fd, uint64_t *offset);
int ft_hmem_no_get_dmabuf_fd(void *buf, size_t len,
			     int *fd, uint64_t *offset);
int ft_hmem_no_get_address_range(void *dev, void **buf, void **base,
				 size_t *size);
int ft_hmem_no_get_ipc_handle(void *buf, void *ipc_handle);
#endif /* _HMEM_H_ */
