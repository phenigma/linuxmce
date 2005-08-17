/*
 * $Id: cx88-alsa.c,v 1.8 2005/08/15 23:44:23 mchehab Exp $
 *
 *  Support for audio capture
 *  PCI function #1 of the cx2388x.
 *
 *    (c) 2005 Mauro Carvalho Chehab <mchehab@brturbo.com.br>
 *    Based on a dummy cx88 module by Gerd Knorr <kraxel@bytesex.org>
 *    Based on bt87x.c by Clemens Ladisch <clemens@ladisch.de>
 *    Based on dummy.c by Jaroslav Kysela <perex@suse.cz>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <asm/delay.h>
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>

#include "compat.h"
#include "cx88.h"
#include "cx88-reg.h"

#define dprintk(level,fmt, arg...)	if (debug >= level) \
	printk(KERN_DEBUG "%s/1: " fmt, chip->core->name , ## arg)


/****************************************************************************
	Data type declarations - Can be moded to a header file later
 ****************************************************************************/

#if 1
#define ANALOG_CLOCK 1792000
#define CLOCK_DIV_MIN 4
#define CLOCK_DIV_MAX 15
#define MAX_PCM_DEVICES		4
#define MAX_PCM_SUBSTREAMS	16
#endif

enum { DEVICE_DIGITAL, DEVICE_ANALOG };

/* These can be replaced after done */
#define MIXER_ADDR_LAST MAX_CX88_INPUT

struct cx88_audio_dev {
	struct cx88_core           *core;
        struct cx88_buffer         *buf;
	struct cx88_dmaqueue       q;

	/* pci i/o */
	struct pci_dev             *pci;
	unsigned char              pci_rev,pci_lat;

	/* audio controls */
	int                        irq;
	int                        dig_rate;		/* Digital sampling rate */

	snd_card_t                 *card;

	spinlock_t                 reg_lock;

	unsigned int               dma_size;
	unsigned int               period_size;

	int                        mixer_volume[MIXER_ADDR_LAST+1][2];
	int                        capture_source[MIXER_ADDR_LAST+1][2];

	long opened;
	snd_pcm_substream_t *substream;
};
typedef struct cx88_audio_dev snd_cx88_card_t;

/****************************************************************************
			Module global static vars
 ****************************************************************************/

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static int enable[SNDRV_CARDS] = {1, [1 ... (SNDRV_CARDS - 1)] = 1};
#if 0
static int pcm_devs[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = 1};
static int pcm_substreams[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = 8};
static snd_cx88_card_t *snd_dummy_cards[SNDRV_CARDS] = SNDRV_DEFAULT_PTR;
#endif

module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable cx88x soundcard. default enabled.");

/****************************************************************************
				Module macros
 ****************************************************************************/

MODULE_DESCRIPTION("ALSA driver module for cx2388x based TV cards");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@brturbo.com.br>");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("{{Conexant,23881},"
			"{{Conexant,23882},"
			"{{Conexant,23883}");
static unsigned int debug = 0;
module_param(debug,int,0644);
MODULE_PARM_DESC(debug,"enable debug messages");

/****************************************************************************
			Module specific funtions
 ****************************************************************************/

/*
 * BOARD Specific: Sets audio DMA
 */

int cx88_start_audio_dma(snd_cx88_card_t *chip)
{
	struct cx88_core *core=chip->core;

	/* setup fifo + format - out channel */
	cx88_sram_channel_setup(core, &cx88_sram_channels[SRAM_CH26],
				chip->buf->bpl, chip->buf->risc.dma);

	/* FIXME reset counter */
	cx_write(MO_VIDY_GPCNTRL,GP_COUNT_CONTROL_RESET);

	/* enable irqs */
	cx_set(MO_PCI_INTMSK, chip->core->pci_irqmask | 0x02);

	/* Enables corresponding bits at AUD_INT_STAT */
#if 1
	cx_set(MO_AUD_INTMSK, (1<<21)||(1<<19));
#endif

	/* start dma */
	cx_set(MO_DEV_CNTRL2, (1<<5)); /* Enables Risc Processor */
	cx_set(MO_AUD_DMACNTRL, 0x11); /* audio downstream FIFO and RISC enable */

	return 0;
}

