/*
 * TE410P  Quad-T1/E1 PCI Driver version 0.1, 12/16/02
 *
 * Written by Mark Spencer <markster@linux-support.net>
 * Based on previous works, designs, and archetectures conceived and
 * written by Jim Dixon <jim@lambdatel.com>.
 *
 * Copyright (C) 2001 Jim Dixon / Zapata Telephony.
 * Copyright (C) 2001, Linux Support Services, Inc.
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#ifdef STANDALONE_ZAPATA
#include "zaptel.h"
#else
#include <linux/zaptel.h>
#endif
#ifdef LINUX26
#include <linux/moduleparam.h>
#endif
#include "wct4xxp.h"

/*
 * Tasklets provide better system interactive response at the cost of the
 * possibility of losing a frame of data at very infrequent intervals.  If
 * you are more concerned with the performance of your machine, enable the
 * tasklets.  If you are strict about absolutely no drops, then do not enable
 * tasklets.
 */

/* #define ENABLE_TASKLETS */

/* Define to get more attention-grabbing but slightly more I/O using
   alarm status */
#define FANCY_ALARM

static int debug;
static int timingcable;
static int highestorder;
static int t1e1override = -1;
static int loopback = 0;
static int alarmdebounce = 0;
static int noburst = 0;

#ifdef FANCY_ALARM
static int altab[] = {
0, 0, 0, 1, 2, 3, 4, 6, 8, 9, 11, 13, 16, 18, 20, 22, 24, 25, 27, 28, 29, 30, 31, 31, 32, 31, 31, 30, 29, 28, 27, 25, 23, 22, 20, 18, 16, 13, 11, 9, 8, 6, 4, 3, 2, 1, 0, 0, 
};
#endif

#define MAX_SPANS 16

#define FLAG_STARTED (1 << 0)
#define FLAG_NMF (1 << 1)
#define FLAG_SENDINGYELLOW (1 << 2)


#define	TYPE_T1	1		/* is a T1 card */
#define	TYPE_E1	2		/* is an E1 card */

#define CANARY 0xc0de

static int inirq = 0;

struct t4 {
	/* This structure exists one per card */
	struct pci_dev *dev;		/* Pointer to PCI device */
	int intcount;
	int num;			/* Which card we are */
	int t1e1;			/* T1/E1 select pins */
	int globalconfig;	/* Whether global setup has been done */
	int syncsrc;			/* active sync source */
	int syncs[4];			/* sync sources */
	int psyncs[4];			/* span-relative sync sources */
	int alarmtimer[4];		/* Alarm timer */
	int redalarms[4];
	int blinktimer;
	int alarmcount[4];			/* How much red alarm we've seen */
#ifdef FANCY_ALARM
	int alarmpos;
#endif
	int irq;			/* IRQ used by device */
	int order;			/* Order */
	int flags;			/* Device flags */
	int spanflags[4];		/* Span flags */
	int syncpos[4];			/* span-relative sync sources */
	int master;				/* Are we master */
	int ledreg;				/* LED Register */
	int e1check[4];			/* E1 check */
	unsigned int dmactrl;
	int e1recover;			/* E1 recovery timer */
	dma_addr_t 	readdma;
	dma_addr_t	writedma;
	unsigned long memaddr;		/* Base address of card */
	unsigned long memlen;
	volatile unsigned int *membase;	/* Base address of card */
	struct zt_span spans[4];	/* Spans */
	struct zt_chan *chans[4];	/* Pointers to blocks of 24(30/31) contiguous zt_chans for each span */
	unsigned char txsigs[4][16];  /* Copy of tx sig registers */
	int loopupcnt[4];		/* loop up code counter */
	int loopdowncnt[4];		/* loop down code counter */
	int spansstarted;		/* number of spans started */
	/* spinlock_t lock; */		/* lock context */
	spinlock_t reglock;		/* lock register access */
	unsigned char ec_chunk1[4][31][ZT_CHUNKSIZE]; /* first EC chunk buffer */
	unsigned char ec_chunk2[4][31][ZT_CHUNKSIZE]; /* second EC chunk buffer */
	volatile unsigned int *writechunk;					/* Double-word aligned write memory */
	volatile unsigned int *readchunk;					/* Double-word aligned read memory */
	unsigned short canary;
#ifdef ENABLE_TASKLETS
	int taskletrun;
	int taskletsched;
	int taskletpending;
	int taskletexec;
	int txerrors;
	struct tasklet_struct t4xxp_tlet;
#endif
	int spantype[4];		/* card type, T1 or E1 */
	unsigned int passno;	/* number of interrupt passes */
	char *variety;
	int last0;		/* for detecting double-missed IRQ */
	int checktiming;	/* Set >0 to cause the timing source to be checked */
};


static void __set_clear(struct t4 *wc, int span);
static int t4_startup(struct zt_span *span);
static int t4_shutdown(struct zt_span *span);
static int t4_rbsbits(struct zt_chan *chan, int bits);
static int t4_maint(struct zt_span *span, int cmd);
static int t4_reset_dma(struct t4 *wc);
static int t4_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data);
static void __t4_set_timing_source(struct t4 *wc, int unit);

#define WC_RDADDR	0
#define WC_WRADDR	1
#define WC_COUNT	2
#define WC_DMACTRL	3	
#define WC_INTR		4
/* #define WC_GPIO		5 */
#define WC_VERSION	6
#define WC_LEDS		7
#define WC_ACTIVATE	(1 << 12)
#define WC_GPIOCTL	8
#define WC_GPIO		9
#define WC_LADDR	10
#define WC_LDATA		11
#define WC_LREAD			(1 << 15)
#define WC_LWRITE		(1 << 16)

#define WC_OFF    (0)
#define WC_RED    (1)
#define WC_GREEN  (2)
#define WC_YELLOW (3)

#define MAX_T4_CARDS 64

#ifdef ENABLE_TASKLETS
static void t4_tasklet(unsigned long data);
#endif

static struct t4 *cards[MAX_T4_CARDS];

static inline void __t4_pci_out(struct t4 *wc, const unsigned int addr, const unsigned int value)
{
	unsigned int tmp;
	wc->membase[addr] = value;
#if 1
	tmp = wc->membase[addr];
	if ((value != tmp) && (addr != WC_LEDS) && (addr != WC_LDATA) &&
		(addr != WC_GPIO) && (addr != WC_INTR))
		printk("Tried to load %08x into %08x, but got %08x instead\n", value, addr, tmp);
#endif		
}

static inline unsigned int __t4_pci_in(struct t4 *wc, const unsigned int addr)
{
	return wc->membase[addr];
}

static inline void t4_pci_out(struct t4 *wc, const unsigned int addr, const unsigned int value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->reglock, flags);
	__t4_pci_out(wc, addr, value);
	spin_unlock_irqrestore(&wc->reglock, flags);
}

static inline void __t4_set_led(struct t4 *wc, int span, int color)
{
	int oldreg = wc->ledreg;
	wc->ledreg &= ~(0x3 << (span << 1));
	wc->ledreg |= (color << (span << 1));
	if (oldreg != wc->ledreg)
		__t4_pci_out(wc, WC_LEDS, wc->ledreg);
}

static inline void t4_activate(struct t4 *wc)
{
	wc->ledreg |= WC_ACTIVATE;
	t4_pci_out(wc, WC_LEDS, wc->ledreg);
}

static inline unsigned int t4_pci_in(struct t4 *wc, const unsigned int addr)
{
	unsigned int ret;
	unsigned long flags;
	
	spin_lock_irqsave(&wc->reglock, flags);
	ret = __t4_pci_in(wc, addr);
	spin_unlock_irqrestore(&wc->reglock, flags);
	return ret;
}

static inline unsigned int __t4_framer_in(struct t4 *wc, int unit, const unsigned int addr)
{
	unsigned int ret;
	unit &= 0x3;
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff));
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff) | ( 1 << 10) | WC_LREAD);
	ret = __t4_pci_in(wc, WC_LDATA);
	__t4_pci_out(wc, WC_LADDR, 0);
	return ret & 0xff;
}

static inline unsigned int t4_framer_in(struct t4 *wc, int unit, const unsigned int addr)
{
	unsigned long flags;
	unsigned int ret;
	spin_lock_irqsave(&wc->reglock, flags);
	ret = __t4_framer_in(wc, unit, addr);
	spin_unlock_irqrestore(&wc->reglock, flags);
	return ret;

}

