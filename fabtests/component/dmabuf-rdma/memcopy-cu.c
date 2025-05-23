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

/*
 *	memcopy-cu.c
 *
 *	This is a memory copy bandwidth test with buffers allocated via cuda
 *	functions.
 *
 *	Copy using memcpy() between buffers allocated with malloc():
 *
 *	    ./cu_memcopy -c memcpy M M
 *
 *	Copy using memcpy between buffers allocated with cudaHostAlloc():
 *
 *	    ./cu_memcopy -c memcpy H H
 *
 *	Copy using GPU between buffers allocated with cudaMalloc():
 *
 *	    ./cu_memcopy -c cuda D D
 *
 *	Copy using GPU from buffer allocated with cudaMalloc() to buffer
 *	allcated with cudaHostAlloc():
 *
 *	    ./cu_memcopy -c cuda D H
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), with
 *	cached command list:
 *
 *	    ./cu_memcopy -c cuda -C D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), with
 *	immediate command list:
 *
 *	    ./cu_memcopy -c cuda -i D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), using
 *	specified device (src=dst=0):
 *
 *	    ./cu_memcopy -c cuda -d 0 D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), using
 *	specified device (src=0, dst=1):
 *
 *	    ./cu_memcopy -c cuda -d 0 -D 1 D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), using
 *	specified command queue group:
 *
 *	    ./cu_memcopy -c cuda -G 2 D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), using
 *	specified devices, command queue group and engine index:
 *
 *	    ./cu_memcopy -c cuda -d 0 -D 1 -G 2 -I 1 D D
 *
 *	Copy using GPU between buffers allocated with cudaMalloc(), using
 *	specified devices, 2nd device's command queue group and engine index:
 *
 *	    ./cu_memcopy -c cuda -d 0 -D 1 -r -G 2 -I 1 D D
 *
 *	Copy using mmap + memcpy between buffers allocated with cudaMalloc(),
 *	mmap() is called before each memcpy():
 *
 *	    ./cu_memcopy -c mmap D D
 *
 *	Copy using mmap + memcpy between buffers allocated with cudaMalloc(),
 *	mmap() is called once (cached):
 *
 *	    ./cu_memcopy -c mmap -C D D
 *
 *	For more options:
 *
 *	    ./cu_memcopy -h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <setjmp.h>
#include "hmem.h"
#include "util.h"
#include "xe.h"

#define MALLOC	0
#define HOST	1
#define DEVICE	2
#define SHARED	3

#if HAVE_CUDA_RUNTIME_H
#include <cuda_runtime.h>
#include <cuda.h>

struct my_buf {
	int where;
	int dev_num;
	void *buf;
	size_t size;
	int fd;
	uint64_t dmabuf_offset;
	void *map;
	size_t map_size;
	void *mapped_buf;
};

const size_t gpu_page_size = 65536;
static int iterations = 1000;

#define CUDA		0
#define MEMCPY		1
#define MMAP		2

static int copy_method = CUDA;
static int async = 0;
static int cache_cmdl = 0;
static int cache_mmap = 0;
static int mmap_all = 1;
static int reverse = 0;
static int import = 0;
static unsigned int card_num = 0;
static unsigned int card_num2 = 0xFF;
static unsigned int ordinal = 0;
static unsigned int engine_index = 0;

#define MAX_MSG_SIZE	(64*1024*1024)

static jmp_buf env;

static int max_devices = 0;

void segv_handler(int signum)
{
	printf("Segmentation fault caught while accessing device buffer\n");
	longjmp(env, 1);
}

void show_device_properties(int device_num)
{
	// cudaDeviceProp props;

	// EXIT_ON_ERROR(cuda_ops.cudaGetDeviceProperties(&props, device_num));
	// printf("Device %d:\n\tname: %s, core_clock: %d, l2 cache size: %d, "
	//        "managed memory: %d\n",
	//        device_num, props.name, props.clockRate,
	//        props.l2CacheSize, props.managedMemory);
}

void init_gpu(void)
{
	int peer;
	EXIT_ON_ERROR(ft_cuda_init());
	EXIT_ON_ERROR(cuda_ops.cuInit(0));

	EXIT_ON_ERROR(cuda_ops.cuDeviceGetCount(&max_devices));

	show_device_properties(card_num);

	if (card_num2 != card_num)
		show_device_properties(card_num2);

	/* check peer access capability */
	if (card_num != card_num2) {
		EXIT_ON_ERROR(cuda_ops.cudaDeviceCanAccessPeer(&peer, card_num,
							card_num2));
		if (peer) {
			EXIT_ON_ERROR(cuda_ops.cudaSetDevice(card_num));
			EXIT_ON_ERROR(cuda_ops.cudaDeviceEnablePeerAccess(card_num2,
				0));
		} else {
			printf("Peer access from device %d to device %d is not "
			"supported\n", card_num, card_num2);
			exit(-1);
		}
		EXIT_ON_ERROR(cuda_ops.cudaDeviceCanAccessPeer(&peer, card_num2,
							card_num));
		if (peer) {
			EXIT_ON_ERROR(cuda_ops.cudaSetDevice(card_num2));
			EXIT_ON_ERROR(cuda_ops.cudaDeviceEnablePeerAccess(card_num,
				0));
		} else {
			printf("Peer access from device %d to device %d is not "
			       "supported\n", card_num2, card_num);
			exit(-1);
		}
	}
}