/*
 * BOARD Specific: Resets audio DMA
 */
int cx88_stop_audio_dma(snd_cx88_card_t *chip)
{
	struct cx88_core *core=chip->core;
	/* stop dma */
	cx_clear(MO_AUD_DMACNTRL, 0x11);

	/* disable irqs */
	cx_clear(MO_PCI_INTMSK, 0x000002);

	cx_set(MO_AUD_INTMSK, 0);
	return 0;
}

#define MAX_IRQ_LOOP 10

static void cx8801_timeout(unsigned long data)
{
	snd_cx88_card_t *chip = (snd_cx88_card_t *)data;

	dprintk(0, "cx88_alsa: %s\n",__FUNCTION__);

	if (debug)
		cx88_sram_channel_dump(chip->core, &cx88_sram_channels[SRAM_CH26]);
	cx88_stop_audio_dma(chip);
#if 0
	do_cancel_buffers(dev,"timeout",1);
#endif
}

/* FIXME: Wrong values*/
static char *cx88_aud_irqs[32] = {
	"y_risci1", "u_risci1", "v_risci1", "vbi_risc1",
	"y_risci2", "u_risci2", "v_risci2", "vbi_risc2",
	"y_oflow",  "u_oflow",  "v_oflow",  "vbi_oflow",
	"y_sync",   "u_sync",   "v_sync",   "vbi_sync",
	"opc_err",  "par_err",  "rip_err",  "pci_abort",
};


static void cx8801_aud_irq(snd_cx88_card_t *chip)
{
	struct cx88_core *core = chip->core;
	u32 status, mask;
#if 0
	u32 count;
#endif
	status = cx_read(MO_AUD_INTSTAT);
	mask   = cx_read(MO_AUD_INTMSK);
	if (0 == (status & mask))
		return;
	cx_write(MO_AUD_INTSTAT, status);
	if (debug  ||  (status & mask & ~0xff))
		cx88_print_irqbits(core->name, "irq aud",
				   cx88_aud_irqs, status, mask);
#if 0 /* FIXME */
	/* risc op code error */
	if (status & (1 << 16)) {
		printk(KERN_WARNING "%s/0: video risc op code error\n",core->name);
		cx_clear(MO_VID_DMACNTRL, 0x11);
		cx_clear(VID_CAPTURE_CONTROL, 0x06);
		cx88_sram_channel_dump(dev->core, &cx88_sram_channels[SRAM_CH21]);
	}

	/* risc1 y */
	if (status & 0x01) {
		spin_lock(&dev->slock);
		count = cx_read(MO_VIDY_GPCNT);
		cx88_wakeup(dev->core, &dev->vidq, count);
		spin_unlock(&dev->slock);
	}

	/* risc1 vbi */
	if (status & 0x08) {
		spin_lock(&dev->slock);
		count = cx_read(MO_VBI_GPCNT);
		cx88_wakeup(dev->core, &dev->vbiq, count);
		spin_unlock(&dev->slock);
	}

	/* risc2 y */
	if (status & 0x10) {
		dprintk(2,"stopper video\n");
		spin_lock(&dev->slock);
		restart_video_queue(dev,&dev->vidq);
		spin_unlock(&dev->slock);
	}

	/* risc2 vbi */
	if (status & 0x80) {
		dprintk(2,"stopper vbi\n");
		spin_lock(&dev->slock);
		cx8800_restart_vbi_queue(dev,&dev->vbiq);
		spin_unlock(&dev->slock);
	}
#endif
}

static irqreturn_t cx8801_irq(int irq, void *dev_id, struct pt_regs *regs)
{
	snd_cx88_card_t *chip = dev_id;
	struct cx88_core *core = chip->core;
	u32 status;
	int loop, handled = 0;

	for (loop = 0; loop < MAX_IRQ_LOOP; loop++) {
		status = cx_read(MO_PCI_INTSTAT) & (core->pci_irqmask | 0x02);
		if (0 == status)
			goto out;
		dprintk( 1, "cx8801_irq\n" );
		dprintk( 1, "    loop: %d/%d\n", loop, MAX_IRQ_LOOP );
		dprintk( 1, "    status: %d\n", status );
		handled = 1;
		cx_write(MO_PCI_INTSTAT, status);

		if (status & core->pci_irqmask)
			cx88_core_irq(core,status);
		if (status & 0x02)
			cx8801_aud_irq(chip);
	};
	if (MAX_IRQ_LOOP == loop) {
		dprintk( 0, "clearing mask\n" );
		dprintk(1,"%s/0: irq loop -- clearing mask\n",
		       core->name);
		cx_write(MO_PCI_INTMSK,0);
	}

 out:
	return IRQ_RETVAL(handled);
}

