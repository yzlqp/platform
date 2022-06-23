KERN_DIR := /home/y/project/yan2/linux/linux-2019_R1

obj-m := axi_ctrl_interface.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` clean