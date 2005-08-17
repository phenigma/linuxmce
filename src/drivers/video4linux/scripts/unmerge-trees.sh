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
patch -p1 <<'DIFF'
diff -upr video4linux/Makefile video4linux.orig/Makefile
--- video4linux/Makefile	2005-07-29 21:44:41.000000000 +0000
+++ video4linux.orig/Makefile	2005-07-29 21:45:09.000000000 +0000
@@ -39,8 +39,8 @@ obj-$(CONFIG_VIDEO_IR)		+= ir-common.o
 obj-$(CONFIG_VIDEO_TUNER)	+= tuner.o tda9887.o
 obj-$(CONFIG_VIDEO_TVAUDIO)	+= msp3400.o tvaudio.o tvmixer.o
 
-obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o cx22702.o dvb-pll.o or51132.o lgdt330x.o mt352.o
-obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o mt352.o tda1004x.o
+obj-$(CONFIG_VIDEO_CX88_DVB)	+= video-buf-dvb.o cx88-dvb.o
+obj-$(CONFIG_VIDEO_SAA7134_DVB) += video-buf-dvb.o saa7134-dvb.o
 
 # 2.6-only stuff
 ifeq ($(VERSION).$(PATCHLEVEL),2.6)
@@ -54,7 +54,6 @@ ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 endif
 
 # for DVB
-EXTRA_CFLAGS += -DDVB_CVS=1
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/dvb-core/
 EXTRA_CFLAGS += -I$(srctree)/drivers/media/dvb/frontends/
 ifeq ($(CONFIG_VIDEO_CX88_DVB),m)
@@ -126,7 +125,6 @@ ifeq ($(inst-m),)
 endif
 
 inst_common := ir-common.ko
-inst_frontends := cx22702.ko dvb-pll.ko mt352.ko lgdt330x.ko or51132.ko tda1004x.ko
 inst_video := btcx-risc.ko bttv.ko tda9887.ko tuner.ko tvaudio.ko tveeprom.ko
 inst_video += tvmixer.ko v4l1-compat.ko v4l2-common.ko
 inst_video += video-buf.ko video-buf-dvb.ko
@@ -153,9 +151,6 @@ install:: rminstall
 	-install -d $(KDIR26)/common
 	-install -m 644 -c $(inst_common) $(KDIR26)/common
 
-	-install -d $(KDIR26)/dvb/frontends
-	-install -m 644 -c $(inst_frontends) $(KDIR26)/dvb/frontends
-
 	-install -d $(KDIR26)/video
 	-install -m 644 -c $(inst_video) $(KDIR26)/video
 
diff -upr video4linux/cx88-dvb.c video4linux.orig/cx88-dvb.c
--- video4linux/cx88-dvb.c	2005-07-29 21:44:41.000000000 +0000
+++ video4linux.orig/cx88-dvb.c	2005-07-29 21:44:31.000000000 +0000
@@ -32,6 +32,10 @@
 #include <linux/config.h>
 #include "compat.h"
 
+#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
+#undef HAVE_LGDT330X
+#endif
+
 #include "cx88.h"
 #include "dvb-pll.h"
 
@@ -212,7 +216,7 @@ static struct or51132_config pchdtv_hd30
 #endif
 
 #ifdef HAVE_LGDT330X
-#if 1
+#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
 static int lgdt330x_pll_set(struct dvb_frontend* fe,
 			    struct dvb_frontend_parameters* params)
 {
@@ -285,7 +289,7 @@ static int lgdt330x_set_ts_param(struct 
 
 static struct lgdt330x_config fusionhdtv_3_gold = {
 	.demod_address    = 0x0e,
-#if 1
+#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
 	.demod_chip       = LGDT3302,
 	.serial_mpeg      = 0x04, /* TPSERIAL for 3302 in TOP_CONTROL */
 	.pll_set          = lgdt330x_pll_set,
@@ -385,7 +389,7 @@ static int dvb_register(struct cx8802_de
 						    &dev->core->i2c_adap);
 		}
 		break;
-#if 1
+#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
 	case CX88_BOARD_DVICO_FUSIONHDTV_5_GOLD:
 		dev->ts_gen_cntrl = 0x08;
 		{
diff -upr video4linux/saa7134-dvb.c video4linux.orig/saa7134-dvb.c
--- video4linux/saa7134-dvb.c	2005-07-29 21:44:41.000000000 +0000
+++ video4linux.orig/saa7134-dvb.c	2005-07-29 21:44:31.000000000 +0000
@@ -32,6 +32,10 @@
 #include <linux/config.h>
 #include "compat.h"
 
+#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
+#undef HAVE_TDA1004X
+#endif
+
 #include "saa7134-reg.h"
 #include "saa7134.h"
 
