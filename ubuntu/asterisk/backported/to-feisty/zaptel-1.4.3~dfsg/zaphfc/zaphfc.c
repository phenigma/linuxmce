/*
 * zaphfc.c - Zaptel driver for HFC-S PCI A based ISDN BRI cards
 *
 * kernel module inspired by HFC PCI ISDN4Linux and Zaptel drivers
 *
 * Copyright (C) 2002, 2003, 2004, 2005 Junghanns.NET GmbH
 *
 * Klaus-Peter Junghanns <kpj@junghanns.net>
 *
 * This program is free software and may be modified and
 * distributed under the terms of the GNU Public License.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#ifdef RTAITIMING
#include <asm/io.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#endif
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <zaptel.h>
#include "zaphfc.h"

#if CONFIG_PCI

#define CLKDEL_TE	0x0f	/* CLKDEL in TE mode */
#define CLKDEL_NT	0x6c	/* CLKDEL in NT mode */

typedef struct {
        int vendor_id;
        int device_id;
        char *vendor_name;
        char *card_name;
} PCI_ENTRY;

static const PCI_ENTRY id_list[] =
{
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_2BD0, "CCD/Billion/Asuscom", "2BD0"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B000, "Billion", "B000"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B006, "Billion", "B006"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B007, "Billion", "B007"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B008, "Billion", "B008"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B009, "Billion", "B009"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B00A, "Billion", "B00A"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B00B, "Billion", "B00B"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B00C, "Billion", "B00C"},
        {PCI_VENDOR_ID_CCD, PCI_DEVICE_ID_CCD_B100, "Seyeon", "B100"},
        {PCI_VENDOR_ID_ABOCOM, PCI_DEVICE_ID_ABOCOM_2BD1, "Abocom/Magitek", "2BD1"},
        {PCI_VENDOR_ID_ASUSTEK, PCI_DEVICE_ID_ASUSTEK_0675, "Asuscom/Askey", "675"},
        {PCI_VENDOR_ID_BERKOM, PCI_DEVICE_ID_BERKOM_T_CONCEPT, "German telekom", "T-Concept"},
        {PCI_VENDOR_ID_BERKOM, PCI_DEVICE_ID_BERKOM_A1T, "German telekom", "A1T"},
        {PCI_VENDOR_ID_ANIGMA, PCI_DEVICE_ID_ANIGMA_MC145575, "Motorola MC145575", "MC145575"},
        {PCI_VENDOR_ID_ZOLTRIX, PCI_DEVICE_ID_ZOLTRIX_2BD0, "Zoltrix", "2BD0"},
        {PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_DIGI_DF_M_IOM2_E,"Digi International", "Digi DataFire Micro V IOM2 (Europe)"},
        {PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_DIGI_DF_M_E,"Digi International", "Digi DataFire Micro V (Europe)"},
        {PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_DIGI_DF_M_IOM2_A,"Digi International", "Digi DataFire Micro V IOM2 (North America)"},
        {PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_DIGI_DF_M_A,"Digi International", "Digi DataFire Micro V (North America)"},
	{0x182d, 0x3069,"Sitecom","Isdn 128 PCI"},
        {0, 0, NULL, NULL},
};

static struct hfc_card *hfc_dev_list = NULL;
static int hfc_dev_count = 0;
static int modes = 0; // all TE
static int debug = 0;
static struct pci_dev *multi_hfc = NULL;
static spinlock_t registerlock = SPIN_LOCK_UNLOCKED;

void hfc_shutdownCard(struct hfc_card *hfctmp) {
    unsigned long flags;

    if (hfctmp == NULL) {
	return;
    }

    if (hfctmp->pci_io == NULL) {
	return;
    }
    
    spin_lock_irqsave(&hfctmp->lock,flags);

    printk(KERN_INFO "zaphfc: shutting down card at %p.\n",hfctmp->pci_io);

    /* Clear interrupt mask */
    hfctmp->regs.int_m2 = 0;
    hfc_outb(hfctmp, hfc_INT_M2, hfctmp->regs.int_m2);

    /* Reset pending interrupts */
    hfc_inb(hfctmp, hfc_INT_S1);

    /* Wait for interrupts that might still be pending */
    spin_unlock_irqrestore(&hfctmp->lock, flags);
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((30 * HZ) / 1000);	// wait 30 ms
    spin_lock_irqsave(&hfctmp->lock,flags);

    /* Remove interrupt handler */
    if (hfctmp->irq) {
	free_irq(hfctmp->irq, hfctmp);
    }

    /* Soft-reset the card */
    hfc_outb(hfctmp, hfc_CIRM, hfc_CIRM_RESET); // softreset on

    spin_unlock_irqrestore(&hfctmp->lock, flags);
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((30 * HZ) / 1000);	// wait 30 ms
    spin_lock_irqsave(&hfctmp->lock,flags);

    hfc_outb(hfctmp,hfc_CIRM,0);	// softreset off

    pci_write_config_word(hfctmp->pcidev, PCI_COMMAND, 0);	// disable memio and bustmaster

    if (hfctmp->fifomem != NULL) {
        kfree(hfctmp->fifomem);
    }
    iounmap((void *) hfctmp->pci_io);
    hfctmp->pci_io = NULL;
    if (hfctmp->pcidev != NULL) {
        pci_disable_device(hfctmp->pcidev);
    }
    spin_unlock_irqrestore(&hfctmp->lock,flags);
    if (hfctmp->ztdev != NULL) {
	zt_unregister(&hfctmp->ztdev->span);
	kfree(hfctmp->ztdev);
	printk(KERN_INFO "unregistered from zaptel.\n");
    }
}

