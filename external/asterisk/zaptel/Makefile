#
# Makefile for tormenta/carrier driver and utilities
# $Id: Makefile,v 1.69 2005/02/04 05:55:51 markster Exp $
#
BASEADDR=0xd0000

#
# Okay, the people at RedHat have to break everything they can possibly even attempt to.
# So, we have to look in /usr/src/linux-2.4/include for header files given their brain dead
# crappy installation.  (Mind you, I'm a RedHat user myself, so I suppose I'm just as
# stupid as they are).  Everyone else who is mildly sane of course links /usr/include/linux
# to their working kernel source directory, the way God himself does, of course
# (assuming He's running Linux -- which we all know He must).
#
HOSTCC=gcc
KINCLUDES=$(shell if [ -d /usr/src/linux-2.4/include ]; then echo /usr/src/linux-2.4/include ; else echo /usr/src/linux/include ; fi)

CFLAGS+=-I. -O4 -g -Wall -DBUILDING_TONEZONE #-DTONEZONE_DRIVER
CFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-fsigned-char"; fi)
CFLAGS+=$(shell if uname -m | grep -q x86_64; then echo "-m64"; fi)
LCFLAGS=-fPIC $(CFLAGS) -DBUILDING_TONEZONE
KFLAGS+=-I/usr/src/linux-2.4/include -O6
KFLAGS+=-DMODULE -D__KERNEL__ -DEXPORT_SYMTAB -I/usr/src/linux/drivers/net \
	-Wall -I. -Wstrict-prototypes -fomit-frame-pointer -I/usr/src/linux/drivers/net/wan -I /usr/src/linux/include -I/usr/src/linux/include/net
KFLAGS+=$(shell if [ -f $(KINCLUDES)/linux/modversions.h ] ; then echo "-DMODVERSIONS -include $(KINCLUDES)/linux/modversions.h" ; fi)
KFLAGS+=$(shell if uname -m | grep -q ppc; then echo "-msoft-float -fsigned-char"; fi)
#
# Features are now configured in zconfig.h
#
#

KFLAGS+=-DSTANDALONE_ZAPATA
CFLAGS+=-DSTANDALONE_ZAPATA

ROOT_PREFIX=
INSTALL_PREFIX=$(DESTDIR)

CONFIG_FILE=$(INSTALL_PREFIX)/etc/zaptel.conf
CFLAGS+=-DZAPTEL_CONFIG=\"$(CONFIG_FILE)\"

BUILDVER=$(shell if uname -r | grep -q ^2.6; then echo "linux26"; else echo "linux24"; fi)
MODCONF=$(shell if [ -d $(ROOT_PREFIX)/etc/modprobe.d ]; then echo "$(ROOT_PREFIX)/etc/modprobe.d/zaptel"; elif [ -d $(ROOT_PREFIX)/etc/modutils ]; then echo "$(ROOT_PREFIX)/etc/modutils/zaptel"; elif [ -f $(ROOT_PREFIX)/etc/modprobe.conf ]; then echo "$(ROOT_PREFIX)/etc/modprobe.conf"; elif [ -f $(ROOT_PREFIX)/etc/modules.conf ]; then echo "$(ROOT_PREFIX)/etc/modules.conf"; else echo $(ROOT_PREFIX)/etc/conf.modules ; fi)

ifeq (${BUILDVER},linux24)
#We only support DEVFS in linux 2.4 kernels, since its considered obsolete post 2.4
DYNFS=$(shell ps ax | grep -v grep | grep -q devfsd && echo "yes")
endif
ifeq (${BUILDVER},linux26)
#Tests for newer linux-2.6 udev support
DYNFS=$(shell ps ax | grep -v grep | grep udevd && echo "yes")
endif


TZOBJS=zonedata.lo tonezone.lo
LIBTONEZONE=libtonezone.so.1.0
MODULES=zaptel tor2 torisa wcusb wcfxo wctdm \
	ztdynamic ztd-eth wct1xxp wct4xxp wcte11xp pciradio \
        ztd-loc # ztdummy
#MODULES+=wcfxsusb

MODULESO=$(shell for x in $(MODULES); do echo "$$x.o "; done )
MODULESKO=$(shell for x in $(MODULES); do echo "$$x.ko "; done )

ZTTOOL=$(shell if [ -f /usr/include/newt.h ]; then echo zttool; fi)
BINS=ztcfg torisatool makefw ztmonitor ztspeed $(ZTTOOL) zttest fxotune

#PRIMARY=wcfxsusb
PRIMARY=torisa
#PRIMARY=wcfxo
PWD=$(shell pwd)
KERNEL_SOURCE?=/lib/modules/`uname -r`/build

