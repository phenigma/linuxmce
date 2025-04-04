/***************************************************************************

    Osborne-1 driver file

The Osborne-1 memory is divided into 3 "banks".

Bank 1 simply consists of 64KB of RAM. The upper 4KB is used for the lower 8
bit of video RAM entries.

Bank 2 holds the BIOS ROM and I/O area. Only addresses 0000-3FFF are used
by bank 2. Bank 2 is divided as follows:
3000-3FFF Unused
2C00-2C03 Video PIA
2A00-2A01 Serial interface
2900-2903 488 PIA
2201-2280 Keyboard
2100-2103 Floppy
1000-1FFF Unused
0000-0FFF BIOS ROM

Bank 3 has the ninth bit needed to complete the full Video RAM. These bits
are stored at F000-FFFF. Only the highest bit is used.

On bootup bank 2 is active.

The actual banking is done through I/O ports 00-03.
00 - Have both bank 2 and bank 1 active. This seems to be the power up default.
01 - Only have bank 1 active.
02 - Have both bank 2 and bank 3 active. (Not 100% sure, also bank 1 from 4000-EFFF?)
03 - Have both bank 2 and bank 1 active.

TODO:
  - Verify frequency of the beep/audio alarm.

***************************************************************************/

#include "includes/osborne1.h"


#define MAIN_CLOCK  15974400


static ADDRESS_MAP_START( osborne1_mem, AS_PROGRAM, 8, osborne1_state )
	AM_RANGE( 0x0000, 0x0FFF ) AM_READ_BANK("bank1") AM_WRITE( osborne1_0000_w )
	AM_RANGE( 0x1000, 0x1FFF ) AM_READ_BANK("bank2") AM_WRITE( osborne1_1000_w )
	AM_RANGE( 0x2000, 0x2FFF ) AM_READWRITE( osborne1_2000_r, osborne1_2000_w )
	AM_RANGE( 0x3000, 0x3FFF ) AM_READ_BANK("bank3") AM_WRITE( osborne1_3000_w )
	AM_RANGE( 0x4000, 0xEFFF ) AM_RAM
	AM_RANGE( 0xF000, 0xFFFF ) AM_READ_BANK("bank4") AM_WRITE( osborne1_videoram_w )
ADDRESS_MAP_END


static ADDRESS_MAP_START( osborne1_io, AS_IO, 8, osborne1_state )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE( 0x00, 0x03 ) AM_WRITE( osborne1_bankswitch_w )
ADDRESS_MAP_END


static INPUT_PORTS_START( osborne1 )
	PORT_START("ROW0")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2)   PORT_CHAR('[') PORT_CHAR(']')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE)   PORT_CHAR('\'') PORT_CHAR('"')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RSHIFT)       PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL)     PORT_CODE(KEYCODE_RCONTROL) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB)          PORT_CHAR('\t')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)          PORT_CHAR(UCHAR_MAMEKEY(ESC))

	PORT_START("ROW1")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)            PORT_CHAR('8') PORT_CHAR('*')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)            PORT_CHAR('7') PORT_CHAR('&')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)            PORT_CHAR('6') PORT_CHAR('^')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)            PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)            PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)            PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)            PORT_CHAR('2') PORT_CHAR('@')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)            PORT_CHAR('1') PORT_CHAR('!')

	PORT_START("ROW2")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)            PORT_CHAR('9') PORT_CHAR('(')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)            PORT_CHAR('o') PORT_CHAR('O')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)            PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)         PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)        PORT_CHAR(' ')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)            PORT_CHAR('0') PORT_CHAR(')')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT)         PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP)           PORT_CHAR(UCHAR_MAMEKEY(UP))

	PORT_START("ROW3")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)            PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)            PORT_CHAR('u') PORT_CHAR('U')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)            PORT_CHAR('y') PORT_CHAR('Y')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)            PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)            PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)            PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)            PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)            PORT_CHAR('q') PORT_CHAR('Q')

	PORT_START("ROW4")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)            PORT_CHAR('k') PORT_CHAR('K')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)            PORT_CHAR('j') PORT_CHAR('J')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)            PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)            PORT_CHAR('g') PORT_CHAR('G')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)            PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)            PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)            PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)            PORT_CHAR('a') PORT_CHAR('A')

	PORT_START("ROW5")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)        PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)            PORT_CHAR('m') PORT_CHAR('M')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)            PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)            PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)            PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)            PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)            PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)            PORT_CHAR('z') PORT_CHAR('Z')

	PORT_START("ROW6")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)       PORT_CHAR('=') PORT_CHAR('+')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)            PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE)    PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)        PORT_CHAR(';') PORT_CHAR(':')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)        PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)        PORT_CHAR('-') PORT_CHAR('_')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN)         PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT)        PORT_CHAR(UCHAR_MAMEKEY(RIGHT))

	PORT_START("ROW7")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_DIPNAME( 0x08, 0, "Alpha Lock" ) PORT_CODE(KEYCODE_CAPSLOCK) PORT_TOGGLE PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