void alloc_buffer(struct my_buf *buf, size_t size, int dev)
{
	EXIT_ON_ERROR(cuda_ops.cudaSetDevice(dev));
	buf->dev_num = dev;
	switch (buf->where) {
	case HOST:
		EXIT_ON_ERROR(cuda_ops.cudaMallocHost(&buf->buf, size));
		break;
	case DEVICE:
		EXIT_ON_ERROR(cuda_ops.cudaMalloc(&buf->buf, size));
		break;
	case MALLOC:
		/* fallthrough */
	default:
		EXIT_ON_ERROR(posix_memalign(&buf->buf, 4096, size));
		if (import)

		break;
	}

	buf->size = size;
	buf->fd = -1;
	buf->map = NULL;
	buf->map_size = (size + 4095) & ~4095UL;

	if (buf->where == MALLOC)
		return;

	EXIT_ON_ERROR(ft_cuda_get_dmabuf_fd(buf->buf, size, &buf->fd,
					    &buf->dmabuf_offset));

	if (cache_mmap && copy_method == MMAP &&
	(buf->where == DEVICE || mmap_all)) {
		buf->map = mmap(NULL, buf->map_size,
				PROT_READ | PROT_WRITE,
				MAP_SHARED, buf->fd, 0);
		if (buf->map == MAP_FAILED) {
			perror("mmap");
			exit(-1);
		}
	}
	buf->mapped_buf = (char *) buf->map + buf->dmabuf_offset;
}

void free_buffer(struct my_buf *buf)
{
	if (buf->map)
		munmap(buf->map, buf->map_size);

	if (buf->fd != -1)
		close(buf->fd);

	switch(buf->where) {
	case HOST:
		CHECK_ERROR(cuda_ops.cudaFreeHost(buf->buf));
		break;
	case DEVICE:
		CHECK_ERROR(cuda_ops.cudaFree(buf->buf));
		break;
	case MALLOC:
		/* fallthrough */
	default:
		free(buf->buf);
		break;
	}
	return;
err_out:
	printf("Error freeing buffer\n");
}

void *get_buf_ptr(struct my_buf *buf, size_t size)
{
	if (buf->map)
		return buf->map;

	if (copy_method == MMAP && buf->where != MALLOC) {
		buf->map = mmap(NULL, buf->map_size, PROT_READ | PROT_WRITE,
				MAP_SHARED, buf->fd, 0);
		if (buf->map == MAP_FAILED) {
			perror("mmap");
			exit(-1);
		}
		return buf->map;
	}

	return buf->buf;
}


void put_buf_ptr(struct my_buf *buf)
{
	if (!cache_mmap && copy_method == MMAP && buf->where != MALLOC) {
		munmap(buf->map, buf->map_size);
		buf->map = NULL;
	}
}

