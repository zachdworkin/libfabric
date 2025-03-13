/*
 * rdm_bw_mt.c
 * This is a mostly stand-alone test.
 * It does not use any common fabtests code but is intended to be run in the
 * fabtest suite. In order to make this test use common code it must be
 * refactored to remove the global resources and put them into objects.
 * These objects can be struct ft_fabric_resources which will contain all
 * resources that can be opened under a single fabtest and
 * struct ft_buffer_resouces for all resources associated with a buffer.
 * For the case of this test it will be:
 *
 * 				fabric
 * 				 |
 * 				domain
 * 				 |
 * 				ep
 * 		      ___________|__________
 *		    / 	    |	    |	    \
 * 		   cq	    av	   cntr	  buffer_resources
 *
 * Each thread must have its own domain, ep, cq, av, cntr, buffer_reources, etc.
 * The buffer_resources will contain the buffer, memory registration, and any
 * other miscelaneous resources any buffer might need.
 * This test comes with a TODO to refactor the common code to support more tests
 * of this type and enable easier development of future tests with more
 * compatible objects instead of global variables.
 * WARNING: Not all options are supported in this test!
 * TODO add MR registration
 */

#include <rdma/fi_cm.h>

#include "shared.h"
#include "benchmark_shared.h"
#include "hmem.h"

#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];

static int num_eps = 1;
static bool bidir = false;
pthread_barrier_t barrier;

struct thread_args {
	pthread_t thread;
	fi_addr_t fiaddr;
	struct fid_domain *domain;
	struct fid_ep *ep;
	struct fid_cq *cq;
	struct fid_cntr *tx_cntr;
	struct fid_cntr *rx_cntr;
	struct fid_av *av;
	struct fid_eq *eq;
	struct fid_mr *tx_mr;
	struct fid_mr *rx_mr;
	void *tx_mr_desc;
	void *rx_mr_desc;
	struct fi_context2 send_ctx;
	struct fi_context2 recv_ctx;
	size_t size;
	char *tx_buf;
	char *rx_buf;
	int id;
	int ret;
};

static struct thread_args *targs = NULL;

static void cleanup_ofi(void)
{
	int ret;
	int i;

	for (i = 0; i < num_eps; i++) {
		if (targs[i].ep) {
			ret = fi_close(&targs[i].ep->fid);
			if (ret)
				printf("fi_close(ep[%d]) failed: %d\n", i, ret);
		}

		if (targs[i].cq) {
			ret = fi_close(&targs[i].cq->fid);
			if (ret)
				printf("fi_close(cq[%d]) failed: %d\n", i, ret);
		}

		if (targs[i].tx_cntr) {
			ret = fi_close(&targs[i].tx_cntr->fid);
			if (ret)
				printf("fi_close(tx_cntr) failed: %d\n", ret);
		}

		if (targs[i].rx_cntr) {
			ret = fi_close(&targs[i].rx_cntr->fid);
			if (ret)
				printf("fi_close(rx_cntr) failed: %d\n", ret);
		}

		if (targs[i].av) {
			ret = fi_close(&targs[i].av->fid);
			if (ret)
				printf("fi_close(av[%d]) failed: %d\n", i, ret);
		}

		if (targs[i].eq) {
			ret = fi_close(&targs[i].eq->fid);
			if (ret)
				printf("fi_close(eq[%d]) failed: %d\n", i, ret);
		}

		if (targs[i].domain) {
			ret = fi_close(&targs[i].domain->fid);
			if (ret)
				printf("fi_close(domain[%d]) failed: %d\n", i,
					ret);
		}
	}

	if (fabric) {
		fi_close(&fabric->fid);
		if (ret)
			printf("fi_close(fabric) failed: %d\n", ret);
	}

	if (fi)
		fi_freeinfo(fi);
	if (hints)
		fi_freeinfo(hints);
	if (targs)
		free(targs);
}

