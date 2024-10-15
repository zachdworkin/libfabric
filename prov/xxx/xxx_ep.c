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

static int xxx_setname(fid_t fid, void *addr, size_t addrlen)
{
        return fi_no_setname(fid, addr, addrlen);
}

static int xxx_getname(fid_t fid, void *addr, size_t *addrlen)
{
        return fi_no_getname(fid, addr, addrlen);
}

static int xxx_getpeer(struct fid_ep *ep, void *addr, size_t *addrlen)
{
        return fi_no_getpeer(ep, addr, addrlen);
}

static int xxx_connect(struct fid_ep *ep, const void *addr, const void *param,
                size_t paramlen)
{
        return fi_no_connect(ep, addr, param, paramlen);
}

static int xxx_listen(struct fid_pep *pep)
{
        return fi_no_listen(pep);
}

static int xxx_accept(struct fid_ep *ep, const void *param, size_t paramlen)
{
        return fi_no_accept(ep, param, paramlen);
}

static int xxx_reject(struct fid_pep *pep, fid_t handle, const void *param,
               size_t paramlen)
{
        return fi_no_reject(pep, handle, param, paramlen);
}

static int xxx_shutdown(struct fid_ep *ep, uint64_t flags)
{
        return fi_no_shutdown(ep, flags);
}

static struct fi_ops_cm xxx_cm_ops = {
	.size = sizeof(struct fi_ops_cm),
	.setname = xxx_setname,
	.getname = xxx_getname,
	.getpeer = xxx_getpeer,
	.connect = xxx_connect,
	.listen = xxx_listen,
	.accept = xxx_accept,
	.reject = xxx_reject,
	.shutdown = xxx_shutdown,
};

static ssize_t xxx_ep_cancel(fid_t ep_fid, void *context)
{
        return -FI_ENOSYS;
}

int xxx_ep_getopt(fid_t fid, int level, int optname, void *optval,
		  size_t *optlen)
{
        return -FI_ENOSYS;
}

int xxx_ep_setopt(fid_t fid, int level, int optname, const void *optval,
		  size_t optlen)
{
        return -FI_ENOSYS;
}

int xxx_tx_ctx(struct fid_ep *sep, int index, struct fi_tx_attr *attr,
               struct fid_ep **tx_ep, void *context)
{
        return fi_no_tx_ctx(sep, index, attr, tx_ep, context);
}

int xxx_rx_ctx(struct fid_ep *sep, int index, struct fi_tx_attr *attr,
               struct fid_ep **tx_ep, void *context)
{
        return fi_no_rx_ctx(sep, index, attr, tx_ep, context);
}

ssize_t xxx_rx_size_left(struct fid_ep *ep)
{
	return fi_no_rx_size_left(ep);
}

ssize_t xxx_tx_size_left(struct fid_ep *ep)
{
	return fi_no_tx_size_left(ep);
}

static struct fi_ops_ep xxx_ep_ops = {
	.size = sizeof(struct fi_ops_ep),
	.cancel = xxx_ep_cancel,
	.getopt = xxx_ep_getopt,
	.setopt = xxx_ep_setopt,
	.tx_ctx = xxx_tx_ctx,
	.rx_ctx = xxx_rx_ctx,
	.rx_size_left = xxx_rx_size_left,
	.tx_size_left = xxx_tx_size_left,
};

static int xxx_ep_close(struct fid *ep_fid)
{
        return fi_no_close(ep_fid);
}

static int xxx_ep_bind(struct fid *ep_fid, struct fid *bfid, uint64_t flags)
{
        return fi_no_bind(ep_fid, bfid, flags);
}

static int xxx_ep_ctrl(struct fid *ep_fid, int command, void *arg)
{
        return fi_no_control(ep_fid, command, arg);
}

static int xxx_ops_open(struct fid *fid, const char *name, uint64_t flags,
                        void **ops, void *context)
{
        return fi_no_ops_open(fid, name, flags, ops, context);
}

static struct fi_ops xxx_ep_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_ep_close,
	.bind = xxx_ep_bind,
	.control = xxx_ep_ctrl,
	.ops_open = xxx_ops_open,
        .tostr = fi_no_tostr,
        .ops_set =  fi_no_ops_set
};

int xxx_endpoint(struct fid_domain *domain, struct fi_info *info,
	         struct fid_ep **ep_fid, void *context)
{
        return fi_no_endpoint(domain, info, ep_fid, context);
}

int xxx_sep_open(struct fid_domain *domain, struct fi_info *info,
                 struct fid_ep **sep, void *context)
{
        return fi_no_scalable_ep(domain, info, sep, context);
}