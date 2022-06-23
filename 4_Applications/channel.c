/**
 * @file channel.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "channel.h"

/*
 * Channel emulation module handles threads
 */
void *chan_func(void *arg)
{
	printtids("chan thread:");

	struct local_variable *varp = (struct local_variable *)arg;
	struct sockaddr_in client_addr;
	int *bufp = NULL;
	int *vaddr = NULL;
	socklen_t len;

	init_chan_config(&bufp, &vaddr, varp);

	while(1) {
		int num = 0;
		pr_info("CE listen...\n");
		num = recvfrom(varp->sockfd_udp_channel, bufp, sizeof(int) * 32, 0, (struct sockaddr *)&client_addr, &len);
		pr_info("====== CE rx %d bytes (%#x) \n", num, num);
		num = chan_config(bufp, vaddr, (struct sockaddr *)&client_addr, varp);
		pr_info("====== CE tx %d bytes (%#x) \n", num, num);
		memset(bufp, 0, sizeof(int)*32);
	}

	return ((void *)0);
}

/*
 * Channel emulation module handles thread initialization
 */
void init_chan_config(int **chan_bufp, int **vaddr, struct local_variable *varp)
{
	*vaddr = mmap(NULL, data_len_byte_4K, PROT_READ | PROT_WRITE, MAP_SHARED, varp->fd_mem, CHAN_CTRL_ADDR);
	if (*vaddr == NULL) {
		pr_err("init_chan_config->mmap fail !!!\n");
	}

	// 32 * 4 =  128 Bytes
	*chan_bufp = (int *)malloc(sizeof(int)*32);
}

/*
 * Channel emulation module data processing
 */
int chan_config(int *bufp, int *vaddr, struct sockaddr *client_addr, struct local_variable *varp)
{
	int count = 0;
	int sendnum = 0;

	if (bufp[0] == 2) {  		 // write then read
		count = bufp[4];
		for (int i = 0; i < count; i++) {
			vaddr[i] = bufp[i+5];
		}

		for (int i = 0; i < count; i++) {
			bufp[i+5] = vaddr[i];
		}
		bufp[2] = 1;			  // ack
		sendnum = sendto(varp->sockfd_udp_channel, bufp, sizeof(int)*32, 0, (struct sockaddr *)client_addr, sizeof(struct sockaddr));

	} else if (bufp[0] == 1) {    // just read
		count = bufp[4];

		for (int i = 0; i < count; i++) {
			bufp[i+5] = vaddr[i];
		}
		bufp[2] = 1;			  //ack
		sendnum = sendto(varp->sockfd_udp_channel, bufp, sizeof(int)*32, 0, (struct sockaddr *)client_addr, sizeof(struct sockaddr));

	} else {
		pr_err("instruction err!!!\n");
	}

	return sendnum;
}