static inline void __t4_framer_out(struct t4 *wc, int unit, const unsigned int addr, const unsigned int value)
{
	unit &= 0x3;
	if (debug)
		printk("Writing %02x to address %02x of unit %d\n", value, addr, unit);
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff));
	__t4_pci_out(wc, WC_LDATA, value);
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff) | (1 << 10));
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff) | (1 << 10) | WC_LWRITE);
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff) | (1 << 10));
	__t4_pci_out(wc, WC_LADDR, (unit << 8) | (addr & 0xff));	
	__t4_pci_out(wc, WC_LADDR, 0);
	if (debug) printk("Write complete\n");
#if 0
	{ unsigned int tmp;
	tmp = t4_framer_in(wc, unit, addr);
	if (tmp != value) {
		printk("Expected %d from unit %d register %d but got %d instead\n", value, unit, addr, tmp);
	} }
#endif	
}

static inline void t4_framer_out(struct t4 *wc, int unit, const unsigned int addr, const unsigned int value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->reglock, flags);
	__t4_framer_out(wc, unit, addr, value);
	spin_unlock_irqrestore(&wc->reglock, flags);
}


static void __set_clear(struct t4 *wc, int span)
{
	int i,j;
	unsigned short val=0;
	for (i=0;i<24;i++) {
		j = (i/8);
		if (wc->spans[span].chans[i].flags & ZT_FLAG_CLEAR) 
			val |= 1 << (7 - (i % 8));
		if ((i % 8)==7) {
			if (debug)
				printk("Putting %d in register %02x on span %d\n",
			       val, 0x2f + j, span + 1);
			__t4_framer_out(wc,span, 0x2f + j, val);
			val = 0;
		}
	}
}

#if 0
static void set_clear(struct t4 *wc, int span)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->reglock, flags);
	__set_clear(wc, span);
	spin_unlock_irqrestore(&wc->reglock, flags);
}
#endif

static int t4_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	struct t4_regs regs;
	int x;
	switch(cmd) {
	case WCT4_GET_REGS:
		for (x=0;x<NUM_PCI;x++)
			regs.pci[x] = t4_pci_in(chan->pvt, x);
		for (x=0;x<NUM_REGS;x++)
			regs.regs[x] = t4_framer_in(chan->pvt, chan->span->offset, x);
		if (copy_to_user((struct t4_regs *)data, &regs, sizeof(regs)))
			return -EFAULT;
		break;
	default:
		return -ENOTTY;
	}
	return 0;
}

static int t4_maint(struct zt_span *span, int cmd)
{
	struct t4 *wc = span->pvt;

	if (wc->spantype[span->offset] == TYPE_E1) {
		switch(cmd) {
		case ZT_MAINT_NONE:
			printk("XXX Turn off local and remote loops E1 XXX\n");
			break;
		case ZT_MAINT_LOCALLOOP:
			printk("XXX Turn on local loopback E1 XXX\n");
			break;
		case ZT_MAINT_REMOTELOOP:
			printk("XXX Turn on remote loopback E1 XXX\n");
			break;
		case ZT_MAINT_LOOPUP:
			printk("XXX Send loopup code E1 XXX\n");
			break;
		case ZT_MAINT_LOOPDOWN:
			printk("XXX Send loopdown code E1 XXX\n");
			break;
		case ZT_MAINT_LOOPSTOP:
			printk("XXX Stop sending loop codes E1 XXX\n");
			break;
		default:
			printk("TE410P: Unknown E1 maint command: %d\n", cmd);
			break;
		}
	} else {
		switch(cmd) {
	    case ZT_MAINT_NONE:
			printk("XXX Turn off local and remote loops T1 XXX\n");
			break;
	    case ZT_MAINT_LOCALLOOP:
			printk("XXX Turn on local loop and no remote loop XXX\n");
			break;
	    case ZT_MAINT_REMOTELOOP:
			printk("XXX Turn on remote loopup XXX\n");
			break;
	    case ZT_MAINT_LOOPUP:
			t4_framer_out(wc, span->offset, 0x21, 0x50);	/* FMR5: Nothing but RBS mode */
			break;
	    case ZT_MAINT_LOOPDOWN:
			t4_framer_out(wc, span->offset, 0x21, 0x60);	/* FMR5: Nothing but RBS mode */
			break;
	    case ZT_MAINT_LOOPSTOP:
			t4_framer_out(wc, span->offset, 0x21, 0x40);	/* FMR5: Nothing but RBS mode */
			break;
	    default:
			printk("TE410P: Unknown T1 maint command: %d\n", cmd);
			break;
	   }
    }
	return 0;
}

static int t4_rbsbits(struct zt_chan *chan, int bits)
{
	u_char m,c;
	int k,n,b;
	struct t4 *wc = chan->pvt;
	unsigned long flags;
	
	if(debug) printk("Setting bits to %d on channel %s\n", bits, chan->name);
	spin_lock_irqsave(&wc->reglock, flags);	
	k = chan->span->offset;
	if (wc->spantype[k] == TYPE_E1) { /* do it E1 way */
		if (chan->chanpos == 16) {
			spin_unlock_irqrestore(&wc->reglock, flags);
			return 0;
		}
		n = chan->chanpos - 1;
		if (chan->chanpos > 15) n--;
		b = (n % 15);
		c = wc->txsigs[k][b];
		m = (n / 15) << 2; /* nibble selector */
		c &= (0xf << m); /* keep the other nibble */
		c |= (bits & 0xf) << (4 - m); /* put our new nibble here */
		wc->txsigs[k][b] = c;
		  /* output them to the chip */
		__t4_framer_out(wc,k,0x71 + b,c); 
	} else if (wc->spans[k].lineconfig & ZT_CONFIG_D4) {
		n = chan->chanpos - 1;
		b = (n/4);
		c = wc->txsigs[k][b];
		m = ((3 - (n % 4)) << 1); /* nibble selector */
		c &= ~(0x3 << m); /* keep the other nibble */
		c |= ((bits >> 2) & 0x3) << m; /* put our new nibble here */
		wc->txsigs[k][b] = c;
		  /* output them to the chip */
		__t4_framer_out(wc,k,0x70 + b,c); 
		__t4_framer_out(wc,k,0x70 + b + 6,c); 
	} else if (wc->spans[k].lineconfig & ZT_CONFIG_ESF) {
		n = chan->chanpos - 1;
		b = (n/2);
		c = wc->txsigs[k][b];
		m = ((n % 2) << 2); /* nibble selector */
		c &= (0xf << m); /* keep the other nibble */
		c |= (bits & 0xf) << (4 - m); /* put our new nibble here */
		wc->txsigs[k][b] = c;
		  /* output them to the chip */
		__t4_framer_out(wc,k,0x70 + b,c); 
	} 
	spin_unlock_irqrestore(&wc->reglock, flags);
	if (debug)
		printk("Finished setting RBS bits\n");
	return 0;
}

static int t4_shutdown(struct zt_span *span)
{
	int tspan;
	int wasrunning;
	unsigned long flags;
	struct t4 *wc = span->pvt;

	tspan = span->offset + 1;
	if (tspan < 0) {
		printk("T4XXP: Span '%d' isn't us?\n", span->spanno);
		return -1;
	}

	spin_lock_irqsave(&wc->reglock, flags);
	wasrunning = span->flags & ZT_FLAG_RUNNING;

	span->flags &= ~ZT_FLAG_RUNNING;
	if (wasrunning)
		wc->spansstarted--;
	__t4_set_led(wc, span->offset, WC_OFF);
	if ((!(wc->spans[0].flags & ZT_FLAG_RUNNING)) &&
	    (!(wc->spans[1].flags & ZT_FLAG_RUNNING)) &&
	    (!(wc->spans[2].flags & ZT_FLAG_RUNNING)) &&
	    (!(wc->spans[3].flags & ZT_FLAG_RUNNING))) {
		/* No longer in use, disable interrupts */
		printk("TE410P: Disabling interrupts since there are no active spans\n");
		wc->dmactrl = 0x0;
		__t4_pci_out(wc, WC_DMACTRL, 0x00000000);
		/* Acknowledge any pending interrupts */
		__t4_pci_out(wc, WC_INTR, 0x00000000);
		__t4_set_timing_source(wc,4);
	} else wc->checktiming = 1;
	spin_unlock_irqrestore(&wc->reglock, flags);
	if (debug)
		printk("Span %d (%s) shutdown\n", span->spanno, span->name);
	return 0;
}

