/**********************************************************************

    MOS 7360/8360 Text Edit Device (TED) emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "emu.h"
#include "mos7360.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define LOG 0


#define VERBOSE_LEVEL 0
#define DBG_LOG(N,M,A) \
	do { \
		if(VERBOSE_LEVEL >= N) \
		{ \
			if( M ) \
				logerror("%11.6f: %-24s", machine().time().as_double(), (char*) M ); \
			logerror A; \
		} \
	} while(0)


#define VREFRESHINLINES 28

#define TIMER1HELPER (m_reg[0] | (m_reg[1] << 8))
#define TIMER2HELPER (m_reg[2] | (m_reg[3] << 8))
#define TIMER3HELPER (m_reg[4] | (m_reg[5] << 8))
#define TIMER1 (TIMER1HELPER ? TIMER1HELPER : 0x10000)
#define TIMER2 (TIMER2HELPER ? TIMER2HELPER : 0x10000)
#define TIMER3 (TIMER3HELPER ? TIMER3HELPER : 0x10000)

#define TED7360_YPOS            40
#define RASTERLINE_2_C16(a)    ((a + m_lines - TED7360_YPOS - 5) % m_lines)
#define C16_2_RASTERLINE(a)    ((a + TED7360_YPOS + 5) % m_lines)
#define XPOS 8
#define YPOS 8

#define SCREENON               (m_reg[6] & 0x10)
#define TEST                   (m_reg[6] & 0x80)
#define VERTICALPOS            (m_reg[6] & 0x07)
#define HORICONTALPOS          (m_reg[7] & 0x07)
#define ECMON                  (m_reg[6] & 0x40)
#define HIRESON                (m_reg[6] & 0x20)
#define MULTICOLORON           (m_reg[7] & 0x10)
#define REVERSEON              (!(m_reg[7] & 0x80))

/* hardware inverts character when bit 7 set (character taken &0x7f) */
/* instead of fetching character with higher number! */
#define LINES25     (m_reg[6] & 0x08)     /* else 24 Lines */
#define LINES       (LINES25 ? 25 : 24)
#define YSIZE       (LINES * 8)
#define COLUMNS40   (m_reg[7] & 0x08)     /* else 38 Columns */
#define COLUMNS     (COLUMNS40 ? 40 : 38)
#define XSIZE       (COLUMNS * 8)

#define INROM       (m_reg[0x12] & 0x04)
#define CHARGENADDR (REVERSEON && !HIRESON && !MULTICOLORON ? ((m_reg[0x13] & 0xfc) << 8) : ((m_reg[0x13] & 0xf8) << 8))
#define BITMAPADDR  ((m_reg[0x12] & 0x38) << 10)
#define VIDEOADDR   ((m_reg[0x14] & 0xf8) << 8)

#define RASTERLINE  (((m_reg[0xa] & 0x01) << 8) | m_reg[0xb])
#define CURSOR1POS  (m_reg[0xd] | ((m_reg[0xc] & 0x03) << 8))
#define CURSOR2POS  (m_reg[0x1b] | ((m_reg[0x1a] & 0x03) << 8))
#define CURSORRATE  ((m_reg[0x1f] & 0x7c) >> 2)

#define BACKGROUNDCOLOR (m_reg[0x15] & 0x7f)
#define FOREGROUNDCOLOR (m_reg[0x16] & 0x7f)
#define MULTICOLOR1     (m_reg[0x17] & 0x7f)
#define MULTICOLOR2     (m_reg[0x18] & 0x7f)
#define FRAMECOLOR      (m_reg[0x19] & 0x7f)

#define TED7360_CLOCK        (m_clock / 4)
#define TED7360_VRETRACERATE ((m_clock == TED7360PAL_CLOCK) ? TED7360PAL_VRETRACERATE : TED7360NTSC_VRETRACERATE)
#define TED7360_LINES        ((m_clock == TED7360PAL_CLOCK) ? TED7360PAL_LINES : TED7360NTSC_LINES)

static attotime TEDTIME_IN_CYCLES(int cycles)
{
	double d = (double)(cycles) / clock();
	return attotime::from_double(d);
}

static int TEDTIME_TO_CYCLES(attotime t)
{
	double d = t.as_double();
	return (int)(d * clock());
}

