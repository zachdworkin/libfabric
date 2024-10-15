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


static int xxx_av_close(struct fid *av_fid)
{
        return fi_no_close(av_fid);
}

static int xxx_av_bind(struct fid *av_fid, struct fid *bfid, uint64_t flags)
{
        return fi_no_bind(av_fid, bfid, flags);
}

static int xxx_av_ctrl(struct fid *av_fid, int command, void *arg)
{
        return fi_no_control(av_fid, command, arg);
}

static int xxx_ops_open(struct fid *av_fid, const char *name, uint64_t flags,
                        void **ops, void *context)
{
        return fi_no_ops_open(av_fid, name, flags, ops, context);
}

static struct fi_ops xxx_ep_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = xxx_av_close,
	.bind = xxx_av_bind,
	.control = xxx_av_ctrl,
	.ops_open = xxx_ops_open,
        .tostr = fi_no_tostr,
        .ops_set =  fi_no_ops_set
};

static int xxx_av_insert(struct fid_av *av_fid, const void *addr, size_t count,
			 fi_addr_t *fi_addr, uint64_t flags, void *context)
{
        return fi_no_av_insert(av_fid, addr, count, fi_addr, flags, context);
}

static xxx_av_insertsvc(struct fid_av *av_fid, const char *node,
                        const char *service, fi_addr_t *fi_addr,
                        uint64_t flags, void *context)
{
        return fi_no_av_insertsvc(av_fid, node, service, fi_addr, flags,
                                  context);
}

static int xxx_av_insertsym(struct fid_av *av_fid, const char *node,
                            size_t nodecnt, const char *service, size_t svccnt,
                            fi_addr_t *fi_addr, uint64_t flags, void *context)
{
        return fi_no_av_insertsym(av_fid, node, nodecnt, service, svccnt,
                                  fi_addr, flags, context);
}

static int xxx_av_remove(struct fid_av *av_fid, fi_addr_t *fi_addr,
                         size_t count, uint64_t flags)
{
        return fi_no_av_remove(av_fid, fi_addr, count, flags);
}

static int xxx_av_lookup(struct fid_av *av_fid, fi_addr_t fi_addr, void *addr,
                         size_t *addrlen)
{
        return fi_no_av_lookup(av_fid, fi_addr, addr, addrlen);
}

static const char * xxx_av_straddr(struct fid_av *av_fid, const void *addr,
                                   char *buf, size_t *len)
{
        return fi_no_av_straddr(av_fid, addr, buf, len);
}

static int xxx_av_set(struct fid_av *av_fid, struct fi_av_set_attr *attr,
                      struct fid_av_set **set, void * context)
{
        return fi_no_av_set(av_fid, attr, set, context);
}

static int xxx_av_insert_auth_key(struct fid_av *av_fid, const void *auth_key,
                                  size_t auth_key_size, fi_addr_t *fi_addr,
                                  uint64_t flags)
{
        // define a enosys interface?
        return -FI_ENOSYS;
}

static int xxx_av_lookup_auth_key(struct fid_av *av_fid, fi_addr_t addr,
                                  void *auth_key, size_t *auth_key_size)
{
         // define a enosys interface?
        return -FI_ENOSYS;
}

static int xxx_av_set_user_id(struct fid_av *av_fid, fi_addr_t fi_addr,
                              fi_addr_t user_id, uint64_t flags)
{
         // define a enosys interface?
        return -FI_ENOSYS;
}

static struct fi_ops_av xxx_av_ops = {
	.size = sizeof(struct fi_ops_av),
	.insert = xxx_av_insert,
	.insertsvc = xxx_av_insertsvc,
	.insertsym = xxx_av_insertsym,
	.remove = xxx_av_remove,
	.lookup = xxx_av_lookup,
	.straddr = xxx_av_straddr,
        .av_set = xxx_av_set,
        .insert_auth_key = xxx_av_insert_auth_key,
        .lookup_auth_key = xxx_av_lookup_auth_key,
        .set_user_id = xxx_av_set_user_id
};

int xxx_av_open(struct fid_domain *domain_fid, struct fi_av_attr *attr,
		struct fid_av **fid_av, void *context)
{
	return fi_no_av_open(domain_fid, attr, fid_av, context);
}