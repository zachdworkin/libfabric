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

static int xxx_query_collective(struct fid_domain *domain,
                                enum fi_collective_op coll,
                                struct fi_collective *attr, uint64_t flags)
{
        return fi_no_query_collective(domain, coll, attr, flags);
}

int xxx_query_atomic(struct fid_domain *domain, enum fi_datatype datatype,
		     enum fi_op op, struct fi_atomic_attr *attr, uint64_t flags)
{
	return fi_no_query_atomic(domain, datatype, op, attr, flags);
}

static struct fi_ops_domain xxx_domain_ops = {
	.size = sizeof(struct fi_ops_domain),
	.av_open = xxx_av_open,
	.cq_open = xxx_cq_open,
	.endpoint = xxx_endpoint,
	.scalable_ep = fi_no_scalable_ep,
	.cntr_open = xxx_cntr_open,
	.poll_open = xxx_poll_open,
	.stx_ctx = fi_no_stx_context,
	.srx_ctx = xxx_srx_context,
	.query_atomic = xxx_query_atomic,
	.query_collective = fi_no_query_collective,
};

static int xxx_domain_close(fid_t fid)
{
	int ret;
	struct xxx_domain *domain;

	domain = container_of(fid, struct xxx_domain,
                              util_domain.domain_fid.fid);

	ret = ofi_domain_close(&domain->util_domain);
	if (ret)
		return ret;

	free(domain);
	return 0;
}

static struct fi_ops xxx_domain_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_domain_close,
	.bind = fi_no_bind,
	.control = fi_no_control,
	.ops_open = fi_no_ops_open,
};

static int xxx_mr_reg(struct fid *fid, const void *buf, size_t len,
	              uint64_t access, uint64_t offset, uint64_t requested_key,
                      uint64_t flags, struct fid_mr **mr_fid, void *context)
{
        return ofi_mr_reg(fid, buf, len, access, offset, requested_key, flags,
                          mr_fid, context);
}

static int xxx_mr_regv(struct fid *fid, const struct iovec *iov, size_t count,
                       uint64_t access, uint64_t offset, uint64_t requested_key,
                       uint64_t flags, struct fid_mr **mr_fid, void *context)
{
        return ofi_mr_regv(fid, iov, count, access, offset, requested_key,
                           flags, mr_fid, context);
}

static int xxx_mr_regattr(struct fid *fid, const struct fi_mr_attr *attr,
		          uint64_t flags, struct fid_mr **mr_fid)
{
        return ofi_mr_regattr(fid, attr, flags, mr_fid);
}

static struct fi_ops_mr xxx_mr_ops = {
	.size = sizeof(struct fi_ops_mr),
	.reg = xxx_mr_reg,
	.regv = xxx_mr_regv,
	.regattr = xxx_mr_regattr,
};

int xxx_domain_open(struct fid_fabric *fabric, struct fi_info *info,
		struct fid_domain **domain, void *context)
{
	int ret;
	struct xxx_domain *xxx_domain;

	ret = ofi_prov_check_info(&xxx_util_prov, fabric->api_version, info);
	if (ret)
		return ret;

	xxx_domain = calloc(1, sizeof(*xxx_domain));
	if (!xxx_domain)
		return -FI_ENOMEM;

	ret = ofi_domain_init(fabric, info, &xxx_domain->util_domain, context,
			      OFI_LOCK_SPINLOCK);
	if (ret) {
		free(xxx_domain);
		return ret;
	}

	*domain = &xxx_domain->util_domain.domain_fid;
	(*domain)->fid.ops = &xxx_domain_fi_ops;
	(*domain)->ops = &xxx_domain_ops;
	(*domain)->mr = &xxx_mr_ops;

	return 0;
}
