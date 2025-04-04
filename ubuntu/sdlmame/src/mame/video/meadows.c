/***************************************************************************

    Meadows S2650 driver

****************************************************************************/

#include "emu.h"
#include "includes/meadows.h"

/* some constants to make life easier */
#define SPR_ADJUST_X    -18
#define SPR_ADJUST_Y    -14


/*************************************
 *
 *  Tilemap callbacks
 *
 *************************************/

TILE_GET_INFO_MEMBER(meadows_state::get_tile_info)
{
	UINT8 *videoram = m_videoram;
	SET_TILE_INFO_MEMBER(0, videoram[tile_index] & 0x7f, 0, 0);
}



/*************************************
 *
 *  Video startup
 *
 *************************************/

void meadows_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(meadows_state::get_tile_info),this), TILEMAP_SCAN_ROWS,  8,8, 32,30);
}



/*************************************
 *
 *  Video RAM write
 *
 *************************************/

WRITE8_MEMBER(meadows_state::meadows_videoram_w)
{
	UINT8 *videoram = m_videoram;
	videoram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}



/*************************************
 *
 *  Sprite RAM write
 *
 *************************************/

WRITE8_MEMBER(meadows_state::meadows_spriteram_w)
{
	machine().primary_screen->update_now();
	m_spriteram[offset] = data;
}



/*************************************
 *
 *  Sprite rendering
 *
 *************************************/

static void draw_sprites(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &clip)
{
	meadows_state *state = machine.driver_data<meadows_state>();
	UINT8 *spriteram = state->m_spriteram;
	int i;

	for (i = 0; i < 4; i++)
	{
		int x = spriteram[i+0] + SPR_ADJUST_X;
		int y = spriteram[i+4] + SPR_ADJUST_Y;
		int code = spriteram[i+8] & 0x0f;       /* bit #0 .. #3 select sprite */
/*      int bank = (spriteram[i+8] >> 4) & 1;      bit #4 selects prom ???    */
		int bank = i;                           /* that fixes it for now :-/ */
		int flip = spriteram[i+8] >> 5;         /* bit #5 flip vertical flag */

		drawgfx_transpen(bitmap, clip, machine.gfx[bank + 1], code, 0, flip, 0, x, y, 0);
	}
}



/*************************************
 *
 *  Primary video update
 *
 *************************************/

UINT32 meadows_state::screen_update_meadows(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	/* draw the background */
	m_bg_tilemap->draw(bitmap, cliprect, 0, 0);

	/* draw the sprites */
	if (machine().gfx[1])
		draw_sprites(machine(), bitmap, cliprect);
	return 0;
}