static int init_av(int i)
{
	int ret;
	size_t len = BUFFER_SIZE;
	char print_buf[BUFFER_SIZE];

	memset(print_buf, 0, BUFFER_SIZE);

	ret = fi_getname(&targs[i].ep->fid, buffer, &len);
	if (ret) {
		printf("fi_getname failed: %d\n", ret);
		return ret;
	}

	len = BUFFER_SIZE;

	ret = ft_sock_send(oob_sock, buffer, len);
	if (ret) {
		printf("ft_sock_send failed: %d\n", ret);
		return ret;
	}

	ret = ft_sock_recv(oob_sock, buffer, len);
	if (ret) {
		printf("ft_sock_recv failed: %d\n", ret);
		return ret;
	}

	ret = fi_av_insert(targs[i].av, buffer, 1, &targs[i].fiaddr, 0, NULL);
	if (ret != 1) {
		printf("fi_av_insert failed: %d\n", ret);
		return ret;
	}

	return 0;
}

static int init_ofi(void)
{
	struct fi_cq_attr cq_attr = {0};
	struct fi_av_attr av_attr = {0};
	struct fi_eq_attr eq_attr = {0};
	struct fi_cntr_attr cntr_attr = {0};
	int ret = FI_SUCCESS, i;

        ret = fi_fabric(fi->fabric_attr, &fabric, NULL);
        if (ret) {
		printf("fi_fabric failed: %d\n", ret);
                return ret;
	}

	targs = calloc(num_eps, sizeof(*targs));
	if (!targs) {
		printf("thread_args calloc failed\n");
		return -FI_ENOMEM;
	}

	for (i = 0; i < num_eps; i++) {
		memset(&cq_attr, 0, sizeof(cq_attr));
		memset(&av_attr, 0, sizeof(av_attr));
		memset(&eq_attr, 0, sizeof(eq_attr));
		memset(&cntr_attr, 0, sizeof(cntr_attr));
		ret = fi_domain(fabric, fi, &targs[i].domain, NULL);
		if (ret) {
			printf("fi_domain failed ep[%d]: %d\n", i, ret);
			return ret;
		}

		ret = fi_endpoint(targs[i].domain, fi, &targs[i].ep, NULL);
		if (ret) {
			printf("fi_endpoint failed: %d\n", ret);
			return ret;
		}

		cq_attr.size = 128;
		cq_attr.format = FI_CQ_FORMAT_CONTEXT;
		ret = fi_cq_open(targs[i].domain, &cq_attr, &targs[i].cq, NULL);
		if (ret) {
			printf("fi_cq_open failed: %d\n", ret);
			return ret;
		}

		ret = fi_cntr_open(targs[i].domain, &cntr_attr, &targs[i].tx_cntr, NULL);
		if (ret) {
			printf("fi_cntr_open failed: %d\n", ret);
			return ret;
		}

		ret = fi_cntr_open(targs[i].domain, &cntr_attr, &targs[i].rx_cntr, NULL);
		if (ret) {
			printf("fi_cntr_open failed: %d\n", ret);
			return ret;
		}

		av_attr.type = FI_AV_UNSPEC;
		av_attr.count = 1;
		ret = fi_av_open(targs[i].domain, &av_attr, &targs[i].av, NULL);
		if (ret) {
			printf("fi_av_open failed: %d\n", ret);
			return ret;
		}

		eq_attr.size = 128;
		eq_attr.wait_obj = FI_WAIT_UNSPEC;
		ret = fi_eq_open(fabric, &eq_attr, &targs[i].eq, NULL);
		if (ret) {
			printf("fi_eq_open failed: %d\n", ret);
			return ret;
		}

		ret = fi_ep_bind(targs[i].ep, &targs[i].av->fid, 0);
		if (ret) {
			printf("fi_ep_bind av failed: %d\n", ret);
			return ret;
		}

		ret = fi_ep_bind(targs[i].ep, &targs[i].cq->fid, FI_TRANSMIT | FI_RECV);
		if (ret) {
			printf("fi_ep_bind cq failed: %d\n", ret);
			return ret;
		}

		ret = fi_ep_bind(targs[i].ep, &targs[i].tx_cntr->fid, FI_SEND);
		if (ret) {
			printf("fi_ep_bind tx_cntr failed: %d\n", ret);
			return ret;
		}

		ret = fi_ep_bind(targs[i].ep, &targs[i].rx_cntr->fid, FI_RECV);
		if (ret) {
			printf("fi_ep_bind rx_cntr failed: %d\n", ret);
			return ret;
		}

		ret = fi_enable(targs[i].ep);
		if (ret) {
			printf("fi_enable failed: %d\n", ret);
			return ret;
		}

		ret = init_av(i);
		if (ret) {
			printf("init_av failed\n");
			return ret;
		}
	}

	return ret;
}


