# The build is done against the running kernel unless you set KVER
# make KVER=2.6.22

ifneq ($(KERNELRELEASE),)
	include $(src)/Kbuild
else
	PWD  := $(shell pwd)
	KVER ?= $(shell uname -r)
	KDIR := /lib/modules/$(KVER)/build
endif

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	Dir=$(DESTDIR)/lib/modules/$(KVER)/kernel/drivers/usb/serial; \
	mkdir -p $$Dir; \
	cp spcp8x5.ko $$Dir

clean:
	rm -f *.ko *.o