void hfc_resetCard(struct hfc_card *hfctmp) {
    unsigned long flags;

    spin_lock_irqsave(&hfctmp->lock,flags);
    pci_write_config_word(hfctmp->pcidev, PCI_COMMAND, PCI_COMMAND_MEMORY);	// enable memio
    hfctmp->regs.int_m2 = 0;
    hfc_outb(hfctmp, hfc_INT_M2, hfctmp->regs.int_m2);

//    printk(KERN_INFO "zaphfc: resetting card.\n");
    pci_set_master(hfctmp->pcidev);
    hfc_outb(hfctmp, hfc_CIRM, hfc_CIRM_RESET);	// softreset on
    spin_unlock_irqrestore(&hfctmp->lock, flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((30 * HZ) / 1000);	// wait 30 ms
    hfc_outb(hfctmp, hfc_CIRM, 0);	// softreset off

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((20 * HZ) / 1000);	// wait 20 ms
    if (hfc_inb(hfctmp,hfc_STATUS) & hfc_STATUS_PCI_PROC) {
	printk(KERN_WARNING "zaphfc: hfc busy.\n");
    }

//    hfctmp->regs.fifo_en = hfc_FIFOEN_D | hfc_FIFOEN_B1 | hfc_FIFOEN_B2;
//    hfctmp->regs.fifo_en = hfc_FIFOEN_D;	/* only D fifos enabled */
    hfctmp->regs.fifo_en = 0;	/* no fifos enabled */
    hfc_outb(hfctmp, hfc_FIFO_EN, hfctmp->regs.fifo_en);

    hfctmp->regs.trm = 2;
    hfc_outb(hfctmp, hfc_TRM, hfctmp->regs.trm);

    if (hfctmp->regs.nt_mode == 1) {
	hfc_outb(hfctmp, hfc_CLKDEL, CLKDEL_NT); /* ST-Bit delay for NT-Mode */
    } else {
	hfc_outb(hfctmp, hfc_CLKDEL, CLKDEL_TE); /* ST-Bit delay for TE-Mode */
    }
    hfctmp->regs.sctrl_e = hfc_SCTRL_E_AUTO_AWAKE;
    hfc_outb(hfctmp, hfc_SCTRL_E, hfctmp->regs.sctrl_e);	/* S/T Auto awake */
    hfctmp->regs.bswapped = 0;	/* no exchange */

    hfctmp->regs.ctmt = hfc_CTMT_TRANSB1 | hfc_CTMT_TRANSB2; // all bchans are transparent , no freaking hdlc
    hfc_outb(hfctmp, hfc_CTMT, hfctmp->regs.ctmt);

    hfctmp->regs.int_m1 = 0;
    hfc_outb(hfctmp, hfc_INT_M1, hfctmp->regs.int_m1);

#ifdef RTAITIMING
    hfctmp->regs.int_m2 = 0;
#else
    hfctmp->regs.int_m2 = hfc_M2_PROC_TRANS;
#endif
    hfc_outb(hfctmp, hfc_INT_M2, hfctmp->regs.int_m2);

    /* Clear already pending ints */
    hfc_inb(hfctmp, hfc_INT_S1);

    if (hfctmp->regs.nt_mode == 1) {
	hfctmp->regs.sctrl = 3 | hfc_SCTRL_NONE_CAP | hfc_SCTRL_MODE_NT;	/* set tx_lo mode, error in datasheet ! */
    } else {
	hfctmp->regs.sctrl = 3 | hfc_SCTRL_NONE_CAP | hfc_SCTRL_MODE_TE;	/* set tx_lo mode, error in datasheet ! */
    }

    hfctmp->regs.mst_mode = hfc_MST_MODE_MASTER;	/* HFC Master Mode */
    hfc_outb(hfctmp, hfc_MST_MODE, hfctmp->regs.mst_mode);

    hfc_outb(hfctmp, hfc_SCTRL, hfctmp->regs.sctrl);
    hfctmp->regs.sctrl_r = 3;
    hfc_outb(hfctmp, hfc_SCTRL_R, hfctmp->regs.sctrl_r);

    hfctmp->regs.connect = 0;
    hfc_outb(hfctmp, hfc_CONNECT, hfctmp->regs.connect);

    hfc_outb(hfctmp, hfc_CIRM, 0x80 | 0x40);	// bit order

    /* Finally enable IRQ output */
#ifndef RTAITIMING
    hfctmp->regs.int_m2 |= hfc_M2_IRQ_ENABLE;
    hfc_outb(hfctmp, hfc_INT_M2, hfctmp->regs.int_m2);
#endif

    /* clear pending ints */
    hfc_inb(hfctmp, hfc_INT_S1); 
    hfc_inb(hfctmp, hfc_INT_S2);
}

void hfc_registerCard(struct hfc_card *hfccard) {
    spin_lock(&registerlock);
    if (hfccard != NULL) {
	hfccard->cardno = hfc_dev_count++;
	hfccard->next = hfc_dev_list;
	hfc_dev_list = hfccard;
    }
    spin_unlock(&registerlock);
}

static void hfc_btrans(struct hfc_card *hfctmp, char whichB) {
    // we are called with irqs disabled from the irq handler
    int count, maxlen, total;
    unsigned char *f1, *f2;
    unsigned short *z1, *z2, newz1;
    int freebytes;

    if (whichB == 1) {
	f1 = (char *)(hfctmp->fifos + hfc_FIFO_B1TX_F1);
        f2 = (char *)(hfctmp->fifos + hfc_FIFO_B1TX_F2);
	z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B1TX_Z1 + (*f1 * 4));
	z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B1TX_Z2 + (*f1 * 4));
    } else {
	f1 = (char *)(hfctmp->fifos + hfc_FIFO_B2TX_F1);
        f2 = (char *)(hfctmp->fifos + hfc_FIFO_B2TX_F2);
	z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B2TX_Z1 + (*f1 * 4));
	z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B2TX_Z2 + (*f1 * 4));
    }

    freebytes = *z2 - *z1;
    if (freebytes <= 0) {
	freebytes += hfc_B_FIFO_SIZE;
    }
    count = ZT_CHUNKSIZE;

    total = count;
    if (freebytes < count) {
	hfctmp->clicks++;
	/* only spit out this warning once per second to not make things worse! */
	if (hfctmp->clicks > 100) {
	    printk(KERN_CRIT "zaphfc: bchan tx fifo full, dropping audio! (z1=%d, z2=%d)\n",*z1,*z2);
	    hfctmp->clicks = 0;
	}
	return;
    }
    
    maxlen = (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL) - *z1;
    if (maxlen > count) {
        maxlen = count;
    }
    newz1 = *z1 + total;
    if (newz1 >= (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL)) { newz1 -= hfc_B_FIFO_SIZE; }

	if (whichB == 1) {
	    memcpy((char *)(hfctmp->fifos + hfc_FIFO_B1TX_ZOFF + *z1),hfctmp->ztdev->chans[0].writechunk, maxlen);
	} else {
	    memcpy((char *)(hfctmp->fifos + hfc_FIFO_B2TX_ZOFF + *z1),hfctmp->ztdev->chans[1].writechunk, maxlen);
	}
	
	count -= maxlen;
	if (count > 0) {
	// Buffer wrap
	    if (whichB == 1) {
	        memcpy((char *)(hfctmp->fifos + hfc_FIFO_B1TX_ZOFF + hfc_B_SUB_VAL),hfctmp->ztdev->chans[0].writechunk+maxlen, count);
	    } else {
	        memcpy((char *)(hfctmp->fifos + hfc_FIFO_B2TX_ZOFF + hfc_B_SUB_VAL),hfctmp->ztdev->chans[1].writechunk+maxlen, count);
	    }
	}

    *z1 = newz1;	/* send it now */

