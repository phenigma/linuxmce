/******************************************************************************
 watara supervision handheld

 PeT mess@utanet.at in december 2000
******************************************************************************/

#include <assert.h>
#include "emu.h"
#include "cpu/m6502/m65c02.h"

#include "includes/svision.h"
#include "imagedev/cartslot.h"
#include "svision.lh"

#define MAKE8_RGB32(red3, green3, blue2) ( ( (red3)<<(16+5)) | ( (green3)<<(8+5)) | ( (blue2)<<(0+6)) )
#define MAKE9_RGB32(red3, green3, blue3) ( ( (red3)<<(16+5)) | ( (green3)<<(8+5)) | ( (blue3)<<(0+5)) )
#define MAKE12_RGB32(red4, green4, blue4) ( ( (red4)<<(16+4)) | ((green4)<<(8+4)) | ((blue4)<<(0+4)) )
#define MAKE24_RGB32(red8, green8, blue8) ( (((red8)&0xf8)<<16) | (((green8)&0xf8)<<8) | (((blue8)&0xf8)) )


// in pixel
#define XSIZE (state->m_reg[0]&~3)
#define XPOS state->m_reg[2]
#define YPOS state->m_reg[3]
#define BANK m_reg[0x26]

TIMER_CALLBACK_MEMBER(svision_state::svision_pet_timer)
{
	switch (m_pet.state)
	{
		case 0:
			m_pet.input = machine().root_device().ioport("JOY2")->read();
			/* fall through */

		case 2: case 4: case 6: case 8:
		case 10: case 12: case 14:
			m_pet.clock=m_pet.state&2;
			m_pet.data=m_pet.input&1;
			m_pet.input>>=1;
			m_pet.state++;
			break;

		case 16+15:
			m_pet.state = 0;
			break;

		default:
			m_pet.state++;
			break;
	}
}

TIMER_DEVICE_CALLBACK_MEMBER(svision_state::svision_pet_timer_dev)
{
	svision_pet_timer(ptr,param);
}

void svision_irq(running_machine &machine)
{
	svision_state *state = machine.driver_data<svision_state>();
	int irq = state->m_svision.timer_shot && (state->BANK & 2);
	irq = irq || (*state->m_dma_finished && (state->BANK & 4));

	machine.device("maincpu")->execute().set_input_line(M65C02_IRQ_LINE, irq ? ASSERT_LINE : CLEAR_LINE);
}

TIMER_CALLBACK_MEMBER(svision_state::svision_timer)
{
	m_svision.timer_shot = TRUE;
	m_svision.timer1->enable(FALSE);
	svision_irq( machine() );
}

READ8_MEMBER(svision_state::svision_r)
{
	int data = m_reg[offset];
	switch (offset)
	{
		case 0x20:
			data = ioport("JOY")->read();
			break;
		case 0x21:
			data &= ~0xf;
			data |= m_reg[0x22] & 0xf;
			if (m_pet.on)
			{
				if (!m_pet.clock)
					data &= ~4;
				if (!m_pet.data)
					data &= ~8;
			}
			break;
		case 0x27:
			data &= ~3;
			if (m_svision.timer_shot)
				data|=1;
			if (*m_dma_finished)
				data|=2;
			break;
		case 0x24:
			m_svision.timer_shot = FALSE;
			svision_irq(machine());
			break;
		case 0x25:
			*m_dma_finished = FALSE;
			svision_irq(machine());
			break;
		default:
			logerror("%.6f svision read %04x %02x\n", machine().time().as_double(),offset,data);
			break;
	}

	return data;
}