/*
 =====================> FIXME
 */

#if 0
static void snd_cx88_pci_error(bt87x_t *chip, unsigned int status)
{
	u16 pci_status;

	pci_read_config_word(chip->pci, PCI_STATUS, &pci_status);
	pci_status &= PCI_STATUS_PARITY | PCI_STATUS_SIG_TARGET_ABORT |
		PCI_STATUS_REC_TARGET_ABORT | PCI_STATUS_REC_MASTER_ABORT |
		PCI_STATUS_SIG_SYSTEM_ERROR | PCI_STATUS_DETECTED_PARITY;
	pci_write_config_word(chip->pci, PCI_STATUS, pci_status);
	if (pci_status != PCI_STATUS_DETECTED_PARITY)
		snd_printk(KERN_ERR "Aieee - PCI error! status %#08x, PCI status %#04x\n",
			   status & ERROR_INTERRUPTS, pci_status);
	else {
		snd_printk(KERN_ERR "Aieee - PCI parity error detected!\n");
		/* error 'handling' similar to aic7xxx_pci.c: */
		chip->pci_parity_errors++;
		if (chip->pci_parity_errors > 20) {
			snd_printk(KERN_ERR "Too many PCI parity errors observed.\n");
			snd_printk(KERN_ERR "Some device on this bus is generating bad parity.\n");
			snd_printk(KERN_ERR "This is an error *observed by*, not *generated by*, this card.\n");
			snd_printk(KERN_ERR "PCI parity error checking has been disabled.\n");
			chip->interrupt_mask &= ~(INT_PPERR | INT_RIPERR);
			snd_bt87x_writel(chip, REG_INT_MASK, chip->interrupt_mask);
		}
	}
}
#endif

/****************************************************************************
				ALSA PCM Interface
 ****************************************************************************/

/*
 * Digital hardware definition
 */
static snd_pcm_hardware_t snd_cx88_digital_hw = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.rates = 0, /* set at runtime */
	.channels_min = 2,
	.channels_max = 2,
	.buffer_bytes_max = 255 * 4092,
	.period_bytes_min = 32,
	.period_bytes_max = 4092,
	.periods_min = 2,
	.periods_max = 255,
};

/*
 * Sets board to provide digital audio
 */
static int snd_cx88_set_digital_hw(snd_cx88_card_t *chip, snd_pcm_runtime_t *runtime)
{
	static struct {
		int rate;
		unsigned int bit;
	} ratebits[] = {
		{8000, SNDRV_PCM_RATE_8000},
		{11025, SNDRV_PCM_RATE_11025},
		{16000, SNDRV_PCM_RATE_16000},
		{22050, SNDRV_PCM_RATE_22050},
		{32000, SNDRV_PCM_RATE_32000},
		{44100, SNDRV_PCM_RATE_44100},
		{48000, SNDRV_PCM_RATE_48000}
	};
	int i;
#if 0
	chip->reg_control |= CTL_DA_IOM_DA;
#endif
	runtime->hw = snd_cx88_digital_hw;
	runtime->hw.rates = SNDRV_PCM_RATE_KNOT;
	for (i = 0; i < ARRAY_SIZE(ratebits); ++i)
		if (chip->dig_rate == ratebits[i].rate) {
			runtime->hw.rates = ratebits[i].bit;
			break;
		}
	runtime->hw.rate_min = chip->dig_rate;
	runtime->hw.rate_max = chip->dig_rate;
	return 0;
}

/*
 * audio open callback
 */
