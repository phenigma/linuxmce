
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
tuner-objs	:= tuner-core.o tuner-simple.o mt20xx.o tda8290.o tea5767.o
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

obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o
obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o

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
 EXTRA_CFLAGS += -DHAVE_CX22702=1
 EXTRA_CFLAGS += -DHAVE_OR51132=1
 EXTRA_CFLAGS += -DHAVE_LGDT330X=1
 EXTRA_CFLAGS += -DHAVE_MT352=1
endif
ifeq ($(CONFIG_VIDEO_SAA7134_DVB),m)
 EXTRA_CFLAGS += -DHAVE_MT352=1
 EXTRA_CFLAGS += -DHAVE_TDA1004X=1
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

EXTRA_CFLAGS += $(if $(wildcard $(srctree)/.mm), -DMM_KERNEL)

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
KDIR26		:= /lib/modules/$(KERNELRELEASE)/kernel/drivers/media

# which files to install?
inst-m		:= $(wildcard *.ko)
ifeq ($(inst-m),)
  inst-m	:= $(obj-m)
endif

inst_common := ir-common.ko
inst_video := btcx-risc.ko bttv.ko tda9887.ko tuner.ko tvaudio.ko tveeprom.ko
inst_video += tvmixer.ko v4l1-compat.ko v4l2-common.ko
inst_video += video-buf.ko video-buf-dvb.ko
inst_video += ir-kbd-gpio.ko ir-kbd-i2c.ko msp3400.ko
inst_cx88 := cx8800.ko cx8801.ko cx8802.ko 
inst_cx88 += cx88-blackbird.ko cx88xx.ko cx88-dvb.ko
inst_saa7134 := saa6752hs.ko saa7134.ko saa7134-empress.ko saa7134-dvb.ko

v4l_modules := $(shell lsmod|cut -d' ' -f1 ) $(patsubst %.ko,%,$(inst-m))

# locales seem to cause trouble sometimes.
LC_ALL = POSIX
export LC_ALL

default:: linux media
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


install:: rminstall
	@echo -e "\nInstalling new V4L modules at corresponding Kernel dir..."

	@strip --strip-debug $(inst-m)

	-install -d $(KDIR26)/common
	-install -m 644 -c $(inst_common) $(KDIR26)/common

	-install -d $(KDIR26)/video
	-install -m 644 -c $(inst_video) $(KDIR26)/video

	-install -d $(KDIR26)/video/cx88
	-install -m 644 -c $(inst_cx88) $(KDIR26)/video/cx88

	-install -d $(KDIR26)/video/saa7134
	-install -m 644 -c $(inst_saa7134) $(KDIR26)/video/saa7134

	/sbin/depmod -a

v4l_install:: rminstall
	@echo -e "\nInstalling new V4L modules at $(DEST)..."

	@strip --strip-debug $(inst-m)

	-install -d $(DEST)
	install -m 644 -c $(inst-m) $(DEST)

	/sbin/depmod -a

rminstall::
	@echo -e "\nEliminating old V4L modules (errors on this step is not a problem).."

	-@rm -r $(DEST) \
		$(addprefix $(KDIR26)/common/, $(inst_common)) \
		$(addprefix $(KDIR26)/dvb/frontends/, $(inst_frontends)) \
		$(addprefix $(KDIR26)/video/, $(inst_video)) \
		$(addprefix $(KDIR26)/video/cx88/, $(inst_cx88)) \
		$(addprefix $(KDIR26)/video/saa7134/, $(inst_saa7134)) \
		$(addprefix $(KDIR26)/common/, $(addsuffix .gz,$(inst_common))) \
		$(addprefix $(KDIR26)/dvb/frontends/, $(addsuffix .gz,$(inst_frontends))) \
		$(addprefix $(KDIR26)/video/, $(addsuffix .gz,$(inst_video))) \
		$(addprefix $(KDIR26)/video/cx88/, $(addsuffix .gz,$(inst_cx88))) \
		$(addprefix $(KDIR26)/video/saa7134/, $(addsuffix .gz,$(inst_saa7134))) 2>/dev/null
	@echo

rmmodules::
	rmmod -w $(shell echo "$(v4l_modules)"|sed s,' ','\n',g|sed s,'-','_',g|sort|uniq -d)

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
	cp ChangeLog /$(snapdir)/ChangeLog-$(date)
	$(MAKE) -C $(snapdir)


#################################################
# other stuff

%.asm: %.o
	objdump -S $< > $@

cardlists:
	scripts/cardlist

