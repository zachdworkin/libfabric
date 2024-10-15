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

static int xxx_getinfo(uint32_t version, const char *node, const char *service,
		       uint64_t flags, const struct fi_info *hints,
		       struct fi_info **info)
{
        return -FI_ENODATA;
}

static void xxx_fini(void)
{
#if HAVE_XXX_DL
	return 0;
#endif
	xxx_cleanup();
}

static void xxx_ini(void)
{
        /* Define */
}

struct fi_provider xxx_prov = {
        .name = "xxx",
        .version = OFI_VERSION_DEF_PROV,
        .fi_version = OFI_VERSION_LATEST,
        .getinfo = xxx_getinfo,
        .fabric = xxx_fabric,
        .cleanup = xxx_fini,
};

struct util_prov xxx_util_prov = {
	.prov = &xxx_prov,
	.info = &xxx_info,
	.flags = 0
};

XXX_INI
{
#if HAVE_XXX_DL
	return 0;
#endif
        return &xxx_prov;
}

