/***************************************************************************

    drivers/mirage.c

    Ensoniq Mirage Sampler
    Preliminary driver by R. Belmont

    Map for Mirage:
    0000-7fff: 32k window on 128k of sample RAM
    8000-bfff: main RAM
    c000-dfff: optional expansion RAM
    e100-e101: 6850 UART (for MIDI)
    e200-e2ff: 6522 VIA
    e408-e40f: filter cut-off frequency
    e410-e417: filter resonance
    e418-e41f: multiplexer address pre-set
    e800-e803: WD1770 FDC
    ec00-ecef: ES5503 "DOC" sound chip
    f000-ffff: boot ROM

    NMI: IRQ from WD1772
    IRQ: DRQ from WD1772 wire-ORed with IRQ from ES5503 wire-ORed with IRQ from VIA6522
    FIRQ: IRQ from 6850 UART

    LED / switch matrix:

            A           B           C             D         E         F         G        DP
    ROW 0:  LOAD UPPER  LOAD LOWER  SAMPLE UPPER  PLAY SEQ  LOAD SEQ  SAVE SEQ  REC SEQ  SAMPLE LOWER
    ROW 1:  3           6           9             5         8         0         2        Enter
    ROW 2:  1           4           7             up arrow  PARAM     dn arrow  VALUE    CANCEL
    L. AN:  SEG A       SEG B       SEG C         SEG D     SEG E     SEG F     SEG G    SEG DP (decimal point)
    R. AN:  SEG A       SEG B       SEG C         SEG D     SEG E     SEG F     SEG G    SEG DP

    Column number in VIA port A bits 0-2 is converted to discrete lines by a 74LS145.
    Port A bit 3 is right anode, bit 4 is left anode
    ROW 0 is read on VIA port A bit 5, ROW 1 in port A bit 6, and ROW 2 in port A bit 7.

    Keyboard models talk to the R6500 through the VIA shifter: CA2 is handshake, CB1 is shift clock, CB2 is shift data.
    This is unconnected on the rackmount version.

***************************************************************************/


#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "machine/6850acia.h"
#include "machine/6522via.h"
#include "machine/wd_fdc.h"
#include "formats/esq8_dsk.h"
#include "sound/es5503.h"

#include "mirage.lh"

class mirage_state : public driver_device
{
public:
	mirage_state(const machine_config &mconfig, device_type type, const char *tag)
	: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_fdc(*this, "wd1772")
	{ }

	required_device<m6809e_device> m_maincpu;
	required_device<wd1772_t> m_fdc;

	virtual void machine_reset();

	int last_sndram_bank;

	DECLARE_FLOPPY_FORMATS( floppy_formats );

