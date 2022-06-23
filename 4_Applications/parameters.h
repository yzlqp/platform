/**
 * @file parameters.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-07-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <sys/ioctl.h>
#include <asm-generic/ioctl.h>

#define PORT_PC     	8886
#define PORT_TRANS 	 	6666
#define PORT_CHANNEL 	7777
#define PORT_9361 	 	9361

#define IPADDR_PC   	"192.168.2.1"
#define NIC_NAME        "eth0"

#define BRAM_CTRL_SEND      0x40000000
#define BRAM_CTRL_RECV      0x42000000
#define CHAN_CTRL_ADDR      0x45C00000

#define MAXSTRINGSZ 		4096
#define data_len_byte_1552  1552
#define data_len_byte_4K 	(1024*4)
#define data_len_byte_12K   (1024*12)  //12288 Byte


/*
 * IOCTL custom commands
 */
#define  IOC_MAGIC_TYPE              's'                                // Define IO device type/magic number
#define  IOC_PLRESET                _IO(IOC_MAGIC_TYPE, 0)              // Initialize the system to reset PL
#define  IOC_TX_START_COUNT         _IOW(IOC_MAGIC_TYPE, 1, int)        // Write registers TX_START and TX_COUNT
#define  IOC_RX_COUNT               _IOR(IOC_MAGIC_TYPE, 2, int)        // Read register RX_COUNT
#define  IOC_RX_END                 _IO(IOC_MAGIC_TYPE, 3)              // Notifies the PL end that data has been fetched and can be notified
#define  IOC_TIMER_INTR_EN          _IOW(IOC_MAGIC_TYPE, 4, int)        // Timer (interrupt number can be customized) interrupt enable
#define  IOC_TIMER_INTR_DIS         _IOW(IOC_MAGIC_TYPE, 5, int)        // Timer (interrupt number can be customized) interrupt disable
#define  IOC_MAXNR                  5                                   // IOC numbers


#endif /* PARAMETERS_H */
