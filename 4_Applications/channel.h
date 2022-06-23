/**
 * @file channel.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "utility.h"

/*
 * Channel emulation module handles threads
 */
void *chan_func(void *arg);

/*
 * Channel emulation module handles thread initialization
 */
void init_chan_config(int **chan_bufp, int **vaddr, struct local_variable *varp);

/*
 * Channel emulation module data processing
 */
int chan_config(int *bufp, int *vaddr, struct sockaddr *client_addr, struct local_variable *varp);


#endif /* CHANNEL_H */