	void fdc_intrq_w(bool state);
	void fdc_drq_w(bool state);
	DECLARE_DRIVER_INIT(mirage);
	virtual void video_start();
	UINT32 screen_update_mirage(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE8_MEMBER(mirage_via_write_porta);
	DECLARE_WRITE8_MEMBER(mirage_via_write_portb);
	DECLARE_READ8_MEMBER(mirage_via_read_porta);
	DECLARE_READ8_MEMBER(mirage_via_read_portb);
	DECLARE_READ8_MEMBER(mirage_via_read_ca1);
	DECLARE_READ8_MEMBER(mirage_via_read_cb1);
	DECLARE_READ8_MEMBER(mirage_via_read_ca2);
	DECLARE_READ8_MEMBER(mirage_via_read_cb2);

	DECLARE_WRITE_LINE_MEMBER(acia_irq_w);

	UINT8 m_l_segs, m_r_segs;
	int   m_l_hi, m_r_hi;
};

FLOPPY_FORMATS_MEMBER( mirage_state::floppy_formats )
	FLOPPY_ESQ8IMG_FORMAT
FLOPPY_FORMATS_END

static SLOT_INTERFACE_START( ensoniq_floppies )
	SLOT_INTERFACE( "35dd", FLOPPY_35_DD )
SLOT_INTERFACE_END

WRITE_LINE_MEMBER(mirage_state::acia_irq_w)
{
	m_maincpu->set_input_line(M6809_FIRQ_LINE, state);
}

void mirage_state::fdc_intrq_w(bool state)
{
	m_maincpu->set_input_line(INPUT_LINE_NMI, state);
}

void mirage_state::fdc_drq_w(bool state)
{
	m_maincpu->set_input_line(M6809_IRQ_LINE, state);
}

static void mirage_doc_irq(device_t *device, int state)
{
//    m_maincpu->set_input_line(M6809_IRQ_LINE, state);
}

static UINT8 mirage_adc_read(device_t *device)
{
	return 0x00;
}

void mirage_state::video_start()
{
}

UINT32 mirage_state::screen_update_mirage(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return 0;
}

void mirage_state::machine_reset()
{
	last_sndram_bank = 0;
	membank("sndbank")->set_base(machine().root_device().memregion("es5503")->base() );
}

static ADDRESS_MAP_START( mirage_map, AS_PROGRAM, 8, mirage_state )
	AM_RANGE(0x0000, 0x7fff) AM_RAMBANK("sndbank")  // 32k window on 128k of wave RAM
	AM_RANGE(0x8000, 0xbfff) AM_RAM         // main RAM
	AM_RANGE(0xc000, 0xdfff) AM_RAM         // expansion RAM
	AM_RANGE(0xe100, 0xe100) AM_DEVREADWRITE("acia6850", acia6850_device, status_read, control_write)
	AM_RANGE(0xe101, 0xe101) AM_DEVREADWRITE("acia6850", acia6850_device, data_read, data_write)
	AM_RANGE(0xe200, 0xe2ff) AM_DEVREADWRITE("via6522", via6522_device, read, write)
	AM_RANGE(0xe400, 0xe4ff) AM_NOP
	AM_RANGE(0xe800, 0xe803) AM_DEVREADWRITE("wd1772", wd1772_t, read, write)
	AM_RANGE(0xec00, 0xecef) AM_DEVREADWRITE("es5503", es5503_device, read, write)
	AM_RANGE(0xf000, 0xffff) AM_ROM AM_REGION("osrom", 0)
ADDRESS_MAP_END

// port A: front panel
// bits 0-2: column select from 0-7
// bits 3/4 = right and left LED enable
// bits 5/6/7 keypad rows 0/1/2 return
WRITE8_MEMBER(mirage_state::mirage_via_write_porta)
{
	UINT8 seg = data & 7;
	static const int segconv[8] =
	{
		16, 8, 32, 2, 1, 64, 128, 4
	};

//  printf("PA: %02x (PC=%x)\n", data, m_maincpu->pc());

	// left LED selected?
	if ((data & 0x10) == 0x10)
	{
		// if the segment number is lower than last time, we've
		// started a new refresh cycle
		if ((seg < m_l_hi) || (seg == 0))
		{
			m_l_segs = segconv[seg];
		}
		else
		{
			m_l_segs |= segconv[seg];
		}

		m_l_hi = seg;
		output_set_digit_value(0, m_l_segs);
//      printf("L LED: seg %d (hi %d conv %02x, %02x)\n", seg, m_l_hi, segconv[seg], m_l_segs);
	}
	// right LED selected?
	if ((data & 0x08) == 0x08)
	{
		// if the segment number is lower than last time, we've
		// started a new refresh cycle
		if ((seg < m_r_hi) || (seg == 0))
		{
			m_r_segs = segconv[seg];
		}
		else
		{
			m_r_segs |= segconv[seg];
		}

		m_r_hi = seg;
		output_set_digit_value(1, m_r_segs);
//      printf("R LED: seg %d (hi %d conv %02x, %02x)\n", seg, m_r_hi, segconv[seg], m_r_segs);
	}
}

// port B:
//  bit 7: OUT UART clock
//  bit 4: OUT disk enable (motor on?)
//  bit 3: OUT sample/play
//  bit 2: OUT mic line/in
//  bit 1: OUT upper/lower bank (64k halves)
//  bit 0: OUT bank 0/bank 1 (32k halves)

WRITE8_MEMBER(mirage_state::mirage_via_write_portb)
{
	int bank = 0;

	// handle sound RAM bank switching
	bank = (data & 2) ? (64*1024) : 0;
	bank += (data & 1) ? (32*1024) : 0;
	if (bank != last_sndram_bank)
	{
		last_sndram_bank = bank;
		membank("sndbank")->set_base(memregion("es5503")->base() + bank);
	}
}

// port A: front panel
READ8_MEMBER(mirage_state::mirage_via_read_porta)
{
	return 0;
}

// port B:
//  bit 6: IN FDC disk ready
//  bit 5: IN 5503 sync (?)
READ8_MEMBER(mirage_state::mirage_via_read_portb)
{
	return 0x60;
}

// external sync pulse
READ8_MEMBER(mirage_state::mirage_via_read_ca1)
{
	return 0;
}

// keyscan
READ8_MEMBER(mirage_state::mirage_via_read_cb1)
{
	return 0;
}

// keyscan
READ8_MEMBER(mirage_state::mirage_via_read_ca2)
{
	return 0;
}


// keyscan
READ8_MEMBER(mirage_state::mirage_via_read_cb2)
{
	return 0;
}

const via6522_interface mirage_via =
{
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_porta),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_portb),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_ca1),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_cb1),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_ca2),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_read_cb2),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_write_porta),
	DEVCB_DRIVER_MEMBER(mirage_state, mirage_via_write_portb),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_CPU_INPUT_LINE("maincpu", M6809_IRQ_LINE)
};

