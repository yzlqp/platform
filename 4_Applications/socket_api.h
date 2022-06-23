/**
 * @file socket_api.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-08
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOCKET_API_H
#define SOCKET_API_H

#include "local_variables.h"

/*
 * Initialize the socket interface
 */
int init_network(struct local_variable *var);

/*
 * Timer processing: data sending function
 */
void sig_tx_handler(int signum, siginfo_t *info, void *myact);

/*
 * Initialize the UDP data transfer socket
 */
int udp_trans_init(struct local_variable *var);

/*
 * Initialize the UDP radio module AD9361 socket
 */
int udp_9361_init(struct local_variable *var);

/*
 * Initialize the UDP channel analog module socket
 */
int udp_channel_init(struct local_variable *var);

/*
 * Initializes the original link layer socket
 */
int raw_skt_init(struct local_variable *var);

#endif /* SOCKET_API_H */