static int snd_cx88_pcm_open(snd_pcm_substream_t *substream)
{
	snd_cx88_card_t *chip = snd_pcm_substream_chip(substream);
	snd_pcm_runtime_t *runtime = substream->runtime;
	int err;

	if (test_and_set_bit(0, &chip->opened))
		return -EBUSY;

	err = snd_cx88_set_digital_hw(chip, runtime);

	if (err < 0)
		goto _error;

	err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0)
		goto _error;

	chip->substream = substream;
	return 0;

_error:
	clear_bit(0, &chip->opened);
	smp_mb__after_clear_bit();
	return err;
}

/*
 * audio close callback
 */
static int snd_cx88_close(snd_pcm_substream_t *substream)
{
	snd_cx88_card_t *chip = snd_pcm_substream_chip(substream);

	chip->substream = NULL;
	clear_bit(0, &chip->opened);
	smp_mb__after_clear_bit();
	return 0;
}

/*
 * hw_params callback
 */
static int snd_cx88_hw_params(snd_pcm_substream_t * substream,
				 snd_pcm_hw_params_t * hw_params)
{
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

/*
 * hw free callback
 */
static int snd_cx88_hw_free(snd_pcm_substream_t * substream)
{
	return snd_pcm_lib_free_pages(substream);
}

/*
 * prepare callback
 */
static int snd_cx88_prepare(snd_pcm_substream_t *substream)
{
	snd_cx88_card_t *chip = snd_pcm_substream_chip(substream);
//	snd_pcm_runtime_t *runtime = substream->runtime;
//	int decimation;

	spin_lock_irq(&chip->reg_lock);

	chip->dma_size = snd_pcm_lib_buffer_bytes(substream);
	chip->period_size = snd_pcm_lib_period_bytes(substream);
#if 0
	chip->reg_control &= ~(CTL_DA_SDR_MASK | CTL_DA_SBR);
	decimation = (ANALOG_CLOCK + runtime->rate / 4) / runtime->rate;
	chip->reg_control |= decimation << CTL_DA_SDR_SHIFT;
	if (runtime->format == SNDRV_PCM_FORMAT_S8)
		chip->reg_control |= CTL_DA_SBR;
	snd_bt87x_writel(chip, REG_GPIO_DMA_CTL, chip->reg_control);

	outl(ensoniq->ctrl, ES_REG(ensoniq, CONTROL));
	outl(ES_MEM_PAGEO(ES_PAGE_DAC), ES_REG(ensoniq, MEM_PAGE));
	outl(runtime->dma_addr, ES_REG(ensoniq, DAC1_FRAME));
	outl((ensoniq->p1_dma_size >> 2) - 1, ES_REG(ensoniq, DAC1_SIZE));
	ensoniq->sctrl &= ~(ES_P1_LOOP_SEL | ES_P1_PAUSE | ES_P1_SCT_RLD | ES_P1_MODEM);
	ensoniq->sctrl |= ES_P1_INT_EN | ES_P1_MODEO(mode);
	outl(ensoniq->sctrl, ES_REG(ensoniq, SERIAL));
	outl((ensoniq->p1_period_size >> snd_ensoniq_sample_shift[mode]) - 1, ES_REG(ensoniq, DAC1_COUNT));
#endif
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}


/*
 * trigger callback
 */
static int snd_cx88_card_trigger(snd_pcm_substream_t *substream, int cmd)
{
//	snd_cx88_card_t *chip = snd_pcm_substream_chip(substream);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
//		return snd_cx88_start(chip);
	case SNDRV_PCM_TRIGGER_STOP:
//		return snd_cx88_stop(chip);
	default:
		return -EINVAL;
	}
}

/*
 * pointer callback
 */
static snd_pcm_uframes_t snd_cx88_pointer(snd_pcm_substream_t *substream)
{
//	snd_cx88_card_t *chip = snd_pcm_substream_chip(substream);
//	snd_pcm_runtime_t *runtime = substream->runtime;

//	return (snd_pcm_uframes_t)bytes_to_frames(runtime, chip->current_line * chip->line_bytes);
}

/*
 * operators
 */
static snd_pcm_ops_t snd_cx88_pcm_ops = {
	.open = snd_cx88_pcm_open,
	.close = snd_cx88_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_cx88_hw_params,
	.hw_free = snd_cx88_hw_free,
	.prepare = snd_cx88_prepare,
	.trigger = snd_cx88_card_trigger,
	.pointer = snd_cx88_pointer,
	.page = snd_pcm_sgbuf_ops_page,
};

