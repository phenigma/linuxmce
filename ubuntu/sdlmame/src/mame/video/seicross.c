/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/seicross.h"

/***************************************************************************

  Convert the color PROMs into a more useable format.

  Seicross has two 32x8 palette PROMs, connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/
void seicross_state::palette_init()
{
	const UINT8 *color_prom = machine().root_device().memregion("proms")->base();
	int i;


	for (i = 0;i < machine().total_colors();i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = (color_prom[i] >> 6) & 0x01;
		bit1 = (color_prom[i] >> 7) & 0x01;
		b = 0x4f * bit0 + 0xa8 * bit1;

		palette_set_color(machine(),i,MAKE_RGB(r,g,b));
	}
}

WRITE8_MEMBER(seicross_state::seicross_videoram_w)
{

	m_videoram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(seicross_state::seicross_colorram_w)
{
	/* bit 5 of the address is not used for color memory. There is just */
	/* 512k of memory; every two consecutive rows share the same memory */
	/* region. */

	offset &= 0xffdf;

	m_colorram[offset] = data;
	m_colorram[offset + 0x20] = data;

	m_bg_tilemap->mark_tile_dirty(offset);
	m_bg_tilemap->mark_tile_dirty(offset + 0x20);
}

TILE_GET_INFO_MEMBER(seicross_state::get_bg_tile_info)
{
	int code = m_videoram[tile_index] + ((m_colorram[tile_index] & 0x10) << 4);
	int color = m_colorram[tile_index] & 0x0f;
	int flags = ((m_colorram[tile_index] & 0x40) ? TILE_FLIPX : 0) | ((m_colorram[tile_index] & 0x80) ? TILE_FLIPY : 0);

	SET_TILE_INFO_MEMBER(0, code, color, flags);
}

void seicross_state::video_start()
{

	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(seicross_state::get_bg_tile_info),this), TILEMAP_SCAN_ROWS,
			8, 8, 32, 32);

	m_bg_tilemap->set_scroll_cols(32);
}

static void draw_sprites(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	seicross_state *state = machine.driver_data<seicross_state>();
	UINT8 *spriteram = state->m_spriteram;
	UINT8 *spriteram_2 = state->m_spriteram2;
	int offs;

	for (offs = state->m_spriteram.bytes() - 4; offs >= 0; offs -= 4)
	{
		int x = spriteram[offs + 3];
		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				(spriteram[offs] & 0x3f) + ((spriteram[offs + 1] & 0x10) << 2) + 128,
				spriteram[offs + 1] & 0x0f,
				spriteram[offs] & 0x40,spriteram[offs] & 0x80,
				x,240-spriteram[offs + 2],0);
		if(x>0xf0)
			drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
					(spriteram[offs] & 0x3f) + ((spriteram[offs + 1] & 0x10) << 2) + 128,
					spriteram[offs + 1] & 0x0f,
					spriteram[offs] & 0x40,spriteram[offs] & 0x80,
					x-256,240-spriteram[offs + 2],0);
	}

	for (offs = state->m_spriteram2.bytes() - 4; offs >= 0; offs -= 4)
	{
		int x = spriteram_2[offs + 3];
		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				(spriteram_2[offs] & 0x3f) + ((spriteram_2[offs + 1] & 0x10) << 2),
				spriteram_2[offs + 1] & 0x0f,
				spriteram_2[offs] & 0x40,spriteram_2[offs] & 0x80,
				x,240-spriteram_2[offs + 2],0);
		if(x>0xf0)
			drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
					(spriteram_2[offs] & 0x3f) + ((spriteram_2[offs + 1] & 0x10) << 2),
					spriteram_2[offs + 1] & 0x0f,
					spriteram_2[offs] & 0x40,spriteram_2[offs] & 0x80,
					x-256,240-spriteram_2[offs + 2],0);
	}
}

UINT32 seicross_state::screen_update_seicross(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int col;

	for (col = 0; col < 32; col++)
		m_bg_tilemap->set_scrolly(col, m_row_scroll[col]);

	m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	draw_sprites(machine(), bitmap, cliprect);
	return 0;
}