void osborne1_state::palette_init()
{
	palette_set_color_rgb( machine(), 0, 0, 0, 0 ); /* Black */
	palette_set_color_rgb( machine(), 1, 0, 255, 0 );   /* Full */
	palette_set_color_rgb( machine(), 2, 0, 128, 0 );   /* Dimmed */
}


static const z80_daisy_config osborne1_daisy_chain[] =
{
/*  { osborne1_z80_reset, osborne1_z80_irq_state, osborne1_z80_irq_ack, osborne1_z80_irq_reti, 0 }, */
	{ "osborne1_daisy" },
	{ NULL }
};

/*
 * The Osborne-1 supports the following disc formats:
 * - Osborne single density: 40 tracks, 10 sectors per track, 256-byte sectors (100 KByte)
 * - Osborne double density: 40 tracks, 5 sectors per track, 1024-byte sectors (200 KByte)
 * - IBM Personal Computer: 40 tracks, 8 sectors per track, 512-byte sectors (160 KByte)
 * - Xerox 820 Computer: 40 tracks, 18 sectors per track, 128-byte sectors (90 KByte)
 * - DEC 1820 double density: 40 tracks, 9 sectors per track, 512-byte sectors (180 KByte)
 *
 */
static LEGACY_FLOPPY_OPTIONS_START(osborne1 )
	LEGACY_FLOPPY_OPTION( osd, "img", "Osborne single density", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([10])
		SECTOR_LENGTH([256])
		FIRST_SECTOR_ID([1]))
	LEGACY_FLOPPY_OPTION( odd, "img", "Osborne double density", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([5])
		SECTOR_LENGTH([1024])
		FIRST_SECTOR_ID([1]))
	LEGACY_FLOPPY_OPTION( ibm, "img", "IBM Personal Computer", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([8])
		SECTOR_LENGTH([512])
		FIRST_SECTOR_ID([1]))
	LEGACY_FLOPPY_OPTION( xerox, "img", "Xerox 820 Computer", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([18])
		SECTOR_LENGTH([128])
		FIRST_SECTOR_ID([1]))
	LEGACY_FLOPPY_OPTION( dec, "img", "DEC 1820 double density", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([1])
		TRACKS([40])
		SECTORS([9])
		SECTOR_LENGTH([512])
		FIRST_SECTOR_ID([1]))
LEGACY_FLOPPY_OPTIONS_END

static const floppy_interface osborne1_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_SSDD_40,
	LEGACY_FLOPPY_OPTIONS_NAME(osborne1),
	NULL,
	NULL
};

static IEEE488_INTERFACE( ieee488_intf )
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER("pia_0", pia6821_device, ca2_w),
	DEVCB_NULL,
	DEVCB_NULL
};