/*
 * create a PCM device
 */
static int __devinit snd_cx88_pcm(snd_cx88_card_t *chip, int device, char *name)
{
	int err;
	snd_pcm_t *pcm;

	err = snd_pcm_new(chip->card, name, device, 0, 1, &pcm);
	if (err < 0)
		return err;
	pcm->private_data = chip;
	strcpy(pcm->name, name);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_cx88_pcm_ops);
	return snd_pcm_lib_preallocate_pages_for_all(pcm,
						     SNDRV_DMA_TYPE_DEV_SG,
						     snd_dma_pci_data(chip->pci),
							128 * 1024,
							(255 * 4092 + 1023) & ~1023);
}

/****************************************************************************
				CONTROL INTERFACE
 ****************************************************************************/
static int snd_cx88_capture_volume_info(snd_kcontrol_t *kcontrol, snd_ctl_elem_info_t *info)
{
	info->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	info->count = 1;
	info->value.integer.min = 0;
	info->value.integer.max = 0x3f;

	return 0;
}

/* OK - TODO: test it */
static int snd_cx88_capture_volume_get(snd_kcontrol_t *kcontrol, snd_ctl_elem_value_t *value)
{
	snd_cx88_card_t *chip = snd_kcontrol_chip(kcontrol);
	struct cx88_core *core=chip->core;

	value->value.integer.value[0] = 0x3f - (cx_read(AUD_VOL_CTL) & 0x3f);

	return 0;
}

/* OK - TODO: test it */
static int snd_cx88_capture_volume_put(snd_kcontrol_t *kcontrol, snd_ctl_elem_value_t *value)
{
	snd_cx88_card_t *chip = snd_kcontrol_chip(kcontrol);
	struct cx88_core *core=chip->core;
	int v;
	u32 old_control;

	spin_lock_irq(&chip->reg_lock);
	old_control = 0x3f - (cx_read(AUD_VOL_CTL) & 0x3f);
	v = 0x3f - (value->value.integer.value[0] & 0x3f);
	cx_andor(AUD_VOL_CTL, 0x3f, v);
	spin_unlock_irq(&chip->reg_lock);

	return v != old_control;
}

static snd_kcontrol_new_t snd_cx88_capture_volume = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "Capture Volume",
	.info = snd_cx88_capture_volume_info,
	.get = snd_cx88_capture_volume_get,
	.put = snd_cx88_capture_volume_put,
};

/*
 ***************************************
 */
#if 0
static int snd_cx88_capture_source_info(snd_kcontrol_t *kcontrol, snd_ctl_elem_info_t *info)
{
	static char *texts[3] = {"TV Tuner", "FM", "Mic/Line"};

	info->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	info->count = 1;
	info->value.enumerated.items = 3;
	if (info->value.enumerated.item > 2)
		info->value.enumerated.item = 2;
	strcpy(info->value.enumerated.name, texts[info->value.enumerated.item]);
	return 0;
}

static int snd_cx88_capture_source_get(snd_kcontrol_t *kcontrol, snd_ctl_elem_value_t *value)
{
	snd_cx88_card_t *chip = snd_kcontrol_chip(kcontrol);
/*#if 0*/
	value->value.enumerated.item[0] = (chip->reg_control & CTL_A_SEL_MASK) >> CTL_A_SEL_SHIFT;
/*#endif*/
	return 0;
}

static int snd_cx88_capture_source_put(snd_kcontrol_t *kcontrol, snd_ctl_elem_value_t *value)
{
	snd_cx88_card_t *chip = snd_kcontrol_chip(kcontrol);
	u32 old_control;
	int changed;
/*#if 0*/
	spin_lock_irq(&chip->reg_lock);
	old_control = chip->reg_control;
	chip->reg_control = (chip->reg_control & ~CTL_A_SEL_MASK)
		| (value->value.enumerated.item[0] << CTL_A_SEL_SHIFT);
	snd_cx88_writel(chip, REG_GPIO_DMA_CTL, chip->reg_control);
	changed = chip->reg_control != old_control;
	spin_unlock_irq(&chip->reg_lock);
/*#endif*/
	return changed;
}