static int mt_reg_mr(struct fi_info *fi, void *buf, size_t size,
		     uint64_t access, uint64_t key, enum fi_hmem_iface iface,
		     uint64_t device, struct fid_domain *dom,
		     struct fid_ep *endpoint, struct fid_mr **mr, void **desc)
{
	struct fi_mr_attr attr = {0};
	struct iovec iov = {0};
	int ret;
	uint64_t flags;
	int dmabuf_fd;
	uint64_t dmabuf_offset;
	struct fi_mr_dmabuf dmabuf = {0};

	if (!ft_need_mr_reg(fi))
	return 0;

	iov.iov_base = buf;
	iov.iov_len = size;

	flags = (iface) ? FI_HMEM_DEVICE_ONLY : 0;

	if (opts.options & FT_OPT_REG_DMABUF_MR) {
		ret = ft_hmem_get_dmabuf_fd(iface, buf, size,
					&dmabuf_fd, &dmabuf_offset);
		if (ret)
			return ret;

		dmabuf.fd = dmabuf_fd;
		dmabuf.offset = dmabuf_offset;
		dmabuf.len = size;
		dmabuf.base_addr = (void *)((uintptr_t) buf - dmabuf_offset);
		flags |= FI_MR_DMABUF;
	}

	ft_fill_mr_attr(&iov, &dmabuf, 1, access, key, iface, device, &attr, flags);
	ret = fi_mr_regattr(dom, &attr, flags, mr);
	if (ret)
		return ret;

	if (desc)
		*desc = fi_mr_desc(*mr);

	if (fi->domain_attr->mr_mode & FI_MR_ENDPOINT) {
		ret = fi_mr_bind(*mr, &endpoint->fid, 0);
	if (ret)
		return ret;

	ret = fi_mr_enable(*mr);
	if (ret)
		return ret;
	}

	return FI_SUCCESS;
}

static int reg_mrs(struct thread_args *targs)
{
	int ret = FI_SUCCESS;

	ret = ft_hmem_alloc(opts.iface, opts.device, (void **) &(targs->tx_buf),
			    targs->size);
	if (ret) {
		printf("ft_hmem_alloc tx %d failed: %d\n", targs->id, ret);
		return ret;
	}

	ret = ft_hmem_alloc(opts.iface, opts.device, (void **) &(targs->rx_buf),
			    targs->size);
	if (ret) {
		printf("ft_hmem_alloc rx %d failed: %d\n", targs->id, ret);
		return ret;
	}

	ret = mt_reg_mr(fi, (void *) targs->tx_buf, targs->size, FI_SEND,
			targs->id, opts.iface, opts.device,
			targs->domain, targs->ep,
			&targs->tx_mr, &targs->tx_mr_desc);
	if (ret) {
		printf("fi_mr_reg tx %d failed: %d\n", targs->id, ret);
		return ret;
	}

	ret = mt_reg_mr(fi, (void *) targs->rx_buf, targs->size, FI_RECV,
			targs->id, opts.iface, opts.device,
			targs->domain, targs->ep,
			&targs->rx_mr, &targs->rx_mr_desc);
	if (ret)
		printf("fi_mr_reg rx %d failed: %d\n", targs->id, ret);

	return ret;
}

static void force_progress(struct fid_cq *cq)
{
	(void) fi_cq_read(cq, NULL, 0);
}