void copy_buffer(struct my_buf *src_buf, struct my_buf *dst_buf, size_t size)
{
	switch (copy_method) {
	case CUDA:
		if (reverse)
			EXIT_ON_ERROR(cuda_ops.cudaSetDevice(dst_buf->dev_num));
		else
			EXIT_ON_ERROR(cuda_ops.cudaSetDevice(src_buf->dev_num));
		if (src_buf->where == HOST) {
			if (dst_buf->where == HOST) {
				if (async)
					EXIT_ON_ERROR(cuda_ops.cudaMemcpyAsync(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyHostToHost, 0));
				else
					EXIT_ON_ERROR(cuda_ops.cudaMemcpy(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyHostToHost));

			}
			if (dst_buf->where == DEVICE) {
				if (async)
					EXIT_ON_ERROR(cuda_ops.cudaMemcpyAsync(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyHostToDevice,
						0));
				else
					EXIT_ON_ERROR(cuda_ops.cudaMemcpy(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyHostToDevice));
			}
		}
		if (src_buf->where == DEVICE) {
			if (dst_buf->where == HOST) {
				if (async)
					EXIT_ON_ERROR(cuda_ops.cudaMemcpyAsync(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyDeviceToHost,
						0));
				else
					EXIT_ON_ERROR(cuda_ops.cudaMemcpy(
						dst_buf->buf, src_buf->buf,
						size, cudaMemcpyDeviceToHost));
			}
			if (dst_buf->where == DEVICE) {
				if (async)
					EXIT_ON_ERROR(cuda_ops.cudaMemcpyAsync(
						dst_buf->buf, src_buf->buf,
						size,
						cudaMemcpyDeviceToDevice,
						0));
				else
					EXIT_ON_ERROR(cuda_ops.cudaMemcpy(
						dst_buf->buf, src_buf->buf,
						size,
						cudaMemcpyDeviceToDevice));
			}
		}
		if (async)
			EXIT_ON_ERROR(cuda_ops.cudaDeviceSynchronize());
		break;
	case MMAP:
		memcpy(get_buf_ptr(dst_buf, size),
		get_buf_ptr(src_buf, size), size);
		put_buf_ptr(src_buf);
		put_buf_ptr(dst_buf);
		break;
	case MEMCPY:
		/* fallthrough */
	default:
		memcpy(dst_buf->buf, src_buf->buf, size);
		break;
	}
}

void fill_buffer(struct my_buf *buf, char c, size_t size)
{
	void *mapped;

	if (buf->where != DEVICE) {
		memset(buf->buf, c, size);
	} else {
		mapped = mmap(NULL, buf->map_size, PROT_READ | PROT_WRITE,
			      MAP_SHARED, buf->fd, 0);
		if (mapped == MAP_FAILED) {
			perror("mmap");
			exit(-1);
		}
		memset(mapped, c, size);
		munmap(mapped, buf->map_size);
	}
}

void check_buffer(struct my_buf *buf, char c, size_t size)
{
	char *p;
	int i;
	size_t errors = 0;

	if (buf->where != DEVICE) {
		p = buf->buf;
		for (i = 0; i< size; i++)
			if (p[i] != c) errors++;
	} else {
		p = mmap(NULL, buf->map_size, PROT_READ | PROT_WRITE,
			 MAP_SHARED, buf->fd, 0);
		if (p == MAP_FAILED) {
			perror("mmap");
			exit(-1);
		}
		for (i = 0; i< size; i++)
			if (p[i] != c) errors++;
		munmap(p, buf->map_size);
	}

	printf("%zd errors found\n", errors);
}

int str_to_location(char *s)
{
	if (!strncasecmp(s, "m", 1))
		return MALLOC;
	else if (!strncasecmp(s, "h", 1))
		return HOST;
	else if (!strncasecmp(s, "d", 1))
		return DEVICE;
	else
		return SHARED;
}

char *location_to_str(int location)
{
	if (location == MALLOC)
		return "Host Memory (malloc)";
	else if (location == HOST)
		return "Host Memory (ze)";
	else if (location == DEVICE)
		return "Device Memory";
	else
		return "Shared Memory";
}

void run_test(struct my_buf *src_buf, struct my_buf *dst_buf)
{
	size_t size;
	int i;
	double t1, t2;

	signal(SIGSEGV, segv_handler);

	if (setjmp(env))
		return;

	fill_buffer(src_buf, 'a', MAX_MSG_SIZE);
	for (size = 1; size <= MAX_MSG_SIZE; size <<= 1) {
		t1 = when();
		for (i = 0; i < iterations; i++)
			copy_buffer(src_buf, dst_buf, size);
		t2 = when();

		printf("%8ld (x%d):%12.2lfus%12.2lfMB/s\n", size, iterations,
			t2 - t1, size * iterations / ((t2-t1)));
	}
	printf("Verifying data ......\n");
	check_buffer(dst_buf, 'a', MAX_MSG_SIZE);
}

