/**
 * @file socket_api.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-08
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <stdio.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utility.h"
#include "socket_api.h"
#include "local_variables.h"

extern struct local_variable local_var;

/*
 * Initialize the UDP socket for all
 */
int init_network(struct local_variable *var)
{
	int ret;

	ret = udp_trans_init(var);
	if (ret) {
		return ret;
	}

	ret = udp_9361_init(var);
	if (ret) {
		return ret;
	}

	ret = udp_channel_init(var);
	if (ret) {
		return ret;
	}

	return ret;
}

/*
 * Initialize the UDP data transfer socket
 */
int udp_trans_init(struct local_variable *var)
{
	var->sockfd_udp_trans = socket(PF_INET, SOCK_DGRAM, 0);
	if (var->sockfd_udp_trans < 0) {
		pr_err("create sockfd_udp_trans false!!!\n");
	    return -1;
	}

	unsigned short int PROT = PORT_TRANS;
	bzero(&var->server_addr_trans, sizeof(var->server_addr_trans));
	var->server_addr_trans.sin_family = AF_INET;
    var->server_addr_trans.sin_addr.s_addr = htonl(INADDR_ANY);
    var->server_addr_trans.sin_port = htons(PROT);

    if (bind(var->sockfd_udp_trans, (struct sockaddr *)&var->server_addr_trans, sizeof(var->server_addr_trans)) < 0) {
    	pr_err("sockfd_udp_trans---> Can not bind!!!\n");
        return -2;
    }

    // PC side address information (default)
	var->client_addr_trans.sin_family = AF_INET;
	inet_pton(AF_INET, IPADDR_PC, &var->client_addr_trans.sin_addr);
	//var->client_addr_trans.sin_addr.s_addr = inet_addr("192.168.2.1");
	unsigned short int PC_PORT = PORT_PC;
	var->client_addr_trans.sin_port = htons(PC_PORT);

    return 0;
}

/*
 * Initialize the UDP radio module AD9361 socket
 */
int udp_9361_init(struct local_variable *var)
{
	var->sockfd_udp_9361 = socket(PF_INET, SOCK_DGRAM, 0);
	if (var->sockfd_udp_9361 < 0) {
		pr_err("create sockfd_udp_9361 false!!!\n");
	    return -1;
	}

	unsigned short int PROT = PORT_9361;
	bzero(&var->server_addr_9361, sizeof(var->server_addr_9361));
	var->server_addr_9361.sin_family = AF_INET;
    var->server_addr_9361.sin_addr.s_addr = htonl(INADDR_ANY);
    var->server_addr_9361.sin_port = htons(PROT);

    if (bind(var->sockfd_udp_9361, (struct sockaddr *)&var->server_addr_9361, sizeof(var->server_addr_9361)) < 0) {
    	pr_err("server_addr_9361---> Can not bind!!!\n");
        return -2;
    }

    return 0;
}

/*
 * Initialize the UDP channel analog module socket
 */
int udp_channel_init(struct local_variable *var)
{
	var->sockfd_udp_channel = socket(PF_INET, SOCK_DGRAM, 0);
	if (var->sockfd_udp_channel < 0) {
		pr_err("create sockfd_udp_channel false!!!\n");
	    return -1;
	}

	unsigned short int PROT = PORT_CHANNEL;
	bzero(&var->server_addr_channel, sizeof(var->server_addr_channel));
	var->server_addr_channel.sin_family = AF_INET;
    var->server_addr_channel.sin_addr.s_addr = htonl(INADDR_ANY);
    var->server_addr_channel.sin_port = htons(PROT);

    if (bind(var->sockfd_udp_channel, (struct sockaddr *)&var->server_addr_channel, sizeof(var->server_addr_channel)) < 0) {
    	pr_err("udp_channel_init---> Can not bind!!!\n");
        return -2;
    }

    return 0;
}

/*
 * Initializes the original link layer socket
 */