static const rgb_t PALETTE[] =
{
/* black, white, red, cyan */
/* purple, green, blue, yellow */
/* orange, light orange, pink, light cyan, */
/* light violett, light green, light blue, light yellow */
/* these 16 colors are 8 times here in different luminance (dark..light) */
/* taken from digitized tv screenshot */
	MAKE_RGB(0x06, 0x01, 0x03), MAKE_RGB(0x2b, 0x2b, 0x2b), MAKE_RGB(0x67, 0x0e, 0x0f), MAKE_RGB(0x00, 0x3f, 0x42),
	MAKE_RGB(0x57, 0x00, 0x6d), MAKE_RGB(0x00, 0x4e, 0x00), MAKE_RGB(0x19, 0x1c, 0x94), MAKE_RGB(0x38, 0x38, 0x00),
	MAKE_RGB(0x56, 0x20, 0x00), MAKE_RGB(0x4b, 0x28, 0x00), MAKE_RGB(0x16, 0x48, 0x00), MAKE_RGB(0x69, 0x07, 0x2f),
	MAKE_RGB(0x00, 0x46, 0x26), MAKE_RGB(0x06, 0x2a, 0x80), MAKE_RGB(0x2a, 0x14, 0x9b), MAKE_RGB(0x0b, 0x49, 0x00),

	MAKE_RGB(0x00, 0x03, 0x02), MAKE_RGB(0x3d, 0x3d, 0x3d), MAKE_RGB(0x75, 0x1e, 0x20), MAKE_RGB(0x00, 0x50, 0x4f),
	MAKE_RGB(0x6a, 0x10, 0x78), MAKE_RGB(0x04, 0x5c, 0x00), MAKE_RGB(0x2a, 0x2a, 0xa3), MAKE_RGB(0x4c, 0x47, 0x00),
	MAKE_RGB(0x69, 0x2f, 0x00), MAKE_RGB(0x59, 0x38, 0x00), MAKE_RGB(0x26, 0x56, 0x00), MAKE_RGB(0x75, 0x15, 0x41),
	MAKE_RGB(0x00, 0x58, 0x3d), MAKE_RGB(0x15, 0x3d, 0x8f), MAKE_RGB(0x39, 0x22, 0xae), MAKE_RGB(0x19, 0x59, 0x00),

	MAKE_RGB(0x00, 0x03, 0x04), MAKE_RGB(0x42, 0x42, 0x42), MAKE_RGB(0x7b, 0x28, 0x20), MAKE_RGB(0x02, 0x56, 0x59),
	MAKE_RGB(0x6f, 0x1a, 0x82), MAKE_RGB(0x0a, 0x65, 0x09), MAKE_RGB(0x30, 0x34, 0xa7), MAKE_RGB(0x50, 0x51, 0x00),
	MAKE_RGB(0x6e, 0x36, 0x00), MAKE_RGB(0x65, 0x40, 0x00), MAKE_RGB(0x2c, 0x5c, 0x00), MAKE_RGB(0x7d, 0x1e, 0x45),
	MAKE_RGB(0x01, 0x61, 0x45), MAKE_RGB(0x1c, 0x45, 0x99), MAKE_RGB(0x42, 0x2d, 0xad), MAKE_RGB(0x1d, 0x62, 0x00),

	MAKE_RGB(0x05, 0x00, 0x02), MAKE_RGB(0x56, 0x55, 0x5a), MAKE_RGB(0x90, 0x3c, 0x3b), MAKE_RGB(0x17, 0x6d, 0x72),
	MAKE_RGB(0x87, 0x2d, 0x99), MAKE_RGB(0x1f, 0x7b, 0x15), MAKE_RGB(0x46, 0x49, 0xc1), MAKE_RGB(0x66, 0x63, 0x00),
	MAKE_RGB(0x84, 0x4c, 0x0d), MAKE_RGB(0x73, 0x55, 0x00), MAKE_RGB(0x40, 0x72, 0x00), MAKE_RGB(0x91, 0x33, 0x5e),
	MAKE_RGB(0x19, 0x74, 0x5c), MAKE_RGB(0x32, 0x59, 0xae), MAKE_RGB(0x59, 0x3f, 0xc3), MAKE_RGB(0x32, 0x76, 0x00),

	MAKE_RGB(0x02, 0x01, 0x06), MAKE_RGB(0x84, 0x7e, 0x85), MAKE_RGB(0xbb, 0x67, 0x68), MAKE_RGB(0x45, 0x96, 0x96),
	MAKE_RGB(0xaf, 0x58, 0xc3), MAKE_RGB(0x4a, 0xa7, 0x3e), MAKE_RGB(0x73, 0x73, 0xec), MAKE_RGB(0x92, 0x8d, 0x11),
	MAKE_RGB(0xaf, 0x78, 0x32), MAKE_RGB(0xa1, 0x80, 0x20), MAKE_RGB(0x6c, 0x9e, 0x12), MAKE_RGB(0xba, 0x5f, 0x89),
	MAKE_RGB(0x46, 0x9f, 0x83), MAKE_RGB(0x61, 0x85, 0xdd), MAKE_RGB(0x84, 0x6c, 0xef), MAKE_RGB(0x5d, 0xa3, 0x29),

	MAKE_RGB(0x02, 0x00, 0x0a), MAKE_RGB(0xb2, 0xac, 0xb3), MAKE_RGB(0xe9, 0x92, 0x92), MAKE_RGB(0x6c, 0xc3, 0xc1),
	MAKE_RGB(0xd9, 0x86, 0xf0), MAKE_RGB(0x79, 0xd1, 0x76), MAKE_RGB(0x9d, 0xa1, 0xff), MAKE_RGB(0xbd, 0xbe, 0x40),
	MAKE_RGB(0xdc, 0xa2, 0x61), MAKE_RGB(0xd1, 0xa9, 0x4c), MAKE_RGB(0x93, 0xc8, 0x3d), MAKE_RGB(0xe9, 0x8a, 0xb1),
	MAKE_RGB(0x6f, 0xcd, 0xab), MAKE_RGB(0x8a, 0xb4, 0xff), MAKE_RGB(0xb2, 0x9a, 0xff), MAKE_RGB(0x88, 0xcb, 0x59),

	MAKE_RGB(0x02, 0x00, 0x0a), MAKE_RGB(0xc7, 0xca, 0xc9), MAKE_RGB(0xff, 0xac, 0xac), MAKE_RGB(0x85, 0xd8, 0xe0),
	MAKE_RGB(0xf3, 0x9c, 0xff), MAKE_RGB(0x92, 0xea, 0x8a), MAKE_RGB(0xb7, 0xba, 0xff), MAKE_RGB(0xd6, 0xd3, 0x5b),
	MAKE_RGB(0xf3, 0xbe, 0x79), MAKE_RGB(0xe6, 0xc5, 0x65), MAKE_RGB(0xb0, 0xe0, 0x57), MAKE_RGB(0xff, 0xa4, 0xcf),
	MAKE_RGB(0x89, 0xe5, 0xc8), MAKE_RGB(0xa4, 0xca, 0xff), MAKE_RGB(0xca, 0xb3, 0xff), MAKE_RGB(0xa2, 0xe5, 0x7a),

	MAKE_RGB(0x01, 0x01, 0x01), MAKE_RGB(0xff, 0xff, 0xff), MAKE_RGB(0xff, 0xf6, 0xf2), MAKE_RGB(0xd1, 0xff, 0xff),
	MAKE_RGB(0xff, 0xe9, 0xff), MAKE_RGB(0xdb, 0xff, 0xd3), MAKE_RGB(0xfd, 0xff, 0xff), MAKE_RGB(0xff, 0xff, 0xa3),
	MAKE_RGB(0xff, 0xff, 0xc1), MAKE_RGB(0xff, 0xff, 0xb2), MAKE_RGB(0xfc, 0xff, 0xa2), MAKE_RGB(0xff, 0xee, 0xff),
	MAKE_RGB(0xd1, 0xff, 0xff), MAKE_RGB(0xeb, 0xff, 0xff), MAKE_RGB(0xff, 0xf8, 0xff), MAKE_RGB(0xed, 0xff, 0xbc)
};


