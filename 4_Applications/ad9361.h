/**
 * @file ad9361.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-09
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef AD9361_H
#define AD9361_H

#include "utility.h"

#define NAMELEN	 128
#define VALUELEN 32

typedef struct {
	unsigned char count;      // Configuration information count
    char name[NAMELEN];       // Name of Configuration Information ,such as "in_voltage0_hardwaregain"
    char mode;                // Configuration mode: 'r' read-only,  'w' Written then reply, 'c' driver detection
    char flag;                // The abandoned flag
    char ack;			      // Acknowledge
    char strval[VALUELEN];    // Configuration values
    long long num;            // Reserved
}ad9361_msg_t;


/*
 * The AD9361 rf module handles threads
 */
void *rf_func(void *arg);

/*
 * AD9361 Rf module configuration information processing
 */
int rf_config(int fd_sock, void *bufp, struct sockaddr *client_addr);


#endif /* AD9361_H */
