/**
 * @file utility.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "utility.h"

extern struct local_variable local_var;

/*
 * Initialize local variable
 */
int init_local_variable(struct local_variable *var)
{

	memset(var, 0, sizeof(struct local_variable));
	var->tx_buf = (unsigned char *)malloc(data_len_byte_1552);
	var->rx_buf = (unsigned char *)malloc(data_len_byte_1552);
	if (var->tx_buf == NULL || var->rx_buf == NULL) {
		pr_err("init_local_variable->malloc error!!!\n");
		return -1;
	}
	memset(var->tx_buf, 0, data_len_byte_1552);
	memset(var->rx_buf, 0, data_len_byte_1552);

	if (pthread_mutex_init(&var->mutex1, NULL) != 0) {
		pr_err("init_local_variable->pthread_mutex_init_0 error!!!\n");
		return -2;
	}

	if (pthread_mutex_init(&var->mutex2, NULL) != 0) {
		pr_err("init_local_variable->pthread_mutex_init_1 error!!!\n");
		return -3;
	}

	if (pthread_mutex_init(&var->mutex_lock, NULL) != 0) {
		pr_err("init_local_variable->pthread_mutex_init_2 error!!!\n");
		return -4;
	}

	if (pthread_cond_init(&var->waitloc, NULL) != 0) {
		pr_err("init_local_variable->pthread_cond_init error!!!\n");
		return -5;
	}

	var->init_done = PTHREAD_ONCE_INIT;
	pthread_once(&var->init_done, thread_init);

	return 0;
}

/*
 * Initialize the system service thread
 */
int init_thread(struct local_variable *var)
{
	int ret;
	pthread_t chantid, rftid, rxtid, timertid;
	pthread_attr_t chanattr, rfattr, rxattr, timerattr;

	sigemptyset(&var->mask);
	sigaddset(&var->mask, SIGUSR1);
	sigaddset(&var->mask, SIGUSR2);
	sigaddset(&var->mask, SIGIO);
	sigaddset(&var->mask, __SIGRTMIN);

	ret = pthread_sigmask(SIG_BLOCK, &var->mask, &var->oldmask);
	if (ret != 0) {
		pr_err("pthread_sigmask-> sig block err: err = %d\n",ret);
	}

	ret = pthread_attr_init(&chanattr);
	if (ret != 0) {
		return ret;
	}

	ret = pthread_attr_init(&rfattr);
	if (ret != 0) {
		return ret;
	}

	ret = pthread_attr_init(&rxattr);
	if (ret != 0) {
		return ret;
	}

	ret = pthread_attr_init(&timerattr);
	if (ret != 0) {
		return ret;
	}

	ret = pthread_attr_setdetachstate(&chanattr, PTHREAD_CREATE_DETACHED);
	if (ret == 0) {
		ret = pthread_create(&chantid, &chanattr, chan_func, var);
		if (ret != 0) {
			pr_err("can not create chan_thread!!!\n");
			return (-4);
		}
	}

	ret = pthread_attr_setdetachstate(&rxattr, PTHREAD_CREATE_DETACHED);
	if (ret == 0) {
		ret = pthread_create(&rxtid, &rxattr, rx_func, var);
		if (ret != 0) {
			pr_err("can not create rx_udp_thread!!!\n");
			return (-4);
		}
	}

	ret = pthread_attr_setdetachstate(&timerattr, PTHREAD_CREATE_DETACHED);
	if (ret == 0) {
		ret = pthread_create(&timertid, &timerattr, timer_func, var);
		if (ret != 0) {
			pr_err("can not create rf_thread!!!\n");
			return (-4);
		}
	}

	ret = pthread_attr_setdetachstate(&rfattr, PTHREAD_CREATE_DETACHED);
	if (ret == 0) {
		ret = pthread_create(&rftid, &rfattr, rf_func, var);
		if (ret != 0) {
			pr_err("can not create rf_thread!!!\n");
			return (-4);
		}
	}

	return ret;
}