all: $(BUILDVER)

linux24: $(MODULESO) $(BINS)

linux26: 
linux26: prereq $(BINS)
	@if ! [ -d $(KERNEL_SOURCE) ]; then echo "You do not appear to have the kernel sources for your current kernel installed."; exit 1 ; fi
	make -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules

obj-m := $(MODULESO) ztdummy.o

#ifneq ($(TOPDIR),)
#include $(TOPDIR)/Rules.make
#endif

MOD_DESTDIR := zaptel

devel: tor2ee 

tests: patgen pattest patlooptest hdlcstress hdlctest hdlcgen hdlcverify timertest

tor2.o: tor2.c tor2-hw.h tor2fw.h zaptel.h
	$(HOSTCC) $(KFLAGS) -c tor2.c

zaptel.o: zaptel.c zaptel.h digits.h arith.h sec.h mec.h sec-2.h mec2.h mec3.h zconfig.h
	$(HOSTCC) $(KFLAGS) -c zaptel.c

torisa.o: torisa.c zaptel.h torisa.h
	$(HOSTCC) $(KFLAGS) -c torisa.c

wcusb.o: wcusb.c wcusb.h zaptel.h
	$(HOSTCC) $(KFLAGS) -c wcusb.c

wcfxsusb.o: wcfxsusb.c wcfxsusb.h zaptel.h
	$(HOSTCC) $(KFLAGS) -c wcfxsusb.c

wcfxo.o: wcfxo.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c wcfxo.c

wct1xxp.o:wct1xxp.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c wct1xxp.c

wcte11xp.o:wcte11xp.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c wcte11xp.c

ztd-loc.o:ztd-loc.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c ztd-loc.c

wct4xxp.o:wct4xxp.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c wct4xxp.c

wctdm.o:wctdm.c zaptel.h wctdm.h
	$(HOSTCC) $(KFLAGS) -c wctdm.c

pciradio.o:pciradio.c zaptel.h radfw.h
	$(HOSTCC) $(KFLAGS) -c pciradio.c

wcs3200p.o:wcs3200p.c zaptel.h
	$(HOSTCC) $(KFLAGS) -c wcs3200p.c

ztdummy.o:ztdummy.c ztdummy.h
	$(HOSTCC) $(KFLAGS) -c ztdummy.c

ztdynamic.o: ztdynamic.c zaptel.h
	$(HOSTCC) $(KFLAGS)  -c ztdynamic.c

ztd-eth.o: ztd-eth.c zaptel.h
	$(HOSTCC) $(KFLAGS)  -c ztd-eth.c

tor2ee.o: tor2-hw.h

tor2ee: tor2ee.o
	$(CC) $(CFLAGS) -o tor2ee tor2ee.o -lpci

zonedata.lo: zonedata.c
	$(CC) -c $(LCFLAGS) -o zonedata.lo zonedata.c

tonezone.lo: tonezone.c
	$(CC) -c $(LCFLAGS) -o tonezone.lo tonezone.c

torisatool: torisatool.o
	$(CC) -o torisatool torisatool.o

tones.h: gendigits
	./gendigits

tor2fw.h: makefw tormenta2.rbt
	./makefw tormenta2.rbt tor2fw > tor2fw.h

radfw.h: makefw pciradio.rbt
	./makefw pciradio.rbt radfw > radfw.h

gendigits: gendigits.o
	$(CC) -o gendigits gendigits.o -lm

zaptel.c: tones.h 

prereq: tones.h tor2fw.h radfw.h

zttool.o: zttool.c zaptel.h

ztprovision.o: ztprovision.c zaptel.h

ztmonitor.o: ztmonitor.c zaptel.h

ztspeed.o: ztspeed.c
	$(CC) -c ztspeed.c

zttool: zttool.o
	$(CC) -o zttool zttool.o -lnewt

ztprovision: ztprovision.o
	$(CC) -o ztprovision ztprovision.o -lnewt

ztmonitor: ztmonitor.o
	$(CC) -o ztmonitor ztmonitor.o

ztcat: ztcat.o
	$(CC) -o ztcat ztcat.o -ltonezone

ztspeed: ztspeed.o
	$(CC) -o ztspeed ztspeed.o

sethdlc-new: sethdlc-new.o
	$(CC) -o sethdlc-new sethdlc-new.o

sethdlc-new.o: sethdlc-new.c
	$(CC) -c $(CFLAGS) -I$(KINCLUDES) sethdlc-new.c