static int read_cq(struct fid_cq *cqueue)
{
	struct fi_cq_entry cq_entry;
	int ret;

	do {
		ret = fi_cq_read(cqueue, &cq_entry, 1);
		if (ret < 0 && ret != -FI_EAGAIN)
			return ret;
		if (ret == 1) {
			return 0;
		}
	} while (1);
}

static int post_send(void *context)
{
	int ret;
	struct thread_args *targs = context;

	do {
		ret = fi_send(targs->ep, targs->tx_buf, targs->size,
			      targs->tx_mr, targs->fiaddr, &targs->send_ctx);
		if (ret != -FI_EAGAIN)
			return ret;

		force_progress(targs->cq);
	} while (1);
}

static int post_recv(void *context)
{
	int ret;
	struct thread_args *targs = context;

	do {
		ret = fi_recv(targs->ep, targs->rx_buf, targs->size,
			      targs->rx_mr, targs->fiaddr, &targs->recv_ctx);
		if (ret != -FI_EAGAIN)
			return ret;

		printf("%d fi_recv failed: %d\n", targs->id, ret);
		force_progress(targs->cq);
	} while (1);
}

static int bw_send(void *context)
{
	int ret;
	struct thread_args *targs = context;

	ret = post_send(context);
	if (ret)
		return ret;
	ret = read_cq(targs->cq);
	if (ret) {
		printf("send read_cq error: %d\n", ret);
		return ret;
	}
	return 0;
}

static int bw_recv(void *context)
{
	int ret = FI_SUCCESS;
	struct thread_args *targs = context;

	ret = post_recv(context);
	if (ret)
		return ret;

	ret = read_cq(targs->cq);
	if (ret) {
		printf("recv read_cq error: %d\n", ret);
		return ret;
	}

	return 0;
}

static void *uni_bandwidth(void *context)
{
	int i, ret;

	pthread_barrier_wait(&barrier);
	for (i = 0; i < opts.warmup_iterations; i++) {
		ret = opts.dst_addr ? bw_send(context) : bw_recv(context);
		if (ret) {
			((struct thread_args *) context)->ret = ret;
			printf("ep[%d] warmup failed iter %d\n", targs->id, i);
			break;
		}
	}

	if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
		ft_start();
	for (i = 0; i < opts.iterations; i++) {
		ret = opts.dst_addr ? bw_send(context) : bw_recv(context);
		if (ret) {
			((struct thread_args *) context)->ret = ret;
			break;
		}
	}
	if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
		ft_stop();

	return  NULL;
}

static void *bi_bandwidth(void *context)
{
	int i, ret;
	struct thread_args *targs = context;

	pthread_barrier_wait(&barrier);
	for (i = 0; i < opts.warmup_iterations; i++) {
		if (opts.dst_addr)
			ret = targs->id % 2 ? bw_send(context) : bw_recv(context);
		else
			ret = targs->id % 2 ? bw_recv(context) : bw_send(context);
		if (ret) {
			((struct thread_args *) context)->ret = ret;
			printf("%d warmup failed iter %d\n", targs->id, i);
			break;
		}
	}

	if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
		ft_start();
	for (i = 0; i < opts.iterations; i++) {
		if (opts.dst_addr)
			ret = targs->id % 2 ? bw_send(context) : bw_recv(context);
		else
			ret = targs->id % 2 ? bw_recv(context) : bw_send(context);
		if (ret)
			break;
	}
	if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
		ft_stop();

	return NULL;
}

