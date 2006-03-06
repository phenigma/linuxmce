ifneq ($(KERNELRELEASE),)
	obj-m	:= vloopback.o

else
	KVER  := $(shell uname -r)
	KLINK := $(shell test -e /lib/modules/${KVER}/source/ && echo source || echo build)
	KSRC  := /lib/modules/$(KVER)/$(KLINK)
	PWD		:= $(shell pwd)
	DEST	:= /lib/modules/$(KVER)/kernel/drivers/misc
default:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

install:
	cp vloopback.ko $(DEST)/.
	/sbin/depmod -a
	/sbin/modprobe videodev 

uninstall:
	rm -f $(DEST)/vloopback.ko
	/sbin/depmod -a
clean:
	rm -f .*.cmd *.o *.mod.c *.ko .v* *~ core
	rm -rf .tmp_versions/
endif