/*
 * Custom printing functions: information
 */
void __attribute__((format(printf,1,2))) pr_info(char* fmt, ...)
{
#if (DEBUG_LEVEL >= INFO_LEVEL)
	va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
#endif
}

/*
 * Custom printing functions: warning
 */
void __attribute__((format(printf,1,2))) pr_warn(char* fmt, ...)
{
#if (DEBUG_LEVEL >= WARN_LEVEL)
    va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
#endif
}

/*
 * Custom printing functions: error
 */
void __attribute__((format(printf,1,2))) pr_err(char* fmt, ...)
{
#if (DEBUG_LEVEL >= ERR_LEVEL)
    va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
#endif
}

/*
 * Custom printing functions: perror
 */
void pr_errno(char* str)
{
#if (DEBUG_LEVEL >= ERR_LEVEL)
	extern int errno;
	char *estr = strerror(errno);
	printf("%s: %s\n", str, estr);
#endif
}

/*
 * Initialize the system rx signal handler function
 */
int init_rx_signal(struct local_variable *var)
{
	int flags;

	if(combine_signal(SIGIO, sig_rx_handler) == SIG_ERR) {
		pr_err("combine signal error!!!");
		return -1;
	}

	fcntl(var->fd_axi_ctl, F_SETOWN, getpid());
	flags = fcntl(var->fd_axi_ctl, F_GETFL);
	fcntl(var->fd_axi_ctl, F_SETFL, flags | FASYNC);
	fcntl(var->fd_axi_ctl, __F_SETSIG, SIGIO);

	return 0;
}

/*
 * Initialize the system tx signal handler function
 */
int init_tx_signal(struct local_variable *var)
{
	int flags;

	if(combine_signal(SIGUSR1, sig_tx_handler) == SIG_ERR) {
		pr_err("combine signal error!!!");
		return -1;
	}

	fcntl(var->fd_timer, F_SETOWN, getpid());
	flags = fcntl(var->fd_timer, F_GETFL);
	fcntl(var->fd_timer, F_SETFL, flags | FASYNC);
	fcntl(var->fd_timer, __F_SETSIG, SIGUSR1);

	return 0;
}

/*
 * Signal binding function
 */
sigfunc combine_signal(int signo, sigfunaction func)
{
	struct sigaction act, oact;
	sigemptyset(&act.sa_mask);
	//sigaddset(&act.sa_mask, SIGIO);

	// Set the signal processing function corresponding to SIGIO
	act.sa_sigaction = func;

	// When sa_flags contain SA_SIGINFO, the system uses sa_sigaction as the signal handler, otherwise sa_handler is used
	act.sa_flags = SA_SIGINFO;

	if (signo == SIGALRM) {
		act.sa_flags |= SA_INTERRUPT;
	} else {
		act.sa_flags |= SA_RESTART;
	}

	if (sigaction(signo, &act, &oact) < 0) {
		return SIG_ERR;
	}

	return oact.sa_handler;
}

/*
 * Print process pid and thread tid
 */
