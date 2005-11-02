#!/bin/sh

echo removing symlinks
rm dvb-pll.c
rm dvb-pll.h
rm mt352.c
rm mt352.h
rm mt352_priv.h
rm cx22702.c
rm cx22702.h
rm lgdt330x.c
rm lgdt330x.h
rm lgdt330x_priv.h
rm or51132.c
rm or51132.h
rm tda1004x.c
rm tda1004x.h
rm sp887x.c
rm sp887x.h
rm nxt6000.c
rm nxt6000.h
rm nxt6000_priv.h
rm cx24110.c
rm cx24110.h
rm or51211.c
rm or51211.h
rm bt878.c
rm bt878.h
rm dvb-bt8xx.c
rm dvb-bt8xx.h
rm dst.c
rm dst_ca.c
rm dst_ca.h
rm dst_common.h
rm dst_priv.h
rm dvbdev.h
rm dmxdev.h
rm dvb_demux.h
rm dvb_net.h
rm dvb_frontend.h
patch -p1 <<'DIFF'
diff -up video4linux/Make.config video4linux.orig/Make.config
--- video4linux/Make.config	2005-10-06 19:17:55.000000000 -0400
+++ video4linux.orig/Make.config	2005-10-06 16:03:26.000000000 -0400
@@ -18,7 +18,6 @@ CONFIG_VIDEO_ALSA	:= n
 
 CONFIG_VIDEO_CX88_DVB	:= n
 CONFIG_VIDEO_SAA7134_DVB := n
-CONFIG_DVB_BT8XX        := n
 
 ifneq ($(KERNELRELEASE),)
 need_dvb = $(shell test $(SUBLEVEL) -ge 12 && echo yes)
@@ -27,6 +26,5 @@ endif
 ifeq ($(need_dvb),yes)
  CONFIG_VIDEO_CX88_DVB	:= m
  CONFIG_VIDEO_SAA7134_DVB := m
- CONFIG_DVB_BT8XX       := m
 endif
 
diff -up video4linux/Makefile video4linux.orig/Makefile
--- video4linux/Makefile	2005-10-06 19:23:41.000000000 -0400
+++ video4linux.orig/Makefile	2005-10-06 16:03:26.000000000 -0400
@@ -50,10 +50,8 @@ obj-$(CONFIG_VIDEO_IR)		+= ir-common.o
 obj-$(CONFIG_VIDEO_TUNER)	+= tuner.o tda9887.o saa6588.o
 obj-$(CONFIG_VIDEO_TVAUDIO)	+= msp3400.o tvaudio.o tvmixer.o
 
-obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o cx22702.o dvb-pll.o or51132.o lgdt330x.o mt352.o
-obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o mt352.o tda1004x.o
-obj-$(CONFIG_DVB_BT8XX)         += bt878.o dvb-bt8xx.o dst.o dst_ca.o
-obj-$(CONFIG_DVB_BT8XX)         += mt352.o sp887x.o nxt6000.o cx24110.o or51211.o
+obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o
+obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o
 
 # 2.6-only stuff
 ifeq ($(VERSION).$(PATCHLEVEL),2.6)
@@ -67,7 +65,6 @@ ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 endif
 
 # for DVB
-EXTRA_CFLAGS += -DDVB_CVS=1
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/dvb-core/
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/frontends/
 ifeq ($(CONFIG_VIDEO_CX88_DVB),m)
@@ -150,9 +147,6 @@ inst_cx88 := cx8800.ko cx8802.ko cx88-al
 inst_cx88 += cx88-blackbird.ko cx88xx.ko cx88-dvb.ko
 inst_saa7134 := saa6752hs.ko saa7134.ko saa7134-empress.ko saa7134-dvb.ko
 inst_usb := em2820.ko
-inst_bt8xx := bt878.ko dvb-bt8xx.ko dst.ko dst_ca.ko
-inst_frontends := cx22702.ko dvb-pll.ko lgdt330x.ko or51132.ko tda1004x.ko
-inst_frontends += mt352.ko sp887x.ko nxt6000.ko cx24110.ko or51211.ko
 
 v4l_modules := $(shell lsmod|cut -d' ' -f1 ) $(patsubst %.ko,%,$(inst-m))
 
@@ -172,12 +166,6 @@ install:: rminstall
 	-install -d $(KDIR26)/common
 	-install -m 644 -c $(inst_common) $(KDIR26)/common
 
-	-install -d $(KDIR26)/dvb/frontends
-	-install -m 644 -c $(inst_frontends) $(KDIR26)/dvb/frontends
-
-	-install -d $(KDIR26)/dvb/bt8xx
-	-install -m 644 -c $(inst_bt8xx) $(KDIR26)/dvb/bt8xx
-
 	-install -d $(KDIR26)/video
 	-install -m 644 -c $(inst_video) $(KDIR26)/video
 
@@ -208,14 +196,12 @@ rminstall::
 	-@rm -r $(DEST) \
 		$(addprefix $(KDIR26)/common/, $(inst_common)) \
 		$(addprefix $(KDIR26)/dvb/frontends/, $(inst_frontends)) \
-		$(addprefix $(KDIR26)/dvb/bt8xx/, $(inst_bt8xx)) \
 		$(addprefix $(KDIR26)/video/, $(inst_video)) \
 		$(addprefix $(KDIR26)/video/cx88/, $(inst_cx88)) \
 		$(addprefix $(KDIR26)/video/saa7134/, $(inst_saa7134)) \
 		$(addprefix $(KDIRUSB)/, $(inst_usb)) \
 		$(addprefix $(KDIR26)/common/, $(addsuffix .gz,$(inst_common))) \
 		$(addprefix $(KDIR26)/dvb/frontends/, $(addsuffix .gz,$(inst_frontends))) \
-		$(addprefix $(KDIR26)/dvb/bt8xx/, $(addsuffix .gz,$(inst_bt8xx))) \
 		$(addprefix $(KDIR26)/video/, $(addsuffix .gz,$(inst_video))) \
 		$(addprefix $(KDIR26)/video/cx88/, $(addsuffix .gz,$(inst_cx88))) \
 		$(addprefix $(KDIR26)/video/saa7134/, $(addsuffix .gz,$(inst_saa7134))) 2>/dev/null \
