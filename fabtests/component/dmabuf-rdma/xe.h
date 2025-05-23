/*
 * Copyright (c) 2021-2022 Intel Corporation.  All rights reserved.
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
 * SOFTWARE.
 */

#ifndef _DMABUF_RDMA_TESTS_XE_H_
#define _DMABUF_RDMA_TESTS_XE_H_

#include <stdint.h>
#include "hmem.h"

#define MAX_GPUS	(8)

/*
 * Buffer location and method of allocation
 */
enum {
	MALLOC,	/* Host memory allocated via malloc and alike */
	HOST,	/* Host memory allocated via zeMemAllocHost */
	DEVICE,	/* Device memory allocated via zeMemAllocDevice */
	SHARED	/* Shared memory allocated via zeMemAllocShared */
};

/*
 * All information related to a buffer allocated via oneAPI L0 API.
 */
struct xe_buf {
	void			*buf;
	void			*base;
	uint64_t		offset;
	size_t			size;
	int			location;
};

/*
 * Alloctaed a buffer from specified location, on the speficied GPU if
 * applicable. The xe_buf output is optional, can pass in NULL if the
 * information is not needed.
 */
void	*alloc_buf(size_t page_size, size_t size, int where, int gpu,
		      struct xe_buf *xe_buf);

/*
 * Get the dma-buf fd associated with the buffer allocated with the hmem
 * functions. Return -1 if it's not a dma-buf object.
 */
int	get_buf_fd(void *buf);

/*
 * Show the fields of the xe_buf structure.
 */
void	show_buf(struct xe_buf *buf);

/*
 * Free the buffer allocated by alloc_buf.
 */
void	free_buf(void *buf, int where);

/*
 * Like memset(). Use hmem to access device memory.
 */
void	set_buf(void *buf, char c, size_t size, int location, int gpu);

/*
 * Like memcpy(). Use oneAPI L0 to access device memory.
 */
void	copy_buf(void *dst, void *src, size_t size, int gpu);


/*
 * Registry for MOFED peer-mem plug-in
 */
int	dmabuf_reg_open(void);
void	dmabuf_reg_close(void);
int	dmabuf_reg_add(uint64_t base, uint64_t size, int fd);
void	dmabuf_reg_remove(uint64_t addr);

#endif /* _DMABUF_RDMA_TESTS_XE_H_ */

