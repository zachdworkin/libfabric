/*
 * Copyright (c) Intel Corporation. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
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

#include "xxx.h"

static int xxx_wait_open(struct fid_fabric *fabric_fid,
			 struct fi_wait_attr *attr,
			 struct fid_wait **waitset)
{
        return fi_no_wait_open(fabric_fid, attr, waitset);
}

static struct fi_ops_fabric xxx_fabric_ops = {
	.size = sizeof(struct fi_ops_fabric),
	.domain = xxx_domain_open,
	.passive_ep = xxx_passive_ep,
	.eq_open = xxx_eq_create,
	.wait_open = xxx_wait_open,
	.trywait = xxx_trywait
};

static int xxx_fabric_close(fid_t fid)
{
        return -FI_ENOSYS;
}

static int xxx_bind(struct fid *fid, struct fid *bfid, uint64_t flags)
{
        return fi_no_bind(fid, bfid, flags);
}

static int xxx_control(struct fid *fid, int command, void *arg)
{
        return fi_no_control(fid, command, arg);
}

static int xxx_ops_open(struct fid *fid, const char *name, uint64_t flags,
                        void **ops, void *context)
{
        return fi_no_ops_open(fid, name, flags, ops, context);
}

static struct fi_ops xxx_fabric_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_fabric_close,
	.bind = xxx_bind,
	.control = xxx_control,
	.ops_open = xxx_ops_open,
        .tostr = fi_no_tostr,
        .ops_set =  fi_no_ops_set
};

int xxx_fabric(struct fi_fabric_attr *attr, struct fid_fabric **fabric,
	       void *context)
{
        return -FI_ENOSYS;
}

