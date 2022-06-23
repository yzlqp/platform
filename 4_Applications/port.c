/**
 * @file port.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-04-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <sys/ioctl.h>
#include <fcntl.h>
#include "port.h"


extern struct local_variable local_var;

/*
 * Platform initialization
 */
int init_platform(struct local_variable *var)
{
	int ret;

	var->fd_axi_ctl = open("/dev/axi_ctrl_cdev", O_RDWR);
    if (var->fd_axi_ctl < 0) {
    	pr_errno("open /dev/axi_ctrl_cdev");
        return -1;
    }
	pr_info("init_platform->open: fd_axi_ctl = %d\n", var->fd_axi_ctl);

	var->fd_timer = open("/dev/axi_timer", O_RDWR);
    if (var->fd_timer < 0) {
    	pr_errno("open /dev/fd_timer");
        return -1;
    }
	pr_info("init_platform->open: fd_timer = %d\n", var->fd_timer);

	var->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (var->fd_mem < 0) {
		pr_errno("open /dev/mem");
		return -2;
	}
	pr_info("init_platform->open: fd_mem = %d\n", var->fd_mem);

	var->map_bramctl_tx = mmap(NULL, data_len_byte_1552, PROT_READ | PROT_WRITE, MAP_SHARED, var->fd_mem, BRAM_CTRL_SEND);
	var->map_bramctl_rx = mmap(NULL, data_len_byte_1552, PROT_READ | PROT_WRITE, MAP_SHARED, var->fd_mem, BRAM_CTRL_RECV);
	if (var->map_bramctl_tx == NULL || var->map_bramctl_rx == NULL ) {
		 pr_err("init_platform: mmap fail !!!\n");
		 return -3;
	}
	pr_info("init_platform->mmap: map_bramctl_tx = %p, map_bramctl_rx = %p\n", var->map_bramctl_tx, var->map_bramctl_rx);

	ret = pl_rst(var);
    if (ret) {
    	pr_errno("pl_reset");
		return -4;
    }
    pr_info("init_platform: pl reset success\n");

	ret = loop_test(var);
	if (ret) {
		pr_err("init_platform->loop_test: err = %d\n", -ret);
		return ret;
	}
	pr_warn("init_platform: Initialization and FPGA self-loop test succeeded\n");

	return ret;
}

/*
 * Signal processing thread for receive data
 */
void* rx_func(void *arg)
{
	printtids("rx func thread:");
	int ret;
	struct local_variable *varp = (struct local_variable *)arg;

	// Unblock SIGIO signal
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGIO);
	ret = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
	if (ret != 0) {
		pr_err("pthread_sigmask-> sig unblock err: err = %d\n", ret);
	}

	// Establish the signal processing function
	ret = init_rx_signal(varp);
	if (ret) {
		pr_err("init_rx_signal err, err = %d\n", ret);
	}

	// Wait for other signals
	int signo;
	sigset_t waitmask;
	sigemptyset(&waitmask);
	sigaddset(&waitmask, __SIGRTMIN);
	sigaddset(&waitmask, SIGUSR2);
	for (;;) {
		ret = sigwait(&waitmask, &signo);
		if(ret != 0) {
			pr_err("rx_func: sigwait err !!!\n");
		}

		switch (signo) {
		case SIGIO:
			pr_warn("rx_func: SIGIO\n");
			break;

		case SIGUSR2:
			pr_warn("rx_func: SIGUSR2\n");
			pthread_mutex_lock(&varp->mutex_lock);
			varp->flag = 1;
			pthread_mutex_unlock(&varp->mutex_lock);
			pthread_cond_signal(&varp->waitloc);
			break;

		case SIGUSR1:
			pr_warn("rx_func: SIGUSR1\n");
			break;

		case __SIGRTMIN:
			pr_warn("rx_func: __SIGRTMIN\n");
			break;

		default:
			pr_warn("rx_func: unexprcted signal %d\n", signo);
		}
	}

}

/*
 * Scheduling threads in time slots for timer
 */