void printtids(const char *s)
{
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();
	pr_info("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

/*
 * Used for pthread once
 */
void thread_init(void)
{
	pthread_key_create(&local_var.key, free);
}

/*
 * Initialize a queue
 */
int queue_init(struct queue *qp)
{
	int err;

	qp->q_head = NULL;
	qp->q_tail = NULL;
	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if(err != 0) {
		return (err);
	}

	return 0;
}

/*
 * Insert a job at the head of the queue
 */
void job_insert(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;
	if(qp->q_head != NULL) {
		qp->q_head->j_prev = jp;
	} else {
		qp->q_tail = jp;  /* list was empty*/
	}
	qp->q_head = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job at the tail of the queue
 */
void job_append(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = NULL;
	jp->j_prev = qp->q_tail;
	if(qp->q_tail != NULL) {
		qp->q_tail->j_next = jp;
	} else {
		qp->q_head = jp;  /* list was empty*/
	}
	qp->q_tail = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}


/*
 * Remove the given job from a queue
 */
void job_remove(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	if(jp == qp->q_head) {
		qp->q_head = jp->j_next;
		if(qp->q_tail == jp)
			qp->q_tail = NULL;
		else
			jp->j_next->j_prev = jp->j_prev;
	} else if(jp == qp->q_tail) {
		qp->q_tail = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID
 */
struct job *job_find(struct queue *qp, pthread_t id)
{
	struct job *jp;

	if(pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return NULL;

	for(jp = qp->q_head; jp != NULL; jp = jp->j_next)
		if(pthread_equal(jp->j_id, id))
			break;

	pthread_rwlock_unlock(&qp->q_lock);
	return(jp);
}

/*
 * Get local time, plus the specified number of minutes, return timespec
 */
void maketimeout(struct timespec *tsp, long minutes)
{
	struct timeval now;

	/* get the current time */
	gettimeofday(&now, NULL);
	tsp->tv_sec = now.tv_sec;
	tsp->tv_nsec = now.tv_usec * 1000;  /* usec to nsec */
	/* add the offset to get timeout value */
	tsp->tv_sec += minutes * 60;
}

/*
 * Used to initialize baseband self-loop tests
 */
int loop_test(struct local_variable *local_varp)
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < data_len_byte_1552; i++) {
		local_varp->tx_buf[i] = rand() % 256;
	}
	memset(local_varp->rx_buf, 0, data_len_byte_1552);

	struct timeval startTime, endTime;
	long long Timeuse;
	int size = data_len_byte_1552;
	gettimeofday(&startTime, NULL);

	memcpy(local_varp->map_bramctl_tx, local_varp->tx_buf, data_len_byte_1552);
	ioctl(local_varp->fd_axi_ctl, IOC_TX_START_COUNT, &size);
	usleep(10*1000);
	memcpy(local_varp->rx_buf, local_varp->map_bramctl_rx, data_len_byte_1552);

#if 0
	//Write data to BRAM via AXI_bram_ctrl_0
	for (unsigned int i = 0; i< data_len_byte_4K/4; i++)
	{
		unsigned int context = 0;
		for(int j = 0; j < 4; j++)
		{
		    context |= ((unsigned int)send_buffer[i*4+j])<<((3-j)*8);
		}
		map_base0[i] = context;
    }

    for (unsigned int i = 0; i <  data_len_byte_4K/4; i++)
    {
    	int numa = 4*i;
    	int numb = 4*i+1;
		int numc = 4*i+2;
		int numd = 4*i+3;
		unsigned int context = *(map_base1 + i) ;
        *(recv_buffer + numa) = (unsigned char)((context) >> 24);
        *(recv_buffer + numb) = (unsigned char)((context << 8 ) >> 24);
        *(recv_buffer + numc) = (unsigned char)((context << 16) >> 24);
        *(recv_buffer + numd) = (unsigned char)((context << 24) >> 24);
    }
#endif

	gettimeofday(&endTime, NULL);
	Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);

	int err = 0;
	for (int i = 0; i < data_len_byte_1552; i++) {
		if (local_varp->tx_buf[i] != local_varp->rx_buf[i]) {
			err++;
		}
	}
	pr_info("FPGA loop test: Timeuse = %lld us, err = %d(%d)\n", Timeuse, err, data_len_byte_1552);

	memset(local_varp->tx_buf, 0, data_len_byte_1552);
	memset(local_varp->rx_buf, 0, data_len_byte_1552);

    return (-err);
}


/*
 * Used to test time delays
 */
void timer_test(void)
{
	static struct timeval curtime;
	struct timeval pretime;
	pretime.tv_usec = curtime.tv_usec;
	gettimeofday(&curtime, NULL);
	pr_warn("time delay: %ld us\n", curtime.tv_usec - pretime.tv_usec);
}

