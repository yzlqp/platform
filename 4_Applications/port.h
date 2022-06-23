/**
 * @file port.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-04-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef PORT_H
#define PORT_H

#include "utility.h"


/*
 * Platform initialization
 */
int init_platform(struct local_variable *var);

/*
 * Signal processing thread for receive data
 */
void *rx_func(void *arg);

/*
 * Scheduling threads in time slots for timer
 */
void* timer_func(void *arg);

/*
 * General purpose signal processing functions
 */
void sig_rx_handler(int signum, siginfo_t *info, void *myact);

/*
 * UDP and RAW data receiving signal processing function
 */
void rx_data_udp(void);

/*
 * PL side reset function
 */
static inline int pl_rst(struct local_variable *var);

/*
 * Timer Interrupt Request
 */
int timer_request(struct local_variable *var);

/*
 * Send data to FPGA
 */
static inline __attribute__((always_inline)) int send_data(struct local_variable *var, uint8_t *send_buffer, uint32_t size);

/*
 * Read data from FPGA
 */
static inline __attribute__((always_inline)) int read_data(struct local_variable *var, uint8_t *read_buffer, uint32_t size);

/*
 * Notify FPGA that data reading is complete
 */
static inline __attribute__((always_inline)) int rx_end(struct local_variable *var);

/*
 * Send data to FPGA
 */
static inline int __attribute__((always_inline)) send_data(struct local_variable *var, uint8_t *send_buffer, uint32_t size)
{
	int ret;

	memcpy(var->map_bramctl_tx, send_buffer, size);
    ret = ioctl(var->fd_axi_ctl, IOC_TX_START_COUNT, &size);
    return ret;
}

/*
 * Read data from FPGA
 */
static inline int __attribute__((always_inline)) read_data(struct local_variable *var, uint8_t *read_buffer, uint32_t size)
{
	//int ret = 0;
	memcpy(read_buffer, var->map_bramctl_rx, size);

    return 0;
}

/*
 * Notify FPGA that data reading is complete
 */
static inline int __attribute__((always_inline)) rx_end(struct local_variable *var)
{
	return ioctl(var->fd_axi_ctl, IOC_RX_END);
}

/*
 * PL side reset function
 */
static inline int pl_rst(struct local_variable *var)
{
	return ioctl(var->fd_axi_ctl, IOC_PLRESET);
}


#endif /* PORT_H */