static int run_size(size_t size)
{
	int ret = FI_SUCCESS;
	int i;

	for (i = 0; i < num_eps; i++) {
		targs[i].tx_buf = calloc(1, size);
		if (!targs[i].tx_buf)
			goto no_mem;

		targs[i].rx_buf = calloc(1, size);
		if (!targs[i].rx_buf)
			goto no_mem;
		targs[i].size = size;
		targs[i].id = i;
		targs[i].ret = FI_SUCCESS;
		ret = reg_mrs(&targs[i]);
		if (ret)
			goto no_mem;
	}

	for (i = 0; i < num_eps; i++) {
		ret = pthread_create(&targs[i].thread, NULL,
				     bidir ? bi_bandwidth : uni_bandwidth,
				     &targs[i]);
		if (ret) {
			printf("pthread_create failed: %d\n", ret);
			return ret;
		}
	}
	for (i = 0; i < num_eps; i++) {
		pthread_join(targs[i].thread, NULL);
		if (targs[i].ret) {
			ret = targs[i].ret;
			goto no_mem;
		}
	}

	show_perf(NULL, size, opts.iterations, &start, &end, num_eps);

no_mem:
	for (i = 0; i < num_eps; i++) {
		if (targs[i].tx_mr)
			fi_close(&targs[i].tx_mr->fid);
		if (targs[i].rx_mr)
			fi_close(&targs[i].rx_mr->fid);
		if (targs[i].tx_buf)
			free(targs[i].tx_buf);
		if (targs[i].rx_buf)
			free(targs[i].rx_buf);
	}

	return ret;
}

static int run_test(void)
{
	int i, ret;

	if (!(opts.options & FT_OPT_SIZE)) {
		for (i = 0; i < TEST_CNT; i++) {
			if (!ft_use_size(i, opts.sizes_enabled))
				continue;
			ret = run_size(test_size[i].size);
			if (ret)
				return ret;
			}
	} else {
		ret = run_size(opts.transfer_size);
		if (ret)
			return ret;
	}

	return FI_SUCCESS;
}

static void usage(void)
{
	fprintf(stderr, "\nrdm_bw_mt test options:\n");
	FT_PRINT_OPTS_USAGE("-g", "enable bidirectional");
	FT_PRINT_OPTS_USAGE("-n <num endpoints>",
			    "number of endpoints (threads) to use");
	fprintf(stderr, "Notice to user: Not all fabtests options are supported"
		" by this test. If something isn't working check if the option"
		" is supported before reporting a bug.\n");
}

int main(int argc, char **argv)
{
        int ret, op;

	opts = INIT_OPTS;
	opts.options |= FT_OPT_OOB_CTRL;

        hints = fi_allocinfo();
	if (!hints)
		return EXIT_FAILURE;

	while ((op = getopt_long(argc, argv, "gn:h" CS_OPTS INFO_OPTS API_OPTS
		BENCHMARK_OPTS, long_opts, &lopt_idx)) != -1) {
		switch (op) {
		default:
			if (!ft_parse_long_opts(op, optarg))
				continue;
			ft_parse_benchmark_opts(op, optarg);
			ft_parseinfo(op, optarg, hints, &opts);
			ft_parsecsopts(op, optarg, &opts);
			ft_parse_api_opts(op, optarg, hints, &opts);
			break;
		case 'g':
			bidir = true;
			break;
		case 'n':
			num_eps = atoi(optarg);
			break;
		case '?':
		case 'h':
			ft_csusage(argv[0], "Multi-Threaded Bandwidth test for "
				   "RDM endpoints.");
			ft_benchmark_usage();
			ft_longopts_usage();
			usage();
			return EXIT_FAILURE;
		}
	}

	if (optind < argc)
		opts.dst_addr = argv[optind];

	hints->ep_attr->type = FI_EP_RDM;
	hints->domain_attr->resource_mgmt = FI_RM_ENABLED;
	hints->domain_attr->threading = FI_THREAD_DOMAIN;
	hints->caps = FI_MSG;
	hints->mode |= FI_CONTEXT | FI_CONTEXT2;
	hints->domain_attr->mr_mode = opts.mr_mode;
	hints->addr_format = opts.address_format;

        ret = ft_init_oob();
        if (ret)
                goto out;

	ret = fi_getinfo(FT_FIVERSION, NULL, NULL, 0, hints, &fi);
	if (ret) {
		printf("fi_getinfo() failed: %d\n", ret);
		goto out;
	}

	ret = init_ofi();
	if (ret) {
		printf("init ofi failed\n");
                goto out;
	}

	ret = pthread_barrier_init(&barrier, NULL, num_eps);
	if (ret)
		goto out;

	ret = run_test();

	pthread_barrier_destroy(&barrier);

out:
	cleanup_ofi();
	ft_close_oob();
        return ret;
}
