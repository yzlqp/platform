/**
 * @file ad9361.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-09
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "ad9361.h"

/*
 * The AD9361 rf module handles threads
 */
void *rf_func(void *arg)
{
	printtids("rf thread:");

	struct local_variable *varp = (struct local_variable *)arg;

	pthread_mutex_lock(&varp->mutex1);
	char *rfbuf;
	rfbuf = (char *)pthread_getspecific(varp->key);
	if(rfbuf == NULL) {
		rfbuf = (char *)malloc(MAXSTRINGSZ);
		if(rfbuf == NULL) {
			pthread_mutex_unlock(&varp->mutex1);
			pr_err("rf_func: malloc err!!!\n");
			return NULL;
		}
	}
	pthread_setspecific(varp->key, rfbuf);
	pthread_mutex_unlock(&varp->mutex1);

	socklen_t len;
	struct sockaddr_in client_addr;

	while (1) {
		int num = 0;
		pr_info("RF listening...\n");
		num = recvfrom(varp->sockfd_udp_9361, rfbuf, MAXSTRINGSZ, 0, (struct sockaddr *)&client_addr, &len);
		pr_info("====== RF rx %d bytes (%#x)\n", num, num);
		num = rf_config(varp->sockfd_udp_9361, rfbuf, (struct sockaddr *)&client_addr);
		pr_info("====== RF tx %d bytes (%#x)\n", num, num);
		memset(rfbuf, 0, MAXSTRINGSZ);
	}

	printtids("rf thread close:");
	return ((void *)0);
}

/*
 * AD9361 Rf module configuration information processing
 */
int rf_config(int fd_sock, void *bufp, struct sockaddr *client_addr)
{
	int fd, num, sendnum;
	char path[128] = {0};
	ad9361_msg_t *msgp = (ad9361_msg_t *)bufp;
	unsigned int count = msgp->count;

	if(count <= 0) {
		pr_warn("Count error %u\n", count);
		return -1;
	}

	for(int i = 0; i < count; i++) {
		strcpy(path, "/sys/bus/iio/devices/iio:device0/");
		switch(msgp->mode) {
		case 'w':
			strcat(path, msgp->name);
			fd = open(path, O_RDWR);
			if (fd < 0) {
				pr_err("case 'w' : open error\n");
				return -1;
			}
			char tmp[32] = {0};
			write(fd, msgp->strval, sizeof(msgp->strval));
			pread(fd, tmp, sizeof(msgp->strval), 0);
			memset(msgp->strval, 0, 32);
			strcpy(msgp->strval, tmp);
			msgp->ack = '1';
			close(fd);
			break;

		case 'r':
			strcat(path, msgp->name);
			fd = open(path, O_RDONLY);
			if (fd < 0) {
				pr_err("case 'r' : open error\n");
				return -1;
			}
			char tmp1[32] = {0};
			read(fd, tmp1, sizeof(msgp->strval));
			memset(msgp->strval, 0, 32);
			strcpy(msgp->strval, tmp1);
			msgp->ack = '1';
			close(fd);
			break;

		case 'c':
			strcat(path, "name");
			fd = open(path, O_RDONLY);
			if (fd < 0) {
				pr_err("case 'c' : open error\n");
				return -1;
			}
			memset(msgp->strval, 0, 32);
			num = read(fd, msgp->strval, sizeof(msgp->strval));
			if (num <= 0) {
				pr_err("case 'c' : read error\n");
				return -1;
			}

			if(!strcmp(msgp->strval,"ad9361-phy\n"))
				msgp->ack = '1';
			else
				pr_err("no detect ad9361\n");
			close(fd);
			break;

		default:
			pr_err("instruction err!!!\n");
		}
		msgp++;
	}
	sendnum = sendto(fd_sock, bufp, sizeof(ad9361_msg_t) * count, 0, (struct sockaddr *)client_addr, sizeof(struct sockaddr));

	return sendnum;
}