static snd_kcontrol_new_t snd_cx88_capture_source = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "Capture Source",
	.info = snd_cx88_capture_source_info,
	.get = snd_cx88_capture_source_get,
	.put = snd_cx88_capture_source_put,
};

#endif

/****************************************************************************
			Basic Flow for Sound Devices
 ****************************************************************************/

/*
 * PCI ID Table - 14f1:8801 and 14f1:8811 means function 1: Audio
 * Only boards with eeprom and byte 1 at eeprom=1 have it
 */

struct pci_device_id cx88_audio_pci_tbl[] = {
	{0x14f1,0x8801,PCI_ANY_ID,PCI_ANY_ID,0,0,0},
	{0x14f1,0x8811,PCI_ANY_ID,PCI_ANY_ID,0,0,0},
	{0, }
};
MODULE_DEVICE_TABLE(pci, cx88_audio_pci_tbl);

/*
 * Chip-specific destructor
 */

static int snd_cx88_free(snd_cx88_card_t *chip)
{
#if 0
	if (chip->mmio) {
		snd_bt87x_stop(chip);
		if (chip->irq >= 0)
			synchronize_irq(chip->irq);

		iounmap(chip->mmio);
	}
#endif
	if (chip->irq >= 0)
		free_irq(chip->irq, chip);

	/* free memory */
	cx88_core_put(chip->core,chip->pci);

	pci_release_regions(chip->pci);
	pci_disable_device(chip->pci);

	kfree(chip);
	return 0;
}

/*
 * Component Destructor
 */
static int snd_cx88_dev_free(snd_device_t *device)
{
	snd_cx88_card_t *chip = device->device_data;
	return snd_cx88_free(chip);
}


/*
 * Alsa Constructor - Component probe
 */

static int devno=0;
static int __devinit snd_cx88_create(snd_card_t *card, struct pci_dev *pci,
				    snd_cx88_card_t **rchip)
{
	snd_cx88_card_t   *chip;
	struct cx88_core  *core;
	int               err;
	static snd_device_ops_t ops = {
		.dev_free = snd_cx88_dev_free
	};

	*rchip = NULL;

 	err = pci_enable_device(pci);
	if (err < 0)
		return err;

	chip = kmalloc(sizeof(*chip),GFP_KERNEL);
	if (NULL == chip) {
		pci_disable_device(pci);
		return -ENOMEM;
	}
	memset(chip,0,sizeof(*chip));

	if (!pci_dma_supported(pci,0xffffffff)) {
		dprintk(0, "%s/1: Oops: no 32bit PCI DMA ???\n",core->name);
		err = -EIO;
		kfree (chip);
		return err;
	}

	/* pci init */
	chip->card = card;
	chip->pci = pci;
	chip->irq = -1;
	spin_lock_init(&chip->reg_lock);

#if 0
	if ((err = pci_request_regions(pci, "CX88 audio")) < 0) {
		kfree(chip);
		pci_disable_device(pci);
		return err;
	}
	chip->mmio = ioremap_nocache(pci_resource_start(pci, 0),
				pci_resource_len(pci, 0));
	if (!chip->mmio) {
		snd_bt87x_free(chip);
		snd_printk(KERN_ERR "cannot remap io memory\n");
		return -ENOMEM;
	}
#endif

	core = cx88_core_get(chip->pci);
	if (NULL == core) {
		err = -EINVAL;
		kfree (chip);
		return err;
	}
	chip->core = core;
#if 1 /* Should be tested if it is wright */
	chip->dig_rate=48000;
#endif

#if 1 /* From cx88-mpeg.c */

	/* init dma queue */
	INIT_LIST_HEAD(&chip->q.active);
	INIT_LIST_HEAD(&chip->q.queued);
	chip->q.timeout.function = cx8801_timeout;
	chip->q.timeout.data     = (unsigned long)chip;
	init_timer(&chip->q.timeout);
	cx88_risc_stopper(chip->pci,&chip->q.stopper,
			MO_AUD_DMACNTRL, 0x11,0x00);

	/* get irq */
	err = request_irq(chip->pci->irq, cx8801_irq,
			  SA_SHIRQ | SA_INTERRUPT, chip->core->name, chip);
	if (err < 0) {
		dprintk(0, "%s: can't get IRQ %d\n",
		       chip->core->name, chip->pci->irq);
		return err;
	}
	cx_set(MO_PCI_INTMSK, core->pci_irqmask);
#endif

	/* print pci info */
	pci_read_config_byte(pci, PCI_CLASS_REVISION, &chip->pci_rev);
        pci_read_config_byte(pci, PCI_LATENCY_TIMER,  &chip->pci_lat);

        dprintk(1,"ALSA %s/%i: found at %s, rev: %d, irq: %d, "
	       "latency: %d, mmio: 0x%lx\n", core->name, devno,
	       pci_name(pci), chip->pci_rev, pci->irq,
	       chip->pci_lat,pci_resource_start(pci,0));

	pci_set_master(pci);
	synchronize_irq(chip->irq);

	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops);
	if (err < 0) {
		snd_cx88_free(chip);
		return err;
	}
	snd_card_set_dev(card, &pci->dev);

	*rchip = chip;

	return 0;
}

