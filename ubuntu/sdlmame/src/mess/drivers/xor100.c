/****************************************************************************************************

        XOR S-100-12

*****************************************************************************************************

        Summary of Monitor commands:

        D xxxx yyyy = dump memory to screen
        F xxxx yyyy zz = fill memory from xxxx to yyyy-1 with zz
        G xxxx         = execute program at xxxx
        H xxxx yyyy aa bb...  = unknown
        L xxxx         = edit memory (. to exit)
        M xxxx yyyy zzzz = Move (copy) memory
        V xxxx           = unknown
        X n     = Select a bank (0 works, others freeze)

        Note some of the commands are a bit buggy, eg F doesn't fill the last byte

*****************************************************************************************************/

/*

    TODO:

    - cannot boot from floppy (at prompt press ^C, wait, it says 'Drive not ready')
    - honor jumper settings
    - CTC signal header
    - serial printer
    - cassette?

*/


#include "includes/xor100.h"

/* Read/Write Handlers */

enum
{
	EPROM_0000 = 0,
	EPROM_F800,
	EPROM_OFF
};

void xor100_state::bankswitch()
{
	address_space &program = m_maincpu->space(AS_PROGRAM);
	int banks = m_ram->size() / 0x10000;

	switch (m_mode)
	{
	case EPROM_0000:
		if (m_bank < banks)
		{
			program.install_write_bank(0x0000, 0xffff, "bank1");
			membank("bank1")->set_entry(1 + m_bank);
		}
		else
		{
			program.unmap_write(0x0000, 0xffff);
		}

		program.install_read_bank(0x0000, 0xf7ff, 0x07ff, 0, "bank2");
		program.install_read_bank(0xf800, 0xffff, "bank3");
		membank("bank2")->set_entry(0);
		membank("bank3")->set_entry(0);
		break;

	case EPROM_F800:
		if (m_bank < banks)
		{
			program.install_write_bank(0x0000, 0xffff, "bank1");
			program.install_read_bank(0x0000, 0xf7ff, "bank2");
			membank("bank1")->set_entry(1 + m_bank);
			membank("bank2")->set_entry(1 + m_bank);
		}
		else
		{
			program.unmap_write(0x0000, 0xffff);
			program.unmap_read(0x0000, 0xf7ff);
		}

		program.install_read_bank(0xf800, 0xffff, "bank3");
		membank("bank3")->set_entry(0);
		break;

	case EPROM_OFF:
		if (m_bank < banks)
		{
			program.install_write_bank(0x0000, 0xffff, "bank1");
			program.install_read_bank(0x0000, 0xf7ff, "bank2");
			program.install_read_bank(0xf800, 0xffff, "bank3");
			membank("bank1")->set_entry(1 + m_bank);
			membank("bank2")->set_entry(1 + m_bank);
			membank("bank3")->set_entry(1 + m_bank);
		}
		else
		{
			program.unmap_write(0x0000, 0xffff);
			program.unmap_read(0x0000, 0xf7ff);
			program.unmap_read(0xf800, 0xffff);
		}
		break;
	}
}

WRITE8_MEMBER( xor100_state::mmu_w )
{
	/*

	    bit     description

	    0       A16
	    1       A17
	    2       A18
	    3       A19
	    4
	    5
	    6
	    7

	*/

	m_bank = data & 0x07;

	bankswitch();
}

WRITE8_MEMBER( xor100_state::prom_toggle_w )
{
	switch (m_mode)
	{
	case EPROM_OFF: m_mode = EPROM_F800; break;
	case EPROM_F800: m_mode = EPROM_OFF; break;
	}

	bankswitch();
}

READ8_MEMBER( xor100_state::prom_disable_r )
{
	m_mode = EPROM_F800;

	bankswitch();

	return 0xff;
}

WRITE8_MEMBER( xor100_state::baud_w )
{
	m_dbrg->str_w(space, 0, data & 0x0f);
	m_dbrg->stt_w(space, 0, data >> 4);
}

WRITE8_MEMBER( xor100_state::i8251_b_data_w )
{
	m_uart_b->data_w(space, 0, data);
	m_terminal->write(space, 0, data);
}