WRITE8_MEMBER(svision_state::svision_w)
{
	int value;
	int delay;

	m_reg[offset] = data;

	switch (offset)
	{
		case 2:
		case 3:
			break;
		case 0x26: /* bits 5,6 memory management for a000? */
			logerror("%.6f svision write %04x %02x\n", machine().time().as_double(),offset,data);
			membank("bank1")->set_base(machine().root_device().memregion("user1")->base() + ((m_reg[0x26] & 0xe0) << 9));
			svision_irq(machine());
			break;
		case 0x23: /* delta hero irq routine write */
			value = data;
			if (!data)
				value = 0x100;
			if (BANK & 0x10)
				delay = 16384;
			else
				delay = 256;
			m_svision.timer1->enable(TRUE);
			m_svision.timer1->reset(machine().device<cpu_device>("maincpu")->cycles_to_attotime(value * delay));
			break;
		case 0x10: case 0x11: case 0x12: case 0x13:
			svision_soundport_w(m_sound, 0, offset & 3, data);
			break;
		case 0x14: case 0x15: case 0x16: case 0x17:
			svision_soundport_w(m_sound, 1, offset & 3, data);
			break;
		case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c:
			svision_sounddma_w(m_sound, space, offset - 0x18, data);
			break;
		case 0x28: case 0x29: case 0x2a:
			svision_noise_w(m_sound, space, offset - 0x28, data);
			break;
		default:
			logerror("%.6f svision write %04x %02x\n", machine().time().as_double(), offset, data);
			break;
	}
}

READ8_MEMBER(svision_state::tvlink_r)
{
	switch(offset)
	{
		default:
			if (offset >= 0x800 && offset < 0x840)
			{
				/* strange effects when modifying palette */
				return svision_r(space, offset);
			}
			else
			{
				return svision_r(space, offset);
			}
	}
}

WRITE8_MEMBER(svision_state::tvlink_w)
{
	switch (offset)
	{
		case 0x0e:
			m_reg[offset] = data;
			m_tvlink.palette_on = data & 1;
			if (m_tvlink.palette_on)
			{
				// hack, normally initialising with palette from ram
				m_tvlink.palette[0] = MAKE12_RGB32(163/16,172/16,115/16); // these are the tron colors messured from screenshot
				m_tvlink.palette[1] = MAKE12_RGB32(163/16,155/16,153/16);
				m_tvlink.palette[2] = MAKE12_RGB32(77/16,125/16,73/16);
				m_tvlink.palette[3] = MAKE12_RGB32(59/16,24/16,20/16);
			}
			else
			{
				// cleaner to use colors from compile time palette, or compose from "fixed" palette values
				m_tvlink.palette[0]=MAKE12_RGB32(0,0,0);
				m_tvlink.palette[1]=MAKE12_RGB32(5*16/256,18*16/256,9*16/256);
				m_tvlink.palette[2]=MAKE12_RGB32(48*16/256,76*16/256,100*16/256);
				m_tvlink.palette[3]=MAKE12_RGB32(190*16/256,190*16/256,190*16/256);
			}
			break;
		default:
			svision_w(space, offset,data);
			if (offset >= 0x800 && offset < 0x840)
			{
				UINT16 c;
				if (offset == 0x803 && data == 0x07)
				{
					/* tron hack */
					m_reg[0x0804]=0x00;
					m_reg[0x0805]=0x01;
					m_reg[0x0806]=0x00;
					m_reg[0x0807]=0x00;
				}
				c = m_reg[0x800] | (m_reg[0x804] << 8);
				m_tvlink.palette[0] = MAKE9_RGB32( (c>>0)&7, (c>>3)&7, (c>>6)&7);
				c = m_reg[0x801] | (m_reg[0x805] << 8);
				m_tvlink.palette[1] = MAKE9_RGB32( (c>>0)&7, (c>>3)&7, (c>>6)&7);
				c = m_reg[0x802] | (m_reg[0x806]<<8);
				m_tvlink.palette[2]=MAKE9_RGB32( (c>>0)&7, (c>>3)&7, (c>>6)&7);
				c = m_reg[0x803] | (m_reg[0x807]<<8);
				m_tvlink.palette[3]=MAKE9_RGB32( (c>>0)&7, (c>>3)&7, (c>>6)&7);
				/* writes to palette effect video color immediately */
				/* some writes modify other registers, */
				/* encoding therefor not known (rgb8 or rgb9) */
			}
	}
}

static ADDRESS_MAP_START( svision_mem , AS_PROGRAM, 8, svision_state )
	AM_RANGE( 0x0000, 0x1fff) AM_RAM
	AM_RANGE( 0x2000, 0x3fff) AM_READWRITE(svision_r, svision_w) AM_SHARE("reg")
	AM_RANGE( 0x4000, 0x5fff) AM_RAM AM_SHARE("videoram")
	AM_RANGE( 0x6000, 0x7fff) AM_NOP
	AM_RANGE( 0x8000, 0xbfff) AM_ROMBANK("bank1")
	AM_RANGE( 0xc000, 0xffff) AM_ROMBANK("bank2")
