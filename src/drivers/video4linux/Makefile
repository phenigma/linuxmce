
ifeq ($(obj),)
obj = .
endif


#################################################
# configuration

include $(obj)/Make.config

# symbol exports
export-objs	:= video-buf.o v4l1-compat.o v4l2-common.o
export-objs	+= ir-common.o
export-objs	+= bttv-if.o btcx-risc.o
export-objs	+= cx88-cards.o cx88-core.o

# drivers objects
bttv-objs	:= bttv-driver.o bttv-cards.o bttv-risc.o bttv-if.o \
		   bttv-vbi.o bttv-i2c.o
saa7134-objs    := saa7134-core.o saa7134-i2c.o saa7134-video.o \
		   saa7134-vbi.o saa7134-tvaudio.o saa7134-oss.o \
		   saa7134-cards.o saa7134-ts.o saa7134-input.o
cx88xx-objs     := cx88-cards.o cx88-core.o cx88-i2c.o cx88-tvaudio.o \
                   cx88-input.o
cx8800-objs     := cx88-video.o cx88-vbi.o
cx8801-objs     := cx88-alsa.o
cx8802-objs     := cx88-mpeg.o
tuner-objs	:= tuner-core.o tuner-simple.o mt20xx.o tda8290.o
list-multi	:= bttv.o saa7134.o cx88xx.o cx8800.o cx8801.o cx8802.o

# what to build
obj-m				:= video-buf.o v4l1-compat.o v4l2-common.o
obj-$(CONFIG_VIDEO_BTTV)	+= btcx-risc.o ir-common.o bttv.o tveeprom.o
obj-$(CONFIG_VIDEO_CX88)	+= btcx-risc.o cx88xx.o cx8800.o cx8801.o cx8802.o \
                                   cx88-blackbird.o tveeprom.o
obj-$(CONFIG_VIDEO_SAA7134)	+= saa7134.o saa7134-empress.o saa6752hs.o
obj-$(CONFIG_VIDEO_IR)		+= ir-common.o
obj-$(CONFIG_VIDEO_TUNER)	+= tuner.o tda9887.o
obj-$(CONFIG_VIDEO_TVAUDIO)	+= msp3400.o tvaudio.o tvmixer.o

obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o cx22702.o dvb-pll.o or51132.o
obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o mt352.o

# 2.6-only stuff
ifeq ($(VERSION).$(PATCHLEVEL),2.6)
  ifeq ($(CONFIG_VIDEO_BTTV),m)
    bttv-objs			+= bttv-gpio.o
    obj-$(CONFIG_VIDEO_IR)	+= ir-kbd-gpio.o ir-kbd-i2c.o 
  endif
  ifeq ($(CONFIG_VIDEO_SAA7134),m)
    obj-$(CONFIG_VIDEO_IR)	+= ir-kbd-i2c.o 
  endif
endif

# for DVB
EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/dvb-core/
EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/frontends/
ifeq ($(CONFIG_VIDEO_CX88_DVB),m)
 EXTRA_CFLAGS += -DCONFIG_VIDEO_CX88_DVB_MODULE=1
endif


#################################################
# compile modules

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system
dummy		:= $(shell echo $(KERNELRELEASE) >  $(obj)/.version)

ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 export-objs	:=
 list-multi	:=
else
 multi-m	:= $(filter $(list-multi), $(obj-m))
 int-m		:= $(sort $(foreach m, $(multi-m), $($(basename $(m))-objs)))
 export-objs	:= $(filter $(int-m) $(obj-m),$(export-objs))
endif

CC += -I$(obj)
EXTRA_CFLAGS += -g

snapshot := $(wildcard $(obj)/.snapshot)
ifneq ($(snapshot),)
SNAPSHOT_CFLAGS	:= -DSNAPSHOT='$(shell cat $(snapshot))'
EXTRA_CFLAGS	+= $(SNAPSHOT_CFLAGS)
endif

bttv.o: $(bttv-objs)
	$(LD) -r -o $@ $(bttv-objs)

cx8800.o: $(cx8800-objs)
	$(LD) -g -r -o $@ $(cx8800-objs)

cx88xx.o: $(cx88xx-objs)
	$(LD) -g -r -o $@ $(cx88xx-objs)

saa7134.o: $(saa7134-objs)
	$(LD) -g -r -o $@ $(saa7134-objs)

-include $(TOPDIR)/Rules.make
else
# take version info from last module build if available
KERNELRELEASE	:= $(shell cat $(obj)/.version 2>/dev/null || uname -r)
endif

KDIR		:= /lib/modules/$(KERNELRELEASE)/build
PWD		:= $(shell pwd)
DEST		:= /lib/modules/$(KERNELRELEASE)/$(MDIR)

# which files to install?
inst-m		:= $(wildcard *.ko)
ifeq ($(inst-m),)
  inst-m	:= $(obj-m)
endif

# locales seem to cause trouble sometimes.
LC_ALL = POSIX
export LC_ALL

default:: linux media
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install::
	strip --strip-debug $(inst-m)
	-su -c "mkdir -p $(DEST); cp -v $(inst-m) $(DEST); depmod -a"

clean::
	-rm -f *~ *.o *.ko *.mod.c
	-rm -f .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd

linux media:
	ln -s . $@

#################################################
# build tarballs

thisdir	:= $(notdir $(PWD))
name	:= $(shell echo $(thisdir) | sed 's/-.*//')
ver	:= $(shell echo $(thisdir) | sed 's/.*-//')
date    := $(shell date +%Y%m%d)
tardest	?= .

snapdir := $(HOME)/snapshot
snap	?= $(name)

release: clean
	rm -f .snapshot
	(cd ..; tar cvzf $(tardest)/$(name)-$(ver).tar.gz $(thisdir))

snapshot snap tarball: clean
	echo $(date) > .snapshot
	(cd ..; tar czf $(snapdir)/$(snap)-$(date).tar.gz $(thisdir))
	$(MAKE) -C $(snapdir)


#################################################
# other stuff

%.asm: %.o
	objdump -S $< > $@

cardlists:
	scripts/cardlist