//    if (count > 0) printk(KERN_CRIT "zaphfc: bchan tx fifo (f1=%d, f2=%d, z1=%d, z2=%d)\n",(*f1) & hfc_FMASK,(*f2) & hfc_FMASK, *z1, *z2);
    return;    
}

static void hfc_brec(struct hfc_card *hfctmp, char whichB) {
    // we are called with irqs disabled from the irq handler
    int count, maxlen, drop;
    volatile unsigned char *f1, *f2;
    volatile unsigned short *z1, *z2, newz2;
    int bytes = 0;

    if (whichB == 1) {
	f1 = (char *)(hfctmp->fifos + hfc_FIFO_B1RX_F1);
        f2 = (char *)(hfctmp->fifos + hfc_FIFO_B1RX_F2);
	z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B1RX_Z1 + (*f1 * 4));
	z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B1RX_Z2 + (*f1 * 4));
    } else {
	f1 = (char *)(hfctmp->fifos + hfc_FIFO_B2RX_F1);
        f2 = (char *)(hfctmp->fifos + hfc_FIFO_B2RX_F2);
	z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B2RX_Z1 + (*f1 * 4));
	z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B2RX_Z2 + (*f1 * 4));
    }

    bytes = *z1 - *z2;
    if (bytes < 0) {
	bytes += hfc_B_FIFO_SIZE;
    }
    count = ZT_CHUNKSIZE;
    
    if (bytes < ZT_CHUNKSIZE) {
#ifndef RTAITIMING
	printk(KERN_CRIT "zaphfc: bchan rx fifo not enough bytes to receive! (z1=%d, z2=%d, wanted %d got %d), probably a buffer overrun.\n",*z1,*z2,ZT_CHUNKSIZE,bytes);
#endif
	return;
    }

    /* allowing the buffering of hfc_BCHAN_BUFFER bytes of audio data works around irq jitter */
    if (bytes > hfc_BCHAN_BUFFER + ZT_CHUNKSIZE) {
	/* if the system is too slow to handle it, we will have to drop it all (except 1 zaptel chunk) */
	drop = bytes - ZT_CHUNKSIZE;
	hfctmp->clicks++;
	/* only spit out this warning once per second to not make things worse! */
	if (hfctmp->clicks > 100) {
	    printk(KERN_CRIT "zaphfc: dropped audio (z1=%d, z2=%d, wanted %d got %d, dropped %d).\n",*z1,*z2,count,bytes,drop);
	    hfctmp->clicks = 0;
	}
	/* hm, we are processing the b chan data tooooo slowly... let's drop the lost audio */
	newz2 = *z2 + drop;
	if (newz2 >= (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL)) { 
	    newz2 -= hfc_B_FIFO_SIZE; 
	}
	*z2 = newz2;
    }

    
    maxlen = (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL) - *z2;
    if (maxlen > count) {
        maxlen = count;
    }
    if (whichB == 1) {
        memcpy(hfctmp->ztdev->chans[0].readchunk,(char *)(hfctmp->fifos + hfc_FIFO_B1RX_ZOFF + *z2), maxlen);
    } else {
        memcpy(hfctmp->ztdev->chans[1].readchunk,(char *)(hfctmp->fifos + hfc_FIFO_B2RX_ZOFF + *z2), maxlen);
    }
    newz2 = *z2 + count;
    if (newz2 >= (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL)) { 
        newz2 -= hfc_B_FIFO_SIZE; 
    }
    *z2 = newz2;
	
    count -= maxlen;
    if (count > 0) {
    // Buffer wrap
        if (whichB == 1) {
	    z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B1RX_Z2 + (*f1 * 4));
    	    memcpy(hfctmp->ztdev->chans[0].readchunk + maxlen,(char *)(hfctmp->fifos + hfc_FIFO_B1RX_ZOFF + hfc_B_SUB_VAL), count);
	} else {
	    z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_B2RX_Z2 + (*f1 * 4));
	    memcpy(hfctmp->ztdev->chans[1].readchunk + maxlen,(char *)(hfctmp->fifos + hfc_FIFO_B2RX_ZOFF + hfc_B_SUB_VAL), count);
	}
	newz2 = *z2 + count;
	if (newz2 >= (hfc_B_FIFO_SIZE + hfc_B_SUB_VAL)) { 
	    newz2 -= hfc_B_FIFO_SIZE; 
	}
    }


    if (whichB == 1) {
	zt_ec_chunk(&hfctmp->ztdev->chans[0], hfctmp->ztdev->chans[0].readchunk, hfctmp->ztdev->chans[0].writechunk);
    } else {
	zt_ec_chunk(&hfctmp->ztdev->chans[1], hfctmp->ztdev->chans[1].readchunk, hfctmp->ztdev->chans[1].writechunk);
    }
    return;    
}


