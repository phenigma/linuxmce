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
patch -p1 <<'DIFF'
diff -upr video4linux.orig/Makefile video4linux/Makefile
--- video4linux.orig/Makefile	2005-08-17 00:48:25.000000000 +0000
+++ video4linux/Makefile	2005-08-17 00:50:13.000000000 +0000
@@ -45,8 +45,8 @@ obj-$(CONFIG_VIDEO_IR)		+= ir-common.o
 obj-$(CONFIG_VIDEO_TUNER)	+= tuner.o tda9887.o saa6588.o
 obj-$(CONFIG_VIDEO_TVAUDIO)	+= msp3400.o tvaudio.o tvmixer.o
 
-obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o
-obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o
+obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o cx22702.o dvb-pll.o or51132.o lgdt330x.o mt352.o
+obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o mt352.o tda1004x.o
 
 # 2.6-only stuff
 ifeq ($(VERSION).$(PATCHLEVEL),2.6)
@@ -60,6 +60,7 @@ ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 endif
 
 # for DVB
+EXTRA_CFLAGS += -DDVB_CVS=1
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/dvb-core/
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/frontends/
 ifeq ($(CONFIG_VIDEO_CX88_DVB),m)
@@ -138,6 +139,7 @@ inst_video += ir-kbd-gpio.ko ir-kbd-i2c.
 inst_cx88 := cx8800.ko cx8802.ko cx88-alsa.ko 
 inst_cx88 += cx88-blackbird.ko cx88xx.ko cx88-dvb.ko
 inst_saa7134 := saa6752hs.ko saa7134.ko saa7134-empress.ko saa7134-dvb.ko
+inst_frontends := cx22702.ko dvb-pll.ko mt352.ko lgdt330x.ko or51132.ko tda1004x.ko
 
 v4l_modules := $(shell lsmod|cut -d' ' -f1 ) $(patsubst %.ko,%,$(inst-m))
 
@@ -157,6 +159,9 @@ install:: rminstall
 	-install -d $(KDIR26)/common
 	-install -m 644 -c $(inst_common) $(KDIR26)/common
 
+	-install -d $(KDIR26)/dvb/frontends
+	-install -m 644 -c $(inst_frontends) $(KDIR26)/dvb/frontends
+
 	-install -d $(KDIR26)/video
 	-install -m 644 -c $(inst_video) $(KDIR26)/video
 
diff -upr video4linux.orig/cx88-dvb.c video4linux/cx88-dvb.c
--- video4linux.orig/cx88-dvb.c	2005-08-17 00:48:25.000000000 +0000
+++ video4linux/cx88-dvb.c	2005-08-17 00:49:02.000000000 +0000
@@ -32,10 +32,6 @@
 #include <linux/config.h>
 #include "compat.h"
 
-#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
-#undef HAVE_LGDT330X
-#endif
-
 #include "cx88.h"
 #include "dvb-pll.h"
 
@@ -216,7 +212,7 @@ static struct or51132_config pchdtv_hd30
 #endif
 
 #ifdef HAVE_LGDT330X
-#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
+#if 1
 static int lgdt330x_pll_set(struct dvb_frontend* fe,
 			    struct dvb_frontend_parameters* params)
 {
@@ -298,7 +294,7 @@ static int lgdt330x_set_ts_param(struct 
 
 static struct lgdt330x_config fusionhdtv_3_gold = {
 	.demod_address    = 0x0e,
-#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
+#if 1
 	.demod_chip       = LGDT3302,
 	.serial_mpeg      = 0x04, /* TPSERIAL for 3302 in TOP_CONTROL */
 	.pll_set          = lgdt330x_pll_set,
@@ -398,7 +394,7 @@ static int dvb_register(struct cx8802_de
 						    &dev->core->i2c_adap);
 		}
 		break;
-#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
+#if 1
 	case CX88_BOARD_DVICO_FUSIONHDTV_5_GOLD:
 		dev->ts_gen_cntrl = 0x08;
 		{
diff -upr video4linux.orig/saa7134-dvb.c video4linux/saa7134-dvb.c
--- video4linux.orig/saa7134-dvb.c	2005-08-17 00:48:25.000000000 +0000
+++ video4linux/saa7134-dvb.c	2005-08-17 00:49:02.000000000 +0000
@@ -32,10 +32,6 @@
 #include <linux/config.h>
 #include "compat.h"
 
-#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
-#undef HAVE_TDA1004X
-#endif
-
 #include "saa7134-reg.h"
 #include "saa7134.h"
 