READ8_MEMBER( xor100_state::fdc_r )
{
	return m_fdc->gen_r(offset) ^ 0xff;
}

WRITE8_MEMBER( xor100_state::fdc_w )
{
	m_fdc->gen_w(offset, data ^ 0xff);
}

READ8_MEMBER( xor100_state::fdc_wait_r )
{
	/*

	    bit     description

	    0
	    1
	    2
	    3
	    4
	    5
	    6
	    7       FDC IRQ

	*/

	if (!m_fdc_irq && !m_fdc_drq)
	{
		fatalerror("Z80 WAIT not supported by MAME core\n");
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
	}

	return !m_fdc_irq << 7;
}

WRITE8_MEMBER( xor100_state::fdc_dcont_w )
{
	/*

	    bit     description

	    0       DS0
	    1       DS1
	    2       DS2
	    3       DS3
	    4
	    5
	    6
	    7       _HLSTB

	*/

	// drive select
	floppy_image_device *floppy = NULL;

	if (BIT(data, 0)) floppy = m_floppy0->get_device();
	if (BIT(data, 1)) floppy = m_floppy1->get_device();
	if (BIT(data, 2)) floppy = m_floppy2->get_device();
	if (BIT(data, 3)) floppy = m_floppy3->get_device();

	m_fdc->set_floppy(floppy);

	if (floppy) floppy->mon_w(0);
}

WRITE8_MEMBER( xor100_state::fdc_dsel_w )
{
	/*

	    bit     description

	    0       J
	    1       K
	    2
	    3
	    4
	    5
	    6
	    7

	*/

	switch (data & 0x03)
	{
	case 0: break;
	case 1: m_fdc_dden = 1; break;
	case 2: m_fdc_dden = 0; break;
	case 3: m_fdc_dden = !m_fdc_dden; break;
	}

	m_fdc->dden_w(m_fdc_dden);
}

/* Memory Maps */

static ADDRESS_MAP_START( xor100_mem, AS_PROGRAM, 8, xor100_state )
	AM_RANGE(0x0000, 0xffff) AM_WRITE_BANK("bank1")
	AM_RANGE(0x0000, 0xf7ff) AM_READ_BANK("bank2")
	AM_RANGE(0xf800, 0xffff) AM_READ_BANK("bank3")
ADDRESS_MAP_END