static void hfc_dtrans(struct hfc_card *hfctmp) {
    // we are called with irqs disabled from the irq handler
    int x;
    int count, maxlen, total;
    unsigned char *f1, *f2, newf1;
    unsigned short *z1, *z2, newz1;
    int frames, freebytes;

    if (hfctmp->ztdev->chans[2].bytes2transmit == 0) {
	return;
    }

    f1 = (char *)(hfctmp->fifos + hfc_FIFO_DTX_F1);
    f2 = (char *)(hfctmp->fifos + hfc_FIFO_DTX_F2);
    z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DTX_Z1 + (*f1 * 4));
    z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DTX_Z2 + (*f1 * 4));

    frames = (*f1 - *f2) & hfc_FMASK;
    if (frames < 0) {
	frames += hfc_MAX_DFRAMES + 1;
    }

    if (frames >= hfc_MAX_DFRAMES) {
	printk(KERN_CRIT "zaphfc: dchan tx fifo total number of frames exceeded!\n");
	return;
    }

    freebytes = *z2 - *z1;
    if (freebytes <= 0) {
	freebytes += hfc_D_FIFO_SIZE;
    }
    count = hfctmp->ztdev->chans[2].bytes2transmit;

    total = count;
    if (freebytes < count) {
	printk(KERN_CRIT "zaphfc: dchan tx fifo not enough free bytes! (z1=%d, z2=%d)\n",*z1,*z2);
	return;
    }
    
    newz1 = (*z1 + count) & hfc_ZMASK;
    newf1 = ((*f1 + 1) & hfc_MAX_DFRAMES) | (hfc_MAX_DFRAMES + 1);	// next frame
    
    if (count > 0) {
	if (debug) {
    	    printk(KERN_CRIT "zaphfc: card %d TX [ ", hfctmp->cardno);
	    for (x=0; x<count; x++) {
		printk("%#2x ",hfctmp->dtransbuf[x]);
	    }
	    if (hfctmp->ztdev->chans[2].eoftx == 1) {
		printk("] %d bytes\n", count);
	    } else {
		printk("..] %d bytes\n", count);
	    }
	}
	maxlen = hfc_D_FIFO_SIZE - *z1;
	if (maxlen > count) {
	    maxlen = count;
	}
	memcpy((char *)(hfctmp->fifos + hfc_FIFO_DTX_ZOFF + *z1),hfctmp->ztdev->chans[2].writechunk, maxlen);
	count -= maxlen;
	if (count > 0) {
	    memcpy((char *)(hfctmp->fifos + hfc_FIFO_DTX_ZOFF),(char *)(hfctmp->ztdev->chans[2].writechunk + maxlen), count);
	}
    }

    *z1 = newz1;

    if (hfctmp->ztdev->chans[2].eoftx == 1) {
	*f1 = newf1;
	z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DTX_Z1 + (*f1 * 4));
	*z1 = newz1;
	hfctmp->ztdev->chans[2].eoftx = 0;
    }
//    printk(KERN_CRIT "zaphfc: dchan tx fifo (f1=%d, f2=%d, z1=%d, z2=%d)\n",(*f1) & hfc_FMASK,(*f2) & hfc_FMASK, *z1, *z2);
    return;    
}

/* receive a complete hdlc frame, skip broken or short frames */
static void hfc_drec(struct hfc_card *hfctmp) {
    int count=0, maxlen=0, framelen=0;
    unsigned char *f1, *f2, *crcstat;
    unsigned short *z1, *z2, oldz2, newz2;

    hfctmp->ztdev->chans[2].bytes2receive=0;
    hfctmp->ztdev->chans[2].eofrx = 0;

    /* put the received data into the zaptel buffer
       we'll call zt_receive() later when the timer fires. */
    f1 = (char *)(hfctmp->fifos + hfc_FIFO_DRX_F1);
    f2 = (char *)(hfctmp->fifos + hfc_FIFO_DRX_F2);

    if (*f1 == *f2) return; /* nothing received, strange eh? */

    z1 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DRX_Z1 + (*f2 * 4));
    z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DRX_Z2 + (*f2 * 4));
    
    /* calculate length of frame, including 2 bytes CRC and 1 byte STAT */
    count = *z1 - *z2;
    
    if (count < 0) { 
	count += hfc_D_FIFO_SIZE; /* ring buffer wrapped */
    }
    count++;
    framelen = count;

    crcstat = (char *)(hfctmp->fifos + hfc_FIFO_DRX_ZOFF + *z1);

    if ((framelen < 4) || (*crcstat != 0x0)) {
	/* the frame is too short for a valid HDLC frame or the CRC is borked */
	printk(KERN_CRIT "zaphfc: empty HDLC frame or bad CRC received (framelen = %d, stat = %#x, card = %d).\n", framelen, *crcstat, hfctmp->cardno);
	oldz2 = *z2;
	*f2 = ((*f2 + 1) & hfc_MAX_DFRAMES) | (hfc_MAX_DFRAMES + 1);	/* NEXT!!! */
        // recalculate z2, because Z2 is a function of F2 Z2(F2) and we INCed F2!!!
	z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DRX_Z2 + (*f2 * 4));
	*z2 = (oldz2 + framelen) & hfc_ZMASK;
	hfctmp->drecinframe = 0;
	hfctmp->regs.int_drec--;
	/* skip short or broken frames */
        hfctmp->ztdev->chans[2].bytes2receive = 0; 
	return;
    }

    count -= 1;	/* strip STAT */
    hfctmp->ztdev->chans[2].eofrx = 1;

    if (count + *z2 <= hfc_D_FIFO_SIZE) {
	maxlen = count;
    } else {
	maxlen = hfc_D_FIFO_SIZE - *z2;
    }

    /* copy first part */
    memcpy(hfctmp->drecbuf, (char *)(hfctmp->fifos + hfc_FIFO_DRX_ZOFF + *z2), maxlen);
    hfctmp->ztdev->chans[2].bytes2receive += maxlen; 
    
    count -= maxlen;
    if (count > 0) {
	/* ring buffer wrapped, copy rest from start of d fifo */
	memcpy(hfctmp->drecbuf + maxlen, (char *)(hfctmp->fifos + hfc_FIFO_DRX_ZOFF), count);
	hfctmp->ztdev->chans[2].bytes2receive += count; 
    }

    /* frame read */
    oldz2 = *z2;
    newz2 = (oldz2 + framelen) & hfc_ZMASK;
    *f2 = ((*f2 + 1) & hfc_MAX_DFRAMES) | (hfc_MAX_DFRAMES + 1);	/* NEXT!!! */
    /* recalculate z2, because Z2 is a function of F2 Z2(F2) and we INCed F2!!! */
    z2 = (unsigned short *)(hfctmp->fifos + hfc_FIFO_DRX_Z2 + (*f2 * 4));
    *z2 = newz2;
    hfctmp->drecinframe = 0;
    hfctmp->regs.int_drec--; 
}