#define NOISE_BUFFER_SIZE_SEC 5

#define TONE_ON         (!(m_reg[0x11] & 0x80))     /* or tone update!? */
#define TONE1_ON        ((m_reg[0x11] & 0x10))
#define TONE1_VALUE     (m_reg[0x0e] | ((m_reg[0x12] & 3) << 8))
#define TONE2_ON        ((m_reg[0x11] & 0x20))
#define TONE2_VALUE     (m_reg[0x0f] | ((m_reg[0x10] & 3) << 8))
#define VOLUME          (m_reg[0x11] & 0x0f)
#define NOISE_ON        (m_reg[0x11] & 0x40)

/*
 * pal 111860.781
 * ntsc 111840.45
 */
#define TONE_FREQUENCY(reg)         ((TED7360_CLOCK >> 3) / (1024 - reg))
#define TONE_FREQUENCY_MIN          (TONE_FREQUENCY(0))
#define NOISE_FREQUENCY             (TED7360_CLOCK / 8 / (1024 - TONE2_VALUE))
#define NOISE_FREQUENCY_MAX         (TED7360_CLOCK / 8)


//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
const device_type MOS7360 = &device_creator<mos7360_device>;


// default address maps
static ADDRESS_MAP_START( mos7360_videoram_map, AS_0, 8, mos7360_device )
	AM_RANGE(0x0000, 0xffff) AM_RAM
ADDRESS_MAP_END


//-------------------------------------------------
//  memory_space_config - return a description of
//  any address spaces owned by this device
//-------------------------------------------------

const address_space_config *mos7360_device::memory_space_config(address_spacenum spacenum) const
{
	switch (spacenum)
	{
		case AS_0: return &m_videoram_space_config;
		default: return NULL;
	}
}



//**************************************************************************
//  INLINE HELPERS
//**************************************************************************

inline void mos7360_device::set_interrupt(int mask)
{
	/* kernel itself polls for timer 2 shot (interrupt disabled!) when cassette loading */
	m_reg[9] |= mask;
	if ((m_reg[0xa] & m_reg[9] & 0x5e))
	{
		if (!(m_reg[9] & 0x80))
		{
			//DBG_LOG(1, "ted7360", ("irq start %.2x\n", mask));
			m_reg[9] |= 0x80;
			m_out_irq_func(ASSERT_LINE);
		}
	}
	m_reg[9] |= mask;
}

inline void mos7360_device::clear_interrupt(int mask)
{
	m_reg[9] &= ~mask;
	if ((m_reg[9] & 0x80) && !(m_reg[9] & m_reg[0xa] & 0x5e))
	{
		DBG_LOG(1, "ted7360", ("irq end %.2x\n", mask));
		m_reg[9] &= ~0x80;
		m_out_irq_func(CLEAR_LINE);
	}
}

inline int mos7360_device::rastercolumn()
{
	return (int) ((machine().time().as_double() - m_rastertime) * TED7360_VRETRACERATE * m_lines * 57 * 8 + 0.5);
}

inline UINT8 mos7360_device::read_ram(offs_t offset)
{
	int rom = m_rom;
	m_rom = 0;

	m_last_data = space(AS_0).read_byte(offset);

	m_rom = rom;

	return m_last_data;
}

inline UINT8 mos7360_device::read_rom(offs_t offset)
{
	int rom = m_rom;
	m_rom = 1;

	m_last_data = space(AS_0).read_byte(offset);

	m_rom = rom;

	return m_last_data;
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  mos7360_device - constructor
//-------------------------------------------------

mos7360_device::mos7360_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, MOS7360, "MOS7360", tag, owner, clock),
		device_memory_interface(mconfig, *this),
		device_sound_interface(mconfig, *this),
		m_videoram_space_config("videoram", ENDIANNESS_LITTLE, 8, 16, 0, NULL, *ADDRESS_MAP_NAME(mos7360_videoram_map)),
		m_stream(NULL)
{
}


