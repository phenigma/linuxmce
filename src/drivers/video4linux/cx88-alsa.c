/*
 * $Id: cx88-alsa.c,v 1.3 2004/10/13 10:39:00 kraxel Exp $
 *
 *  Support for audio capture
 *  PCI function #1 of the cx2388x.
 *
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
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

#include "cx88.h"

/* ------------------------------------------------------------------ */

MODULE_DESCRIPTION("alsa driver module for cx2388x based TV cards");
MODULE_AUTHOR("Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------ */

static int __devinit cx8801_initdev(struct pci_dev *pci_dev,
				    const struct pci_device_id *pci_id)
{
	struct cx8801_dev *dev;
	struct cx88_core *core;
	int err;

	dev = kmalloc(sizeof(*dev),GFP_KERNEL);
	if (NULL == dev)
		return -ENOMEM;
	memset(dev,0,sizeof(*dev));

	/* pci init */
	dev->pci = pci_dev;
	if (pci_enable_device(pci_dev)) {
		err = -EIO;
		goto fail_free;
	}
	core = cx88_core_get(dev->pci);
	if (NULL == core) {
		err = -EINVAL;
		goto fail_free;
	}
	dev->core = core;

	/* print pci info */
	pci_read_config_byte(pci_dev, PCI_CLASS_REVISION, &dev->pci_rev);
        pci_read_config_byte(pci_dev, PCI_LATENCY_TIMER,  &dev->pci_lat);
        printk(KERN_INFO "%s/1: found at %s, rev: %d, irq: %d, "
	       "latency: %d, mmio: 0x%lx\n", core->name,
	       pci_name(pci_dev), dev->pci_rev, pci_dev->irq,
	       dev->pci_lat,pci_resource_start(pci_dev,0));

	pci_set_master(pci_dev);
	if (!pci_dma_supported(pci_dev,0xffffffff)) {
		printk("%s/1: Oops: no 32bit PCI DMA ???\n",core->name);
		err = -EIO;
		goto fail_core;
	}

	printk("%s/1: no alsa support yet, this is just a dummy module\n",
	       core->name);
	pci_set_drvdata(pci_dev,dev);
	return 0;

 fail_core:
	cx88_core_put(core,dev->pci);
 fail_free:
	kfree(dev);
	return err;
}

static void __devexit cx8801_finidev(struct pci_dev *pci_dev)
{
        struct cx8801_dev *dev = pci_get_drvdata(pci_dev);

	pci_disable_device(pci_dev);

	/* unregister stuff */
	pci_set_drvdata(pci_dev, NULL);

	/* free memory */
	cx88_core_put(dev->core,dev->pci);
	kfree(dev);
}

struct pci_device_id cx8801_pci_tbl[] = {
	{
		.vendor       = 0x14f1,
		.device       = 0x8801,
                .subvendor    = PCI_ANY_ID,
                .subdevice    = PCI_ANY_ID,
	},{
		.vendor       = 0x14f1,
		.device       = 0x8811,
                .subvendor    = PCI_ANY_ID,
                .subdevice    = PCI_ANY_ID,
	},{
		/* --- end of list --- */
	}
};
MODULE_DEVICE_TABLE(pci, cx8801_pci_tbl);

static struct pci_driver cx8801_pci_driver = {
        .name     = "cx8801",
        .id_table = cx8801_pci_tbl,
        .probe    = cx8801_initdev,
        .remove   = cx8801_finidev,
#if 0
	.suspend  = cx8801_suspend,
	.resume   = cx8801_resume,
#endif
};

static int cx8801_init(void)
{
	printk(KERN_INFO "cx2388x alsa driver version %d.%d.%d loaded\n",
	       (CX88_VERSION_CODE >> 16) & 0xff,
	       (CX88_VERSION_CODE >>  8) & 0xff,
	       CX88_VERSION_CODE & 0xff);
#ifdef SNAPSHOT
	printk(KERN_INFO "cx2388x: snapshot date %04d-%02d-%02d\n",
	       SNAPSHOT/10000, (SNAPSHOT/100)%100, SNAPSHOT%100);
#endif
	return pci_module_init(&cx8801_pci_driver);
}

static void cx8801_fini(void)
{
	pci_unregister_driver(&cx8801_pci_driver);
}

module_init(cx8801_init);
module_exit(cx8801_fini);

/* ----------------------------------------------------------- */
/*
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
