#!/bin/sh

echo creating symlinks
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/dvb-pll.c         dvb-pll.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/dvb-pll.h         dvb-pll.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/mt352.c           mt352.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/mt352.h           mt352.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/mt352_priv.h      mt352_priv.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/cx22702.c         cx22702.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/cx22702.h         cx22702.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/lgdt330x.c        lgdt330x.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/lgdt330x.h        lgdt330x.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/lgdt330x_priv.h   lgdt330x_priv.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/or51132.c         or51132.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/or51132.h         or51132.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/tda1004x.c        tda1004x.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/tda1004x.h        tda1004x.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/sp887x.c          sp887x.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/sp887x.h          sp887x.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/nxt6000.c         nxt6000.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/nxt6000.h         nxt6000.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/nxt6000_priv.h    nxt6000_priv.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/cx24110.c         cx24110.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/cx24110.h         cx24110.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/or51211.c         or51211.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/frontends/or51211.h         or51211.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/bt878.c               bt878.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/bt878.h               bt878.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dvb-bt8xx.c           dvb-bt8xx.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dvb-bt8xx.h           dvb-bt8xx.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dst.c                 dst.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dst_ca.c              dst_ca.c
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dst_ca.h              dst_ca.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dst_common.h          dst_common.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/bt8xx/dst_priv.h            dst_priv.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/dvb-core/dvbdev.h           dvbdev.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/dvb-core/dmxdev.h           dmxdev.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/dvb-core/dvb_demux.h        dvb_demux.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/dvb-core/dvb_net.h          dvb_net.h
ln -s ../dvb-kernel/linux/drivers/media/dvb/dvb-core/dvb_frontend.h     dvb_frontend.h
patch -p1 <<'DIFF'
diff -up video4linux.orig/Make.config video4linux/Make.config
--- video4linux.orig/Make.config	2005-10-06 16:03:26.000000000 -0400
+++ video4linux/Make.config	2005-10-06 19:17:55.000000000 -0400
@@ -18,6 +18,7 @@ CONFIG_VIDEO_ALSA	:= n
 
 CONFIG_VIDEO_CX88_DVB	:= n
 CONFIG_VIDEO_SAA7134_DVB := n
+CONFIG_DVB_BT8XX        := n
 
 ifneq ($(KERNELRELEASE),)
 need_dvb = $(shell test $(SUBLEVEL) -ge 12 && echo yes)
@@ -26,5 +27,6 @@ endif
 ifeq ($(need_dvb),yes)
  CONFIG_VIDEO_CX88_DVB	:= m
  CONFIG_VIDEO_SAA7134_DVB := m
+ CONFIG_DVB_BT8XX       := m
 endif
 
diff -up video4linux.orig/Makefile video4linux/Makefile
--- video4linux.orig/Makefile	2005-10-06 16:03:26.000000000 -0400
+++ video4linux/Makefile	2005-10-06 19:23:41.000000000 -0400
@@ -50,8 +50,10 @@ obj-$(CONFIG_VIDEO_IR)		+= ir-common.o
 obj-$(CONFIG_VIDEO_TUNER)	+= tuner.o tda9887.o saa6588.o
 obj-$(CONFIG_VIDEO_TVAUDIO)	+= msp3400.o tvaudio.o tvmixer.o
 
-obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o
-obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o
+obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o cx22702.o dvb-pll.o or51132.o lgdt330x.o mt352.o
+obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o mt352.o tda1004x.o
+obj-$(CONFIG_DVB_BT8XX)         += bt878.o dvb-bt8xx.o dst.o dst_ca.o
+obj-$(CONFIG_DVB_BT8XX)         += mt352.o sp887x.o nxt6000.o cx24110.o or51211.o
 
 # 2.6-only stuff
 ifeq ($(VERSION).$(PATCHLEVEL),2.6)
@@ -65,6 +67,7 @@ ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 endif
 
 # for DVB
+EXTRA_CFLAGS += -DDVB_CVS=1
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/dvb-core/
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/frontends/
 ifeq ($(CONFIG_VIDEO_CX88_DVB),m)
@@ -147,6 +150,9 @@ inst_cx88 := cx8800.ko cx8802.ko cx88-al
 inst_cx88 += cx88-blackbird.ko cx88xx.ko cx88-dvb.ko
 inst_saa7134 := saa6752hs.ko saa7134.ko saa7134-empress.ko saa7134-dvb.ko
 inst_usb := em2820.ko
+inst_bt8xx := bt878.ko dvb-bt8xx.ko dst.ko dst_ca.ko
+inst_frontends := cx22702.ko dvb-pll.ko lgdt330x.ko or51132.ko tda1004x.ko
+inst_frontends += mt352.ko sp887x.ko nxt6000.ko cx24110.ko or51211.ko
 
 v4l_modules := $(shell lsmod|cut -d' ' -f1 ) $(patsubst %.ko,%,$(inst-m))
 
@@ -166,6 +172,12 @@ install:: rminstall
 	-install -d $(KDIR26)/common
 	-install -m 644 -c $(inst_common) $(KDIR26)/common
 
+	-install -d $(KDIR26)/dvb/frontends
+	-install -m 644 -c $(inst_frontends) $(KDIR26)/dvb/frontends
+
+	-install -d $(KDIR26)/dvb/bt8xx
+	-install -m 644 -c $(inst_bt8xx) $(KDIR26)/dvb/bt8xx
+
 	-install -d $(KDIR26)/video
 	-install -m 644 -c $(inst_video) $(KDIR26)/video
 
@@ -196,12 +208,14 @@ rminstall::
 	-@rm -r $(DEST) \
 		$(addprefix $(KDIR26)/common/, $(inst_common)) \
 		$(addprefix $(KDIR26)/dvb/frontends/, $(inst_frontends)) \
+		$(addprefix $(KDIR26)/dvb/bt8xx/, $(inst_bt8xx)) \
 		$(addprefix $(KDIR26)/video/, $(inst_video)) \
 		$(addprefix $(KDIR26)/video/cx88/, $(inst_cx88)) \
 		$(addprefix $(KDIR26)/video/saa7134/, $(inst_saa7134)) \
 		$(addprefix $(KDIRUSB)/, $(inst_usb)) \
 		$(addprefix $(KDIR26)/common/, $(addsuffix .gz,$(inst_common))) \
 		$(addprefix $(KDIR26)/dvb/frontends/, $(addsuffix .gz,$(inst_frontends))) \
+		$(addprefix $(KDIR26)/dvb/bt8xx/, $(addsuffix .gz,$(inst_bt8xx))) \
 		$(addprefix $(KDIR26)/video/, $(addsuffix .gz,$(inst_video))) \
 		$(addprefix $(KDIR26)/video/cx88/, $(addsuffix .gz,$(inst_cx88))) \
 		$(addprefix $(KDIR26)/video/saa7134/, $(addsuffix .gz,$(inst_saa7134))) 2>/dev/null \