//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void mos7360_device::device_config_complete()
{
	// inherit a copy of the static data
	const mos7360_interface *intf = reinterpret_cast<const mos7360_interface *>(static_config());
	if (intf != NULL)
		*static_cast<mos7360_interface *>(this) = *intf;

	// or initialize to defaults if none provided
	else
	{
		memset(&m_out_irq_cb, 0, sizeof(m_out_irq_cb));
		memset(&m_in_k_cb, 0, sizeof(m_in_k_cb));
	}
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void mos7360_device::device_start()
{
	// get the screen device
	m_screen = machine().device<screen_device>(m_screen_tag);
	assert(m_screen != NULL);

	// get the CPU device
	m_cpu = machine().device<cpu_device>(m_cpu_tag);
	assert(m_cpu != NULL);

	// resolve callbacks
	m_out_irq_func.resolve(m_out_irq_cb, *this);
	m_in_k_func.resolve(m_in_k_cb, *this);

	// allocate timers
	m_timer1 = timer_alloc(TIMER_ID_1);
	m_timer2 = timer_alloc(TIMER_ID_2);
	m_timer3 = timer_alloc(TIMER_ID_3);

	// allocate screen bitmap
	m_screen->register_screen_bitmap(m_bitmap);

	// create sound stream
	m_stream = machine().sound().stream_alloc(*this, 0, 1, machine().sample_rate());

	// buffer for fastest played sample for 5 second so we have enough data for min 5 second
	m_noisesize = NOISE_FREQUENCY_MAX * NOISE_BUFFER_SIZE_SEC;
	m_noise = auto_alloc_array(machine(), UINT8, m_noisesize);

	{
		int noiseshift = 0x7ffff8;
		UINT8 data;

		for (int i = 0; i < m_noisesize; i++)
		{
			data = 0;
			if (noiseshift & 0x400000)
				data |= 0x80;
			if (noiseshift & 0x100000)
				data |= 0x40;
			if (noiseshift & 0x010000)
				data |= 0x20;
			if (noiseshift & 0x002000)
				data |= 0x10;
			if (noiseshift & 0x000800)
				data |= 0x08;
			if (noiseshift & 0x000080)
				data |= 0x04;
			if (noiseshift & 0x000010)
				data |= 0x02;
			if (noiseshift & 0x000004)
				data |= 0x01;
			m_noise[i] = data;
			if (((noiseshift & 0x400000) == 0) != ((noiseshift & 0x002000) == 0))
				noiseshift = (noiseshift << 1) | 1;
			else
				noiseshift <<= 1;
		}
	}

	// register for state saving
	//save_item(NAME());
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void mos7360_device::device_reset()
{
	memset(m_reg, 0, ARRAY_LENGTH(m_reg));

	m_rom = 1;  // FIXME: at start should be RAM or ROM? old c16 code set it to ROM at init: is it correct?

	m_lines = TED7360_LINES;
	m_chargenaddr = m_bitmapaddr = m_videoaddr = 0;
	m_timer1_active = m_timer2_active = m_timer3_active = 0;
	m_cursor1 = 0;

	m_rasterline = 0;
	m_lastline = 0;

	m_rastertime = 0.0;

	m_frame_count = 0;

	m_x_begin = 0;
	m_x_end = 0;
	m_y_begin = 0;
	m_y_end = 0;

	memset(m_c16_bitmap, 0, ARRAY_LENGTH(m_c16_bitmap));
	memset(m_bitmapmulti, 0, ARRAY_LENGTH(m_bitmapmulti));
	memset(m_mono, 0, ARRAY_LENGTH(m_mono));
	memset(m_monoinversed, 0, ARRAY_LENGTH(m_monoinversed));
	memset(m_multi, 0, ARRAY_LENGTH(m_multi));
	memset(m_ecmcolor, 0, ARRAY_LENGTH(m_ecmcolor));
	memset(m_colors, 0, ARRAY_LENGTH(m_colors));

	m_tone1pos = 0;
	m_tone2pos = 0;
	m_tone1samples = 1;
	m_tone2samples = 1;
	m_noisepos = 0;
	m_noisesamples = 1;
}


//-------------------------------------------------
//  device_timer - handler timer events
//-------------------------------------------------

void mos7360_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_ID_1:
		// proved by digisound of several intros like eoroidpro
		m_timer1->adjust(TEDTIME_IN_CYCLES(TIMER1), 1);
		m_timer1_active = 1;
		set_interrupt(0x08);
		break;

	case TIMER_ID_2:
		m_timer2->adjust(TEDTIME_IN_CYCLES(0x10000), 2);
		m_timer2_active = 1;
		set_interrupt(0x10);
		break;

	case TIMER_ID_3:
		m_timer3->adjust(TEDTIME_IN_CYCLES(0x10000), 3);
		m_timer3_active = 1;
		set_interrupt(0x40);
		break;
	}
}


//-------------------------------------------------
//  sound_stream_update - handle update requests for
//  our sound stream
//-------------------------------------------------

void mos7360_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	int i, v, a;
	stream_sample_t *buffer = outputs[0];

	for (i = 0; i < samples; i++)
	{
		v = 0;

		if (TONE1_ON)
		{
			if (m_tone1pos <= m_tone1samples / 2 || !TONE_ON)
				v += 0x2ff; // depends on the volume between sound and noise

			m_tone1pos++;

			if (m_tone1pos > m_tone1samples)
				m_tone1pos = 0;
		}

		if (TONE2_ON || NOISE_ON )
		{
			if (TONE2_ON)
			{                          /*higher priority ?! */
				if (m_tone2pos <= m_tone2samples / 2 || !TONE_ON)
					v += 0x2ff;

				m_tone2pos++;

				if (m_tone2pos > m_tone2samples)
					m_tone2pos = 0;
			}
			else
			{
				v += m_noise[(int) ((double) m_noisepos * m_noisesize / m_noisesamples)];
				m_noisepos++;

				if ((double) m_noisepos / m_noisesamples >= 1.0)
					m_noisepos = 0;
			}
		}

		a = VOLUME;
		if (a > 8)
			a = 8;

		v = v * a;

		buffer[i] = v;
	}
}