static ADDRESS_MAP_START( xor100_io, AS_IO, 8, xor100_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_DEVREADWRITE(I8251_A_TAG, i8251_device, data_r, data_w)
	AM_RANGE(0x01, 0x01) AM_DEVREADWRITE(I8251_A_TAG, i8251_device, status_r, control_w)
	AM_RANGE(0x02, 0x02) AM_DEVREAD(I8251_B_TAG, i8251_device, data_r) AM_WRITE(i8251_b_data_w)
	AM_RANGE(0x03, 0x03) AM_DEVREADWRITE(I8251_B_TAG, i8251_device, status_r, control_w)
	AM_RANGE(0x04, 0x07) AM_DEVREADWRITE(I8255A_TAG, i8255_device, read, write)
	AM_RANGE(0x08, 0x08) AM_WRITE(mmu_w)
	AM_RANGE(0x09, 0x09) AM_WRITE(prom_toggle_w)
	AM_RANGE(0x0a, 0x0a) AM_READ(prom_disable_r)
	AM_RANGE(0x0b, 0x0b) AM_READ_PORT("DSW0") AM_WRITE(baud_w)
	AM_RANGE(0x0c, 0x0f) AM_DEVREADWRITE(Z80CTC_TAG, z80ctc_device, read, write)
	AM_RANGE(0xf8, 0xfb) AM_READWRITE(fdc_r, fdc_w)
	AM_RANGE(0xfc, 0xfc) AM_READWRITE(fdc_wait_r, fdc_dcont_w)
	AM_RANGE(0xfd, 0xfd) AM_WRITE(fdc_dsel_w)
ADDRESS_MAP_END

/* Input Ports */

static INPUT_PORTS_START( xor100 )

	PORT_START("DSW0")
	PORT_DIPNAME( 0x0f, 0x05, "Serial Port A" )
	PORT_DIPSETTING(    0x00, "50 baud" )
	PORT_DIPSETTING(    0x01, "75 baud" )
	PORT_DIPSETTING(    0x02, "110 baud" )
	PORT_DIPSETTING(    0x03, "134.5 baud" )
	PORT_DIPSETTING(    0x04, "150 baud" )
	PORT_DIPSETTING(    0x05, "300 baud" )
	PORT_DIPSETTING(    0x06, "600 baud" )
	PORT_DIPSETTING(    0x07, "1200 baud" )
	PORT_DIPSETTING(    0x08, "1800 baud" )
	PORT_DIPSETTING(    0x09, "2000 baud" )
	PORT_DIPSETTING(    0x0a, "2400 baud" )
	PORT_DIPSETTING(    0x0b, "3600 baud" )
	PORT_DIPSETTING(    0x0c, "4800 baud" )
	PORT_DIPSETTING(    0x0d, "7200 baud" )
	PORT_DIPSETTING(    0x0e, "9600 baud" )
	PORT_DIPSETTING(    0x0f, "19200 baud" )
	PORT_DIPNAME( 0xf0, 0xe0, "Serial Port B" )
	PORT_DIPSETTING(    0x00, "50 baud" )
	PORT_DIPSETTING(    0x10, "75 baud" )
	PORT_DIPSETTING(    0x20, "110 baud" )
	PORT_DIPSETTING(    0x30, "134.5 baud" )
	PORT_DIPSETTING(    0x40, "150 baud" )
	PORT_DIPSETTING(    0x50, "300 baud" )
	PORT_DIPSETTING(    0x60, "600 baud" )
	PORT_DIPSETTING(    0x70, "1200 baud" )
	PORT_DIPSETTING(    0x80, "1800 baud" )
	PORT_DIPSETTING(    0x90, "2000 baud" )
	PORT_DIPSETTING(    0xa0, "2400 baud" )
	PORT_DIPSETTING(    0xb0, "3600 baud" )
	PORT_DIPSETTING(    0xc0, "4800 baud" )
	PORT_DIPSETTING(    0xd0, "7200 baud" )
	PORT_DIPSETTING(    0xe0, "9600 baud" )
	PORT_DIPSETTING(    0xf0, "19200 baud" )

	PORT_START("J1")
	PORT_CONFNAME( 0x01, 0x01, "J1 Wait State")
	PORT_CONFSETTING( 0x00, "No Wait States" )
	PORT_CONFSETTING( 0x01, "1 M1 Wait State" )

	PORT_START("J2")
	PORT_CONFNAME( 0x01, 0x01, "J2 CPU Speed")
	PORT_CONFSETTING( 0x00, "2 MHz" )
	PORT_CONFSETTING( 0x01, "4 MHz" )

	PORT_START("J3")
	PORT_CONFNAME( 0x01, 0x00, "J3")
	PORT_CONFSETTING( 0x00, "" )
	PORT_CONFSETTING( 0x01, "" )

	PORT_START("J4-J5")
	PORT_CONFNAME( 0x01, 0x01, "J4/J5 EPROM Type")
	PORT_CONFSETTING( 0x00, "2708" )
	PORT_CONFSETTING( 0x01, "2716" )

	PORT_START("J6")
	PORT_CONFNAME( 0x01, 0x01, "J6 EPROM")
	PORT_CONFSETTING( 0x00, "Disabled" )
	PORT_CONFSETTING( 0x01, "Enabled" )

	PORT_START("J7")
	PORT_CONFNAME( 0x01, 0x00, "J7 I/O Port Addresses")
	PORT_CONFSETTING( 0x00, "00-0F" )
	PORT_CONFSETTING( 0x01, "10-1F" )

	PORT_START("J8")
	PORT_CONFNAME( 0x01, 0x00, "J8")
	PORT_CONFSETTING( 0x00, "" )
	PORT_CONFSETTING( 0x01, "" )

	PORT_START("J9")
	PORT_CONFNAME( 0x01, 0x01, "J9 Power On RAM")
	PORT_CONFSETTING( 0x00, "Enabled" )
	PORT_CONFSETTING( 0x01, "Disabled" )
INPUT_PORTS_END

/* COM5016 Interface */

WRITE_LINE_MEMBER( xor100_state::com5016_fr_w )
{
	m_uart_a->transmit_clock();
	m_uart_a->receive_clock();
}

WRITE_LINE_MEMBER( xor100_state::com5016_ft_w )
{
	m_uart_b->transmit_clock();
	m_uart_b->receive_clock();
}

static COM8116_INTERFACE( com5016_intf )
{
	DEVCB_NULL,                 /* fX/4 output */
	DEVCB_DRIVER_LINE_MEMBER(xor100_state, com5016_fr_w),   /* fR output */
	DEVCB_DRIVER_LINE_MEMBER(xor100_state, com5016_ft_w),   /* fT output */
	{ 101376, 67584, 46080, 37686, 33792, 16896, 8448, 4224, 2816, 2534, 2112, 1408, 1056, 704, 528, 264 }, // WRONG?
	{ 101376, 67584, 46080, 37686, 33792, 16896, 8448, 4224, 2816, 2534, 2112, 1408, 1056, 704, 528, 264 }, // WRONG?
};

/* Printer 8251A Interface */

static const i8251_interface printer_8251_intf =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

/* Terminal 8251A Interface */

static const i8251_interface terminal_8251_intf =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

/* Printer 8255A Interface */

READ8_MEMBER(xor100_state::i8255_pc_r)
{
	/*

	    bit     description

	    PC0
	    PC1
	    PC2
	    PC3
	    PC4     ON LINE
	    PC5     BUSY
	    PC6     _ACK
	    PC7

	*/

	UINT8 data = 0;

	/* on line */
	data |= m_centronics->vcc_r() << 4;

	/* busy */
	data |= m_centronics->busy_r() << 5;

	return data;
}

static I8255A_INTERFACE( printer_8255_intf )
{
	DEVCB_NULL,
	DEVCB_DEVICE_MEMBER(CENTRONICS_TAG, centronics_device, write),
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER(CENTRONICS_TAG, centronics_device, strobe_w),
	DEVCB_DRIVER_MEMBER(xor100_state, i8255_pc_r),
	DEVCB_NULL
};

static const centronics_interface xor100_centronics_intf =
{
	DEVCB_DEVICE_LINE_MEMBER(I8255A_TAG, i8255_device, pc4_w),
	DEVCB_NULL,
	DEVCB_NULL
};

/* Z80-CTC Interface */

WRITE_LINE_MEMBER( xor100_state::ctc_z0_w )
{
}

WRITE_LINE_MEMBER( xor100_state::ctc_z1_w )
{
}

WRITE_LINE_MEMBER( xor100_state::ctc_z2_w )
{
}

static Z80CTC_INTERFACE( ctc_intf )
{
	DEVCB_CPU_INPUT_LINE(Z80_TAG, INPUT_LINE_IRQ0), /* interrupt handler */
	DEVCB_DRIVER_LINE_MEMBER(xor100_state, ctc_z0_w),           /* ZC/TO0 callback */
	DEVCB_DRIVER_LINE_MEMBER(xor100_state, ctc_z1_w),           /* ZC/TO1 callback */
	DEVCB_DRIVER_LINE_MEMBER(xor100_state, ctc_z2_w)        /* ZC/TO2 callback */
};

/* WD1795-02 Interface */

static SLOT_INTERFACE_START( xor100_floppies )
	SLOT_INTERFACE( "8ssdd", FLOPPY_8_SSDD ) // Shugart SA-100
SLOT_INTERFACE_END

void xor100_state::fdc_intrq_w(bool state)
{
	m_fdc_irq = state;
	m_ctc->trg0(state);

	if (state)
	{
		fatalerror("Z80 WAIT not supported by MAME core\n");
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
	}
}

void xor100_state::fdc_drq_w(bool state)
{
	m_fdc_drq = state;

	if (state)
	{
		fatalerror("Z80 WAIT not supported by MAME core\n");
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
	}
}

/* Terminal Interface */

WRITE8_MEMBER( xor100_state::xor100_kbd_put )
{
	m_uart_b->receive_character(data);
}

static GENERIC_TERMINAL_INTERFACE( xor100_terminal_intf )
{
	DEVCB_DRIVER_MEMBER(xor100_state, xor100_kbd_put)
};

static S100_INTERFACE( s100_intf )
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_CPU_INPUT_LINE(Z80_TAG, Z80_INPUT_LINE_WAIT),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
};

