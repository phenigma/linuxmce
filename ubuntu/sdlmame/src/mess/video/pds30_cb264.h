#pragma once

#ifndef __NUBUS_CB264SE30_H__
#define __NUBUS_CB264SE30_H__

#include "emu.h"
#include "machine/nubus.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nubus_cb264se30_device

class nubus_cb264se30_device :
		public device_t,
		public device_nubus_card_interface
{
public:
		// construction/destruction
		nubus_cb264se30_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
		nubus_cb264se30_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
		virtual const rom_entry *device_rom_region() const;

		UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
protected:
		// device-level overrides
		virtual void device_start();
		virtual void device_reset();
		virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

		DECLARE_READ32_MEMBER(cb264se30_r);
		DECLARE_WRITE32_MEMBER(cb264se30_w);
		DECLARE_READ32_MEMBER(vram_r);
		DECLARE_WRITE32_MEMBER(vram_w);

public:
		UINT8 *m_vram;
		UINT32 *m_vram32;
		UINT32 m_mode, m_vbl_disable, m_toggle;
		UINT32 m_palette[256], m_colors[3], m_count, m_clutoffs;
		screen_device *m_screen;
		emu_timer *m_timer;
};


// device type definition
extern const device_type PDS030_CB264SE30;

#endif  /* __NUBUS_CB264SE30_H__ */