void mos7360_device::draw_character(int ybegin, int yend, int ch, int yoff, int xoff, UINT16 *color)
{
	int y, code;

	for (y = ybegin; y <= yend; y++)
	{
		if (INROM)
			code = read_rom(m_chargenaddr + ch * 8 + y);
		else
			code = read_ram(m_chargenaddr + ch * 8 + y);

		m_bitmap.pix32(y + yoff, 0 + xoff) = PALETTE[color[code >> 7]];
		m_bitmap.pix32(y + yoff, 1 + xoff) = PALETTE[color[(code >> 6) & 1]];
		m_bitmap.pix32(y + yoff, 2 + xoff) = PALETTE[color[(code >> 5) & 1]];
		m_bitmap.pix32(y + yoff, 3 + xoff) = PALETTE[color[(code >> 4) & 1]];
		m_bitmap.pix32(y + yoff, 4 + xoff) = PALETTE[color[(code >> 3) & 1]];
		m_bitmap.pix32(y + yoff, 5 + xoff) = PALETTE[color[(code >> 2) & 1]];
		m_bitmap.pix32(y + yoff, 6 + xoff) = PALETTE[color[(code >> 1) & 1]];
		m_bitmap.pix32(y + yoff, 7 + xoff) = PALETTE[color[code & 1]];
	}
}

void mos7360_device::draw_character_multi(int ybegin, int yend, int ch, int yoff, int xoff)
{
	int y, code;

	for (y = ybegin; y <= yend; y++)
	{
		if (INROM)
			code = read_rom(m_chargenaddr + ch * 8 + y);
		else
			code = read_ram(m_chargenaddr + ch * 8 + y);

		m_bitmap.pix32(y + yoff, 0 + xoff) =
			m_bitmap.pix32(y + yoff, 1 + xoff) = PALETTE[m_multi[code >> 6]];
		m_bitmap.pix32(y + yoff, 2 + xoff) =
			m_bitmap.pix32(y + yoff, 3 + xoff) = PALETTE[m_multi[(code >> 4) & 3]];
		m_bitmap.pix32(y + yoff, 4 + xoff) =
			m_bitmap.pix32(y + yoff, 5 + xoff) = PALETTE[m_multi[(code >> 2) & 3]];
		m_bitmap.pix32(y + yoff, 6 + xoff) =
			m_bitmap.pix32(y + yoff, 7 + xoff) = PALETTE[m_multi[code & 3]];
	}
}

void mos7360_device::draw_bitmap(int ybegin, int yend, int ch, int yoff, int xoff)
{
	int y, code;

	for (y = ybegin; y <= yend; y++)
	{
		code = read_ram(m_bitmapaddr + ch * 8 + y);

		m_bitmap.pix32(y + yoff, 0 + xoff) = PALETTE[m_c16_bitmap[code >> 7]];
		m_bitmap.pix32(y + yoff, 1 + xoff) = PALETTE[m_c16_bitmap[(code >> 6) & 1]];
		m_bitmap.pix32(y + yoff, 2 + xoff) = PALETTE[m_c16_bitmap[(code >> 5) & 1]];
		m_bitmap.pix32(y + yoff, 3 + xoff) = PALETTE[m_c16_bitmap[(code >> 4) & 1]];
		m_bitmap.pix32(y + yoff, 4 + xoff) = PALETTE[m_c16_bitmap[(code >> 3) & 1]];
		m_bitmap.pix32(y + yoff, 5 + xoff) = PALETTE[m_c16_bitmap[(code >> 2) & 1]];
		m_bitmap.pix32(y + yoff, 6 + xoff) = PALETTE[m_c16_bitmap[(code >> 1) & 1]];
		m_bitmap.pix32(y + yoff, 7 + xoff) = PALETTE[m_c16_bitmap[code & 1]];
	}
}

void mos7360_device::draw_bitmap_multi(int ybegin, int yend, int ch, int yoff, int xoff)
{
	int y, code;

	for (y = ybegin; y <= yend; y++)
	{
		code = read_ram(m_bitmapaddr + ch * 8 + y);

		m_bitmap.pix32(y + yoff, 0 + xoff) =
			m_bitmap.pix32(y + yoff, 1 + xoff) = PALETTE[m_bitmapmulti[code >> 6]];
		m_bitmap.pix32(y + yoff, 2 + xoff) =
			m_bitmap.pix32(y + yoff, 3 + xoff) = PALETTE[m_bitmapmulti[(code >> 4) & 3]];
		m_bitmap.pix32(y + yoff, 4 + xoff) =
			m_bitmap.pix32(y + yoff, 5 + xoff) = PALETTE[m_bitmapmulti[(code >> 2) & 3]];
		m_bitmap.pix32(y + yoff, 6 + xoff) =
			m_bitmap.pix32(y + yoff, 7 + xoff) = PALETTE[m_bitmapmulti[code & 3]];
	}
}

void mos7360_device::draw_cursor(int ybegin, int yend, int yoff, int xoff, int color)
{
	int y;

	for (y = ybegin; y <= yend; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			m_bitmap.pix32(y + yoff, x + xoff) = PALETTE[color];
		}
	}
}

