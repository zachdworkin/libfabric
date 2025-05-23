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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "shared.h"
#include "hmem.h"
#include "util.h"
#include "xe.h"

extern int buf_location;

void show_buf(struct xe_buf *buf)
{
	printf("Allocation: buf %p alloc_base %p alloc_size %ld offset 0x%lx\n",
		buf->buf, buf->base, buf->size, buf->offset);
}

int get_buf_fd(void *buf)
{
	void *ipc;

	memset(&ipc, 0, sizeof(ipc));
	CHECK_ERROR(ft_hmem_get_ipc_handle(opts.iface, buf, &ipc));

	return (int)*(uint64_t *)&ipc;

err_out:
	return -1;
}

void *alloc_buf(size_t page_size, size_t size, int where, int gpu,
		   struct xe_buf *xe_buf)
{
	void *buf = NULL;
	ze_device_handle_t alloc_dev = 0;
	void *alloc_base;
	size_t alloc_size;

	switch (where) {
	  case MALLOC:
		posix_memalign(&buf, page_size, size);
		alloc_base = buf;
		alloc_size = size;
		break;
	  case HOST:
		EXIT_ON_ERROR(ft_hmem_alloc_host(&buf, size));
		break;
	  case DEVICE:
		// Fall through
	  default:
		EXIT_ON_ERROR(ft_hmem_alloc(iface, gpu, &buf, size));
		break;
	}

	if (where != MALLOC) {
		EXIT_ON_ERROR(ft_hmem_get_address_range(opts.iface, gpu,
						        &buf, &alloc_base,
						        &alloc_size));
		if (ft_check_opts(FT_OPT_REG_DMABUF_MR))
			EXIT_ON_ERROR(dmabuf_reg_add((uintptr_t)alloc_base,
						     alloc_size,
						     xe_get_buf_fd(buf)));
	}

	if (xe_buf) {
		xe_buf->buf = buf;
		xe_buf->base = alloc_base;
		xe_buf->size = alloc_size;
		xe_buf->offset = (char *)buf - (char *)alloc_base;
		xe_buf->location = where;
		xe_show_buf(xe_buf);
	}
	return buf;
}

void free_buf(void *buf, int where)
{
	if (where == MALLOC) {
		free(buf);
	} else {
		if (ft_check_opts(FT_OPT_REG_DMABUF_MR))
			dmabuf_reg_remove((uint64_t)buf);
		CHECK_ERROR(ft_hmem_free(opts.iface, buf));
	}
err_out:
	return;
}

void set_buf(void *buf, char c, size_t size, int location, int gpu)
{
	if (location == MALLOC)
		memset(buf, c, size);
	else
		EXIT_ON_ERROR(ft_hmem_memset(opts.iface, gpu, buf,
				  (int)c, size));
}

void copy_buf(void *dst, void *src, size_t size, int gpu)
{
	EXIT_ON_ERROR(ft_hmem_copy_to(opts.iface, gpu, dst, src, size));
}