static ACIA6850_INTERFACE( mirage_acia6850_interface )
{
	0,              // tx clock
	0,              // rx clock
	DEVCB_NULL,         // rx in
	DEVCB_NULL,         // rx out
	DEVCB_NULL,         // cts in
	DEVCB_NULL,         // rts out
	DEVCB_NULL,         // dcd in
	DEVCB_DRIVER_LINE_MEMBER(mirage_state, acia_irq_w)
};

static MACHINE_CONFIG_START( mirage, mirage_state )
	MCFG_CPU_ADD("maincpu", M6809E, 4000000)
	MCFG_CPU_PROGRAM_MAP(mirage_map)

	MCFG_DEFAULT_LAYOUT( layout_mirage )

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_ES5503_ADD("es5503", 7000000, 2, mirage_doc_irq, mirage_adc_read)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)

	MCFG_VIA6522_ADD("via6522", 1000000, mirage_via)

	MCFG_ACIA6850_ADD("acia6850", mirage_acia6850_interface)

	MCFG_WD1772x_ADD("wd1772", 8000000)
	MCFG_FLOPPY_DRIVE_ADD("wd1772:0", ensoniq_floppies, "35dd", 0, mirage_state::floppy_formats)
MACHINE_CONFIG_END

static INPUT_PORTS_START( mirage )
INPUT_PORTS_END

ROM_START( enmirage )
	ROM_REGION(0x1000, "osrom", 0)
	ROM_LOAD( "mirage.bin",   0x0000, 0x1000, CRC(9fc7553c) SHA1(ec6ea5613eeafd21d8f3a7431a35a6ff16eed56d) )

	ROM_REGION(0x20000, "es5503", ROMREGION_ERASE)
ROM_END

DRIVER_INIT_MEMBER(mirage_state,mirage)
{

	floppy_connector *con = machine().device<floppy_connector>("wd1772:0");
	floppy_image_device *floppy = con ? con->get_device() : 0;
	if (floppy)
	{
		m_fdc->set_floppy(floppy);
		m_fdc->setup_intrq_cb(wd1772_t::line_cb(FUNC(mirage_state::fdc_intrq_w), this));
		m_fdc->setup_drq_cb(wd1772_t::line_cb(FUNC(mirage_state::fdc_drq_w), this));

		floppy->ss_w(0);
	}

	m_l_hi = m_r_hi = 9;
	m_l_segs = m_r_segs = 0;
}

CONS( 1984, enmirage, 0, 0, mirage, mirage, mirage_state, mirage, "Ensoniq", "Ensoniq Mirage", GAME_NOT_WORKING )