void mos7360_device::drawlines(int first, int last)
{
	int line, vline, end;
	int attr, ch, c1, c2, ecm;
	int offs, yoff, xoff, ybegin, yend, xbegin, xend;
	int i;

	m_lastline = last;

	/* top part of display not rastered */
	first -= TED7360_YPOS;
	last -= TED7360_YPOS;
	if ((first >= last) || (last <= 0))
		return;
	if (first < 0)
		first = 0;

	if (!SCREENON)
	{
		for (line = first; (line < last) && (line < m_bitmap.height()); line++)
		{
			for (int x = 0; x < m_bitmap.width(); x++)
			{
				m_bitmap.pix32(line, x) = PALETTE[0];
			}
		}
		return;
	}

	if (COLUMNS40)
		xbegin = XPOS, xend = xbegin + 320;
	else
		xbegin = XPOS + 7, xend = xbegin + 304;

	if (last < m_y_begin)
		end = last;
	else
		end = m_y_begin + YPOS;
	{
		for (line = first; line < end; line++)
		{
			for (int x = 0; x < m_bitmap.width(); x++)
			{
				m_bitmap.pix32(line, x) = PALETTE[FRAMECOLOR];
			}
		}
	}
	if (LINES25)
		vline = line - m_y_begin - YPOS;
	else
		vline = line - m_y_begin - YPOS + 8 - VERTICALPOS;

	if (last < m_y_end + YPOS)
		end = last;
	else
		end = m_y_end + YPOS;

	for (; line < end; vline = (vline + 8) & ~7, line = line + 1 + yend - ybegin)
	{
		offs = (vline >> 3) * 40;
		ybegin = vline & 7;
		yoff = line - ybegin;
		yend = (yoff + 7 < end) ? 7 : (end - yoff - 1);
		/* rendering 39 characters */
		/* left and right borders are overwritten later */

		for (xoff = m_x_begin + XPOS; xoff < m_x_end + XPOS; xoff += 8, offs++)
		{
			if (HIRESON)
			{
				ch = read_ram((m_videoaddr | 0x400) + offs);
				attr = read_ram(m_videoaddr + offs);
				c1 = ((ch >> 4) & 0xf) | (attr << 4);
				c2 = (ch & 0xf) | (attr & 0x70);
				m_bitmapmulti[1] = m_c16_bitmap[1] = c1 & 0x7f;
				m_bitmapmulti[2] = m_c16_bitmap[0] = c2 & 0x7f;
				if (MULTICOLORON)
				{
					draw_bitmap_multi(ybegin, yend, offs, yoff, xoff);
				}
				else
				{
					draw_bitmap(ybegin, yend, offs, yoff, xoff);
				}
			}
			else
			{
				ch = read_ram((m_videoaddr | 0x400) + offs);
				attr = read_ram(m_videoaddr + offs);
				// levente harsfalvi's docu says cursor off in ecm and multicolor
				if (ECMON)
				{
					// hardware reverse off
					ecm = ch >> 6;
					m_ecmcolor[0] = m_colors[ecm];
					m_ecmcolor[1] = attr & 0x7f;
					draw_character(ybegin, yend, ch & ~0xc0, yoff, xoff, m_ecmcolor);
				}
				else if (MULTICOLORON)
				{
					// hardware reverse off
					if (attr & 8)
					{
						m_multi[3] = attr & 0x77;
						draw_character_multi(ybegin, yend, ch, yoff, xoff);
					}
					else
					{
						m_mono[1] = attr & 0x7f;
						draw_character(ybegin, yend, ch, yoff, xoff, m_mono);
					}
				}
				else if (m_cursor1 && (offs == CURSOR1POS))
				{
					draw_cursor(ybegin, yend, yoff, xoff, attr & 0x7f);
				}
				else if (REVERSEON && (ch & 0x80))
				{
					m_monoinversed[0] = attr & 0x7f;
					if (m_cursor1 && (attr & 0x80))
						draw_cursor(ybegin, yend, yoff, xoff, m_monoinversed[0]);
					else
						draw_character(ybegin, yend, ch & ~0x80, yoff, xoff, m_monoinversed);
				}
				else
				{
					m_mono[1] = attr & 0x7f;
					if (m_cursor1 && (attr & 0x80))
						draw_cursor(ybegin, yend, yoff, xoff, m_mono[0]);
					else
						draw_character(ybegin, yend, ch, yoff, xoff, m_mono);
				}
			}
		}

		for (i = ybegin; i <= yend; i++)
		{
			for (int x = 0; x < xbegin; x++)
			{
				m_bitmap.pix32(yoff + i, x) = PALETTE[FRAMECOLOR];
			}

			for (int x = xend; x < m_bitmap.width(); x++)
			{
				m_bitmap.pix32(yoff + i, x) = PALETTE[FRAMECOLOR];
			}
		}
	}

	if (last < m_bitmap.height())
		end = last;
	else
		end = m_bitmap.height();

	for (; line < end; line++)
	{
		for (int x = 0; x < m_bitmap.width(); x++)
		{
			m_bitmap.pix32(line, x) = PALETTE[FRAMECOLOR];
		}
	}
}

void mos7360_device::soundport_w(int offset, int data)
{
	// int old = m_reg[offset];
	m_stream->update();

	switch (offset)
	{
	case 0x0e:
	case 0x12:
		if (offset == 0x12)
			m_reg[offset] = (m_reg[offset] & ~3) | (data & 3);
		else
			m_reg[offset] = data;

		m_tone1samples = machine().sample_rate() / TONE_FREQUENCY (TONE1_VALUE);
		DBG_LOG(1, "ted7360", ("tone1 %d %d sample:%d\n", TONE1_VALUE, TONE_FREQUENCY(TONE1_VALUE), m_tone1samples));
		break;

	case 0xf:
	case 0x10:
		m_reg[offset] = data;

		m_tone2samples = machine().sample_rate() / TONE_FREQUENCY (TONE2_VALUE);
		DBG_LOG (1, "ted7360", ("tone2 %d %d sample:%d\n", TONE2_VALUE, TONE_FREQUENCY(TONE2_VALUE), m_tone2samples));

		m_noisesamples = (int) ((double) NOISE_FREQUENCY_MAX * machine().sample_rate() * NOISE_BUFFER_SIZE_SEC / NOISE_FREQUENCY);
		DBG_LOG (1, "ted7360", ("noise %d sample:%d\n", NOISE_FREQUENCY, m_noisesamples));

		if (!NOISE_ON || ((double) m_noisepos / m_noisesamples >= 1.0))
			m_noisepos = 0;
		break;

	case 0x11:
		m_reg[offset] = data;
		DBG_LOG(1, "ted7360", ("%s volume %d, %s %s %s\n", TONE_ON?"on":"off",
						VOLUME, TONE1_ON?"tone1":"", TONE2_ON?"tone2":"", NOISE_ON?"noise":""));

		if (!TONE_ON||!TONE1_ON) m_tone1pos = 0;
		if (!TONE_ON||!TONE2_ON) m_tone2pos = 0;
		if (!TONE_ON||!NOISE_ON) m_noisepos = 0;
		break;
	}
}


