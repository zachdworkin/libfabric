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

static inline int xxx_pep_setname(fid_t fid, void *addr, size_t addrlen)
{
        return fi_no_setname(fid, addr, addrlen);
}

static inline int xxx_pep_getname(fid_t fid, void *addr, size_t *addrlen)
{
        return fi_no_getname(fid, addr, addrlen);
}

static inline int xxx_pep_getpeer(struct fid_ep *ep, void *addr,
                                  size_t *addrlen)
{
        return fi_no_getpeer(ep, addr, addrlen);
}

static inline int xxx_pep_listen(struct fid_pep *pep)
{
        return fi_no_listen(pep);
}

static inline int xxx_pep_connect(struct fid_ep *ep, const void *addr,
                                  const void *param, size_t paramlen)
{
        return fi_no_connect(ep, addr, param, paramlen);
}

static inline int xxx_pep_accept(struct fid_ep *ep, const void *param,
                                 size_t paramlen)
{
        return fi_no_accept(ep, param, paramlen);
}

static inline int xxx_pep_reject(struct fid_pep *pep, fid_t handle,
                                 const void *param, size_t paramlen)
{
        return fi_no_reject(pep, handle, param, paramlen);
}

static inline int xxx_pep_shutdown(struct fid_ep *ep, uint64_t flags)
{
        return fi_no_shutdown(ep, flags);
}

static inline int xxx_pep_join(struct fid_ep *ep, const void *addr,
                               uint64_t flags, struct fid_mc **mc,
                               void *context)
{
        return fi_no_join(ep, addr, flags, mc, context);
}

static struct fi_ops_cm xxx_pep_cm_ops = {
	.size = sizeof(struct fi_ops_cm),
	.setname = xxx_pep_setname,
	.getname = xxx_pep_getname,
	.getpeer = xxx_pep_getpeer,
	.connect = xxx_pep_connect,
	.listen = xxx_pep_listen,
	.accept = xxx_pep_accept,
	.reject = xxx_pep_reject,
	.shutdown = xxx_pep_shutdown,
	.join = xxx_pep_join,
};

static int xxx_pep_close(struct fid *pep_fid)
{
        return fi_no_close(pep_fid);
}

static int xxx_pep_bind(struct fid *pep_fid, struct fid *bfid, uint64_t flags)
{
        return fi_no_bind(pep_fid, bfid, flags);
}

static int xxx_pep_ctrl(struct fid *pep_fid, int command, void *arg)
{
        return fi_no_control(pep_fid, command, arg);
}

static int xxx_ops_open(struct fid *fid, const char *name, uint64_t flags,
                        void **ops, void *context)
{
        return fi_no_ops_open(fid, name, flags, ops, context);
}

static struct fi_ops xxx_pep_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_pep_close,
	.bind = xxx_pep_bind,
	.control = xxx_pep_ctrl,
	.ops_open = xxx_ops_open,
        .tostr = fi_no_tostr,
        .ops_set =  fi_no_ops_set
};

int xxx_passive_ep(struct fid_fabric *fabric, struct fi_info *info,
		   struct fid_pep **pep, void *context)
{
        return fi_no_passive_ep(fabric, info, pep, context);
}