static int t4_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	int i;
	struct t4 *wc = span->pvt;

	if (debug)
		printk("TE410P: Configuring span %d\n", span->spanno);
	/* XXX We assume lineconfig is okay and shouldn't XXX */	
	span->lineconfig = lc->lineconfig;
	span->txlevel = lc->lbo;
	span->rxlevel = 0;
	if (lc->sync < 0)
		lc->sync = 0;
	if (lc->sync > 4)
		lc->sync = 0;
	
	/* remove this span number from the current sync sources, if there */
	for(i = 0; i < 4; i++) {
		if (wc->syncs[i] == span->spanno) {
			wc->syncs[i] = 0;
			wc->psyncs[i] = 0;
		}
	}
	wc->syncpos[span->offset] = lc->sync;
	/* if a sync src, put it in proper place */
	if (lc->sync) {
		wc->syncs[lc->sync - 1] = span->spanno;
		wc->psyncs[lc->sync - 1] = span->offset + 1;
	}
	wc->checktiming = 1;
	/* If we're already running, then go ahead and apply the changes */
	if (span->flags & ZT_FLAG_RUNNING)
		return t4_startup(span);
	return 0;
}

static int t4_chanconfig(struct zt_chan *chan, int sigtype)
{
	int alreadyrunning;
	unsigned long flags;
	struct t4 *wc = chan->pvt;

	alreadyrunning = wc->spans[chan->span->offset].flags & ZT_FLAG_RUNNING;
	if (debug) {
		if (alreadyrunning)
			printk("TE410P: Reconfigured channel %d (%s) sigtype %d\n", chan->channo, chan->name, sigtype);
		else
			printk("TE410P: Configured channel %d (%s) sigtype %d\n", chan->channo, chan->name, sigtype);
	}		
	/* nothing more to do if an E1 */
	if (wc->spantype[chan->span->offset] == TYPE_E1) return 0;
	spin_lock_irqsave(&wc->reglock, flags);	
	if (alreadyrunning)
		__set_clear(wc, chan->span->offset);
	spin_unlock_irqrestore(&wc->reglock, flags);	
	return 0;
}

static int t4_open(struct zt_chan *chan)
{
#ifndef LINUX26
	MOD_INC_USE_COUNT;
#endif	
	return 0;
}

static int t4_close(struct zt_chan *chan)
{
#ifndef LINUX26
	MOD_DEC_USE_COUNT;
#endif
	return 0;
}

static void init_spans(struct t4 *wc)
{
	int x,y,c;
	for (x=0;x<4;x++) {
		sprintf(wc->spans[x].name, "TE4/%d/%d", 
		       wc->num, x + 1);
		sprintf(wc->spans[x].desc, "TE410P (PCI) Card %d Span %d", wc->num, x+1);
		wc->spans[x].spanconfig = t4_spanconfig;
		wc->spans[x].chanconfig = t4_chanconfig;
		wc->spans[x].startup = t4_startup;
		wc->spans[x].shutdown = t4_shutdown;
		wc->spans[x].rbsbits = t4_rbsbits;
		wc->spans[x].maint = t4_maint;
		wc->spans[x].open = t4_open;
		wc->spans[x].close  = t4_close;
		if (wc->spantype[x] == TYPE_T1) {
			wc->spans[x].channels = 24;
			wc->spans[x].deflaw = ZT_LAW_MULAW;
		} else {
			wc->spans[x].channels = 31;
			wc->spans[x].deflaw = ZT_LAW_ALAW;
		}
		wc->spans[x].chans = wc->chans[x];
		wc->spans[x].flags = ZT_FLAG_RBS;
		wc->spans[x].linecompat = ZT_CONFIG_AMI | ZT_CONFIG_B8ZS | ZT_CONFIG_D4 | ZT_CONFIG_ESF;
		wc->spans[x].ioctl = t4_ioctl;
		wc->spans[x].pvt = wc;
		wc->spans[x].offset = x;
		init_waitqueue_head(&wc->spans[x].maintq);
		for (y=0;y<wc->spans[x].channels;y++) {
			struct zt_chan *mychans = wc->chans[x] + y;
			sprintf(mychans->name, "TE4/%d/%d/%d", wc->num, x + 1, y + 1);
			mychans->sigcap = ZT_SIG_EM | ZT_SIG_CLEAR | ZT_SIG_FXSLS | ZT_SIG_FXSGS | ZT_SIG_FXSKS |
									 ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_FXOKS | ZT_SIG_CAS | ZT_SIG_EM_E1 | ZT_SIG_DACS_RBS;
			c = (x * wc->spans[x].channels) + y;
			mychans->pvt = wc;
			mychans->chanpos = y + 1;
		}
	}
}

static void t4_serial_setup(struct t4 *wc, int unit)
{
	if (!wc->globalconfig) {
		wc->globalconfig = 1;
		printk("TE410P: Setting up global serial parameters\n");
		t4_framer_out(wc, 0, 0x85, 0xe0);	/* GPC1: Multiplex mode enabled, FSC is output, active low, RCLK from channel 0 */
		t4_framer_out(wc, 0, 0x08, 0x01);	/* IPC: Interrupt push/pull active low */
	
		/* Global clocks (8.192 Mhz CLK) */
		t4_framer_out(wc, 0, 0x92, 0x00);	
		t4_framer_out(wc, 0, 0x93, 0x18);
		t4_framer_out(wc, 0, 0x94, 0xfb);
		t4_framer_out(wc, 0, 0x95, 0x0b);
		t4_framer_out(wc, 0, 0x96, 0x00);
		t4_framer_out(wc, 0, 0x97, 0x0b);
		t4_framer_out(wc, 0, 0x98, 0xdb);
		t4_framer_out(wc, 0, 0x99, 0xdf);
	}

	/* Configure interrupts */	
	t4_framer_out(wc, unit, 0x46, 0x40);	/* GCR: Interrupt on Activation/Deactivation of AIX, LOS */

	/* Configure system interface */
	t4_framer_out(wc, unit, 0x3e, 0xc2);	/* SIC1: 8.192 Mhz clock/bus, double buffer receive / transmit, byte interleaved */
	t4_framer_out(wc, unit, 0x3f, unit << 1); /* SIC2: No FFS, no center receive eliastic buffer, phase */
	t4_framer_out(wc, unit, 0x40, 0x04);	/* SIC3: Edges for capture */
	t4_framer_out(wc, unit, 0x45, 0x00);	/* CMR2: We provide sync and clock for tx and rx. */
	if (!wc->t1e1) {
		t4_framer_out(wc, unit, 0x22, 0x03);	/* XC0: Normal operation of Sa-bits */
		t4_framer_out(wc, unit, 0x23, 0x84);	/* XC1: 0 offset */
		t4_framer_out(wc, unit, 0x24, 0x03);	/* RC0: Just shy of 1023 */
		t4_framer_out(wc, unit, 0x25, 0x84);	/* RC1: The rest of RC0 */
	} else {
		t4_framer_out(wc, unit, 0x22, 0x00);	/* XC0: Normal operation of Sa-bits */
		t4_framer_out(wc, unit, 0x23, 0x04);	/* XC1: 0 offset */
		t4_framer_out(wc, unit, 0x24, 0x04);	/* RC0: Just shy of 1023 */
		t4_framer_out(wc, unit, 0x25, 0x04);	/* RC1: The rest of RC0 */
	}
	
	/* Configure ports */
	t4_framer_out(wc, unit, 0x80, 0x00);	/* PC1: SPYR/SPYX input on RPA/XPA */
	t4_framer_out(wc, unit, 0x81, 0x22);	/* PC2: RMFB/XSIG output/input on RPB/XPB */
	t4_framer_out(wc, unit, 0x82, 0x65);	/* PC3: Some unused stuff */
	t4_framer_out(wc, unit, 0x83, 0x35);	/* PC4: Some more unused stuff */
	t4_framer_out(wc, unit, 0x84, 0x01);	/* PC5: XMFS active low, SCLKR is input, RCLK is output */
	if (debug)
		printk("Successfully initialized serial bus for unit %d\n", unit);
}