#ifndef RTAITIMING
#ifdef LINUX26
static irqreturn_t hfc_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#else
static void hfc_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#endif
    struct hfc_card *hfctmp = dev_id;
    unsigned long flags = 0;
    unsigned char stat;
#else
static void hfc_service(struct hfc_card *hfctmp) {
#endif
    struct zt_hfc *zthfc;
    unsigned char s1, s2, l1state;
    int x;

    if (!hfctmp) {
#ifndef RTAITIMING
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
#else
	/* rtai */
	return;
#endif
    }

    if (!hfctmp->pci_io) {
	    printk(KERN_WARNING "%s: IO-mem disabled, cannot handle interrupt\n",
		   __FUNCTION__);
#ifndef RTAITIMING
#ifdef LINUX26
	    return IRQ_NONE;
#else
	    return;
#endif		
#else
	/* rtai */
	return;
#endif
    }
    
    /*	we assume a few things in this irq handler:
	- the hfc-pci will only generate "timer" irqs (proc/non-proc)
	- we need to use every 8th IRQ (to generate 1khz timing)
	OR
	- if we use rtai for timing the hfc-pci will not generate ANY irq,
	  instead rtai will call this "fake" irq with a 1khz realtime timer. :)
	- rtai will directly service the card, not like it used to by triggering
	  the linux irq
    */

#ifndef RTAITIMING
    spin_lock_irqsave(&hfctmp->lock, flags);
    stat = hfc_inb(hfctmp, hfc_STATUS);

    if ((stat & hfc_STATUS_ANYINT) == 0) {
        // maybe we are sharing the irq
	spin_unlock_irqrestore(&hfctmp->lock,flags);
#ifdef LINUX26
	return IRQ_NONE;
#else
	return;
#endif		
    }
#endif

    s1 = hfc_inb(hfctmp, hfc_INT_S1);
    s2 = hfc_inb(hfctmp, hfc_INT_S2); 
    if (s1 != 0) {
	if (s1 & hfc_INTS_TIMER) {
	    // timer (bit 7)
	    // printk(KERN_CRIT "timer %d %d %d.\n", stat, s1, s2);
	}
	if (s1 & hfc_INTS_L1STATE) {
	    // state machine (bit 6)
	    // printk(KERN_CRIT "zaphfc: layer 1 state machine interrupt\n");
	    zthfc = hfctmp->ztdev;
	    l1state = hfc_inb(hfctmp,hfc_STATES)  & hfc_STATES_STATE_MASK;
	    if (hfctmp->regs.nt_mode == 1) {
		if (debug) {
	    	    printk(KERN_CRIT "zaphfc: card %d layer 1 state = G%d\n", hfctmp->cardno, l1state);
		}
		switch (l1state) {
		    case 3:
#ifdef RTAITIMING
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT] layer 1 ACTIVATED (G%d) [realtime]", hfctmp->cardno, l1state);
#else
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT] layer 1 ACTIVATED (G%d)", hfctmp->cardno, l1state);
#endif
			break;
		    default:
#ifdef RTAITIMING
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT] layer 1 DEACTIVATED (G%d) [realtime]", hfctmp->cardno, l1state);
#else
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT] layer 1 DEACTIVATED (G%d)", hfctmp->cardno, l1state);
#endif
		}
		if (l1state == 2) {
		    hfc_outb(hfctmp, hfc_STATES, hfc_STATES_ACTIVATE | hfc_STATES_DO_ACTION | hfc_STATES_NT_G2_G3);
		} else if (l1state == 3) {
		    // fix to G3 state (see specs)
		    hfc_outb(hfctmp, hfc_STATES, hfc_STATES_LOAD_STATE | 3);
		}
	    } else {
		if (debug) {
	    	    printk(KERN_CRIT "zaphfc: card %d layer 1 state = F%d\n", hfctmp->cardno, l1state);
		}
		switch (l1state) {
		    case 7:
#ifdef RTAITIMING
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE] layer 1 ACTIVATED (F%d) [realtime]", hfctmp->cardno, l1state);
#else
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE] layer 1 ACTIVATED (F%d)", hfctmp->cardno, l1state);
#endif
			break;
		    default:
#ifdef RTAITIMING
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE] layer 1 DEACTIVATED (F%d) [realtime]", hfctmp->cardno, l1state);
#else
			sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE] layer 1 DEACTIVATED (F%d)", hfctmp->cardno, l1state);
#endif
		}
		if (l1state == 3) {
		    hfc_outb(hfctmp, hfc_STATES, hfc_STATES_DO_ACTION | hfc_STATES_ACTIVATE);
		}
	    }
	    
	}
	if (s1 & hfc_INTS_DREC) {
	    // D chan RX (bit 5)
	    hfctmp->regs.int_drec++;
	    // mr. zapata there is something for you!
	//    printk(KERN_CRIT "d chan rx\n");		    
	}
	if (s1 & hfc_INTS_B2REC) {
	    // B2 chan RX (bit 4)
	}
	if (s1 & hfc_INTS_B1REC) {
	    // B1 chan RX (bit 3)
	}
	if (s1 & hfc_INTS_DTRANS) {
	    // D chan TX (bit 2)
//	    printk(KERN_CRIT "zaphfc: dchan frame transmitted.\n");
	}
	if (s1 & hfc_INTS_B2TRANS) {
	    // B2 chan TX (bit 1)
	}
	if (s1 & hfc_INTS_B1TRANS) {
	    // B1 chan TX (bit 0)
	}
    }