//-------------------------------------------------
//  read - register read
//-------------------------------------------------

READ8_MEMBER( mos7360_device::read )
{
	int val = 0;

	switch (offset)
	{
	case 0:
		if (m_timer1)
			val = TEDTIME_TO_CYCLES(m_timer1->remaining()) & 0xff;
		else
			val = m_reg[offset];
		break;
	case 1:
		if (m_timer1)
			val = TEDTIME_TO_CYCLES(m_timer1->remaining()) >> 8;
		else
			val = m_reg[offset];
		break;
	case 2:
		if (m_timer2)
			val = TEDTIME_TO_CYCLES(m_timer2->remaining()) & 0xff;
		else
			val = m_reg[offset];
		break;
	case 3:
		if (m_timer2)
			val = TEDTIME_TO_CYCLES(m_timer2->remaining()) >> 8;
		else
			val = m_reg[offset];
		break;
	case 4:
		if (m_timer3)
			val = TEDTIME_TO_CYCLES(m_timer3->remaining()) & 0xff;
		else
			val = m_reg[offset];
		break;
	case 5:
		if (m_timer3)
			val = TEDTIME_TO_CYCLES(m_timer3->remaining()) >> 8;
		else
			val = m_reg[offset];
		break;
	case 7:
		val = (m_reg[offset] & ~0x40);
		if (m_clock == TED7360NTSC_CLOCK)
			val |= 0x40;
		break;
	case 9:
		val = m_reg[offset] | 1;
		break;
	case 0xa:
		val = m_reg[offset];
		break;
	case 0xb:
		val = m_reg[offset];
		break;
	case 0x0c:
		val = m_reg[offset] |= 0xfc;
		break;
	case 0x13:
		val = m_reg[offset] & ~1;
		if (m_rom)
			val |= 1;
		break;
	case 0x1c:                         /*rasterline */
		drawlines(m_lastline, m_rasterline);
		val = ((RASTERLINE_2_C16(m_rasterline) & 0x100) >> 8) | 0xfe;   /* expected by matrix */
		break;
	case 0x1d:                         /*rasterline */
		drawlines(m_lastline, m_rasterline);
		val = RASTERLINE_2_C16(m_rasterline) & 0xff;
		break;
	case 0x1e:                         /*rastercolumn */
		val = rastercolumn() / 2;   /* pengo >=0x99 */
		break;
	case 0x1f:
		val = ((m_rasterline & 7) << 4) | (m_reg[offset] & 0x0f);
		DBG_LOG(1, "port_w", ("read from cursorblink %.2x\n", val));
		break;
	default:
		val = m_reg[offset];
		break;
	}

	if ((offset != 8) && (offset >= 6) && (offset != 0x1c) && (offset != 0x1d) && (offset != 9) && ((offset < 0x15) || (offset > 0x19)))
	{
		DBG_LOG(1, "port_r", ("%.2x:%.2x\n", offset, val));
	}

	return val;
}


//-------------------------------------------------
//  write - register write
//-------------------------------------------------

WRITE8_MEMBER( mos7360_device::write )
{
	int old;

	if ((offset != 8) && ((offset < 0x15) || (offset > 0x19)))
	{
		DBG_LOG(1, "port_w", ("%.2x:%.2x\n", offset, data));
	}

	switch (offset)
	{
	case 0xe:
	case 0xf:
	case 0x10:
	case 0x11:
	case 0x12:
		soundport_w(offset, data);
		break;
	}
	switch (offset)
	{
	case 0:                        /* stop timer 1 */
		m_reg[offset] = data;

		if (m_timer1_active)
		{
			m_reg[1] = TEDTIME_TO_CYCLES(m_timer1->remaining()) >> 8;
			m_timer1->reset();
			m_timer1_active = 0;
		}
		break;
	case 1:                        /* start timer 1 */
		m_reg[offset] = data;
		m_timer1->adjust(TEDTIME_IN_CYCLES(TIMER1), 1);
		m_timer1_active = 1;
		break;
	case 2:                        /* stop timer 2 */
		m_reg[offset] = data;
		if (m_timer2_active)
		{
			m_reg[3] = TEDTIME_TO_CYCLES(m_timer2->remaining()) >> 8;
			m_timer2->reset();
			m_timer2_active = 0;
		}
		break;
	case 3:                        /* start timer 2 */
		m_reg[offset] = data;
		m_timer2->adjust(TEDTIME_IN_CYCLES(TIMER2), 2);
		m_timer2_active = 1;
		break;
	case 4:                        /* stop timer 3 */
		m_reg[offset] = data;
		if (m_timer3_active)
		{
			m_reg[5] = TEDTIME_TO_CYCLES(m_timer3->remaining()) >> 8;
			m_timer3->reset();
			m_timer3_active = 0;
		}
		break;
	case 5:                        /* start timer 3 */
		m_reg[offset] = data;
		m_timer3->adjust(TEDTIME_IN_CYCLES(TIMER3), 3);
		m_timer3_active = 1;
		break;
	case 6:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			if (LINES25)
			{
				m_y_begin = 0;
				m_y_end = m_y_begin + 200;
			}
			else
			{
				m_y_begin = 4;
				m_y_end = m_y_begin + 192;
			}
			m_chargenaddr = CHARGENADDR;
		}
		break;
	case 7:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			if (COLUMNS40)
			{
				m_x_begin = 0;
				m_x_end = m_x_begin + 320;
			}
			else
			{
				m_x_begin = HORICONTALPOS;
				m_x_end = m_x_begin + 320;
			}
			DBG_LOG(3, "port_w", ("%s %s\n", data & 0x40 ? "ntsc" : "pal", data & 0x20 ? "hori freeze" : ""));
			m_chargenaddr = CHARGENADDR;
		}
		break;
	case 8:
		m_reg[offset] = m_in_k_func(data);
		break;
	case 9:
		if (data & 0x08)
			clear_interrupt(8);
		if (data & 0x10)
			clear_interrupt(0x10);
		if (data & 0x40)
			clear_interrupt(0x40);
		if (data & 0x02)
			clear_interrupt(2);
		break;
	case 0xa:
		old = data;
		m_reg[offset] = data | 0xa0;