static void __t4_set_timing_source(struct t4 *wc, int unit)
{
	unsigned int timing;
	int x;
	if (unit != wc->syncsrc) {
		timing = 0x34;		/* CMR1: RCLK unit, 8.192 Mhz TCLK, RCLK is 8.192 Mhz */
		if ((unit > -1) && (unit < 4)) {
			timing |= (unit << 6);
			for (x=0;x<4;x++)  /* set all 4 receive reference clocks to unit */
				__t4_framer_out(wc, x, 0x44, timing);
			__t4_pci_out(wc, WC_DMACTRL, wc->dmactrl | (1 << 29));
		} else {
			for (x=0;x<4;x++) /* set each receive reference clock to itself */
				__t4_framer_out(wc, x, 0x44, timing | (x << 6));
			__t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
		}
		wc->syncsrc = unit;
		if ((unit < 0) || (unit > 3))
			unit = 0;
		else
			unit++;
		for (x=0;x<4;x++)
			wc->spans[x].syncsrc = unit;
	} else {
		if (debug)
			printk("TE410P: Timing source already set to %d\n", unit);
	}
#if	0
	printk("wct4xxp: Timing source set to %d\n",unit);
#endif
}

static void __t4_set_timing_source_auto(struct t4 *wc)
{
	int x;
	wc->checktiming = 0;
	for (x=0;x<4;x++) {
		if (wc->syncs[x]) {
			if ((wc->spans[wc->syncs[x] - 1].flags & ZT_FLAG_RUNNING) && 
				!(wc->spans[wc->syncs[x] - 1].alarms & (ZT_ALARM_RED | ZT_ALARM_BLUE) )) {
					/* Valid timing sourc e*/
					__t4_set_timing_source(wc, wc->syncs[x] - 1);
					return;
			}
		}
	}
	__t4_set_timing_source(wc, 4);
}

static void __t4_configure_t1(struct t4 *wc, int unit, int lineconfig, int txlevel)
{
	unsigned int fmr4, fmr2, fmr1, fmr0, lim2;
	char *framing, *line;
	int mytxlevel;
	if ((txlevel > 7) || (txlevel < 4))
		mytxlevel = 0;
	else
		mytxlevel = txlevel - 4;
	fmr1 = 0x9c; /* FMR1: Mode 1, T1 mode, CRC on for ESF, 8.192 Mhz system data rate, no XAIS */
	fmr2 = 0x22; /* FMR2: no payload loopback, auto send yellow alarm */
	if (loopback)
		fmr2 |= 0x4;
	fmr4 = 0x0c; /* FMR4: Lose sync on 2 out of 5 framing bits, auto resync */
	lim2 = 0x21; /* LIM2: 50% peak is a "1", Advanced Loss recovery */
	lim2 |= (mytxlevel << 6);	/* LIM2: Add line buildout */
	__t4_framer_out(wc, unit, 0x1d, fmr1);
	__t4_framer_out(wc, unit, 0x1e, fmr2);

	/* Configure line interface */
	if (lineconfig & ZT_CONFIG_AMI) {
		line = "AMI";
		fmr0 = 0xa0;
	} else {
		line = "B8ZS";
		fmr0 = 0xf0;
	}
	if (lineconfig & ZT_CONFIG_D4) {
		framing = "D4";
	} else {
		framing = "ESF";
		fmr4 |= 0x2;
		fmr2 |= 0xc0;
	}
	__t4_framer_out(wc, unit, 0x1c, fmr0);
	__t4_framer_out(wc, unit, 0x20, fmr4);
	__t4_framer_out(wc, unit, 0x21, 0x40);	/* FMR5: Enable RBS mode */

	__t4_framer_out(wc, unit, 0x37, 0xf0 );	/* LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS */
	__t4_framer_out(wc, unit, 0x36, 0x08);	/* LIM0: Enable auto long haul mode, no local loop (must be after LIM1) */

	__t4_framer_out(wc, unit, 0x02, 0x50);	/* CMDR: Reset the receiver and transmitter line interface */
	__t4_framer_out(wc, unit, 0x02, 0x00);	/* CMDR: Reset the receiver and transmitter line interface */

	__t4_framer_out(wc, unit, 0x3a, lim2);	/* LIM2: 50% peak amplitude is a "1" */
	__t4_framer_out(wc, unit, 0x38, 0x0a);	/* PCD: LOS after 176 consecutive "zeros" */
	__t4_framer_out(wc, unit, 0x39, 0x15);	/* PCR: 22 "ones" clear LOS */
		
	/* Generate pulse mask for T1 */
	switch(mytxlevel) {
	case 3:
		__t4_framer_out(wc, unit, 0x26, 0x07);	/* XPM0 */
		__t4_framer_out(wc, unit, 0x27, 0x01);	/* XPM1 */
		__t4_framer_out(wc, unit, 0x28, 0x00);	/* XPM2 */
		break;
	case 2:
		__t4_framer_out(wc, unit, 0x26, 0x8c);	/* XPM0 */
		__t4_framer_out(wc, unit, 0x27, 0x11);	/* XPM1 */
		__t4_framer_out(wc, unit, 0x28, 0x01);	/* XPM2 */
		break;
	case 1:
		__t4_framer_out(wc, unit, 0x26, 0x8c);	/* XPM0 */
		__t4_framer_out(wc, unit, 0x27, 0x01);	/* XPM1 */
		__t4_framer_out(wc, unit, 0x28, 0x00);	/* XPM2 */
		break;
	case 0:
	default:
		__t4_framer_out(wc, unit, 0x26, 0xd7);	/* XPM0 */
		__t4_framer_out(wc, unit, 0x27, 0x22);	/* XPM1 */
		__t4_framer_out(wc, unit, 0x28, 0x01);	/* XPM2 */
		break;
	}
	printk("TE410P: Span %d configured for %s/%s\n", unit + 1, framing, line);
}

static void __t4_configure_e1(struct t4 *wc, int unit, int lineconfig)
{
	unsigned int fmr2, fmr1, fmr0;
	unsigned int cas = 0;
	char *crc4 = "";
	char *framing, *line;
	fmr1 = 0x44; /* FMR1: E1 mode, Automatic force resync, PCM30 mode, 8.192 Mhz backplane, no XAIS */
	fmr2 = 0x03; /* FMR2: Auto transmit remote alarm, auto loss of multiframe recovery, no payload loopback */
	if (loopback)
		fmr2 |= 0x4;
	if (lineconfig & ZT_CONFIG_CRC4) {
		fmr1 |= 0x08;	/* CRC4 transmit */
		fmr2 |= 0xc0;	/* CRC4 receive */
		crc4 = "/CRC4";
	}
	__t4_framer_out(wc, unit, 0x1d, fmr1);
	__t4_framer_out(wc, unit, 0x1e, fmr2);

	/* Configure line interface */
	if (lineconfig & ZT_CONFIG_AMI) {
		line = "AMI";
		fmr0 = 0xa0;
	} else {
		line = "HDB3";
		fmr0 = 0xf0;
	}
	if (lineconfig & ZT_CONFIG_CCS) {
		framing = "CCS";
	} else {
		framing = "CAS";
		cas = 0x40;
	}
	__t4_framer_out(wc, unit, 0x1c, fmr0);

	__t4_framer_out(wc, unit, 0x37, 0xf0 /*| 0x6 */ );	/* LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS */
	__t4_framer_out(wc, unit, 0x36, 0x08);	/* LIM0: Enable auto long haul mode, no local loop (must be after LIM1) */

	__t4_framer_out(wc, unit, 0x02, 0x50);	/* CMDR: Reset the receiver and transmitter line interface */
	__t4_framer_out(wc, unit, 0x02, 0x00);	/* CMDR: Reset the receiver and transmitter line interface */

	/* Condition receive line interface for E1 after reset */
	__t4_framer_out(wc, unit, 0xbb, 0x17);
	__t4_framer_out(wc, unit, 0xbc, 0x55);
	__t4_framer_out(wc, unit, 0xbb, 0x97);
	__t4_framer_out(wc, unit, 0xbb, 0x11);
	__t4_framer_out(wc, unit, 0xbc, 0xaa);
	__t4_framer_out(wc, unit, 0xbb, 0x91);
	__t4_framer_out(wc, unit, 0xbb, 0x12);
	__t4_framer_out(wc, unit, 0xbc, 0x55);
	__t4_framer_out(wc, unit, 0xbb, 0x92);
	__t4_framer_out(wc, unit, 0xbb, 0x0c);
	__t4_framer_out(wc, unit, 0xbb, 0x00);
	__t4_framer_out(wc, unit, 0xbb, 0x8c);
	
	__t4_framer_out(wc, unit, 0x3a, 0x20);	/* LIM2: 50% peak amplitude is a "1" */
	__t4_framer_out(wc, unit, 0x38, 0x0a);	/* PCD: LOS after 176 consecutive "zeros" */
	__t4_framer_out(wc, unit, 0x39, 0x15);	/* PCR: 22 "ones" clear LOS */
	
	__t4_framer_out(wc, unit, 0x20, 0x9f);	/* XSW: Spare bits all to 1 */
	__t4_framer_out(wc, unit, 0x21, 0x1c|cas);	/* XSP: E-bit set when async. AXS auto, XSIF to 1 */
	
	
	/* Generate pulse mask for E1 */
	__t4_framer_out(wc, unit, 0x26, 0x54);	/* XPM0 */
	__t4_framer_out(wc, unit, 0x27, 0x02);	/* XPM1 */
	__t4_framer_out(wc, unit, 0x28, 0x00);	/* XPM2 */
	printk("TE410P: Span %d configured for %s/%s%s\n", unit + 1, framing, line, crc4);
}