#ifdef RTAITIMING
    /* fake an irq */
    s2 |= hfc_M2_PROC_TRANS;
#endif
    if (s2 != 0) {
	if (s2 & hfc_M2_PMESEL) {
	    // kaboom irq (bit 7)
	    printk(KERN_CRIT "zaphfc: sync lost, pci performance too low. you might have some cpu throtteling enabled.\n");
	}
	if (s2 & hfc_M2_GCI_MON_REC) {
	    // RxR monitor channel (bit 2)
	}
	if (s2 & hfc_M2_GCI_I_CHG) {
	    // GCI I-change  (bit 1)
	}
	if (s2 & hfc_M2_PROC_TRANS) {
	    // processing/non-processing transition  (bit 0)
	    hfctmp->ticks++;
#ifndef RTAITIMING
	    if (hfctmp->ticks > 7) {
		// welcome to zaptel timing :)
#endif
	    	hfctmp->ticks = 0;

		if (hfctmp->ztdev->span.flags & ZT_FLAG_RUNNING) {
		    // clear dchan buffer
		    hfctmp->ztdev->chans[2].bytes2transmit = 0;
		    hfctmp->ztdev->chans[2].maxbytes2transmit = hfc_D_FIFO_SIZE;

		    zt_transmit(&(hfctmp->ztdev->span));

		    hfc_btrans(hfctmp,1);
		    hfc_btrans(hfctmp,2);
		    hfc_dtrans(hfctmp);
		}

		hfc_brec(hfctmp,1);
		hfc_brec(hfctmp,2);
		if (hfctmp->regs.int_drec > 0) {
		    // dchan data to read
		    hfc_drec(hfctmp);
		    if (hfctmp->ztdev->chans[2].bytes2receive > 0) {
			    if (debug) {
    				printk(KERN_CRIT "zaphfc: card %d RX [ ", hfctmp->cardno);
				if (hfctmp->ztdev->chans[2].eofrx) {
				    /* dont output CRC == less user confusion */
				    for (x=0; x < hfctmp->ztdev->chans[2].bytes2receive - 2; x++) {
					printk("%#2x ", hfctmp->drecbuf[x]);
				    }
				    printk("] %d bytes\n", hfctmp->ztdev->chans[2].bytes2receive - 2);
				} else {
				    for (x=0; x < hfctmp->ztdev->chans[2].bytes2receive; x++) {
					printk("%#2x ", hfctmp->drecbuf[x]);
				    }
				    printk("..] %d bytes\n", hfctmp->ztdev->chans[2].bytes2receive);
				}
			    }
		    }
		} else {
			// hmm....ok, let zaptel receive nothing
		    hfctmp->ztdev->chans[2].bytes2receive = 0;
		}
		if (hfctmp->ztdev->span.flags & ZT_FLAG_RUNNING) {
		    zt_receive(&(hfctmp->ztdev->span));
		}
		
#ifndef RTAITIMING
	    }
#endif
	}

    }
#ifndef RTAITIMING
    spin_unlock_irqrestore(&hfctmp->lock,flags);
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
#endif
}


static int zthfc_open(struct zt_chan *chan) {
    struct zt_hfc *zthfc = chan->pvt;
    struct hfc_card *hfctmp = zthfc->card;
    
    if (!hfctmp) {
    return 0;
    }
#ifndef LINUX26  
    MOD_INC_USE_COUNT;
#else
    try_module_get(THIS_MODULE);
#endif
    return 0;
}

static int zthfc_close(struct zt_chan *chan) {
    struct zt_hfc *zthfc = chan->pvt;
    struct hfc_card *hfctmp = zthfc->card;

    if (!hfctmp) {
	return 0;
    }

#ifndef LINUX26  
    MOD_DEC_USE_COUNT;
#else
    module_put(THIS_MODULE);
#endif
    return 0;
}

static int zthfc_rbsbits(struct zt_chan *chan, int bits) {
    return 0;
}

static int zthfc_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data) {
        switch(cmd) {
        default:
                return -ENOTTY;
        }
        return 0;
}

static int zthfc_startup(struct zt_span *span) {
    struct zt_hfc *zthfc = span->pvt;
    struct hfc_card *hfctmp = zthfc->card;
    int alreadyrunning;
    
    if (hfctmp == NULL) {
	printk(KERN_INFO "zaphfc: no card for span at startup!\n");
    }
    alreadyrunning = span->flags & ZT_FLAG_RUNNING;
    
    if (!alreadyrunning) {
	span->chans[2].flags &= ~ZT_FLAG_HDLC;
	span->chans[2].flags |= ZT_FLAG_BRIDCHAN;
	
	span->flags |= ZT_FLAG_RUNNING;

	hfctmp->ticks = -2;
	hfctmp->clicks = 0;
	hfctmp->regs.fifo_en = hfc_FIFOEN_D | hfc_FIFOEN_B1 | hfc_FIFOEN_B2;
        hfc_outb(hfctmp, hfc_FIFO_EN, hfctmp->regs.fifo_en);
    } else {
	return 0;
    }

    // drivers, start engines!
    hfc_outb(hfctmp, hfc_STATES, hfc_STATES_DO_ACTION | hfc_STATES_ACTIVATE);
    return 0;
}

static int zthfc_shutdown(struct zt_span *span) {
    return 0;
}

static int zthfc_maint(struct zt_span *span, int cmd) {
    return 0;
}

static int zthfc_chanconfig(struct zt_chan *chan, int sigtype) {
//    printk(KERN_CRIT "chan_config sigtype=%d\n", sigtype);
    return 0;
}

static int zthfc_spanconfig(struct zt_span *span, struct zt_lineconfig *lc) {
    span->lineconfig = lc->lineconfig;
    return 0;
}

