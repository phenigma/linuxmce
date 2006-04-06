ifneq ($(KERNELRELEASE),)
	obj-m	:= vloopback.o

else
	KVER  := $(shell uname -r)
	KLINK := $(shell test -e /lib/modules/${KVER}/source/ && echo source || echo build)
	KSRC  := /lib/modules/$(KVER)/$(KLINK)
	PWD		:= $(shell pwd)
	DEST	:= /lib/modules/$(KVER)/kernel/drivers/misc
default: replaces
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

replaces:
	for i in postinst; do \
	    cat mkr_$$i.sh.in | sed  -e 's/_KVER_/$(KVER)/g'  > mkr_$$i.sh ;\
	    chmod +x mkr_$$i.sh ;\
	done

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
