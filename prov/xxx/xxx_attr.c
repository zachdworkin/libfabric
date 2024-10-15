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

struct fi_tx_attr xxx_tx_attr = {
	.caps           	= 0, /* define */
	.mode           	= 0, /* define */
	.op_flags       	= 0, /* define */
	.msg_order      	= 0, /* define */
	.comp_order     	= 0, /* define */
	.inject_size    	= 0, /* define */
	.size           	= 0, /* define */
	.iov_limit      	= 0, /* define */
	.tclass         	= 0  /* define */
};

struct fi_rx_attr xxx_rx_attr = {
	.caps                    = 0, /* define */
	.mode                    = 0, /* define */
	.op_flags                = 0, /* define */
	.msg_order               = 0, /* define */
	.comp_order              = 0, /* define */
	.total_buffered_recv     = 0, /* define */
	.size                    = 0, /* define */
	.iov_limit               = 0  /* define */
};

struct fi_ep_attr xxx_ep_attr = {
	.type			= 0,	/* define */
	.protocol		= 0,	/* define */
	.protocol_version	= 0,	/* define */
	.max_msg_size		= 0,	/* define */
	.msg_prefix_size	= 0,	/* define */
	.max_order_raw_size	= 0,	/* define */
	.max_order_war_size	= 0,	/* define */
	.max_order_waw_size	= 0,	/* define */
	.mem_tag_format		= 0,	/* define */
	.tx_ctx_cnt		= 0,	/* define */
	.rx_ctx_cnt		= 0,	/* define */
	.auth_key_size		= 0,	/* define */
	.auth_key		= NULL	/* define */
};

struct fi_domain_attr xxx_domain_attr = {
	.domain			= 0,	/* define */
	.name			= "xxx",/* define */
	.threading		= 0,	/* define */
	.control_progress	= 0,	/* define */
	.progress		= 0,	/* define */
	.resource_mgmt		= 0,	/* define */
	.av_type		= 0,	/* define */
	.mr_mode		= 0,	/* define */
	.mr_key_size		= 0,	/* define */
	.cq_data_size		= 0,	/* define */
	.cq_cnt			= 0,	/* define */
	.ep_cnt			= 0,	/* define */
	.tx_ctx_cnt		= 0,	/* define */
	.rx_ctx_cnt		= 0,	/* define */
	.max_ep_tx_ctx		= 0,	/* define */
	.max_ep_rx_ctx		= 0,	/* define */
	.max_ep_stx_ctx		= 0,	/* define */
	.max_ep_srx_ctx		= 0,	/* define */
	.cntr_cnt		= 0,	/* define */
	.mr_iov_limit		= 0,	/* define */
	.caps			= 0,	/* define */
	.mode			= 0,	/* define */
	.auth_key		= NULL,	/* define */
	.auth_key_size		= 0,	/* define */
	.max_err_data		= 0,	/* define */
	.mr_cnt			= 0,	/* define */
	.tclass			= 0,	/* define */
	.max_ep_auth_key	= 0,	/* define */
	.max_group_id		= 0 	/* define */
};

struct fi_fabric_attr xxx_fabric_attr = {
	.fabric			= NULL,			/* define */
	.name			= "xxx",		/* define */
	.prov_name		= NULL,			/* define */
	.prov_version		= OFI_VERSION_DEF_PROV,	/* define */
	.api_version		= 0			/* define */
};

struct fi_info xxx_info = {
	.caps 			= 0, 		    /* define */
	.addr_format		= FI_FORMAT_UNSPEC, /* define */
	.tx_attr		= &xxx_tx_attr,     /* define */
	.rx_attr		= &xxx_rx_attr,     /* define */
	.ep_attr		= &xxx_ep_attr,     /* define */
	.domain_attr		= &xxx_domain_attr, /* define */
	.fabric_attr		= &xxx_fabric_attr, /* define */
	.next			= NULL,             /* define */
};
