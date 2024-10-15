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
 *      - Redistributions in binary form must reqroduce the above
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

static ssize_t xxx_eq_read(struct fid_eq *eq_fid, uint32_t *event,
			   void *buf, size_t len, uint64_t flags)
{
        return fi_no_eq_read(eq_fid, event, buf, len, flags);
}

static inline ssize_t
fi_eq_read(struct fid_eq *eq, uint32_t *event, void *buf,
	   size_t len, uint64_t flags)
{
	return fi_no_eq_read(eq, event, buf, len, flags);
}

static inline ssize_t xxx_eq_readerr(struct fid_eq *eq,
                                     struct fi_eq_err_entry *buf,
                                     uint64_t flags)
{
	return fi_no_eq_readerr(eq, buf, flags);
}

static inline ssize_t xxx_eq_write(struct fid_eq *eq, uint32_t event,
                                   const void *buf, size_t len, uint64_t flags)
{
	return fi_no_eq_write(eq, event, buf, len, flags);
}

static inline ssize_t xxx_eq_sread(struct fid_eq *eq, uint32_t *event,
                                   void *buf, size_t len, int timeout,
                                   uint64_t flags)
{
	return fi_no_eq_sread(eq, event, buf, len, timeout, flags);
}

static inline const char * xxx_eq_strerror(struct fid_eq *eq, int prov_errno,
                                           const void *err_data, char *buf,
                                           size_t len)
{
	return fi_no_eq_strerror(eq, prov_errno, err_data, buf, len);
}

static struct fi_ops_eq xxx_eq_ops = {
	.size = sizeof(struct fi_ops_eq),
	.read = xxx_eq_read,
	.readerr = ofi_eq_readerr,
	.sread = ofi_eq_sread,
	.write = ofi_eq_write,
	.strerror = ofi_eq_strerror,
};

static int xxx_eq_close(struct fid *eq_fid)
{
        return fi_no_close(eq_fid);
}

static int xxx_eq_bind(struct fid *eq_fid, struct fid *bfid, uint64_t flags)
{
        return fi_no_bind(eq_fid, bfid, flags);
}

static int xxx_eq_ctrl(struct fid *eq_fid, int command, void *arg)
{
        return fi_no_control(eq_fid, command, arg);
}

static int xxx_ops_open(struct fid *fid, const char *name, uint64_t flags,
                        void **ops, void *context)
{
        return fi_no_ops_open(fid, name, flags, ops, context);
}

static struct fi_ops xxx_eq_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_eq_close,
	.bind = xxx_eq_bind,
	.control = xxx_eq_ctrl,
	.ops_open = xxx_ops_open,
        .tostr = fi_no_tostr,
        .ops_set =  fi_no_ops_set
};

int xxx_eq_create(struct fid_fabric *fabric_fid, struct fi_eq_attr *attr,
		  struct fid_eq **eq_fid, void *context)
{
        return fi_no_eq_open(fabric_fid, attr, eq_fid, context);
}