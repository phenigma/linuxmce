# By default, the build is done against the running kernel version.
# to build against a different kernel version, set KVER
#
#  make KVER=2.6.11-alpha
#
#  Alternatively, set KDIR
#
#  make KDIR=/usr/src/linux

MDIR := ivtv

KVER ?= $(shell uname -r)
KDIR ?= /lib/modules/$(KVER)/build

all:: ivtv-svnversion.h

ifeq ($(wildcard $(KDIR)/Rules.make),)

ifneq ($(KERNELRELEASE),)
include $(src)/Kbuild
else

all::
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

install:: all
	$(MAKE) INSTALL_MOD_PATH=$(DESTDIR) INSTALL_MOD_DIR=$(MDIR) \
		-C $(KDIR) M=$(CURDIR) modules_install

clean::
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

endif
else
include $(CURDIR)/Makefile2.4
endif

install::
	/sbin/depmod -a
ifneq ($(wildcard saa7127.o),)
	@for m in ` find /lib/modules/$(KVER)/kernel -name msp3400.ko -o -name tuner.ko -o -name tda9887.ko -o -name tveeprom.ko ` ; \
	 do \
	 	if ! cmp -s $$m `basename $$m` ; \
		then \
			echo "Module $$m conflicts with the ivtv module of the same name -- please hide or delete it." ; \
			echo "To hide:  mv $$m $$m.HIDE" ; \
			echo "You will then need to run depmod." ; \
		fi ; \
	 done
endif
	@echo
	@echo "v4l-cx2341x-init.mpg needs copying to the hotplug firmware"
	@echo "directory if needed for PVR350 mpeg initialization"

clean::
	rm -f ivtv-svnversion.h ivtv-svnversion.h.tmp

ivtv-svnversion.h:
	@if [ -f ivtv-svnrelease.h ]; then \
		echo '#include "ivtv-svnrelease.h"' > $@.tmp; \
	elif [ -d .svn ]; then \
		echo '#define IVTV_DRIVER_VERSION_COMMENT' \
		  '"(development revision' \
		  "`svnversion -nc . | sed -e s/^[^:]*://`"')"' > $@.tmp; \
	else date +'#define IVTV_DRIVER_VERSION_COMMENT "(development snapshot compiled on %c)"' > $@.tmp; fi
	@(chmod 666 $@.tmp 2> /dev/null || /bin/true)
	@cmp -s $@ $@.tmp || (mv $@.tmp $@ ; echo created $@)

.PHONY: ivtv-svnversion.h