static int t4_startup(struct zt_span *span)
{
	int i;
	int tspan;
	unsigned long flags;
	int alreadyrunning;
	struct t4 *wc = span->pvt;

	tspan = span->offset + 1;
	if (tspan < 0) {
		printk("TE410P: Span '%d' isn't us?\n", span->spanno);
		return -1;
	}

	spin_lock_irqsave(&wc->reglock, flags);

	alreadyrunning = span->flags & ZT_FLAG_RUNNING;

	/* initialize the start value for the entire chunk of last ec buffer */
	for(i = 0; i < span->channels; i++)
	{
		memset(wc->ec_chunk1[span->offset][i],
			ZT_LIN2X(0,&span->chans[i]),ZT_CHUNKSIZE);
		memset(wc->ec_chunk2[span->offset][i],
			ZT_LIN2X(0,&span->chans[i]),ZT_CHUNKSIZE);
	}
	/* Force re-evaluation fo timing source */
	if (timingcable)
		wc->syncsrc = -1;

	if (wc->spantype[span->offset] == TYPE_E1) { /* if this is an E1 card */
		__t4_configure_e1(wc, span->offset, span->lineconfig);
	} else { /* is a T1 card */
		__t4_configure_t1(wc, span->offset, span->lineconfig, span->txlevel);
		__set_clear(wc, span->offset);
	}
	
	if (!alreadyrunning) {
		span->flags |= ZT_FLAG_RUNNING;
		wc->spansstarted++;
			/* enable interrupts */
		/* Start DMA, enabling DMA interrupts on read only */
		wc->dmactrl = 0xc0000003 | (1 << 29);
		if (noburst)
			wc->dmactrl |= (1 << 26);
		__t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
	}

	spin_unlock_irqrestore(&wc->reglock, flags);
	if (wc->syncs[0] == span->spanno) printk("SPAN %d: Primary Sync Source\n",span->spanno);
	if (wc->syncs[1] == span->spanno) printk("SPAN %d: Secondary Sync Source\n",span->spanno);
	if (wc->syncs[2] == span->spanno) printk("SPAN %d: Tertiary Sync Source\n",span->spanno);
	if (wc->syncs[3] == span->spanno) printk("SPAN %d: Quaternary Sync Source\n",span->spanno);
	return 0;
}

static inline void e1_check(struct t4 *wc, int span, int val)
{
	if ((wc->spans[span].channels > 24) &&
	    (wc->spans[span].flags & ZT_FLAG_RUNNING) &&
	    !(wc->spans[span].alarms) &&
	    (!wc->e1recover))   {
		if (val != 0x1b) {
			wc->e1check[span]++;
		} else
			wc->e1check[span] = 0;
		if (wc->e1check[span] > 100) {
			/* Wait 1000 ms */
			wc->e1recover = 1000 * 8;
			memset(wc->e1check, 0, sizeof(wc->e1check));
			if (debug)
				printk("Detected loss of E1 alignment on span %d!\n", span);
			t4_reset_dma(wc);
		}
	}
}

static void t4_receiveprep(struct t4 *wc, int irq)
{
	volatile unsigned int *readchunk;
	int dbl = 0;
	int x,y,z;
	unsigned int tmp;
	int offset=0;
	if (!wc->t1e1)
		offset = 4;
	if (irq & 1) {
		/* First part */
		readchunk = wc->readchunk;
		if (!wc->last0) 
			dbl = 1;
		wc->last0 = 0;
	} else {
		readchunk = wc->readchunk + ZT_CHUNKSIZE * 32;
		if (wc->last0) 
			dbl = 1;
		wc->last0 = 1;
	}
	if (dbl) {
		for (x=0;x<4;x++)
			wc->spans[x].irqmisses++;
		if (debug)
			printk("TE410P: Double/missed interrupt detected\n");
	}
	for (x=0;x<ZT_CHUNKSIZE;x++) {
		for (z=0;z<24;z++) {
			/* All T1/E1 channels */
			tmp = readchunk[z+1+offset];
			wc->spans[3].chans[z].readchunk[x] = tmp & 0xff;
			wc->spans[2].chans[z].readchunk[x] = (tmp & 0xff00) >> 8;
			wc->spans[1].chans[z].readchunk[x] = (tmp & 0xff0000) >> 16;
			wc->spans[0].chans[z].readchunk[x] = tmp >> 24;
		}
		if (wc->t1e1) {
			if (wc->e1recover > 0)
				wc->e1recover--;
			tmp = readchunk[0];
			e1_check(wc, 3, (tmp & 0x7f));
			e1_check(wc, 2, (tmp & 0x7f00) >> 8);
			e1_check(wc, 1, (tmp & 0x7f0000) >> 16);
			e1_check(wc, 0, (tmp & 0x7f000000) >> 24);
			for (z=24;z<31;z++) {
				/* Only E1 channels now */
				tmp = readchunk[z+1];
				if (wc->spans[3].channels > 24)
					wc->spans[3].chans[z].readchunk[x] = tmp & 0xff;
				if (wc->spans[2].channels > 24)
					wc->spans[2].chans[z].readchunk[x] = (tmp & 0xff00) >> 8;
				if (wc->spans[1].channels > 24)
					wc->spans[1].chans[z].readchunk[x] = (tmp & 0xff0000) >> 16;
				if (wc->spans[0].channels > 24)
					wc->spans[0].chans[z].readchunk[x] = tmp >> 24;
			}
		}
		/* Advance pointer by 4 TDM frame lengths */
		readchunk += 32;
	}
	for (x=0;x<4;x++) {
		if (wc->spans[x].flags & ZT_FLAG_RUNNING) {
			for (y=0;y<wc->spans[x].channels;y++) {
				/* Echo cancel double buffered data */
				zt_ec_chunk(&wc->spans[x].chans[y], 
				    wc->spans[x].chans[y].readchunk, 
					wc->ec_chunk2[x][y]);
				memcpy(wc->ec_chunk2[x][y],wc->ec_chunk1[x][y],
					ZT_CHUNKSIZE);
				memcpy(wc->ec_chunk1[x][y],
					wc->spans[x].chans[y].writechunk,
						ZT_CHUNKSIZE);
			}
			zt_receive(&wc->spans[x]);
		}
	}
}

static void t4_transmitprep(struct t4 *wc, int irq)
{
	volatile unsigned int *writechunk;
	int x,y,z;
	unsigned int tmp;
	int offset=0;
	if (!wc->t1e1)
		offset = 4;
	if (irq & 1) {
		/* First part */
		writechunk = wc->writechunk + 1;
	} else {
		writechunk = wc->writechunk + ZT_CHUNKSIZE * 32 + 1;
	}
	for (y=0;y<4;y++) {
		if (wc->spans[y].flags & ZT_FLAG_RUNNING) 
			zt_transmit(&wc->spans[y]);
	}

	for (x=0;x<ZT_CHUNKSIZE;x++) {
		/* Once per chunk */
		for (z=0;z<24;z++) {
			/* All T1/E1 channels */
			tmp = (wc->spans[3].chans[z].writechunk[x]) | 
				  (wc->spans[2].chans[z].writechunk[x] << 8) |
				  (wc->spans[1].chans[z].writechunk[x] << 16) |
				  (wc->spans[0].chans[z].writechunk[x] << 24);
			writechunk[z+offset] = tmp;
		}
		if (wc->t1e1) {
			for (z=24;z<31;z++) {
				/* Only E1 channels now */
				tmp = 0;
				if (wc->spans[3].channels > 24)
					tmp |= wc->spans[3].chans[z].writechunk[x];
				if (wc->spans[2].channels > 24)
					tmp |= (wc->spans[2].chans[z].writechunk[x] << 8);
				if (wc->spans[1].channels > 24)
					tmp |= (wc->spans[1].chans[z].writechunk[x] << 16);
				if (wc->spans[0].channels > 24)
					tmp |= (wc->spans[0].chans[z].writechunk[x] << 24);
				writechunk[z] = tmp;
			}
		}
		/* Advance pointer by 4 TDM frame lengths */
		writechunk += 32;
	}

}