ADDRESS_MAP_END

static ADDRESS_MAP_START( tvlink_mem , AS_PROGRAM, 8, svision_state )
	AM_RANGE( 0x0000, 0x1fff) AM_RAM
	AM_RANGE( 0x2000, 0x3fff) AM_READWRITE(tvlink_r, tvlink_w) AM_SHARE("reg")
	AM_RANGE( 0x4000, 0x5fff) AM_RAM AM_SHARE("videoram")
	AM_RANGE( 0x6000, 0x7fff) AM_NOP
	AM_RANGE( 0x8000, 0xbfff) AM_ROMBANK("bank1")
	AM_RANGE( 0xc000, 0xffff) AM_ROMBANK("bank2")
ADDRESS_MAP_END

static INPUT_PORTS_START( svision )
	PORT_START("JOY")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP   )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_NAME("B")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_NAME("A")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SELECT) PORT_NAME("Select")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START) PORT_NAME("Start/Pause")
INPUT_PORTS_END

static INPUT_PORTS_START( svisions )
	PORT_START("JOY")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP   ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_NAME("B") PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_NAME("A") PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SELECT) PORT_NAME("Select") PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START) PORT_NAME("Start/Pause") PORT_PLAYER(1)
	PORT_START("JOY2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP   ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_NAME("2nd B") PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_NAME("2nd A") PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SELECT) PORT_NAME("2nd Select") PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START) PORT_NAME("2nd Start/Pause") PORT_PLAYER(2)
INPUT_PORTS_END

/* most games contain their graphics in roms, and have hardware to
   draw complete rectangular objects */

#define PALETTE_START 0

/* palette in red, green, blue triples */
static const unsigned char svision_palette[] =
{
#if 0
	/* greens grabbed from a scan of a handheld
	 * in its best adjustment for contrast
	 */
	86, 121, 86,
	81, 115, 90,
	74, 107, 101,
	54, 78, 85
#else
	/* grabbed from chris covell's black white pics */
	0xe0, 0xe0, 0xe0,
	0xb9, 0xb9, 0xb9,
	0x54, 0x54, 0x54,
	0x12, 0x12, 0x12
#endif
};

/* palette in red, green, blue tribles */
static const unsigned char svisionp_palette[] =
{
	// pal
	1, 1, 3,
	5, 18, 9,
	48, 76, 100,
	190, 190, 190
};

/* palette in red, green, blue tribles */
static const unsigned char svisionn_palette[] =
{
	0, 0, 0,
	188, 242, 244, // darker
	129, 204, 255,
	245, 249, 248
};

void svision_state::palette_init()
{
	int i;

	for( i = 0; i < sizeof(svision_palette) / 3; i++ ) {
		palette_set_color_rgb(machine(), i, svision_palette[i*3], svision_palette[i*3+1], svision_palette[i*3+2] );
	}
}
PALETTE_INIT_MEMBER(svision_state,svisionn)
{
	int i;

	for ( i = 0; i < sizeof(svisionn_palette) / 3; i++ ) {
		palette_set_color_rgb(machine(), i, svisionn_palette[i*3], svisionn_palette[i*3+1], svisionn_palette[i*3+2] );
	}
}
PALETTE_INIT_MEMBER(svision_state,svisionp)
{
	int i;

	for ( i = 0; i < sizeof(svisionn_palette) / 3; i++ ) {
		palette_set_color_rgb(machine(), i, svisionp_palette[i*3], svisionp_palette[i*3+1], svisionp_palette[i*3+2] );
	}
}

UINT32 svision_state::screen_update_svision(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	svision_state *state = machine().driver_data<svision_state>();
	int x, y, i, j=XPOS/4+YPOS*0x30;
	UINT8 *videoram = m_videoram;

	if (BANK&8)
	{
		for (y=0; y<160; y++)
		{
			UINT16 *line = &bitmap.pix16(y, 3 - (XPOS & 3));
			for (x=3-(XPOS&3),i=0; x<160+3 && x<XSIZE+3; x+=4,i++)
			{
				UINT8 b=videoram[j+i];
				line[3]=((b>>6)&3)+PALETTE_START;
				line[2]=((b>>4)&3)+PALETTE_START;
				line[1]=((b>>2)&3)+PALETTE_START;
				line[0]=((b>>0)&3)+PALETTE_START;
				line+=4;
			}
			j += 0x30;
			if (j >= 8160)
				j = 0; //sssnake
		}
	}
	else
	{
		bitmap.plot_box(3, 0, 162, 159, PALETTE_START);
	}
	return 0;
}

UINT32 svision_state::screen_update_tvlink(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	svision_state *state = machine().driver_data<svision_state>();
	int x, y, i, j = XPOS/4+YPOS*0x30;
	UINT8 *videoram = m_videoram;

	if (BANK & 8)
	{
		for (y = 0; y < 160; y++)
		{
			UINT32 *line = &bitmap.pix32(y, 3 - (XPOS & 3));
			for (x = 3 - (XPOS & 3), i = 0; x < 160 + 3 && x < XSIZE + 3; x += 4, i++)
			{
				UINT8 b=videoram[j+i];
				line[3]=m_tvlink.palette[(b>>6)&3];
				line[2]=m_tvlink.palette[(b>>4)&3];
				line[1]=m_tvlink.palette[(b>>2)&3];
				line[0]=m_tvlink.palette[(b>>0)&3];
				line+=4;
			}
			j += 0x30;
			if (j >= 8160)
				j = 0; //sssnake
		}
	}
	else
	{
		bitmap.plot_box(3, 0, 162, 159, machine().pens[PALETTE_START]);
	}
	return 0;
}

INTERRUPT_GEN_MEMBER(svision_state::svision_frame_int)
{
	if (BANK&1)
		device.execute().set_input_line(INPUT_LINE_NMI, PULSE_LINE);

	svision_sound_decrement(m_sound);
}

DRIVER_INIT_MEMBER(svision_state,svision)
{
	m_svision.timer1 = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(svision_state::svision_timer),this));
	m_sound = machine().device("custom");
	m_dma_finished = svision_dma_finished(m_sound);
	m_pet.on = FALSE;
	membank("bank2")->set_base(memregion("user1")->base() + 0x1c000);
}