static int zthfc_initialize(struct zt_hfc *zthfc) {
    struct hfc_card *hfctmp = zthfc->card;
    int i;

    memset(&zthfc->span, 0x0, sizeof(struct zt_span)); // you never can tell...

    sprintf(zthfc->span.name, "ZTHFC%d", hfc_dev_count + 1);
    if (hfctmp->regs.nt_mode == 1) {
#ifdef RTAITIMING
	sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT] [realtime]", hfc_dev_count + 1);
#else
	sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [NT]", hfc_dev_count + 1);
#endif
    } else {
#ifdef RTAITIMING
	sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE] [realtime]", hfc_dev_count + 1);
#else
	sprintf(zthfc->span.desc, "HFC-S PCI A ISDN card %d [TE]", hfc_dev_count + 1);
#endif
    }

    zthfc->span.spanconfig = zthfc_spanconfig;
    zthfc->span.chanconfig = zthfc_chanconfig;
    zthfc->span.startup = zthfc_startup;
    zthfc->span.shutdown = zthfc_shutdown;
    zthfc->span.maint = zthfc_maint;
    zthfc->span.rbsbits = zthfc_rbsbits;
    zthfc->span.open = zthfc_open;
    zthfc->span.close = zthfc_close;
    zthfc->span.ioctl = zthfc_ioctl;

    zthfc->span.chans = zthfc->chans;
    zthfc->span.channels = 3;
    zthfc->span.deflaw = ZT_LAW_ALAW;
    zthfc->span.linecompat = ZT_CONFIG_AMI | ZT_CONFIG_CCS; // <--- this is really BS
    zthfc->span.offset = 0;
    init_waitqueue_head(&zthfc->span.maintq);
    zthfc->span.pvt = zthfc;

    for (i = 0; i < zthfc->span.channels; i++) {
	memset(&(zthfc->chans[i]), 0x0, sizeof(struct zt_chan));
	sprintf(zthfc->chans[i].name, "ZTHFC%d/%d/%d", hfc_dev_count + 1,0,i + 1);
	zthfc->chans[i].pvt = zthfc;
	zthfc->chans[i].sigcap =  ZT_SIG_EM | ZT_SIG_CLEAR | ZT_SIG_FXSLS | ZT_SIG_FXSGS | ZT_SIG_FXSKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_FXOKS | ZT_SIG_CAS | ZT_SIG_SF;
	zthfc->chans[i].chanpos = i + 1; 
    }

    if (zt_register(&zthfc->span,0)) {
	printk(KERN_CRIT "unable to register zaptel device!\n");
	return -1;
    }
//    printk(KERN_CRIT "zaphfc: registered zaptel device!\n");
    return 0;
}

#ifdef RTAITIMING
#define TICK_PERIOD  1000000
#define TICK_PERIOD2 1000000000
#define TASK_PRIORITY 1
#define STACK_SIZE 10000

static RT_TASK rt_task;
static struct hfc_card *rtai_hfc_list[hfc_MAX_CARDS];
static unsigned char rtai_hfc_counter = 0;

static void rtai_register_hfc(struct hfc_card *hfctmp) {
    rtai_hfc_list[rtai_hfc_counter++] = hfctmp;
}

static void rtai_loop(int t) {
    int i=0;
    for (;;) {
	for (i=0; i < rtai_hfc_counter; i++) {
	    if (rtai_hfc_list[i] != NULL)
		hfc_service(rtai_hfc_list[i]);
	}
        rt_task_wait_period();
    }
}
#endif

