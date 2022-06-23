/**
 * @file main.c
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-05-09
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "utility.h"


struct local_variable local_var;

int main(int argc, char **argv)
{
    int ret;

	ret = init_local_variable(&local_var);
    if (ret) {
    	pr_err("init_local_variable: err = %d\n", ret);
        exit(-1);
    }

	ret = init_platform(&local_var);
    if (ret) {
    	pr_err("init_platform: err = %d\n", ret);
        exit(-1);
    }

    ret = init_network(&local_var);
    if (ret) {
    	pr_err("udp_server_init: err = %d\n", ret);
        exit(-1);
    }

    ret = init_thread(&local_var);
    if (ret) {
    	pr_err("init_thread: err = %d\n", ret);
        exit(-1);
    }

    printtids("main thread:");
    while (1) {
    	pthread_mutex_lock(&local_var.mutex_lock);
    	while (local_var.flag == 0) {
        	pthread_cond_wait(&local_var.waitloc, &local_var.mutex_lock);
        }
        pthread_mutex_unlock(&local_var.mutex_lock);

        // SIGUSR2 has been caught and is now blocked; do whatever
        local_var.flag = 0;

        // TODO something
        pr_warn("SIGUSR2 has been caught and is now blocked\n");
    }
    return 0;
}
