ifneq ($(KERNELRELEASE),)
	obj-m	:= vloopback.o

else
	KDIR	:= /lib/modules/$(shell uname -r)/build
	PWD		:= $(shell pwd)
	DEST	:= /lib/modules/$(shell uname -r)/kernel/drivers/misc
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	cp vloopback.ko $(DEST)/.
	depmod -a
	modprobe videodev 

uninstall:
	rm -f $(DEST)/vloopback.ko
	depmod -a
clean:
	rm -f .*.cmd *.o *.mod.c *.ko *~ core
	rm -rf .tmp_versions/
endif