static int __devinit cx88_audio_initdev(struct pci_dev *pci,
				    const struct pci_device_id *pci_id)
{
	snd_card_t       *card;
	snd_cx88_card_t  *chip;
	int              err;

	if (devno >= SNDRV_CARDS)
		return (-ENODEV);

	if (!enable[devno]) {
		++devno;
		return (-ENOENT);
	}

	card = snd_card_new(index[devno], id[devno], THIS_MODULE, 0);
	if (!card)
		return (-ENOMEM);

	err = snd_cx88_create(card, pci, &chip);
	if (err < 0)
		return (err);

/*
	err = snd_cx88_pcm(chip, DEVICE_DIGITAL, "CX88 Digital");
	if (err < 0)
		goto fail_free;
*/
	err = snd_ctl_add(card, snd_ctl_new1(&snd_cx88_capture_volume, chip));
	if (err < 0) {
		snd_card_free(card);
		return (err);
	}

	strcpy (card->driver, "CX88_ALSA");
	sprintf(card->shortname, "Conexant CX%x", pci->device);
	sprintf(card->longname, "%s at %#lx",
		card->shortname, pci_resource_start(pci, 0));
	strcpy (card->mixername, "CX88");

	dprintk (0, "%s/%i: Alsa support for cx2388x boards\n",
	       card->driver,devno);

	err = snd_card_register(card);
	if (err < 0) {
		snd_card_free(card);
		return (err);
	}

	pci_set_drvdata(pci,card);

	devno++;
	return 0;
}
/*
 * ALSA destructor
 */
static void __devexit cx88_audio_finidev(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
	pci_set_drvdata(pci, NULL);

	devno--;
}

#if 0
	.suspend  = cx88_audio_suspend,
	.resume   = cx88_audio_resume,
#endif
/*
 * PCI driver definition
 */

static struct pci_driver cx88_audio_pci_driver = {
        .name     = "cx88_audio",
        .id_table = cx88_audio_pci_tbl,
        .probe    = cx88_audio_initdev,
        .remove   = cx88_audio_finidev,
	SND_PCI_PM_CALLBACKS
};

/****************************************************************************
				LINUX MODULE INIT
 ****************************************************************************/

/*
 * module init
 */
static int cx88_audio_init(void)
{
	printk(KERN_INFO "cx2388x alsa driver version %d.%d.%d loaded\n",
	       (CX88_VERSION_CODE >> 16) & 0xff,
	       (CX88_VERSION_CODE >>  8) & 0xff,
	       CX88_VERSION_CODE & 0xff);
#ifdef SNAPSHOT
	printk(KERN_INFO "cx2388x: snapshot date %04d-%02d-%02d\n",
	       SNAPSHOT/10000, (SNAPSHOT/100)%100, SNAPSHOT%100);
#endif
	return pci_module_init(&cx88_audio_pci_driver);
}

/*
 * module remove
 */
static void cx88_audio_fini(void)
{
	pci_unregister_driver(&cx88_audio_pci_driver);
}

module_init(cx88_audio_init);
module_exit(cx88_audio_fini);

/* ----------------------------------------------------------- */
/*
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