void* timer_func(void *arg)
{
	printtids("timer func thread:");
	int ret;
	struct local_variable *varp = (struct local_variable *)arg;

	// Unblock SIGUSR1 signal
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	ret = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
	if (ret != 0) {
		pr_err("pthread_sigmask-> sig unblock err: err = %d\n", ret);
	}

	// Establish the signal processing function
	ret = init_tx_signal(varp);
	if (ret) {
		pr_err("init_tx_signal err, err = %d\n", ret);
	}

	// Set UDP socket Non_bolck
	ret = fcntl(varp->sockfd_udp_trans, F_GETFL, 0);
	if (ret < 0) {
		pr_errno("fcntl F_GETFL fail");
	}
	ret = fcntl(varp->sockfd_udp_trans, F_SETFL, ret | O_NONBLOCK);
	if (ret < 0) {
		pr_errno("fcntl F_SETFL fail");
	}

	// Start hardware timer
	timer_request(varp);

	// The original socket interface is initialized
    ret = raw_skt_init(varp);
    if (ret) {
    	pr_err("raw_skt_init err, err = %d\n", ret);
    }

	// Wait for other signals
	int signo;
	sigset_t waitmask;
	sigemptyset(&waitmask);
	sigaddset(&waitmask, __SIGRTMIN);
	sigaddset(&waitmask, SIGUSR2);
	for (;;) {
		ret = sigwait(&waitmask, &signo);
		if(ret != 0) {
			pr_err("timer_func: sigwait err !!!\n");
		}

		switch (signo) {
		case SIGIO:
			pr_warn("timer_func: SIGIO\n");
			break;

		case SIGUSR2:
			pr_warn("timer_func: SIGUSR2\n");
			pthread_mutex_lock(&varp->mutex_lock);
			varp->flag = 1;
			pthread_mutex_unlock(&varp->mutex_lock);
			pthread_cond_signal(&varp->waitloc);
			break;

		case SIGUSR1:
			pr_warn("timer_func: SIGUSR1\n");
			break;

		case __SIGRTMIN:
			pr_warn("timer_func: __SIGRTMIN\n");
			break;

		default:
			pr_warn("timer_func: unexprcted signal %d\n", signo);
		}
	}

}
/*
 * General purpose signal processing functions
 */
void sig_rx_handler(int signum, siginfo_t *info, void *myact)
{
	if (info->si_code == 1) {
		rx_data_udp();
	} else if (info->si_code == 3) {
		pr_err("si_code err, si_code = %d", info->si_code);
	} else {
		pr_err("si_code err, si_code = %d", info->si_code);
	}
	//timer_test();
}

/*
 * UDP and RAW data receiving signal processing function
 */
void rx_data_udp()
{
	int count = 0;
	uint32_t sendnum = 0;

	// Get data from FPGA
	read_data(&local_var, local_var.rx_buf, data_len_byte_1552);

	// Gets the number of bytes field
	for (int j = 3; j <= 4; j++) {
		sendnum |= ((u32_t)local_var.rx_buf[1519 + j]) << ((4 - j) * 8);
	}

	// Determine whether it is UDP socket data (2) or raw socket data (1)
	if (local_var.rx_buf[1524] == 1) {
		count = sendto(local_var.sockfd_raw, local_var.rx_buf, sendnum, 0, (struct sockaddr *)&local_var.raw_addr_dst, sizeof(local_var.raw_addr_dst));
		pr_info("raw rx: should send %d bytes, actuall send %d bytes ======\n", sendnum, count);
	} else if (local_var.rx_buf[1524] == 2) {
		count = sendto(local_var.sockfd_udp_trans, local_var.rx_buf, sendnum, 0, (struct sockaddr *)&local_var.client_addr_trans, sizeof(local_var.client_addr_trans));
		pr_info("udp rx: should send %d bytes, actuall send %d bytes ======\n", sendnum, count);
	}

	// Clear the receive buffer
	memset(local_var.rx_buf, 0, data_len_byte_1552);

	// Notify PL that the data has been read
	rx_end(&local_var);
}

/*
 * Timer Interrupt Request
 */
int timer_request(struct local_variable *var)
{
	// Device tree number of interrupt property nodes
	int arg = 2;

	// Enable timed interrupt
	ioctl(var->fd_timer, IOC_TIMER_INTR_EN, &arg);

	//ioctl(var->fd_timer, IOC_TIMER_INTR_DIS, &arg);

	return 0;
}