#if 0
		m_reg[9] = (m_reg[9] & 0xa1) | (m_reg[9] & data & 0x5e);
		if (m_reg[9] & 0x80)
			clear_interrupt(0);
#endif
		if ((data ^ old) & 1)
		{
			/* DBG_LOG(1,"set rasterline hi",("soll:%d\n",RASTERLINE)); */
		}
		break;
	case 0xb:
		if (data != m_reg[offset])
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			/*  DBG_LOG(1,"set rasterline lo",("soll:%d\n",RASTERLINE)); */
		}
		break;
	case 0xc:
	case 0xd:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
		}
		break;
	case 0x12:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_bitmapaddr = BITMAPADDR;
			m_chargenaddr = CHARGENADDR;
			DBG_LOG(3, "port_w", ("bitmap %.4x %s\n",  BITMAPADDR, INROM ? "rom" : "ram"));
		}
		break;
	case 0x13:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_chargenaddr = CHARGENADDR;
			DBG_LOG(3, "port_w", ("chargen %.4x %s %d\n", CHARGENADDR, data & 2 ? "" : "doubleclock", data & 1));
		}
		break;
	case 0x14:
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_videoaddr = VIDEOADDR;
			DBG_LOG(3, "port_w", ("videoram %.4x\n", VIDEOADDR));
		}
		break;
	case 0x15:                         /* backgroundcolor */
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_monoinversed[1] = m_mono[0] = m_bitmapmulti[0] = m_multi[0] = m_colors[0] = BACKGROUNDCOLOR;
		}
		break;
	case 0x16:                         /* foregroundcolor */
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_bitmapmulti[3] = m_multi[1] = m_colors[1] = FOREGROUNDCOLOR;
		}
		break;
	case 0x17:                         /* multicolor 1 */
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_multi[2] = m_colors[2] = MULTICOLOR1;
		}
		break;
	case 0x18:                         /* multicolor 2 */
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_colors[3] = MULTICOLOR2;
		}
		break;
	case 0x19:                         /* framecolor */
		if (m_reg[offset] != data)
		{
			drawlines(m_lastline, m_rasterline);
			m_reg[offset] = data;
			m_colors[4] = FRAMECOLOR;
		}
		break;
	case 0x1c:
		m_reg[offset] = data;          /*? */
		DBG_LOG(1, "port_w", ("write to rasterline high %.2x\n",
										data));
		break;
	case 0x1f:
		m_reg[offset] = data;
		DBG_LOG(1, "port_w", ("write to cursorblink %.2x\n", data));
		break;
	default:
		m_reg[offset] = data;
		break;
	}
}


//-------------------------------------------------
//  update_screen - update screen
//-------------------------------------------------

UINT32 mos7360_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);

	return 0;
}

WRITE_LINE_MEMBER( mos7360_device::rom_switch_w )
{
	m_rom = state;
}

READ_LINE_MEMBER( mos7360_device::rom_switch_r )
{
	return m_rom;
}

void mos7360_device::frame_interrupt_gen()
{
	if ((m_reg[0x1f] & 0xf) >= 0x0f)
	{
		/*  if (m_frame_count >= CURSORRATE) */
		m_cursor1 ^= 1;
		m_reg[0x1f] &= ~0xf;
		m_frame_count = 0;
	}
	else
		m_reg[0x1f]++;
}

void mos7360_device::raster_interrupt_gen()
{
	m_rasterline++;
	m_rastertime = machine().time().as_double();
	if (m_rasterline >= m_lines)
	{
		m_rasterline = 0;
		drawlines(m_lastline, TED7360_LINES);
		m_lastline = 0;
	}

	if (m_rasterline == C16_2_RASTERLINE(RASTERLINE))
	{
		drawlines(m_lastline, m_rasterline);
		set_interrupt(2);
	}
}


//-------------------------------------------------
//  bus_r - data bus read
//-------------------------------------------------

UINT8 mos7360_device::bus_r()
{
	return m_last_data;
}


//-------------------------------------------------
//  cs0_r - chip select 0 read
//-------------------------------------------------

int mos7360_device::cs0_r(offs_t offset)
{
	if (m_rom && offset >= 0x8000 && offset < 0xc000)
	{
		return 0;
	}

	return 1;
}


//-------------------------------------------------
//  cs0_r - chip select 1 read
//-------------------------------------------------

int mos7360_device::cs1_r(offs_t offset)
{
	if (m_rom && ((offset >= 0xc000 && offset < 0xfd00) || (offset >= 0xff20)))
	{
		return 0;
	}

	return 1;
}