int raw_skt_init(struct local_variable *var)
{
	var->sockfd_raw = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(var->sockfd_raw < 0) {
		perror("error to create raw socket!!!\n");
		return -1;
	}

	memset(&var->raw_addr_dst, 0, sizeof(struct sockaddr_ll));
	var->raw_addr_dst.sll_family = AF_PACKET;
	var->raw_addr_dst.sll_protocol = htons(ETH_P_ALL);
	strcpy(var->req.ifr_name, NIC_NAME);                    //NIC_NAME = "eth0" is the NIC name
	ioctl(var->sockfd_raw, SIOCGIFINDEX, &var->req);        //Get the network interface
	var->raw_addr_dst.sll_ifindex =	var->req.ifr_ifindex;
	var->raw_addr_dst.sll_pkttype =	PACKET_OUTGOING;		//TODO
	int nSendBuf_raw = 1024*1024;
	int nRecvBuf_raw = 1024*1024;
	setsockopt(var->sockfd_raw, SOL_SOCKET, SO_RCVBUF, (const void *)&nRecvBuf_raw, sizeof(int));
	setsockopt(var->sockfd_raw, SOL_SOCKET, SO_SNDBUF, (const void *)&nSendBuf_raw, sizeof(int));

	int recv,send;
	socklen_t len1 = sizeof(recv),len2 = sizeof(send);
	getsockopt(var->sockfd_raw, SOL_SOCKET, SO_RCVBUF, &recv, &len1);
	getsockopt(var->sockfd_raw, SOL_SOCKET, SO_SNDBUF, &send, &len2);
	pr_info("recv buf = %d, send buf = %d\n", recv, send);

	memset(&var->raw_addr_src, 0, sizeof(struct sockaddr_ll));
	var->raw_addr_src.sll_family = AF_PACKET;
	var->raw_addr_src.sll_protocol = htons(ETH_P_ALL);
	var->raw_addr_src.sll_ifindex =	var->req.ifr_ifindex;
	var->raw_addr_src.sll_pkttype =	PACKET_OTHERHOST;

    /*if (bind(var->sockfd_raw, (struct sockaddr *)&var->raw_addr_src, sizeof(struct sockaddr_ll)) < 0) {
    	printf("raw_skt_init---> Can not bind!!!\n");
        return -1;
    }*/

	// Set promisc
	if (-1 == ioctl(var->sockfd_raw, SIOCGIFFLAGS, &var->req)) {	 //Get the network interface flag
	   perror("ioctl");
	   close(var->sockfd_raw);
	   return (-1);
	}
	var->req.ifr_flags |= IFF_PROMISC;
	if (-1 == ioctl(var->sockfd_raw, SIOCSIFFLAGS, &var->req)) {    //Set the network card in promisc
	   perror("ioctl");
	   close(var->sockfd_raw);
	   return (-1);
	}
	pr_warn("Set Promisc successfully\n");

	// Set Non_bolck
	int flag = fcntl(var->sockfd_raw, F_GETFL, 0);
	if (flag < 0) {
	    perror("fcntl F_GETFL fail");
	}
	if (fcntl(var->sockfd_raw, F_SETFL, flag | O_NONBLOCK) < 0) {
	    perror("fcntl F_SETFL fail");
	}

	return 0;
}

/*
 * Timer processing: data sending function
 */
void sig_tx_handler(int signum, siginfo_t *info, void *myact)
{
	struct sockaddr_ll addr_src;
	socklen_t len = sizeof(struct sockaddr_ll);

	// The UDP data sent to the local host is obtained preferentially
	int recvcount = recvfrom(local_var.sockfd_udp_trans, local_var.tx_buf, data_len_byte_1552, 0, (struct sockaddr *)&local_var.client_addr_trans, &local_var.len);
	if (recvcount > 1) {
		local_var.tx_buf[1520] = (u8_t)((recvcount) >> 24);
		local_var.tx_buf[1521] = (u8_t)((recvcount << 8 ) >> 24);
		local_var.tx_buf[1522] = (u8_t)((recvcount << 16) >> 24);
		local_var.tx_buf[1523] = (u8_t)((recvcount << 24) >> 24);
		local_var.tx_buf[1524] = 2;
		send_data(&local_var, local_var.tx_buf, data_len_byte_1552);
		memset(local_var.tx_buf, 0, data_len_byte_1552);
		pr_info("====== udp tx %d bytes (%#x)\n", recvcount, recvcount);
		return;
	}

	// Secondly, the captured data of Ethernet network card is obtained
	recvcount = recvfrom(local_var.sockfd_raw, local_var.tx_buf, data_len_byte_1552, 0, (struct sockaddr *)&addr_src, &len);
	if (recvcount >= 10 && addr_src.sll_pkttype != PACKET_OUTGOING && addr_src.sll_pkttype != PACKET_HOST) {
		local_var.tx_buf[1520] = (u8_t)((recvcount) >> 24);
		local_var.tx_buf[1521] = (u8_t)((recvcount << 8 ) >> 24);
		local_var.tx_buf[1522] = (u8_t)((recvcount << 16) >> 24);
		local_var.tx_buf[1523] = (u8_t)((recvcount << 24) >> 24);
		local_var.tx_buf[1524] = 1;
		send_data(&local_var, local_var.tx_buf, data_len_byte_1552);
		memset(local_var.tx_buf, 0, data_len_byte_1552);
		pr_info("====== raw tx %d bytes (%#x)\n", recvcount, recvcount);
		pr_info("====== raw type = %d\n", addr_src.sll_pkttype);
		return;
	}
}

/*
 * Reference, has been deprecated
 */
int udp_skt_init_old(struct local_variable *var)
{
	unsigned short int PC_PORT = PORT_PC;
	if ((var->sockfd_udp_trans = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	      printf("create sockfd_send SOCK_DGRAM false!!!\n");
	      return -1;
	}

    //PC side address information
	var->server_addr_trans.sin_family = AF_INET;
	inet_pton(AF_INET, IPADDR_PC, &var->server_addr_trans.sin_addr);
	//var->server_addr_send.sin_addr.s_addr = inet_addr("192.168.2.1");
	var->server_addr_trans.sin_port = htons(PC_PORT);

    unsigned short int linux_PORT = PORT_PC;
    if ((var->sockfd_udp_trans = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("create sockfd_recv SOCK_DGRAM false!!!\n");
        return -1;
    }

    //Binding Local Port
    var->len = sizeof(var->server_addr_trans);
    var->server_addr_trans.sin_family = AF_INET;
    var->server_addr_trans.sin_addr.s_addr = htonl(INADDR_ANY);
    var->server_addr_trans.sin_port = htons(linux_PORT);
    if (bind(var->sockfd_udp_trans, (struct sockaddr *)&var->server_addr_trans, sizeof(var->server_addr_trans)) < 0) {
         printf("Can not bind!!!\n");
         return -1;
    }

	return 0;
}