libtonezone.a: $(TZOBJS)
	ar rcs libtonezone.a $(TZOBJS)

$(LIBTONEZONE): $(TZOBJS)
	$(CC) -shared -Wl,-soname,$(LIBTONEZONE) -lm -o $@ $(TZOBJS)
	[ `id -u` = 0 ] && /sbin/ldconfig || :
	ln -sf $(LIBTONEZONE) libtonezone.so                                  
	ln -sf $(LIBTONEZONE) libtonezone.so.1                                  

ztcfg.c: ztcfg.h

ztcfg-shared: ztcfg.o $(LIBTONEZONE)
	$(CC) -o ztcfg-shared ztcfg.o -lm -L. -ltonezone

ztcfg: ztcfg.o libtonezone.a
	$(CC) -o ztcfg ztcfg.o -lm -L. libtonezone.a

ztcfg-dude: ztcfg-dude.o mknotch.o complex.o $(LIBTONEZONE)
	$(CC) -o ztcfg ztcfg-dude.o mknotch.o complex.o -lm -L. -ltonezone

mknotch.o: mknotch.cc
	$(CC) -c mknotch.cc

complex.o: complex.cc
	$(CC) -c complex.cc

usbfxstest.o: usbfxstest.c
	$(CC) -g -c usbfxstest.c

usbfxstest: usbfxstest.o 
	$(CC) -o usbfxstest usbfxstest.o -lzap

fxstest: fxstest.o 
	$(CC) -o fxstest fxstest.o -ltonezone

fxotune: fxotune.o
	$(CC) -o fxotune fxotune.o -lm

fxsdump: fxsdump.o
	$(CC) -o fxsdump fxsdump.o -lm

stackcheck: checkstack $(BUILDVER)
	./checkstack *.o

devices:
ifeq ($(DYNFS),)
	mkdir -p $(INSTALL_PREFIX)/dev/zap
	rm -f $(INSTALL_PREFIX)/dev/zap/ctl
	rm -f $(INSTALL_PREFIX)/dev/zap/channel
	rm -f $(INSTALL_PREFIX)/dev/zap/pseudo
	rm -f $(INSTALL_PREFIX)/dev/zap/timer
	rm -f $(INSTALL_PREFIX)/dev/zap/253
	rm -f $(INSTALL_PREFIX)/dev/zap/252
	rm -f $(INSTALL_PREFIX)/dev/zap/251
	rm -f $(INSTALL_PREFIX)/dev/zap/250
	mknod $(INSTALL_PREFIX)/dev/zap/ctl c 196 0
	mknod $(INSTALL_PREFIX)/dev/zap/timer c 196 253
	mknod $(INSTALL_PREFIX)/dev/zap/channel c 196 254
	mknod $(INSTALL_PREFIX)/dev/zap/pseudo c 196 255
	N=1; \
	while [ $$N -lt 250 ]; do \
		rm -f $(INSTALL_PREFIX)/dev/zap/$$N; \
		mknod $(INSTALL_PREFIX)/dev/zap/$$N c 196 $$N; \
		N=$$[$$N+1]; \
	done
else
	@echo "**** Dynamic filesystem detected -- not creating device nodes"
	@echo "**** If you are running udev, read README.udev"
endif