DRIVER_INIT_MEMBER(svision_state,svisions)
{
	m_svision.timer1 = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(svision_state::svision_timer),this));
	m_sound = machine().device("custom");
	m_dma_finished = svision_dma_finished(m_sound);
	membank("bank2")->set_base(memregion("user1")->base() + 0x1c000);
	m_svision.timer1 = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(svision_state::svision_timer),this));
	m_pet.on = TRUE;
	m_pet.timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(svision_state::svision_pet_timer),this));
}

static DEVICE_IMAGE_LOAD( svision_cart )
{
	UINT32 size;
	UINT8 *temp_copy;
	int mirror, i;

	if (image.software_entry() == NULL)
	{
		size = image.length();
		temp_copy = auto_alloc_array(image.device().machine(), UINT8, size);

		if (size > image.device().machine().root_device().memregion("user1")->bytes())
		{
			image.seterror(IMAGE_ERROR_UNSPECIFIED, "Unsupported cartridge size");
			auto_free(image.device().machine(), temp_copy);
			return IMAGE_INIT_FAIL;
		}

		if (image.fread( temp_copy, size) != size)
		{
			image.seterror(IMAGE_ERROR_UNSPECIFIED, "Unable to fully read from file");
			auto_free(image.device().machine(), temp_copy);
			return IMAGE_INIT_FAIL;
		}
	}
	else
	{
		size = image.get_software_region_length("rom");
		temp_copy = auto_alloc_array(image.device().machine(), UINT8, size);
		memcpy(temp_copy, image.get_software_region("rom"), size);
	}

	mirror = image.device().machine().root_device().memregion("user1")->bytes() / size;

	/* With the following, we mirror the cart in the whole "user1" memory region */
	for (i = 0; i < mirror; i++)
		memcpy(image.device().machine().root_device().memregion("user1")->base() + i * size, temp_copy, size);

	auto_free(image.device().machine(), temp_copy);

	return IMAGE_INIT_PASS;
}

void svision_state::machine_reset()
{
	m_svision.timer_shot = FALSE;
	*m_dma_finished = FALSE;
	membank("bank1")->set_base(memregion("user1")->base());
}


