#pragma once

#ifndef __ISA_IDE_H__
#define __ISA_IDE_H__

#include "emu.h"
#include "machine/isa.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> isa16_ide_device

class isa16_ide_device : public device_t,
	public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_ide_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	bool is_primary() { return m_is_primary; }
	DECLARE_WRITE_LINE_MEMBER(ide_interrupt);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
	virtual void device_config_complete() { m_shortname = "isa_ide"; }

private:
	// internal state
	bool m_is_primary;
};


// device type definition
extern const device_type ISA16_IDE;

#endif  /* __ISA_IDE_H__ */