/* F4 Character Displayer */
static const gfx_layout osborne1_charlayout =
{
	8, 10,                  /* 8 x 10 characters */
	128,                    /* 128 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*128*8, 1*128*8, 2*128*8, 3*128*8, 4*128*8, 5*128*8, 6*128*8, 7*128*8, 8*128*8, 9*128*8 },
	8                   /* every char takes 16 x 1 bytes */
};

static GFXDECODE_START( osborne1 )
	GFXDECODE_ENTRY( "chargen", 0x0000, osborne1_charlayout, 0, 1 )
GFXDECODE_END

static MACHINE_CONFIG_START( osborne1, osborne1_state )
	MCFG_CPU_ADD( "maincpu", Z80, MAIN_CLOCK/4 )
	MCFG_CPU_PROGRAM_MAP( osborne1_mem)
	MCFG_CPU_IO_MAP( osborne1_io)
	MCFG_CPU_CONFIG( osborne1_daisy_chain )


	MCFG_DEVICE_ADD( "osborne1_daisy", OSBORNE1_DAISY, 0 )

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_UPDATE_DRIVER(osborne1_state, screen_update)
	MCFG_SCREEN_RAW_PARAMS( MAIN_CLOCK/2, 512, 0, 416, 260, 0, 240 )
	MCFG_GFXDECODE(osborne1)
	MCFG_PALETTE_LENGTH( 3 )

	MCFG_SPEAKER_STANDARD_MONO( "mono" )
	MCFG_SOUND_ADD( BEEPER_TAG, BEEP, 0 )
	MCFG_SOUND_ROUTE( ALL_OUTPUTS, "mono", 1.00 )

	MCFG_PIA6821_ADD( "pia_0", osborne1_ieee_pia_config )
	MCFG_PIA6821_ADD( "pia_1", osborne1_video_pia_config )

	MCFG_MB8877_ADD("mb8877", default_wd17xx_interface_2_drives )

	MCFG_LEGACY_FLOPPY_2_DRIVES_ADD(osborne1_floppy_interface)
	MCFG_IEEE488_BUS_ADD(ieee488_intf)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("68K")    /* 64KB Main RAM and 4Kbit video attribute RAM */
MACHINE_CONFIG_END


ROM_START( osborne1 )
	ROM_REGION(0x1000, "maincpu", 0)
	ROM_SYSTEM_BIOS( 0, "ver144", "BIOS version 1.44" )
	ROMX_LOAD( "osb144.bin", 0x0000, 0x1000, CRC(c0596b14) SHA1(ee6a9cc9be3ddc5949d3379351c1d58a175ce9ac), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "verA", "BIOS version A" )
	ROMX_LOAD( "osba.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "ver12", "BIOS version 1.2" )
	ROMX_LOAD( "osb12.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "ver121", "BIOS version 1.2.1" )
	ROMX_LOAD( "osb121.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "ver13", "BIOS version 1.3" )
	ROMX_LOAD( "osb13.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 5, "ver14", "BISO version 1.4" )
	ROMX_LOAD( "osb14.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(6) )
	ROM_SYSTEM_BIOS( 6, "ver143", "BIOS version 1.43" )
	ROMX_LOAD( "osb143.bin", 0x0000, 0x1000, NO_DUMP, ROM_BIOS(7) )
	ROM_REGION( 0x800, "chargen", 0 )
	ROM_LOAD( "osbchr.bin", 0x0000, 0x800, BAD_DUMP CRC(6c1eab0d) SHA1(b04459d377a70abc9155a5486003cb795342c801) )
ROM_END

/*    YEAR  NAME        PARENT  COMPAT  MACHINE     INPUT       INIT        COMPANY     FULLNAME        FLAGS */
COMP( 1981, osborne1,   0,      0,      osborne1,   osborne1, osborne1_state,   osborne1,   "Osborne",  "Osborne-1",    0 )