void usage(char *prog_name)
{
	printf("Usage: %s [<options>] <src> <dst>\n", prog_name);
	printf("Options:\n");
	printf("\t-n <iterations>     number of iterations to perform copy "
				      "operation for each message size\n");
	printf("\t-c <copy-method>    method used for copy operations, can be "
				      "'cuda' (default), 'memcpy', 'mmap', "
				      "and 'mmap-device'\n");
	printf("\t-i                  use async copies\n");
	printf("\t-d <device>         device to use (default: 0)\n");
	printf("\t-D <device2>        second device to use (default: the same "
				      "as the first device)\n");
	printf("\t-r                  reverse the direction by creating "
				      "command queue on the device with the "
				      "destination buffer\n");
	printf("\t-M                  import malloc'ed buffer into L0 before "
				      "the copy\n");
	printf("\t<src>               location of source buffer -- "
				      "'M':malloc, 'H':host, 'D':device, "
				      "'S':shared\n");
	printf("\t<dst>               location of destination buffer -- "
				      "'M':malloc, 'H':host, 'D':device, "
				      "'S':shared\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct my_buf src_buf = {
		.dev_num = 0,
		.where = MALLOC,
		.buf = NULL,
		.size = 0,
		.fd = -1,
		.map = NULL,
		.map_size = 0
	};
	struct my_buf dst_buf = {
		.dev_num = 0,
		.where = MALLOC,
		.buf = NULL,
		.size = 0,
		.fd = -1,
		.map = NULL,
		.map_size = 0
	};
	int c;

	while ((c = getopt(argc, argv, "n:c:Cid:D:G:I:rMh")) != -1)
	{
		switch (c) {
		case 'n':
			iterations = atoi(optarg);
			break;
		case 'c':
			if (!strcasecmp(optarg, "cuda"))
				copy_method = CUDA;
			else if (!strcasecmp(optarg, "memcpy"))
				copy_method = MEMCPY;
			else if (!strcasecmp(optarg, "mmap"))
				copy_method = MMAP, mmap_all = 1;
			else if (!strcasecmp(optarg, "mmap-device"))
				copy_method = MMAP, mmap_all = 0;
			else
				printf("Invalid copy method '%s', using "
				       "default (cuda).\n", optarg);
			break;
		case 'C':
			cache_cmdl = cache_mmap = 1;
			break;
		case 'i':
			async = 1;
			break;
		case 'd':
			card_num = atoi(optarg);
			break;
		case 'D':
			card_num2 = atoi(optarg);
			break;
		case 'G':
			ordinal = atoi(optarg);
			break;
		case 'I':
			engine_index = atoi(optarg);
			break;
		case 'r':
			reverse = 1;
			break;
		case 'M':
			import = 1;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	if (argc < optind + 2)
		usage(argv[0]);

	src_buf.where = str_to_location(argv[optind]);
	dst_buf.where = str_to_location(argv[optind + 1]);

	if (card_num2 == 0xFF || src_buf.where == HOST || dst_buf.where == HOST)
		card_num2 = card_num;

	init_gpu();

	alloc_buffer(&src_buf, MAX_MSG_SIZE, card_num);
	alloc_buffer(&dst_buf, MAX_MSG_SIZE, card_num2);

	printf("Copy from %s to %s, ",
		location_to_str(src_buf.where), location_to_str(dst_buf.where));

	if (copy_method == MEMCPY)
		printf("using memcpy\n");
	else if (copy_method == MMAP)
		printf("using mmap (%s) on %s\n",
			cache_mmap ? "cached" : "non-cached",
			mmap_all ? "all memory except malloc" :
			"device memory");
	else
		printf("using %s cuda copy (%s)\n",
			async ? "async" : "sync",
			cache_cmdl ? "cached" : "non-cached");

	printf("Import external pointers: %s\n", import ? "yes" : "no");

	run_test(&src_buf, &dst_buf);

	free_buffer(&src_buf);
	free_buffer(&dst_buf);
	return 0;
}

#endif /* HAVE_CUDA_RUNTIME_H */