static SLOT_INTERFACE_START( xor100_s100_cards )
SLOT_INTERFACE_END

/* Machine Initialization */

void xor100_state::machine_start()
{
	int banks = m_ram->size() / 0x10000;
	UINT8 *ram = m_ram->pointer();
	UINT8 *rom = memregion(Z80_TAG)->base();

	/* setup memory banking */
	membank("bank1")->configure_entries(1, banks, ram, 0x10000);
	membank("bank2")->configure_entry(0, rom);
	membank("bank2")->configure_entries(1, banks, ram, 0x10000);
	membank("bank3")->configure_entry(0, rom);
	membank("bank3")->configure_entries(1, banks, ram + 0xf800, 0x10000);

	/* register for state saving */
	save_item(NAME(m_mode));
	save_item(NAME(m_bank));
	save_item(NAME(m_fdc_irq));
	save_item(NAME(m_fdc_drq));
	save_item(NAME(m_fdc_dden));
}

void xor100_state::machine_reset()
{
	m_mode = EPROM_0000;

	bankswitch();
}

/* Machine Driver */

static MACHINE_CONFIG_START( xor100, xor100_state )
	/* basic machine hardware */
	MCFG_CPU_ADD(Z80_TAG, Z80, XTAL_8MHz/2)
	MCFG_CPU_PROGRAM_MAP(xor100_mem)
	MCFG_CPU_IO_MAP(xor100_io)

	/* devices */
	MCFG_I8251_ADD(I8251_A_TAG, /*XTAL_8MHz/2,*/ printer_8251_intf)
	MCFG_I8251_ADD(I8251_B_TAG, /*XTAL_8MHz/2,*/ terminal_8251_intf)
	MCFG_I8255A_ADD(I8255A_TAG, printer_8255_intf)
	MCFG_Z80CTC_ADD(Z80CTC_TAG, XTAL_8MHz/2, ctc_intf)
	MCFG_COM8116_ADD(COM5016_TAG, 5000000, com5016_intf)
	MCFG_FD1795x_ADD(WD1795_TAG, XTAL_8MHz/4)
	MCFG_FLOPPY_DRIVE_ADD(WD1795_TAG":0", xor100_floppies, "8ssdd", NULL, floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD(WD1795_TAG":1", xor100_floppies, "8ssdd", NULL, floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD(WD1795_TAG":2", xor100_floppies, NULL,    NULL, floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD(WD1795_TAG":3", xor100_floppies, NULL,    NULL, floppy_image_device::default_floppy_formats)

	MCFG_CENTRONICS_PRINTER_ADD(CENTRONICS_TAG, xor100_centronics_intf)
	MCFG_GENERIC_TERMINAL_ADD(TERMINAL_TAG, xor100_terminal_intf)

	// S-100
	MCFG_S100_BUS_ADD(Z80_TAG, s100_intf)
	MCFG_S100_SLOT_ADD("s100_1", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_2", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_3", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_4", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_5", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_6", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_7", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_8", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_9", xor100_s100_cards, NULL, NULL)
	MCFG_S100_SLOT_ADD("s100_10", xor100_s100_cards, NULL, NULL)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("64K")
	MCFG_RAM_EXTRA_OPTIONS("128K,192K,256K,320K,384K,448K,512K")
MACHINE_CONFIG_END

/* ROMs */

ROM_START( xor100 )
	ROM_REGION( 0x800, Z80_TAG, 0 )
	ROM_SYSTEM_BIOS( 0, "v185", "v1.85" )
	ROMX_LOAD( "xp 185.8b", 0x000, 0x800, CRC(0d0bda8d) SHA1(11c83f7cd7e6a570641b44a2f2cc5737a7dd8ae3), ROM_BIOS(1) )
ROM_END

/* System Drivers */

/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT       INIT    COMPANY                 FULLNAME        FLAGS */
COMP( 1980, xor100, 0,      0,      xor100,     xor100, driver_device,     0,   "Xor Data Science",     "XOR S-100-12", GAME_SUPPORTS_SAVE | GAME_NO_SOUND_HW)