static void __t4_check_sigbits(struct t4 *wc, int span)
{
	int a,i,rxs;
	if (!(wc->spans[span].flags & ZT_FLAG_RUNNING))
		return;
	if (wc->spantype[span] == TYPE_E1) {
		for (i = 0; i < 15; i++) {
			a = __t4_framer_in(wc, span, 0x71 + i);
			/* Get high channel in low bits */
			rxs = (a & 0xf);
			if (!(wc->spans[span].chans[i+16].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i+16].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i+16], rxs);
			}
			rxs = (a >> 4) & 0xf;
			if (!(wc->spans[span].chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i], rxs);
			}
		}
	} else if (wc->spans[span].lineconfig & ZT_CONFIG_D4) {
		for (i = 0; i < 24; i+=4) {
			a = __t4_framer_in(wc, span, 0x70 + (i>>2));
			/* Get high channel in low bits */
			rxs = (a & 0x3) << 2;
			if (!(wc->spans[span].chans[i+3].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i+3].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i+3], rxs);
			}
			rxs = (a & 0xc);
			if (!(wc->spans[span].chans[i+2].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i+2].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i+2], rxs);
			}
			rxs = (a >> 2) & 0xc;
			if (!(wc->spans[span].chans[i+1].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i+1].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i+1], rxs);
			}
			rxs = (a >> 4) & 0xc;
			if (!(wc->spans[span].chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i], rxs);
			}
		}
	} else {
		for (i = 0; i < 24; i+=2) {
			a = __t4_framer_in(wc, span, 0x70 + (i>>1));
			/* Get high channel in low bits */
			rxs = (a & 0xf);
			if (!(wc->spans[span].chans[i+1].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i+1].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i+1], rxs);
			}
			rxs = (a >> 4) & 0xf;
			if (!(wc->spans[span].chans[i].sig & ZT_SIG_CLEAR)) {
				if (wc->spans[span].chans[i].rxsig != rxs)
					zt_rbsbits(&wc->spans[span].chans[i], rxs);
			}
		}
	}
}

static void __t4_do_counters(struct t4 *wc)
{
	int span;
	for (span=0;span<4;span++) {
		if (wc->alarmtimer[span]) {
			if (!--wc->alarmtimer[span]) {
				wc->spans[span].alarms &= ~(ZT_ALARM_RECOVER);
				zt_alarm_notify(&wc->spans[span]);
			}
		}
	}
}

static void __t4_check_alarms(struct t4 *wc, int span)
{
	unsigned char c,d;
	int alarms;
	int x,j;

	if (!(wc->spans[span].flags & ZT_FLAG_RUNNING))
		return;

	c = __t4_framer_in(wc, span, 0x4c);
	d = __t4_framer_in(wc, span, 0x4d);

	/* Assume no alarms */
	alarms = 0;

	/* And consider only carrier alarms */
	wc->spans[span].alarms &= (ZT_ALARM_RED | ZT_ALARM_BLUE | ZT_ALARM_NOTOPEN);

	if (wc->spantype[span] == TYPE_E1) {
		if (c & 0x04) {
			/* No multiframe found, force RAI high after 400ms only if
			   we haven't found a multiframe since last loss
			   of frame */
			if (!(wc->spanflags[span] & FLAG_NMF)) {
				__t4_framer_out(wc, span, 0x20, 0x9f | 0x20);	/* LIM0: Force RAI High */
				wc->spanflags[span] |= FLAG_NMF;
				printk("NMF workaround on!\n");
			}
			__t4_framer_out(wc, span, 0x1e, 0xc3);	/* Reset to CRC4 mode */
			__t4_framer_out(wc, span, 0x1c, 0xf2);	/* Force Resync */
			__t4_framer_out(wc, span, 0x1c, 0xf0);	/* Force Resync */
		} else if (!(c & 0x02)) {
			if ((wc->spanflags[span] & FLAG_NMF)) {
				__t4_framer_out(wc, span, 0x20, 0x9f);	/* LIM0: Clear forced RAI */
				wc->spanflags[span] &= ~FLAG_NMF;
				printk("NMF workaround off!\n");
			}
		}
	} else {
		/* Detect loopup code if we're not sending one */
		if ((!wc->spans[span].mainttimer) && (d & 0x08)) {
			/* Loop-up code detected */
			if ((wc->loopupcnt[span]++ > 80)  && (wc->spans[span].maintstat != ZT_MAINT_REMOTELOOP)) {
				__t4_framer_out(wc, span, 0x36, 0x08);	/* LIM0: Disable any local loop */
				__t4_framer_out(wc, span, 0x37, 0xf6 );	/* LIM1: Enable remote loop */
				wc->spans[span].maintstat = ZT_MAINT_REMOTELOOP;
			}
		} else
			wc->loopupcnt[span] = 0;
		/* Same for loopdown code */
		if ((!wc->spans[span].mainttimer) && (d & 0x10)) {
			/* Loop-down code detected */
			if ((wc->loopdowncnt[span]++ > 80)  && (wc->spans[span].maintstat == ZT_MAINT_REMOTELOOP)) {
				__t4_framer_out(wc, span, 0x36, 0x08);	/* LIM0: Disable any local loop */
				__t4_framer_out(wc, span, 0x37, 0xf0 );	/* LIM1: Disable remote loop */
				wc->spans[span].maintstat = ZT_MAINT_NONE;
			}
		} else
			wc->loopdowncnt[span] = 0;
	}

	if (wc->spans[span].lineconfig & ZT_CONFIG_NOTOPEN) {
		for (x=0,j=0;x < wc->spans[span].channels;x++)
			if ((wc->spans[span].chans[x].flags & ZT_FLAG_OPEN) ||
			    (wc->spans[span].chans[x].flags & ZT_FLAG_NETDEV))
				j++;
		if (!j)
			alarms |= ZT_ALARM_NOTOPEN;
	}

	if (c & 0xa0) {
		if (wc->alarmcount[span] >= alarmdebounce) 
			alarms |= ZT_ALARM_RED;
		else
			wc->alarmcount[span]++;
	} else
		wc->alarmcount[span] = 0;
	if (c & 0x4)
		alarms |= ZT_ALARM_BLUE;

	if (((!wc->spans[span].alarms) && alarms) || 
	    (wc->spans[span].alarms && (!alarms))) 
		wc->checktiming = 1;

	/* Keep track of recovering */
	if ((!alarms) && wc->spans[span].alarms) 
		wc->alarmtimer[span] = ZT_ALARMSETTLE_TIME;
	if (wc->alarmtimer[span])
		alarms |= ZT_ALARM_RECOVER;

	/* If receiving alarms, go into Yellow alarm state */
	if (alarms && !(wc->spanflags[span] & FLAG_SENDINGYELLOW)) {
		unsigned char fmr4;
#if 1
		printk("wct4xxp: Setting yellow alarm on span %d\n", span + 1);
#endif
		/* We manually do yellow alarm to handle RECOVER and NOTOPEN, otherwise it's auto anyway */
		fmr4 = __t4_framer_in(wc, span, 0x20);
		__t4_framer_out(wc, span, 0x20, fmr4 | 0x20);
		wc->spanflags[span] |= FLAG_SENDINGYELLOW;
	} else if ((!alarms) && (wc->spanflags[span] & FLAG_SENDINGYELLOW)) {
		unsigned char fmr4;
#if 1
		printk("wct4xxp: Clearing yellow alarm on span %d\n", span + 1);
#endif
		/* We manually do yellow alarm to handle RECOVER  */
		fmr4 = __t4_framer_in(wc, span, 0x20);
		__t4_framer_out(wc, span, 0x20, fmr4 & ~0x20);
		wc->spanflags[span] &= ~FLAG_SENDINGYELLOW;
	}

	/* Re-check the timing source when we enter/leave alarm, not withstanding
	   yellow alarm */
	if (c & 0x10)
		alarms |= ZT_ALARM_YELLOW;
	if (wc->spans[span].mainttimer || wc->spans[span].maintstat) 
		alarms |= ZT_ALARM_LOOPBACK;
	wc->spans[span].alarms = alarms;
	zt_alarm_notify(&wc->spans[span]);
}