install:  all devices $(LIBTONEZONE)
	install -D -m 755 ztcfg $(INSTALL_PREFIX)/sbin/ztcfg
	if [ -f sethdlc-new ]; then \
		install -D -m 755 sethdlc-new $(INSTALL_PREFIX)/sbin/sethdlc; \
	elif [ -f sethdlc ]; then \
		install -D -m 755 sethdlc $(INSTALL_PREFIX)/sbin/sethdlc ; \
	fi
	if [ -f zttool ]; then install -D -m 755 zttool $(INSTALL_PREFIX)/sbin/zttool; fi

	if [ -f zaptel.ko ]; then \
		for x in $(MODULESKO) ztdummy.ko; do \
			install -D -m 644 $$x $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/$$x ; \
		done; \
		if ! [ -f wcfxsusb.ko ]; then \
			rm -f $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/wcfxsusb.ko; \
		fi; \
		rm -f $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/wcfxs.ko; \
	else \
		for x in $(MODULESO); do \
			install -D -m 644 $$x $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/$$x ; \
		done; \
		if ! [ -f wcfxsusb.o ]; then \
			rm -f $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/wcfxsusb.o; \
		fi; \
		rm -f $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/wcfxs.o; \
	fi

	install -D -m 755 $(LIBTONEZONE) $(INSTALL_PREFIX)/usr/lib/$(LIBTONEZONE)
	if [ -x /usr/sbin/sestatus ] && (/usr/sbin/sestatus | grep "SELinux status:" | grep -q "enabled") ; then restorecon -v $(INSTALL_PREFIX)/usr/lib/$(LIBTONEZONE); fi
	install -D -m 644 zaptel.h $(INSTALL_PREFIX)/usr/include/linux/zaptel.h
	install -D -m 644 torisa.h $(INSTALL_PREFIX)/usr/include/linux/torisa.h
	install -D -m 644 tonezone.h $(INSTALL_PREFIX)/usr/include/tonezone.h
	( cd $(INSTALL_PREFIX)/usr/lib ; rm -f libtonezone.so ; ln -sf $(LIBTONEZONE) libtonezone.so )
	( cd $(INSTALL_PREFIX)/usr/lib ; rm -f libtonezone.so.1 ; ln -sf $(LIBTONEZONE) libtonezone.so.1 )
	[ `id -u` = 0 ] && /sbin/ldconfig || :
	if [ -f $(MODCONF) ]; then mv -f $(MODCONF) $(MODCONF).bak ; fi
	cat $(MODCONF).bak | grep -v "alias char-major-250" | \
	grep -v "post-install torisa /sbin/ztcfg" | \
	grep -v "post-install wcfxsusb /sbin/ztcfg" | \
	grep -v "post-install wctdm /sbin/ztcfg" > $(MODCONF) || true
	if ! grep "options torisa" $(MODCONF); then \
		echo "options torisa base=$(BASEADDR)" >> $(MODCONF); \
	fi
	if ! grep "alias char-major-196" $(MODCONF); then \
		echo "alias char-major-196 $(PRIMARY)" >> $(MODCONF); \
	fi
		
	for x in $(MODULES); do \
		if ! grep -q "post-install $$x" $(MODCONF); then \
			if ! grep -q "install $$x " $(MODCONF); then \
				if [ "$$x" != "zaptel" ] ; then \
					if [ -f zaptel.ko ]; then echo "install $$x /sbin/modprobe --ignore-install $$x && /sbin/ztcfg" >> $(MODCONF); \
					else echo "post-install $$x /sbin/ztcfg" >> $(MODCONF); \
					fi; \
				fi; \
			fi; \
		fi; \
	done
	if ! grep "alias wcfxs" $(MODCONF); then \
		echo "alias wcfxs wctdm" >> $(MODCONF); \
	fi \

	if [ -d /etc/modutils ]; then \
		/sbin/update-modules ; \
	fi
	[ `id -u` = 0 ] && /sbin/depmod -a || :
	[ -f $(CONFIG_FILE) ] || install -D -m 644 zaptel.conf.sample $(CONFIG_FILE)

config:
	if [ -d $(INSTALL_PREFIX)/etc/rc.d/init.d ]; then \
		install -D -m 755 zaptel.init $(INSTALL_PREFIX)/etc/rc.d/init.d/zaptel; \
		chkconfig --add zaptel; \
	elif [ -d $(INSTALL_PREFIX)/etc/init.d ]; then \
		install -D -m 755 zaptel.init $(INSTALL_PREFIX)/etc/init.d/zaptel; \
		chkconfig --add zaptel; \
	fi 
	if [ -d /etc/default ] && [ ! -f /etc/default/zaptel ]; then \
			install -D -m 644 zaptel.sysconfig $(INSTALL_PREFIX)/etc/default/zaptel; \
	fi
	if [ -d /etc/sysconfig ] && [ ! -f /etc/sysconfig/zaptel ]; then \
		install -D -m 644 zaptel.sysconfig $(INSTALL_PREFIX)/etc/sysconfig/zaptel; \
	fi
	if [ -d /etc/sysconfig/network-scripts ]; then \
		install -D -m 755 ifup-hdlc $(INSTALL_PREFIX)/etc/sysconfig/network-scripts/ifup-hdlc; \
	fi

update:
	@if [ -d CVS ]; then \
		echo "Updating from CVS..." ; \
		cvs -q -z3 update -Pd; \
	else \
		echo "Not CVS";  \
	fi
	
clean:
	rm -f torisatool makefw tor2fw.h radfw.h
	rm -f ${BINS}
	rm -f *.o ztcfg tzdriver sethdlc sethdlc-new
	rm -f $(TZOBJS) $(LIBTONEZONE) *.lo
	rm -f *.ko *.mod.c .*o.cmd
	rm -rf .tmp_versions
	rm -f gendigits tones.h
	rm -f libtonezone*
	rm -f tor2ee
	rm -f fxotune
	rm -f core