int hfc_findCards(int pcivendor, int pcidevice, char *vendor_name, char *card_name) {
    struct pci_dev *tmp;
    struct hfc_card *hfctmp = NULL;
    struct zt_hfc *zthfc = NULL;

    tmp = pci_find_device(pcivendor, pcidevice, multi_hfc);
    while (tmp != NULL) {
	multi_hfc = tmp;	// skip this next time.

	if (pci_enable_device(tmp)) {
	    multi_hfc = NULL;
	    return -1;
	}
	pci_set_master(tmp);

	hfctmp = kmalloc(sizeof(struct hfc_card), GFP_KERNEL);
	if (!hfctmp) {
	    printk(KERN_WARNING "zaphfc: unable to kmalloc!\n");
	    pci_disable_device(tmp);
	    multi_hfc = NULL;
	    return -ENOMEM;
	}
	memset(hfctmp, 0x0, sizeof(struct hfc_card));
	spin_lock_init(&hfctmp->lock);
	
	hfctmp->pcidev = tmp;
	hfctmp->pcibus = tmp->bus->number;
	hfctmp->pcidevfn = tmp->devfn; 

	if (!tmp->irq) {
	    printk(KERN_WARNING "zaphfc: no irq!\n");
	} else {
	    hfctmp->irq = tmp->irq;
	}

	hfctmp->pci_io = (char *) tmp->resource[1].start;
	if (!hfctmp->pci_io) {
	    printk(KERN_WARNING "zaphfc: no iomem!\n");
	    kfree(hfctmp);
	    pci_disable_device(tmp);
	    multi_hfc = NULL;
	    return -1;
	}
	
	hfctmp->fifomem = kmalloc(65536, GFP_KERNEL);
	if (!hfctmp->fifomem) {
	    printk(KERN_WARNING "zaphfc: unable to kmalloc fifomem!\n");
	    kfree(hfctmp);
	    pci_disable_device(tmp);
	    multi_hfc = NULL;
	    return -ENOMEM;
	} else {
	    memset(hfctmp->fifomem, 0x0, 65536);
	    hfctmp->fifos = (void *)(((ulong) hfctmp->fifomem) & ~0x7FFF) + 0x8000;
	    pci_write_config_dword(hfctmp->pcidev, 0x80, (u_int) virt_to_bus(hfctmp->fifos));
	    hfctmp->pci_io = ioremap((ulong) hfctmp->pci_io, 256);
	}

#ifdef RTAITIMING
	/* we need no stinking irq */
	hfctmp->irq = 0;
#else
	if (request_irq(hfctmp->irq, &hfc_interrupt, SA_INTERRUPT | SA_SHIRQ, "zaphfc", hfctmp)) {
	    printk(KERN_WARNING "zaphfc: unable to register irq\n");
	    kfree(hfctmp->fifomem);
	    kfree(hfctmp);
	    iounmap((void *) hfctmp->pci_io);
	    pci_disable_device(tmp);
	    multi_hfc = NULL;
	    return -EIO;
	}
#endif

#ifdef RTAITIMING
	rtai_register_hfc(hfctmp);
#endif
	printk(KERN_INFO
		       "zaphfc: %s %s configured at mem %lx fifo %lx(%#x) IRQ %d HZ %d\n",
			vendor_name, card_name,
		       (unsigned long) hfctmp->pci_io,
		       (unsigned long) hfctmp->fifos,
		       (u_int) virt_to_bus(hfctmp->fifos),
		       hfctmp->irq, HZ); 
	pci_write_config_word(hfctmp->pcidev, PCI_COMMAND, PCI_COMMAND_MEMORY);	// enable memio
	hfctmp->regs.int_m1 = 0;	// no ints
	hfctmp->regs.int_m2 = 0;	// not at all
	hfc_outb(hfctmp,hfc_INT_M1,hfctmp->regs.int_m1);
	hfc_outb(hfctmp,hfc_INT_M2,hfctmp->regs.int_m2);

	if ((modes & (1 << hfc_dev_count)) != 0) {
	    printk(KERN_INFO "zaphfc: Card %d configured for NT mode\n",hfc_dev_count);
	    hfctmp->regs.nt_mode = 1;
	} else {
	    printk(KERN_INFO "zaphfc: Card %d configured for TE mode\n",hfc_dev_count);
	    hfctmp->regs.nt_mode = 0;
	}

	zthfc = kmalloc(sizeof(struct zt_hfc),GFP_KERNEL);
	if (!zthfc) {
	    printk(KERN_CRIT "zaphfc: unable to kmalloc!\n");
	    hfc_shutdownCard(hfctmp);
	    kfree(hfctmp);
	    multi_hfc = NULL;
	    return -ENOMEM;
	}
	memset(zthfc, 0x0, sizeof(struct zt_hfc));

	zthfc->card = hfctmp;
	zthfc_initialize(zthfc);
	hfctmp->ztdev = zthfc;

	memset(hfctmp->drecbuf, 0x0, sizeof(hfctmp->drecbuf));
	hfctmp->ztdev->chans[2].readchunk = hfctmp->drecbuf;

	memset(hfctmp->dtransbuf, 0x0, sizeof(hfctmp->dtransbuf));
	hfctmp->ztdev->chans[2].writechunk = hfctmp->dtransbuf;

	memset(hfctmp->brecbuf[0], 0x0, sizeof(hfctmp->brecbuf[0]));
	hfctmp->ztdev->chans[0].readchunk = hfctmp->brecbuf[0];
	memset(hfctmp->btransbuf[0], 0x0, sizeof(hfctmp->btransbuf[0]));
	hfctmp->ztdev->chans[0].writechunk = hfctmp->btransbuf[0];

	memset(hfctmp->brecbuf[1], 0x0, sizeof(hfctmp->brecbuf[1]));
	hfctmp->ztdev->chans[1].readchunk = hfctmp->brecbuf[1];
	memset(hfctmp->btransbuf[1], 0x0, sizeof(hfctmp->btransbuf[1]));
	hfctmp->ztdev->chans[1].writechunk = hfctmp->btransbuf[1];


	hfc_registerCard(hfctmp);
	hfc_resetCard(hfctmp);
	tmp = pci_find_device(pcivendor, pcidevice, multi_hfc);
    }
    return 0;
}



int init_module(void) {
    int i = 0;
#ifdef RTAITIMING
    RTIME tick_period;
    for (i=0; i < hfc_MAX_CARDS; i++) {
	rtai_hfc_list[i] = NULL;
    }
    rt_set_periodic_mode();
#endif
    i = 0;
    while (id_list[i].vendor_id) {
	multi_hfc = NULL;
	hfc_findCards(id_list[i].vendor_id, id_list[i].device_id, id_list[i].vendor_name, id_list[i].card_name);
	i++;
    }
#ifdef RTAITIMING
    for (i=0; i < hfc_MAX_CARDS; i++) {
        if (rtai_hfc_list[i]) {
	    printk(KERN_INFO
		       "zaphfc: configured %d at mem %#x fifo %#x(%#x) for realtime servicing\n",
			rtai_hfc_list[i]->cardno,
		       (u_int) rtai_hfc_list[i]->pci_io,
		       (u_int) rtai_hfc_list[i]->fifos,
		       (u_int) virt_to_bus(rtai_hfc_list[i]->fifos));

	}
    }
    rt_task_init(&rt_task, rtai_loop, 1, STACK_SIZE, TASK_PRIORITY, 0, 0);
    tick_period = start_rt_timer(nano2count(TICK_PERIOD));
    rt_task_make_periodic(&rt_task, rt_get_time() + tick_period, tick_period);
#endif
    printk(KERN_INFO "zaphfc: %d hfc-pci card(s) in this box.\n", hfc_dev_count);
    return 0;
}

void cleanup_module(void) {
    struct hfc_card *tmpcard;
#ifdef RTAITIMING
    stop_rt_timer();
    rt_task_delete(&rt_task);
#endif
    printk(KERN_INFO "zaphfc: stop\n");
//    spin_lock(&registerlock);
    while (hfc_dev_list != NULL) {
	if (hfc_dev_list == NULL) break;
	hfc_shutdownCard(hfc_dev_list);
	tmpcard = hfc_dev_list;
	hfc_dev_list = hfc_dev_list->next;
	if (tmpcard != NULL) {
	    kfree(tmpcard);
	    tmpcard = NULL;
	    printk(KERN_INFO "zaphfc: freed one card.\n");
	}
    }
//    spin_unlock(&registerlock);
}
#endif


#ifdef LINUX26
module_param(modes, int, 0600);
module_param(debug, int, 0600);
#else
MODULE_PARM(modes,"i");
MODULE_PARM(debug,"i");
#endif

MODULE_DESCRIPTION("HFC-S PCI A Zaptel Driver");
MODULE_AUTHOR("Klaus-Peter Junghanns <kpj@junghanns.net>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif	