static inline void __handle_leds(struct t4 *wc)
{
	int x;

	wc->blinktimer++;
	for (x=0;x<4;x++) {
		if (wc->spans[x].flags & ZT_FLAG_RUNNING) {
			if (wc->spans[x].alarms & (ZT_ALARM_RED | ZT_ALARM_BLUE)) {
#ifdef FANCY_ALARM
				if (wc->blinktimer == (altab[wc->alarmpos] >> 1)) {
					__t4_set_led(wc, x, WC_RED);
				}
				if (wc->blinktimer == 0xf) {
					__t4_set_led(wc, x, WC_OFF);
				}
#else
				if (wc->blinktimer == 160) {
					__t4_set_led(wc, x, WC_RED);
				} else if (wc->blinktimer == 480) {
					__t4_set_led(wc, x, WC_OFF);
				}
#endif
			} else if (wc->spans[x].alarms & ZT_ALARM_YELLOW) {
				/* Yellow Alarm */
				__t4_set_led(wc, x, WC_YELLOW);
			} else if (wc->spans[x].mainttimer || wc->spans[x].maintstat) {
#ifdef FANCY_ALARM
				if (wc->blinktimer == (altab[wc->alarmpos] >> 1)) {
					__t4_set_led(wc, x, WC_GREEN);
				}
				if (wc->blinktimer == 0xf) {
					__t4_set_led(wc, x, WC_OFF);
				}
#else
				if (wc->blinktimer == 160) {
					__t4_set_led(wc, x, WC_GREEN);
				} else if (wc->blinktimer == 480) {
					__t4_set_led(wc, x, WC_OFF);
				}
#endif
			} else {
				/* No Alarm */
				__t4_set_led(wc, x, WC_GREEN);
			}
		}	else
				__t4_set_led(wc, x, WC_OFF);

	}
#ifdef FANCY_ALARM
	if (wc->blinktimer == 0xf) {
		wc->blinktimer = -1;
		wc->alarmpos++;
		if (wc->alarmpos >= (sizeof(altab) / sizeof(altab[0])))
			wc->alarmpos = 0;
	}
#else
	if (wc->blinktimer == 480)
		wc->blinktimer = 0;
#endif
}

#ifdef LINUX26
static irqreturn_t t4_interrupt(int irq, void *dev_id, struct pt_regs *regs)
#else
static void t4_interrupt(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
	struct t4 *wc = dev_id;
	unsigned long flags;
	int x;
	
	unsigned int status;
#if 0
	unsigned int status2;
#endif

#if 0
	if (wc->intcount < 20)
		printk("Pre-interrupt\n");
#endif
	
	inirq = 1;
	/* Make sure it's really for us */
	status = t4_pci_in(wc, WC_INTR);
	t4_pci_out(wc, WC_INTR, 0);

	/* Ignore if it's not for us */
	if (!status)
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		

	if (!wc->spansstarted) {
		printk("Not prepped yet!\n");
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
	}

	wc->intcount++;
#if 0
	if (wc->intcount < 20)
		printk("Got interrupt, status = %08x\n", status);
#endif		

	if (status & 0x3) {
		t4_receiveprep(wc, status);
		t4_transmitprep(wc, status);
	}
	
#if 0
	if ((wc->intcount < 10) || !(wc->intcount % 1000)) {
		status2 = t4_framer_in(wc, 0, 0x6f);
		printk("Status2: %04x\n", status2);
		for (x = 0;x<4;x++) {
			status2 = t4_framer_in(wc, x, 0x4c);
			printk("FRS0/%d: %04x\n", x, status2);
		}
	}
#endif
	spin_lock_irqsave(&wc->reglock, flags);

	__handle_leds(wc);

	__t4_do_counters(wc);

	x = wc->intcount & 15 /* 63 */;
	switch(x) {
	case 0:
	case 1:
	case 2:
	case 3:
		__t4_check_sigbits(wc, x);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		__t4_check_alarms(wc, x - 4);
		break;
	}

	if (wc->checktiming > 0)
		__t4_set_timing_source_auto(wc);
	spin_unlock_irqrestore(&wc->reglock, flags);
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
}

static int t4_reset_dma(struct t4 *wc)
{
	/* Turn off DMA and such */
	wc->dmactrl = 0x0;
	t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
	t4_pci_out(wc, WC_COUNT, 0);
	t4_pci_out(wc, WC_RDADDR, 0);
	t4_pci_out(wc, WC_WRADDR, 0);
	t4_pci_out(wc, WC_INTR, 0);
	/* Turn it all back on */
	t4_pci_out(wc, WC_RDADDR, wc->readdma);
	t4_pci_out(wc, WC_WRADDR, wc->writedma);
	t4_pci_out(wc, WC_COUNT, ((ZT_MAX_CHUNKSIZE * 2 * 32 - 1) << 18) | ((ZT_MAX_CHUNKSIZE * 2 * 32 - 1) << 2));
	t4_pci_out(wc, WC_INTR, 0);
	wc->dmactrl = 0xc0000003 | (1 << 29);
	if (noburst)
		wc->dmactrl |= (1 << 26);
	t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
	return 0;
}

static int t4_hardware_init(struct t4 *wc)
{
	int x;
	unsigned int version;
	unsigned int falcver;

	version = t4_pci_in(wc, WC_VERSION);
	printk("TE410P version %08x, burst %s\n", version, noburst ? "OFF" : "ON");

	/* Make sure DMA engine is not running and interrupts are acknowledged */
	wc->dmactrl = 0x0;
	t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
	/* Reset Framer and friends */
	t4_pci_out(wc, WC_LEDS, 0x00000000);

	/* Set DMA addresses */
	t4_pci_out(wc, WC_RDADDR, wc->readdma);
	t4_pci_out(wc, WC_WRADDR, wc->writedma);

	/* Setup counters, interrupt flags */
	t4_pci_out(wc, WC_COUNT, ((ZT_MAX_CHUNKSIZE * 2 * 32 - 1) << 18) | ((ZT_MAX_CHUNKSIZE * 2 * 32 - 1) << 2));
	
	/* Reset pending interrupts */
	t4_pci_out(wc, WC_INTR, 0x00000000);

	/* Read T1/E1 status */
	if (t1e1override > -1)
		wc->t1e1 = t1e1override;
	else
		wc->t1e1 = ((t4_pci_in(wc, WC_LEDS)) & 0x0f00) >> 8;
	wc->order = ((t4_pci_in(wc, WC_LEDS)) & 0xf0000000) >> 28;

	/* Setup LEDS, take out of reset */
	t4_pci_out(wc, WC_LEDS, 0x000000ff);
	t4_activate(wc);
	
	t4_framer_out(wc, 0, 0x4a, 0xaa);
	falcver = t4_framer_in(wc, 0 ,0x4a);
	printk("FALC version: %08x, Board ID: %02x\n", falcver, wc->order);

	for (x=0;x< 11;x++)
		printk("Reg %d: 0x%08x\n", x, t4_pci_in(wc, x));
	return 0;
}

static int __devinit t4_launch(struct t4 *wc)
{
	int x;
	unsigned long flags;
	if (wc->spans[0].flags & ZT_FLAG_REGISTERED)
		return 0;
	printk("TE410P: Launching card: %d\n", wc->order);

	/* Setup serial parameters and system interface */
	for (x=0;x<4;x++)
		t4_serial_setup(wc, x);

	if (zt_register(&wc->spans[0], 0)) {
		printk(KERN_ERR "Unable to register span %s\n", wc->spans[0].name);
		return -1;
	}
	if (zt_register(&wc->spans[1], 0)) {
		printk(KERN_ERR "Unable to register span %s\n", wc->spans[1].name);
		zt_unregister(&wc->spans[0]);
		return -1;
	}
	if (zt_register(&wc->spans[2], 0)) {
		printk(KERN_ERR "Unable to register span %s\n", wc->spans[2].name);
		zt_unregister(&wc->spans[0]);
		zt_unregister(&wc->spans[1]);
		return -1;
	}
	if (zt_register(&wc->spans[3], 0)) {
		printk(KERN_ERR "Unable to register span %s\n", wc->spans[3].name);
		zt_unregister(&wc->spans[0]);
		zt_unregister(&wc->spans[1]);
		zt_unregister(&wc->spans[2]);
		return -1;
	}
	wc->checktiming = 1;
	spin_lock_irqsave(&wc->reglock, flags);
	__t4_set_timing_source(wc,4);
	spin_unlock_irqrestore(&wc->reglock, flags);
#ifdef ENABLE_TASKLETS
	tasklet_init(&wc->t4_tlet, t4_tasklet, (unsigned long)wc);
#endif
	return 0;
}

