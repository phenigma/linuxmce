/***************************************************************************
    zx.c

    video hardware
    Juergen Buchmueller <pullmoll@t-online.de>, Dec 1999

    The ZX has a very unorthodox video RAM system.  To start a scanline,
    the CPU must jump to video RAM at 0xC000, which is a mirror of the
    RAM at 0x4000.  The video chip (ULA?) pulls a switcharoo and changes
    the video bytes as they are fetched by the CPU.

    The video chip draws the scanline until a HALT instruction (0x76) is
    reached, which indicates no further video RAM for this scanline.  Any
    other video byte is used to generate a tile and at the same time,
    appears to the CPU as a NOP (0x00) instruction.

****************************************************************************/

#include "includes/zx.h"

emu_timer *ula_nmi = NULL;
//emu_timer *ula_irq = NULL;
//int ula_nmi_active;
int ula_frame_vsync = 0;
//int ula_scancode_count = 0;
int ula_scanline_count = 0;

/*
 * Toggle the video output between black and white.
 * This happens whenever the ULA scanline IRQs are enabled/disabled.
 * Normally this is done during the synchronized zx_ula_r() function,
 * which outputs 8 pixels per code, but if the video sync is off
 * (during tape IO or sound output) zx_ula_bkgnd() is used to
 * simulate the display of a ZX80/ZX81.
 */
void zx_state::zx_ula_bkgnd(UINT8 color)
{
	screen_device *screen = machine().first_screen();
	int width = screen->width();
	int height = screen->height();
	const rectangle &visarea = screen->visible_area();

	if (m_ula_frame_vsync == 0 && color != m_old_c)
	{
		int y, new_x, new_y;
		rectangle r;
		bitmap_ind16 &bitmap = m_bitmap;

		new_y = machine().primary_screen->vpos();
		new_x = machine().primary_screen->hpos();
/*      logerror("zx_ula_bkgnd: %3d,%3d - %3d,%3d\n", state->m_old_x, state->m_old_y, new_x, new_y);*/
		y = m_old_y;
		for (;;)
		{
			if (y == new_y)
			{
				r.set(m_old_x, new_x, y, y);
				bitmap.fill(color, r);
				break;
			}
			else
			{
				r.set(m_old_x, visarea.max_x, y, y);
				bitmap.fill(color, r);
				m_old_x = 0;
			}
			if (++y == height)
				y = 0;
		}
		m_old_x = (new_x + 1) % width;
		m_old_y = new_y;
		m_old_c = color;
	}
}

/*
 * PAL:  310 total lines,
 *            0.. 55 vblank
 *           56..247 192 visible lines
 *          248..303 vblank
 *          304...   vsync
 * NTSC: 262 total lines
 *            0.. 31 vblank
 *           32..223 192 visible lines
 *          224..233 vblank
 */
TIMER_CALLBACK_MEMBER(zx_state::zx_ula_nmi)
{
	/*
	 * An NMI is issued on the ZX81 every 64us for the blanked
	 * scanlines at the top and bottom of the display.
	 */
	screen_device *screen = machine().first_screen();
	int height = screen->height();
	const rectangle& r1 = screen->visible_area();
	rectangle r;

	bitmap_ind16 &bitmap = m_bitmap;
	r.set(r1.min_x, r1.max_x, m_ula_scanline_count, m_ula_scanline_count);
	bitmap.fill(1, r);
//  logerror("ULA %3d[%d] NMI, R:$%02X, $%04x\n", machine().primary_screen->vpos(), ula_scancode_count, (unsigned) machine().device("maincpu")->state().state_int(Z80_R), (unsigned) machine().device("maincpu")->state().state_int(Z80_PC));
	machine().device("maincpu")->execute().set_input_line(INPUT_LINE_NMI, PULSE_LINE);
	if (++m_ula_scanline_count == height)
		m_ula_scanline_count = 0;
}

