/***************************************************************************

    Renegade Video Hardware

***************************************************************************/

#include "emu.h"
#include "includes/renegade.h"


WRITE8_MEMBER(renegade_state::renegade_videoram_w)
{
	UINT8 *videoram = m_videoram;
	videoram[offset] = data;
	offset = offset % (64 * 16);
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(renegade_state::renegade_videoram2_w)
{
	m_videoram2[offset] = data;
	offset = offset % (32 * 32);
	m_fg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(renegade_state::renegade_flipscreen_w)
{
	flip_screen_set(~data & 0x01);
}

WRITE8_MEMBER(renegade_state::renegade_scroll0_w)
{
	m_scrollx = (m_scrollx & 0xff00) | data;
}

WRITE8_MEMBER(renegade_state::renegade_scroll1_w)
{
	m_scrollx = (m_scrollx & 0xff) | (data << 8);
}

TILE_GET_INFO_MEMBER(renegade_state::get_bg_tilemap_info)
{
	UINT8 *videoram = m_videoram;
	const UINT8 *source = &videoram[tile_index];
	UINT8 attributes = source[0x400]; /* CCC??BBB */
	SET_TILE_INFO_MEMBER(
		1 + (attributes & 0x7),
		source[0],
		attributes >> 5,
		0);
}

TILE_GET_INFO_MEMBER(renegade_state::get_fg_tilemap_info)
{
	const UINT8 *source = &m_videoram2[tile_index];
	UINT8 attributes = source[0x400];
	SET_TILE_INFO_MEMBER(
		0,
		(attributes & 3) * 256 + source[0],
		attributes >> 6,
		0);
}

void renegade_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(renegade_state::get_bg_tilemap_info),this), TILEMAP_SCAN_ROWS,      16, 16, 64, 16);
	m_fg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(renegade_state::get_fg_tilemap_info),this), TILEMAP_SCAN_ROWS,   8, 8, 32, 32);

	m_fg_tilemap->set_transparent_pen(0);
	m_bg_tilemap->set_scrolldx(256, 0);

	state_save_register_global(machine(), m_scrollx);
}

static void draw_sprites(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	renegade_state *state = machine.driver_data<renegade_state>();
	UINT8 *source = state->m_spriteram;
	UINT8 *finish = source + 96 * 4;

	while (source < finish)
	{
		int sy = 240 - source[0];

		if (sy >= 16)
		{
			int attributes = source[1]; /* SFCCBBBB */
			int sx = source[3];
			int sprite_number = source[2];
			int sprite_bank = 9 + (attributes & 0xf);
			int color = (attributes >> 4) & 0x3;
			int xflip = attributes & 0x40;

			if (sx > 248)
				sx -= 256;

			if (state->flip_screen())
			{
				sx = 240 - sx;
				sy = 240 - sy;
				xflip = !xflip;
			}

			if (attributes & 0x80) /* big sprite */
			{
				sprite_number &= ~1;
				drawgfx_transpen(bitmap, cliprect, machine.gfx[sprite_bank],
					sprite_number + 1,
					color,
					xflip, state->flip_screen(),
					sx, sy + (state->flip_screen() ? -16 : 16), 0);
			}
			else
			{
				sy += (state->flip_screen() ? -16 : 16);
			}
			drawgfx_transpen(bitmap, cliprect, machine.gfx[sprite_bank],
				sprite_number,
				color,
				xflip, state->flip_screen(),
				sx, sy, 0);
		}
		source += 4;
	}
}

UINT32 renegade_state::screen_update_renegade(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->set_scrollx(0, m_scrollx);
	m_bg_tilemap->draw(bitmap, cliprect, 0 , 0);
	draw_sprites(machine(), bitmap, cliprect);
	m_fg_tilemap->draw(bitmap, cliprect, 0 , 0);
	return 0;
}