static int __devinit t4_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int res;
	struct t4 *wc;
	int x,f;
#if 0
	int y;
	unsigned int *canary;
#endif
	
	
	if (pci_enable_device(pdev)) {
		res = -EIO;
	} else {
		wc = kmalloc(sizeof(struct t4), GFP_KERNEL);
		if (wc) {
			memset(wc, 0x0, sizeof(struct t4));
			spin_lock_init(&wc->reglock);
			wc->variety = (char *)(ent->driver_data);

			wc->memaddr = pci_resource_start(pdev, 0);
			wc->memlen = pci_resource_len(pdev, 0);
			wc->membase = ioremap(wc->memaddr, wc->memlen);
			/* This rids of the Double missed interrupt message after loading */
			wc->last0 = 1;
#if 0
			if (!request_mem_region(wc->memaddr, wc->memlen, wc->variety))
				printk("wct4: Unable to request memory region :(, using anyway...\n");
#endif
			if (pci_request_regions(pdev, wc->variety))
				printk("wct4xxp: Unable to request regions\n");

			printk("Found TE410P at base address %08lx, remapped to %p\n", wc->memaddr, wc->membase);

			wc->dev = pdev;

			wc->writechunk = 
				/* 32 channels, Double-buffer, Read/Write, 4 spans */
				(unsigned int *)pci_alloc_consistent(pdev, ZT_MAX_CHUNKSIZE * 32 * 2 * 2 * 4, &wc->writedma);
			if (!wc->writechunk) {
				printk("wct1xxp: Unable to allocate DMA-able memory\n");
				return -ENOMEM;
			}

			/* Read is after the whole write piece (in words) */
			wc->readchunk = wc->writechunk + ZT_CHUNKSIZE * 32 * 2;

			/* Same thing but in bytes...  */
			wc->readdma = wc->writedma + ZT_CHUNKSIZE * 32 * 2 * 4;

			/* Initialize Write/Buffers to all blank data */
			memset((void *)wc->writechunk,0xff,ZT_MAX_CHUNKSIZE * 2 * 32 * 4);
			memset((void *)wc->readchunk,0xff,ZT_MAX_CHUNKSIZE * 2 * 32 * 4);
#if 0
			memset((void *)wc->readchunk,0xff,ZT_MAX_CHUNKSIZE * 2 * 32 * 4);
			/* Initialize canary */
			canary = (unsigned int *)(wc->readchunk + ZT_CHUNKSIZE * 64 * 4 - 4);
			*canary = (CANARY << 16) | (0xffff);
#endif			

			/* Enable bus mastering */
			pci_set_master(pdev);

			/* Keep track of which device we are */
			pci_set_drvdata(pdev, wc);

			/* Initialize hardware */
			t4_hardware_init(wc);
			for(x = 0; x < MAX_T4_CARDS; x++) {
				if (!cards[x]) break;
			}
			if (x >= MAX_T4_CARDS) {
				printk("No cards[] slot available!!\n");
				return -ENOMEM;
			}
			wc->num = x;
			cards[x] = wc;
			

			if (request_irq(pdev->irq, t4_interrupt, SA_INTERRUPT | SA_SHIRQ, "t4xxp", wc)) {
				printk("t4xxp: Unable to request IRQ %d\n", pdev->irq);
				kfree(wc);
				return -EIO;
			}
			for (x=0;x<4;x++) {
				if (wc->t1e1 & (1 << x)) {
					wc->spantype[x] = TYPE_E1;
					wc->chans[x] = kmalloc(sizeof(struct zt_chan) * 31,GFP_KERNEL);
					memset(wc->chans[x],0,sizeof(struct zt_chan) * 31);
				} else {
					wc->spantype[x] = TYPE_T1;
					wc->chans[x] = kmalloc(sizeof(struct zt_chan) * 24,GFP_KERNEL);
					memset(wc->chans[x],0,sizeof(struct zt_chan) * 24);
				}
				if (!wc->chans[x])
					return -ENOMEM;
			}
			init_spans(wc);

			/* Launch cards as appropriate */
			x = 0;
			for(;;) {
				/* Find a card to activate */
				f = 0;
				for (x=0;cards[x];x++) {
					if (cards[x]->order <= highestorder) {
						t4_launch(cards[x]);
						if (cards[x]->order == highestorder)
							f = 1;
					}
				}
				/* If we found at least one, increment the highest order and search again, otherwise stop */
				if (f) 
					highestorder++;
				else
					break;
			}

			printk("Found a Wildcard: %s\n", wc->variety);
			res = 0;
		} else
			res = -ENOMEM;
	}
	return res;
}

static int t4_hardware_stop(struct t4 *wc)
{

	/* Turn off DMA, leave interrupts enabled */
	wc->dmactrl = 0x0000000;
	t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);	/* Turn on only the read interrupts, not the write */
	t4_pci_out(wc, WC_DMACTRL, wc->dmactrl);
	t4_pci_out(wc, WC_INTR, 0x00000000);

	current->state = TASK_UNINTERRUPTIBLE;
	schedule_timeout((25 * HZ) / 1000);

	/* Turn off counter, address, etc */
	t4_pci_out(wc, WC_COUNT, 0x000000);
	t4_pci_out(wc, WC_RDADDR, 0x0000000);
	t4_pci_out(wc, WC_WRADDR, 0x0000000);
	t4_pci_out(wc, WC_GPIO, 0x0000000);
	t4_pci_out(wc, WC_LEDS, 0x00000000);

	printk("\nStopped TE410P, Turned off DMA\n");
	return 0;
}

static void __devexit t4_remove_one(struct pci_dev *pdev)
{
	struct t4 *wc = pci_get_drvdata(pdev);
	if (wc) {
		/* Stop hardware */
		t4_hardware_stop(wc);

		/* Unregister spans */
		if (wc->spans[0].flags & ZT_FLAG_REGISTERED)
			zt_unregister(&wc->spans[0]);
		if (wc->spans[1].flags & ZT_FLAG_REGISTERED)
			zt_unregister(&wc->spans[1]);
		if (wc->spans[2].flags & ZT_FLAG_REGISTERED)
			zt_unregister(&wc->spans[2]);
		if (wc->spans[3].flags & ZT_FLAG_REGISTERED)
			zt_unregister(&wc->spans[3]);
#if 0
		/* Stop any DMA */
		__t1xxp_stop_dma(wc);

		/* In case hardware is still there */
		__t1xxp_disable_interrupts(wc);
		
		t1xxp_stop_stuff(wc);
#endif

		if (wc->membase)
			iounmap((void *)wc->membase);

		pci_release_regions(pdev);		
#if 0
		if (wc->memaddr)
			release_mem_region(wc->memaddr, wc->memlen);
#endif

		/* Immediately free resources */
		pci_free_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 32 * 4, (void *)wc->writechunk, wc->writedma);
#if 1
		free_irq(pdev->irq, wc);
#endif		
		cards[wc->num] = NULL;
		pci_set_drvdata(pdev, NULL);
	}
}

static struct pci_device_id t4_pci_tbl[] __devinitdata =
{
	{ 0x10ee, 0x0314, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long)"Wildcard TE410P-Xilinx" },
	{ 0, }
};

static struct pci_driver t4_driver = {
	name: 	"t4xxp",
	probe: 	t4_init_one,
#ifdef LINUX26
	remove:	__devexit_p(t4_remove_one),
#else
	remove:	t4_remove_one,
#endif
	suspend: NULL,
	resume:	NULL,
	id_table: t4_pci_tbl,
};

static int __init t4_init(void)
{
	int res;
	res = pci_module_init(&t4_driver);
	if (res)
		return -ENODEV;
	return 0;
}

static void __exit t4_cleanup(void)
{
	pci_unregister_driver(&t4_driver);
}


MODULE_AUTHOR("Mark Spencer");
MODULE_DESCRIPTION("TE410P PCI Driver");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
#ifdef LINUX26
module_param(debug, int, 0600);
module_param(loopback, int, 0600);
module_param(noburst, int, 0600);
module_param(timingcable, int, 0600);
module_param(t1e1override, int, 0600);
module_param(alarmdebounce, int, 0600);
#else
MODULE_PARM(debug, "i");
MODULE_PARM(loopback, "i");
MODULE_PARM(noburst, "i");
MODULE_PARM(timingcable, "i");
MODULE_PARM(t1e1override, "i");
MODULE_PARM(alarmdebounce, "i");
#endif

MODULE_DEVICE_TABLE(pci, t4_pci_tbl);

module_init(t4_init);
module_exit(t4_cleanup);