MACHINE_RESET_MEMBER(svision_state,tvlink)
{
	m_svision.timer_shot = FALSE;
	*m_dma_finished = FALSE;
	membank("bank1")->set_base(memregion("user1")->base());
	m_tvlink.palette_on = FALSE;

	memset(m_reg + 0x800, 0xff, 0x40); // normally done from m_tvlink microcontroller
	m_reg[0x82a] = 0xdf;

	m_tvlink.palette[0] = MAKE24_RGB32(svisionp_palette[(PALETTE_START+0)*3+0], svisionp_palette[(PALETTE_START+0)*3+1], svisionp_palette[(PALETTE_START+0)*3+2]);
	m_tvlink.palette[1] = MAKE24_RGB32(svisionp_palette[(PALETTE_START+1)*3+0], svisionp_palette[(PALETTE_START+1)*3+1], svisionp_palette[(PALETTE_START+1)*3+2]);
	m_tvlink.palette[2] = MAKE24_RGB32(svisionp_palette[(PALETTE_START+2)*3+0], svisionp_palette[(PALETTE_START+2)*3+1], svisionp_palette[(PALETTE_START+2)*3+2]);
	m_tvlink.palette[3] = MAKE24_RGB32(svisionp_palette[(PALETTE_START+3)*3+0], svisionp_palette[(PALETTE_START+3)*3+1], svisionp_palette[(PALETTE_START+3)*3+2]);
}

static MACHINE_CONFIG_START( svision, svision_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M65C02, 4000000)        /* ? stz used! speed? */
	MCFG_CPU_PROGRAM_MAP(svision_mem)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", svision_state,  svision_frame_int)


	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(61)
	MCFG_SCREEN_SIZE(3+160+3, 160)
	MCFG_SCREEN_VISIBLE_AREA(3+0, 3+160-1, 0, 160-1)
	MCFG_SCREEN_UPDATE_DRIVER(svision_state, screen_update_svision)

	MCFG_PALETTE_LENGTH(ARRAY_LENGTH(svision_palette) * 3)

	MCFG_DEFAULT_LAYOUT(layout_svision)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("custom", SVISION, 0)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.50)

	/* cartridge */
	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("bin,ws,sv")
	MCFG_CARTSLOT_MANDATORY
	MCFG_CARTSLOT_INTERFACE("svision_cart")
	MCFG_CARTSLOT_LOAD(svision_cart)

	/* Software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list","svision")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svisions, svision )
	MCFG_TIMER_DRIVER_ADD_PERIODIC("pet_timer", svision_state, svision_pet_timer_dev, attotime::from_seconds(8))
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svisionp, svision )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(4430000)
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_PALETTE_INIT_OVERRIDE(svision_state, svisionp )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svisionn, svision )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(3560000/*?*/)
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_PALETTE_INIT_OVERRIDE(svision_state, svisionn )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( tvlinkp, svisionp )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(tvlink_mem)

	MCFG_MACHINE_RESET_OVERRIDE(svision_state, tvlink )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_UPDATE_DRIVER(svision_state, screen_update_tvlink)

MACHINE_CONFIG_END

ROM_START(svision)
	ROM_REGION(0x20000, "user1", ROMREGION_ERASE00)
ROM_END


#define rom_svisions rom_svision
#define rom_svisionn rom_svision
#define rom_svisionp rom_svision
#define rom_tvlinkp rom_svision

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME        PARENT  COMPAT  MACHINE     INPUT       INIT             COMPANY     FULLNAME */
// marketed under a ton of firms and names
CONS(1992,  svision,    0,  0,  svision,    svision, svision_state, svision,    "Watara",   "Super Vision", 0)
// svdual 2 connected via communication port
CONS( 1992, svisions,      svision,          0,svisions,  svisions, svision_state,    svisions,   "Watara", "Super Vision (PeT Communication Simulation)", 0 )

CONS( 1993, svisionp,      svision,          0,svisionp,  svision, svision_state,    svision,   "Watara", "Super Vision (PAL TV Link Colored)", 0 )
CONS( 1993, svisionn,      svision,          0,svisionn,  svision, svision_state,    svision,   "Watara", "Super Vision (NTSC TV Link Colored)", 0 )
// svtvlink (2 supervisions)
// tvlink (pad supervision simulated)
CONS( 199?, tvlinkp,      svision,          0,tvlinkp,  svision, svision_state,    svision,   "Watara", "TV Link PAL", 0 )