TIMER_CALLBACK_MEMBER(zx_state::zx_ula_irq)
{

	/*
	 * An IRQ is issued on the ZX80/81 whenever the R registers
	 * bit 6 goes low. In MESS this IRQ timed from the first read
	 * from the copy of the DFILE in the upper 32K in zx_ula_r().
	 */
	if (m_ula_irq_active)
	{
//      logerror("ULA %3d[%d] IRQ, R:$%02X, $%04x\n", machine().primary_screen->vpos(), ula_scancode_count, (unsigned) machine().device("maincpu")->state().state_int(Z80_R), (unsigned) machine().device("maincpu")->state().state_int(Z80_PC));

		m_ula_irq_active = 0;
		machine().device("maincpu")->execute().set_input_line(0, HOLD_LINE);
	}
}

void zx_ula_r(running_machine &machine, int offs, const char *region, const UINT8 param)
{
	zx_state *state = machine.driver_data<zx_state>();
	screen_device *screen = machine.first_screen();
	int offs0 = offs & 0x7fff;
	UINT8 *rom = machine.root_device().memregion("maincpu")->base();
	UINT8 chr = rom[offs0];

	if ((!state->m_ula_irq_active) && (chr == 0x76))
	{
		bitmap_ind16 &bitmap = state->m_bitmap;
		UINT16 y, *scanline;
		UINT16 ireg = machine.device("maincpu")->state().state_int(Z80_I) << 8;
		UINT8 data, *chrgen, creg;

		if (param)
			creg = machine.device("maincpu")->state().state_int(Z80_B);
		else
			creg = machine.device("maincpu")->state().state_int(Z80_C);

		chrgen = state->memregion(region)->base();

		if ((++state->m_ula_scanline_count == screen->height()) || (creg == 32))
		{
			state->m_ula_scanline_count = 0;
			state->m_offs1 = offs0;
		}

		state->m_ula_frame_vsync = 3;

		state->m_charline_ptr = 0;

		for (y = state->m_offs1+1; ((y < offs0) && (state->m_charline_ptr < ARRAY_LENGTH(state->m_charline))); y++)
		{
			state->m_charline[state->m_charline_ptr] = rom[y];
			state->m_charline_ptr++;
		}
		for (y = state->m_charline_ptr; y < ARRAY_LENGTH(state->m_charline); y++)
			state->m_charline[y] = 0;

		machine.scheduler().timer_set(machine.device<cpu_device>("maincpu")->cycles_to_attotime(((32 - state->m_charline_ptr) << 2)), timer_expired_delegate(FUNC(zx_state::zx_ula_irq),state));
		state->m_ula_irq_active++;

		scanline = &bitmap.pix16(state->m_ula_scanline_count);
		y = 0;

		for (state->m_charline_ptr = 0; state->m_charline_ptr < ARRAY_LENGTH(state->m_charline); state->m_charline_ptr++)
		{
			chr = state->m_charline[state->m_charline_ptr];
			data = chrgen[ireg | ((chr & 0x3f) << 3) | ((8 - creg)&7) ];
			if (chr & 0x80) data ^= 0xff;

			scanline[y++] = (data >> 7) & 1;
			scanline[y++] = (data >> 6) & 1;
			scanline[y++] = (data >> 5) & 1;
			scanline[y++] = (data >> 4) & 1;
			scanline[y++] = (data >> 3) & 1;
			scanline[y++] = (data >> 2) & 1;
			scanline[y++] = (data >> 1) & 1;
			scanline[y++] = (data >> 0) & 1;
			state->m_charline[state->m_charline_ptr] = 0;
		}

		if (creg == 1) state->m_offs1 = offs0;
	}
}

void zx_state::video_start()
{
	m_ula_nmi = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(zx_state::zx_ula_nmi),this));
	m_ula_irq_active = 0;
	machine().primary_screen->register_screen_bitmap(m_bitmap);
}

void zx_state::screen_eof_zx(screen_device &screen, bool state)
{
	// rising edge
	if (state)
	{
		/* decrement video synchronization counter */
		if (m_ula_frame_vsync)
			--m_ula_frame_vsync;
	}
}

UINT32 zx_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}
