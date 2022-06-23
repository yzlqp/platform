/**
 * @file utility.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2022-04-07 15:22
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <termios.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include "type.h"
#include <bits/sigaction.h>
#include "local_variables.h"
#include "parameters.h"
#include "port.h"
#include "socket_api.h"
#include "ad9361.h"
#include "channel.h"


typedef void (*sigfunc)(int);
typedef void (*sigfunaction) (int, siginfo_t *, void *);
/*
 * Print level and current print level
 */
#define ERR_LEVEL 	1
#define WARN_LEVEL 	2
#define INFO_LEVEL 	3
#define DEBUG_LEVEL 2

/*
 * little-endian big-endian transformation macro function
 */
#define __SWP16(A) \
	((( (uint16)(A) & 0xff00) >> 8) | (( (uint16)(A) & 0x00ff) << 8))

#define __SWP32(A)   ((( (uint32)(A) & 0xff000000) >> 24) | \
		(( (uint32)(A) & 0x00ff0000) >> 8 )   | \
		(( (uint32)(A) & 0x0000ff00) << 8 )   | \
		(( (uint32)(A) & 0x000000ff) << 24))

/*
 * container_of macro function
 */
#define offsetof(TYPE, MEMBER)	((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({ \
		const typeof(((type *)0)->member) *_mptr = (ptr); \
		(type *)((char *)_mptr - offsetof(type, member)); })



/*
 * Initialize local variable
 */
int init_local_variable(struct local_variable *local_varp);

/*
 * Initialize the system service thread
 */
int init_thread(struct local_variable *var);

/*
 * Custom printing functions: information
 */
void __attribute__((format(printf,1,2))) pr_info(char* fmt, ...);

/*
 * Custom printing functions: warning
 */
void __attribute__((format(printf,1,2))) pr_warn(char* fmt, ...);

/*
 * Custom printing functions: error
 */
void __attribute__((format(printf,1,2))) pr_err(char* fmt, ...);

/*
 * Custom printing functions: perror
 */
void pr_errno(char* str);

/*
 * Initialize the system rx signal handler function
 */
int init_rx_signal(struct local_variable *var);

/*
 * Initialize the system tx signal handler function
 */
int init_tx_signal(struct local_variable *var);

/*
 * Generic signal binding function
 */
sigfunc combine_signal(int signo, sigfunaction func);

/*
 * Print process pid and thread tid
 */
void printtids(const char *s);

/*
 * Used for pthread once
 */
void thread_init(void);

/*
 * Used to initialize baseband self-loop tests
 */
int loop_test(struct local_variable *local_varp);

/*
 * Get local time, plus the specified number of minutes, return timespec
 */
void maketimeout(struct timespec *tsp, long minutes);

/*
 * Initialize a queue
 */
int queue_init(struct queue *qp);

/*
 * Insert a job at the head of the queue
 */
void job_insert(struct queue *qp, struct job *jp);

/*
 * Append a job at the tail of the queue
 */
void job_append(struct queue *qp, struct job *jp);

/*
 * Remove the given job from a queue
 */
void job_remove(struct queue *qp, struct job *jp);

/*
 * Find a job for the given thread ID
 */
struct job *job_find(struct queue *qp, pthread_t id);

/*
 * Used to test time delays
 */
void timer_test(void);

#endif /* UTILITY_H */
