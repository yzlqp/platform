/**
 * @file local_variables.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _LOCAL_VARIABLES_H
#define _LOCAL_VARIABLES_H

#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include "type.h"
#include "parameters.h"


struct job {
	struct job *j_next;
	struct job *j_prev;
	pthread_t j_id;
	void *data;
};

struct queue {
	struct job *q_head;
	struct job *q_tail;
	pthread_rwlock_t q_lock;
};

struct local_variable {

	/* file descriptor */
	int fd_axi_ctl;
	int fd_timer;
	int fd_mem;

	/* mmap vaddr */
	unsigned char *map_bramctl_tx;
	unsigned char *map_bramctl_rx;

	/* buf addr*/
	unsigned char *tx_buf;
	unsigned char *rx_buf;

	/* socket for UDP */
	int  sockfd_udp_trans;
	int  sockfd_udp_9361;
	int  sockfd_udp_channel;
	struct sockaddr_in server_addr_trans, client_addr_trans;
	struct sockaddr_in server_addr_9361;
	struct sockaddr_in server_addr_channel;
    socklen_t len;

	/* socket for RAW */
	int sockfd_raw;                        //raw socket
	struct sockaddr_ll raw_addr_src;	   //Raw socket address structure
	struct sockaddr_ll raw_addr_dst;       //Raw socket address structure
	struct ifreq req;	                   //Network interface address

	/* pthread */
	pthread_mutex_t mutex1;
	pthread_mutex_t mutex2;
	pthread_mutex_t mutex_lock;
	pthread_cond_t waitloc;
	pthread_key_t key;
	pthread_once_t init_done;
	int flag;
	sigset_t mask;
	sigset_t oldmask;
};

#endif /* _LOCAL_VARIABLES_H */
