	/***************************************************************************

	gba.c

	File to handle emulation of the video hardware of the Game Boy Advance

	By R. Belmont, MooglyGuy, Harmony

***************************************************************************/

#include "emu.h"
#include "includes/gba.h"

#define VERBOSE_LEVEL   (0)

INLINE void verboselog(running_machine &machine, int n_level, const char *s_fmt, ...)
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%08x: %s", machine.device("maincpu")->safe_pc(), buf );
	}
}

static const int coeff[32] = {
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};

/* Drawing functions */
static void draw_roz_bitmap_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, INT32 X, INT32 Y, INT32 PA, INT32 PB, INT32 PC, INT32 PD, INT32 *currentx, INT32 *currenty, int changed, int depth);
static void draw_roz_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, INT32 X, INT32 Y, INT32 PA, INT32 PB, INT32 PC, INT32 PD, INT32 *currentx, INT32 *currenty, int changed);
static void draw_bg_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, UINT32 hofs, UINT32 vofs);
static void draw_gba_oam_window(gba_state *state, running_machine &machine, UINT32 *scanline, int y);
static void draw_gba_oam(gba_state *state, running_machine &machine, UINT32 *scanline, int y);
static void invalid_gba_draw_function(running_machine &machine, gba_state *state, int y, UINT32* line0, UINT32* line1, UINT32* line2, UINT32* line3, UINT32* lineOBJ, UINT32* lineOBJWin, UINT32* lineMix, int aux);

/* Utility functions */
INLINE int is_in_window(gba_state *state, int x, int window);
INLINE UINT32 alpha_blend_pixel(UINT32 color0, UINT32 color1, int ca, int cb);
INLINE UINT32 increase_brightness(UINT32 color, int coeff_);
INLINE UINT32 decrease_brightness(UINT32 color, int coeff_);

#include "gbamode0.c"
#include "gbamode1.c"
#include "gbamode2.c"
#include "gbam345.c"

static void (*const gba_draw_scanline_modes[8][3])(running_machine &machine, gba_state *state, int y, UINT32* line0, UINT32* line1, UINT32* line2, UINT32* line3, UINT32* lineOBJ, UINT32* lineOBJWin, UINT32* lineMix, int aux) =
{
	/* All modes have three sub-modes: No effects, effects, and windowed effects. */
	{   /* Mode 0: 4 non-rotatable tilemaps and 1 OAM layer */
		&draw_mode0_scanline,
		&draw_mode0_scanline_nowindow,
		&draw_mode0_scanline_all
	},
	{   /* Mode 1: 2 non-rotatable tilemaps, 1 rotozoomable tilemap, and 1 OAM layer */
		&draw_mode1_scanline,
		&draw_mode1_scanline_nowindow,
		&draw_mode1_scanline_all
	},
	{   /* Mode 2: 2 rotozoomable tilemaps, and 1 OAM layer */
		&draw_mode2_scanline,
		&draw_mode2_scanline_nowindow,
		&draw_mode2_scanline_all
	},
	{   /* Mode 3: 1 rotatable 8bpp bitmap and one OAM layer */
		&draw_roz_bitmap_mode_scanline,
		&draw_roz_bitmap_mode_scanline_nowindow,
		&draw_roz_bitmap_mode_scanline_all
	},
	{   /* Mode 4: 1 rotatable 16bpp bitmap and one OAM layer */
		&draw_roz_bitmap_mode_scanline,
		&draw_roz_bitmap_mode_scanline_nowindow,
		&draw_roz_bitmap_mode_scanline_all
	},
	{   /* Mode 5: 1 rotatable 4bpp bitmap and one OAM layer */
		&draw_roz_bitmap_mode_scanline,
		&draw_roz_bitmap_mode_scanline_nowindow,
		&draw_roz_bitmap_mode_scanline_all
	},
	{
		&invalid_gba_draw_function,
		&invalid_gba_draw_function,
		&invalid_gba_draw_function,
	},
	{
		&invalid_gba_draw_function,
		&invalid_gba_draw_function,
		&invalid_gba_draw_function,
	},
};

static void invalid_gba_draw_function(running_machine &machine, gba_state *state, int y, UINT32* line0, UINT32* line1, UINT32* line2, UINT32* line3, UINT32* lineOBJ, UINT32* lineOBJWin, UINT32* lineMix, int aux)
{
	fatalerror( "Invalid screen mode (6 or 7)!\n" );
}

static void draw_roz_bitmap_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, INT32 X, INT32 Y, INT32 PA, INT32 PB, INT32 PC, INT32 PD, INT32 *currentx, INT32 *currenty, int changed, int depth)
{
	UINT8 *src8 = (UINT8 *)state->m_gba_vram.target();
	UINT16 *src16 = (UINT16 *)state->m_gba_vram.target();
	UINT16 *palette = (UINT16 *)state->m_gba_pram.target();
	INT32 sx = (depth == 4) ? 160 : 240;
	INT32 sy = (depth == 4) ? 128 : 160;
	UINT32 prio = ((ctrl & BGCNT_PRIORITY) << 25) + 0x1000000;
	INT32 dx, dmx, dy, dmy, startx, starty;
	INT32 rx, ry, pixx, pixy, x;

	if ((depth == 8) && (state->m_DISPCNT & DISPCNT_FRAMESEL))
		src8 += 0xa000;

	if ((depth == 4) && (state->m_DISPCNT & DISPCNT_FRAMESEL))
		src16 += 0xa000/2;

	// sign extend roz parameters
	if (X & 0x08000000) X |= 0xf0000000;
	if (Y & 0x08000000) Y |= 0xf0000000;
	if (PA & 0x8000) PA |= 0xffff0000;
	if (PB & 0x8000) PB |= 0xffff0000;
	if (PC & 0x8000) PC |= 0xffff0000;
	if (PD & 0x8000) PD |= 0xffff0000;

	// re-assign parameters for convenience's sake
	dx = PA;
	dmx = PB;
	dy = PC;
	dmy = PD;
	startx = X;
	starty = Y;

	if(ypos == 0)
		changed = 3;

	if(changed & 1)
		*currentx = startx;
	else
		*currentx += dmx;

	if(changed & 2)
		*currenty = starty;
	else
		*currenty += dmy;

	rx = *currentx;
	ry = *currenty;

	if(ctrl & BGCNT_MOSAIC)
	{
		INT32 mosaic_line = ((state->m_MOSAIC & 0x00f0) >> 4) + 1;
		INT32 tempy = (ypos / mosaic_line) * mosaic_line;
		rx = startx + tempy*dmx;
		ry = starty + tempy*dmy;
	}

	pixx = rx >> 8;
	pixy = ry >> 8;

	for(x = 0; x < 240; x++)
	{
		if(pixx < 0 || pixy < 0 || pixx >= sx || pixy >= sy)
		{
			scanline[x] = 0x80000000;
		}
		else
		{
			if (depth == 8)
			{
				UINT8 color = src8[pixy*sx + pixx];
				scanline[x] = color ? (palette[color] | prio) : 0x80000000;
			}
			else
			{
				scanline[x] = src16[pixy*sx + pixx] | prio;
			}
		}

		rx += dx;
		ry += dy;

		pixx = rx >> 8;
		pixy = ry >> 8;
	}

	if(ctrl & BGCNT_MOSAIC)
	{
		INT32 mosaicx = (state->m_MOSAIC & 0x0f) + 1;
		if(mosaicx > 1)
		{
			INT32 m = 1;
			for(x = 0; x < 239; x++)
			{
				scanline[x+1] = scanline[x];
				m++;
				if(m == mosaicx)
				{
					m = 1;
					x++;
				}
			}
		}
	}
}

static void draw_roz_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, INT32 X, INT32 Y, INT32 PA, INT32 PB, INT32 PC, INT32 PD, INT32 *currentx, INT32 *currenty, int changed)
{
	UINT32 base, mapbase, size;
	static const INT32 sizes[4] = { 128, 256, 512, 1024 };
	INT32 cx, cy, x, pixx, pixy;
	UINT8 *mgba_vram = (UINT8 *)state->m_gba_vram.target();
	UINT32 tile;
	UINT16 *pgba_pram = (UINT16 *)state->m_gba_pram.target();
	UINT16 pixel;
	UINT32 prio = ((ctrl & BGCNT_PRIORITY) << 25) + 0x1000000;

	for(x = 0; x < 240; x++)
	{
		scanline[x] = 0x80000000;
	}

	if (state->m_DISPCNT & enablemask)
	{
		base = ((ctrl & BGCNT_CHARBASE) >> BGCNT_CHARBASE_SHIFT) * 0x4000;          // VRAM base of tiles
		mapbase = ((ctrl & BGCNT_SCREENBASE) >> BGCNT_SCREENBASE_SHIFT) * 0x800;    // VRAM base of map
		size = (ctrl & BGCNT_SCREENSIZE) >> BGCNT_SCREENSIZE_SHIFT;                 // size of map in submaps

		// sign extend roz parameters
		if (X & 0x08000000) X |= 0xf0000000;
		if (Y & 0x08000000) Y |= 0xf0000000;
		if (PA & 0x8000) PA |= 0xffff0000;
		if (PB & 0x8000) PB |= 0xffff0000;
		if (PC & 0x8000) PC |= 0xffff0000;
		if (PD & 0x8000) PD |= 0xffff0000;

		if(ypos == 0)
		{
			changed = 3;
		}

		if(changed & 1)
		{
			*currentx = X;
		}
		else
		{
			*currentx += PB;
		}

		if(changed & 2)
		{
			*currenty = Y;
		}
		else
		{
			*currenty += PD;
		}

		cx = *currentx;
		cy = *currenty;

		if(ctrl & BGCNT_MOSAIC)
		{
			int mosaicy = ((state->m_MOSAIC & 0xf0) >> 4) + 1;
			int y = ypos % mosaicy;
			cx -= y*PB;
			cy -= y*PD;
		}

		pixx = cx >> 8;
		pixy = cy >> 8;

		if(ctrl & BGCNT_PALETTESET_WRAP)
		{
			pixx %= sizes[size];
			pixy %= sizes[size];
			if(pixx < 0)
			{
				pixx += sizes[size];
			}
			if(pixy < 0)
			{
				pixy += sizes[size];
			}
		}

		if(ctrl & BGCNT_PALETTE256)
		{
			for(x = 0; x < 240; x++)
			{
				if(pixx < 0 || pixy < 0 || pixx >= sizes[size] || pixy >= sizes[size])
				{
					scanline[x] = 0x80000000;
				}
				else
				{
					int tilex = pixx & 7;
					int tiley = pixy & 7;

					tile = mgba_vram[mapbase + (pixx >> 3) + (pixy >> 3) * (sizes[size] >> 3)];
					pixel = mgba_vram[base + (tile << 6) + (tiley << 3) + tilex];

					// plot it
					scanline[x] = pixel ? (pgba_pram[pixel] | prio) : 0x80000000;
				}

				cx += PA;
				cy += PC;

				pixx = cx >> 8;
				pixy = cy >> 8;

				if(ctrl & BGCNT_PALETTESET_WRAP)
				{
					pixx %= sizes[size];
					pixy %= sizes[size];
					if(pixx < 0)
					{
						pixx += sizes[size];
					}
					if(pixy < 0)
					{
						pixy += sizes[size];
					}
				}
			}
		}
		else
		{
			for(x = 0; x < 240; x++)
			{
				if(pixx < 0 || pixy < 0 || pixx >= sizes[size] || pixy >= sizes[size])
				{
					scanline[x] = 0x80000000;
				}
				else
				{
					int tilex = pixx & 7;
					int tiley = pixy & 7;

					tile = mgba_vram[mapbase + (pixx >> 3) + (pixy >> 3) * (sizes[size] >> 3)];
					pixel = mgba_vram[base + (tile << 6) + (tiley << 3) + tilex];

					// plot it
					scanline[x] = pixel ? (pgba_pram[pixel] | prio) : 0x80000000;
				}

				cx += PA;
				cy += PC;

				pixx = cx >> 8;
				pixy = cy >> 8;

				if(ctrl & BGCNT_PALETTESET_WRAP)
				{
					pixx %= sizes[size];
					pixy %= sizes[size];
					if(pixx < 0)
					{
						pixx += sizes[size];
					}
					if(pixy < 0)
					{
						pixy += sizes[size];
					}
				}
			}
		}

		if(ctrl & BGCNT_MOSAIC)
		{
			int mosaicx = (state->m_MOSAIC & 0x0f) + 1;
			if(mosaicx > 1)
			{
				int m = 1;
				for(x = 0; x < 239; x++)
				{
					scanline[x+1] = scanline[x];
					m++;
					if(m == mosaicx)
					{
						m = 1;
						x++;
					}
				}
			}
		}
	}
}

static void draw_bg_scanline(gba_state *state, UINT32 *scanline, int ypos, UINT32 enablemask, UINT32 ctrl, UINT32 hofs, UINT32 vofs)
{
	UINT8 *vram = (UINT8*)state->m_gba_vram.target();
	UINT16 *palette = (UINT16*)state->m_gba_pram.target();
	UINT8 *chardata = &vram[((ctrl & BGCNT_CHARBASE) >> BGCNT_CHARBASE_SHIFT) * 0x4000];
	UINT16 *screendata = (UINT16*)&vram[((ctrl & BGCNT_SCREENBASE) >> BGCNT_SCREENBASE_SHIFT) * 0x800];
	UINT32 priority = ((ctrl & BGCNT_PRIORITY) << 25) + 0x1000000;
	INT32 width = 256;
	INT32 height = 256;
	INT32 maskx, masky, pixx, pixy;
	UINT8 use_mosaic = (ctrl & BGCNT_MOSAIC) ? 1 : 0;
	INT32 mosaicx = (state->m_MOSAIC & 0x000f) + 1;
	INT32 mosaicy = ((state->m_MOSAIC & 0x00f0) >> 4) + 1;
	INT32 stride;
	INT32 x;

	if(!(state->m_DISPCNT & enablemask))
	{
		for(x = 0; x < 240; x++)
		{
			scanline[x] = 0x80000000;
		}

		return;
	}

	switch((ctrl & BGCNT_SCREENSIZE) >> BGCNT_SCREENSIZE_SHIFT)
	{
		case 1:
			width = 512;
			break;
		case 2:
			height = 512;
			break;
		case 3:
			width = 512;
			height = 512;
			break;
	}

	maskx = width - 1;
	masky = height - 1;

	pixx = hofs & maskx;
	pixy = (vofs + ypos) & masky;

	if(use_mosaic)
	{
		if((ypos % mosaicy) != 0)
		{
			mosaicy = (ypos / mosaicy) * mosaicy;
			pixy = (vofs + mosaicy) & masky;
		}
	}

	if(pixy > 255 && height > 256)
	{
		pixy &= 0x000000ff;
		screendata += 0x400;
		if(width > 256)
		{
			screendata += 0x400;
		}
	}

	stride = (pixy >> 3) << 5;

	if(ctrl & BGCNT_PALETTE256)
	{
		UINT16 *src = screendata + 0x400 * (pixx >> 8) + ((pixx & 255) >> 3) + stride;
		for(x = 0; x < 240; x++)
		{
			UINT16 data = *src;
			INT32 tile = data & TILEOBJ_TILE;
			INT32 tilex = pixx & 7;
			INT32 tiley = pixy & 7;
			UINT8 color;

			if(data & TILEOBJ_HFLIP)
			{
				tilex = 7 - tilex;
			}
			if(data & TILEOBJ_VFLIP)
			{
				tiley = 7 - tiley;
			}

			color = chardata[(tile << 6) + (tiley << 3) + tilex];

			if(color)
			{
				scanline[x] = palette[color] | priority;
			}
			else
			{
				scanline[x] = 0x80000000;
			}

			if(data & TILEOBJ_HFLIP)
			{
				if(tilex == 0)
				{
					src++;
				}
			}
			else if(tilex == 7)
			{
				src++;
			}

			pixx++;
			if(pixx == 256)
			{
				if(width > 256)
				{
					src = screendata + 0x400 + stride;
				}
				else
				{
					src = screendata + stride;
					pixx = 0;
				}
			}
			else if(pixx >= width)
			{
				pixx = 0;
				src = screendata + stride;
			}
		}
	}
	else
	{
		UINT16 *src = screendata + 0x400 * (pixx >> 8) + ((pixx & 255) >> 3) + stride;
		for(x = 0; x < 240; x++)
		{
			UINT16 data = *src;
			INT32 tile = data & TILEOBJ_TILE;
			INT32 tilex = pixx & 7;
			INT32 tiley = pixy & 7;
			UINT8 color;
			UINT8 palindex;

			if(data & TILEOBJ_HFLIP)
			{
				tilex = 7 - tilex;
			}
			if(data & TILEOBJ_VFLIP)
			{
				tiley = 7 - tiley;
			}

			color = chardata[(tile << 5) + (tiley << 2) + (tilex >> 1)];

			if(tilex & 1)
			{
				color >>= 4;
			}
			else
			{
				color &= 0x0f;
			}

			palindex = (data >> 8) & 0x00f0;
			if(color)
			{
				scanline[x] = palette[palindex + color] | priority;
			}
			else
			{
				scanline[x] = 0x80000000;
			}

			if(data & TILEOBJ_HFLIP)
			{
				if(tilex == 0)
				{
					src++;
				}
			}
			else if(tilex == 7)
			{
				src++;
			}

			pixx++;
			if(pixx == 256)
			{
				if(width > 256)
				{
					src = screendata + 0x400 + stride;
				}
				else
				{
					src = screendata + stride;
					pixx = 0;
				}
			}
			else if(pixx >= width)
			{
				pixx = 0;
				src = screendata + stride;
			}
		}
	}

	if(use_mosaic)
	{
		if(mosaicx > 1)
		{
			INT32 m = 1;
			for(x = 0; x < 239; x++)
			{
				scanline[x+1] = scanline[x];
				m++;
				if(m == mosaicx)
				{
					m = 1;
					x++;
				}
			}
		}
	}
}

static void draw_gba_oam_window(gba_state *state, running_machine &machine, UINT32 *scanline, int y)
{
	INT16 gba_oamindex;
	UINT32 tilebytebase, tileindex, tiledrawindex;
	UINT32 width, height;
	UINT16 *pgba_oam = (UINT16 *)state->m_gba_oam.target();
	int x = 0;
	UINT8 *src = (UINT8*)state->m_gba_vram.target();

	for(x = 0; x < 240; x++)
	{
		scanline[x] = 0x80000000;
	}

	if( state->m_DISPCNT & DISPCNT_OBJWIN_EN )
	{
		for( gba_oamindex = 127; gba_oamindex >= 0; gba_oamindex-- )
		{
			INT32 sx, sy;
			UINT16 attr0, attr1, attr2;
			INT32 cury;

			attr0 = pgba_oam[(4*gba_oamindex)+0];
			attr1 = pgba_oam[(4*gba_oamindex)+1];
			attr2 = pgba_oam[(4*gba_oamindex)+2];

			sx = (attr1 & OBJ_X_COORD);
			sy = (attr0 & OBJ_Y_COORD);

			if(sy > 160)
			{
				sy -= 256;
			}

			if ((attr0 & OBJ_MODE) != OBJ_MODE_WINDOW)
			{
				continue;
			}
			else
			{
				width = 0;
				height = 0;
				switch (attr0 & OBJ_SHAPE )
				{
					case OBJ_SHAPE_SQR:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 1;
								height = 1;
								break;
							case OBJ_SIZE_16:
								width = 2;
								height = 2;
								break;
							case OBJ_SIZE_32:
								width = 4;
								height = 4;
								break;
							case OBJ_SIZE_64:
								width = 8;
								height = 8;
								break;
						}
						break;
					case OBJ_SHAPE_HORIZ:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 2;
								height = 1;
								break;
							case OBJ_SIZE_16:
								width = 4;
								height = 1;
								break;
							case OBJ_SIZE_32:
								width = 4;
								height = 2;
								break;
							case OBJ_SIZE_64:
								width = 8;
								height = 4;
								break;
						}
						break;
					case OBJ_SHAPE_VERT:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 1;
								height = 2;
								break;
							case OBJ_SIZE_16:
								width = 1;
								height = 4;
								break;
							case OBJ_SIZE_32:
								width = 2;
								height = 4;
								break;
							case OBJ_SIZE_64:
								width = 4;
								height = 8;
								break;
						}
						break;
					default:
						width = 0;
						height = 0;
						verboselog(machine, 0, "OAM error: Trying to draw OBJ with OBJ_SHAPE = 3!\n" );
						break;
				}

				tiledrawindex = tileindex = (attr2 & OBJ_TILENUM);
				tilebytebase = 0x10000; // the index doesn't change in the higher modes, we just ignore sprites that are out of range

				if (attr0 & OBJ_ROZMODE_ROZ)
				{
					INT32 fx, fy, ax, ay, rx, ry;
					INT16 dx, dy, dmx, dmy;
					UINT8 color;

					width *= 8;
					height *= 8;

					if ((attr0 & OBJ_ROZMODE) == OBJ_ROZMODE_DISABLE)
					{
						continue;
					}

					fx = width;
					fy = height;

					if((attr0 & OBJ_ROZMODE) == OBJ_ROZMODE_DBLROZ)
					{
						fx *= 2;
						fy *= 2;
					}

					cury = y - sy;

					if(cury >= 0 && cury < fy)
					{
						if(sx < 240 || ((sx + fx) & 0x1ff) < 240)
						{
							int rot = (attr1 & OBJ_SCALE_PARAM) >> OBJ_SCALE_PARAM_SHIFT;
							dx  = (INT16)pgba_oam[(rot << 4)+3];
							dmx = (INT16)pgba_oam[(rot << 4)+7];
							dy  = (INT16)pgba_oam[(rot << 4)+11];
							dmy = (INT16)pgba_oam[(rot << 4)+15];

							rx = (width << 7) - (fx >> 1)*dx - (fy >> 1)*dmx + cury * dmx;
							ry = (height << 7) - (fx >> 1)*dy - (fy >> 1)*dmy + cury * dmy;

							if((attr0 & OBJ_PALMODE) == OBJ_PALMODE_256)
							{
								int inc = 32;
								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}
								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = sx >> 2;
								}
								else
								{
									tiledrawindex &= 0x3fe;
								}
								for(x = 0; x < fx; x++)
								{
									ax = rx >> 8;
									ay = ry >> 8;

									if(ax < 0 || ax >= sx || ay < 0 || ay >= sy)
									{
									}
									else
									{
										color = src[tilebytebase + ((((tiledrawindex + (ay >> 3) * inc) << 5) + ((ay & 0x07) << 3) + ((ax >> 3) << 6) + (ax & 0x07)) & 0x7fff)];

										if(color)
										{
											scanline[sx] = 1;
										}
									}

									sx = (sx + 1) & 0x1ff;

									rx += dx;
									ry += dy;
								}
							}
							else
							{
								int inc = 32;
								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}
								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = sx >> 3;
								}
								for(x = 0; x < fx; x++)
								{
									ax = rx >> 8;
									ay = ry >> 8;

									if(ax < 0 || ax >= sx || ay < 0 || ay >= sy)
									{
									}
									else
									{
										color = src[tilebytebase + ((((tiledrawindex + (ay >> 3) * inc) << 5) + ((ay & 0x07) << 2) + ((ax >> 3) << 5) + ((ax & 0x07) >> 1)) & 0x7fff)];

										if(ax & 1)
										{
											color >>= 4;
										}
										else
										{
											color &= 0x0f;
										}

										if(color)
										{
											scanline[sx] = 1;
										}
									}

									sx = (sx + 1) & 0x1ff;

									rx += dx;
									ry += dy;
								}
							}
						}
					}
				}
				else
				{
					INT32 ax;
					int cury_ = y - sy;

					width *= 8;
					height *= 8;

					if(cury_ >= 0 && cury_ < height)
					{
						if( ( (sx < 240) || ( ( (sx + width) & 0x1ff ) < 240 ) ) && (attr0 & OBJ_ROZMODE_DISABLE) == 0)
						{
							if((attr0 & OBJ_PALMODE) == OBJ_PALMODE_256)
							{
								int inc = 32;
								int address = 0;
								if(attr1 & OBJ_VFLIP)
								{
									cury_ = height - cury_ - 1;
								}
								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 2;
								}
								else
								{
									tiledrawindex &= 0x3fe;
								}

								ax = 0;
								if(attr1 & OBJ_HFLIP)
								{
									ax = width - 1;
								}

								address = 0x10000 + ((((tiledrawindex + (cury_ >> 3) * inc) << 5) + ((cury_ & 7) << 3) + ((ax >> 3) << 6) + (ax & 7)) & 0x7fff);

								if(attr1 & OBJ_HFLIP)
								{
									ax = 7;
								}

								for(x = 0; x < width; x++)
								{
									if(sx < 240)
									{
										UINT8 color = src[address];

										if(color)
										{
											scanline[sx] = 1;
										}
									}

									sx = (sx + 1) & 0x1ff;

									if(attr1 & OBJ_HFLIP)
									{
										ax--;
										address--;
										if(ax == -1)
										{
											address -= 56;
											ax = 7;
										}
										if(address < 0x10000)
										{
											address += 0x8000;
										}
									}
									else
									{
										ax++;
										address++;
										if(ax == 8)
										{
											address += 56;
											ax = 0;
										}
										if(address > 0x17fff)
										{
											address -= 0x8000;
										}
									}
								}
							}
							else
							{
								int inc = 32;
								UINT32 address;
								if(attr1 & OBJ_VFLIP)
								{
									cury_ = height - cury_ - 1;
								}
								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 3;
								}

								ax = 0;
								if(attr1 & OBJ_HFLIP)
								{
									ax = width - 1;
								}

								address = 0x10000 + ((((tiledrawindex + (cury_ >> 3) * inc) << 5) + ((cury_ & 0x07) << 2) + ((ax >> 3) << 5) + ((ax & 0x07) >> 1)) & 0x7fff);
								if(attr1 & OBJ_HFLIP)
								{
									ax = 7;
									for(x = width - 1; x >= 0; x--)
									{
										if(sx < 240)
										{
											UINT8 color = src[address];
											if(x & 1)
											{
												color >>= 4;
											}
											else
											{
												color &= 0x0f;
											}

											if(color)
											{
												scanline[sx] = 1;
											}
										}

										sx = (sx + 1) & 0x1ff;
										ax--;
										if((x & 1) == 0)
										{
											address--;
										}
										if(ax == -1)
										{
											ax = 7;
											address -= 28;
										}
										if(address < 0x10000)
										{
											address += 0x8000;
										}
									}
								}
								else
								{
									for(x = 0; x < width; x++)
									{
										if(sx < 240)
										{
											UINT8 color = src[address];

											if(x & 1)
											{
												color >>= 4;
											}
											else
											{
												color &= 0x0f;
											}

											if(color)
											{
												scanline[sx] = 1;
											}
										}

										sx = (sx + 1) & 0x1ff;
										ax++;
										if(x & 1)
										{
											address++;
										}
										if(ax == 8)
										{
											address += 28;
											ax = 0;
										}
										if(address > 0x17fff)
										{
											address -= 0x8000;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

static void draw_gba_oam(gba_state *state, running_machine &machine, UINT32 *scanline, int y)
{
	INT16 gba_oamindex;
	INT32 mosaiccnt = 0;
	INT32 mosaicy = ((state->m_MOSAIC & 0xf000) >> 12) + 1;
	INT32 mosaicx = ((state->m_MOSAIC & 0x0f00) >>  8) + 1;
	UINT32 tileindex, tiledrawindex; //, tilebytebase
	UINT8 width, height;
	UINT16 *pgba_oam = (UINT16 *)state->m_gba_oam.target();
	UINT8 *src = (UINT8 *)state->m_gba_vram.target();
	UINT16 *palette = (UINT16*)state->m_gba_pram.target();
	int x = 0;

	for(x = 0; x < 240; x++)
	{
		scanline[x] = 0x80000000;
	}

	if( state->m_DISPCNT & DISPCNT_OBJ_EN )
	{
		for( gba_oamindex = 0; gba_oamindex < 128; gba_oamindex++ )
		{
			UINT16 attr0, attr1, attr2;
			INT32 cury;
			UINT32 prio;
			UINT32 priority;

			attr0 = pgba_oam[(4*gba_oamindex)+0];
			attr1 = pgba_oam[(4*gba_oamindex)+1];
			attr2 = pgba_oam[(4*gba_oamindex)+2];
			priority = (attr2 & OBJ_PRIORITY) >> OBJ_PRIORITY_SHIFT;
			prio = (priority << 25) | ((attr0 & OBJ_MODE) << 6);

			if ((attr0 & OBJ_MODE) != OBJ_MODE_WINDOW)
			{
				width = 0;
				height = 0;
				switch (attr0 & OBJ_SHAPE)
				{
					case OBJ_SHAPE_SQR:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 1;
								height = 1;
								break;
							case OBJ_SIZE_16:
								width = 2;
								height = 2;
								break;
							case OBJ_SIZE_32:
								width = 4;
								height = 4;
								break;
							case OBJ_SIZE_64:
								width = 8;
								height = 8;
								break;
						}
						break;
					case OBJ_SHAPE_HORIZ:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 2;
								height = 1;
								break;
							case OBJ_SIZE_16:
								width = 4;
								height = 1;
								break;
							case OBJ_SIZE_32:
								width = 4;
								height = 2;
								break;
							case OBJ_SIZE_64:
								width = 8;
								height = 4;
								break;
						}
						break;
					case OBJ_SHAPE_VERT:
						switch(attr1 & OBJ_SIZE )
						{
							case OBJ_SIZE_8:
								width = 1;
								height = 2;
								break;
							case OBJ_SIZE_16:
								width = 1;
								height = 4;
								break;
							case OBJ_SIZE_32:
								width = 2;
								height = 4;
								break;
							case OBJ_SIZE_64:
								width = 4;
								height = 8;
								break;
						}
						break;
					default:
						width = 0;
						height = 0;
						verboselog(machine, 0, "OAM error: Trying to draw OBJ with OBJ_SHAPE = 3!\n" );
						break;
				}

				tiledrawindex = tileindex = (attr2 & OBJ_TILENUM);
//              tilebytebase = 0x10000; // the index doesn't change in the higher modes, we just ignore sprites that are out of range

				if (attr0 & OBJ_ROZMODE_ROZ)
				{
					INT32 sx, sy;
					INT32 fx, fy, rx, ry;
					INT16 dx, dy, dmx, dmy;

					width *= 8;
					height *= 8;

					if ((attr0 & OBJ_ROZMODE) == OBJ_ROZMODE_DISABLE)
					{
						continue;
					}

					sx = (attr1 & OBJ_X_COORD);
					sy = (attr0 & OBJ_Y_COORD);

					if(sy > 160)
					{
						sy -= 256;
					}

					fx = width;
					fy = height;

					if((attr0 & OBJ_ROZMODE) == OBJ_ROZMODE_DBLROZ)
					{
						fx *= 2;
						fy *= 2;
					}

					cury = y - sy;

					if(cury >= 0 && cury < fy)
					{
						if(sx < 240 || ((sx + fx) & 0x1ff) < 240)
						{
							INT32 oamparam = (attr1 & OBJ_SCALE_PARAM) >> OBJ_SCALE_PARAM_SHIFT;

							dx  = (INT16)pgba_oam[(oamparam << 4)+3];
							dmx = (INT16)pgba_oam[(oamparam << 4)+7];
							dy  = (INT16)pgba_oam[(oamparam << 4)+11];
							dmy = (INT16)pgba_oam[(oamparam << 4)+15];

							if(attr0 & OBJ_MOSAIC)
							{
								cury -= (cury % mosaicy);
							}

							rx = (width << 7) - (fx >> 1)*dx - (fy >> 1)*dmx + cury*dmx;
							ry = (height << 7) - (fx >> 1)*dy - (fy >> 1)*dmy + cury*dmy;

							if((attr0 & OBJ_PALMODE) == OBJ_PALMODE_256)
							{
								INT32 inc = 32;

								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 2;
								}
								else
								{
									tiledrawindex &= 0x3fe;
								}

								for(x = 0; x < fx; x++)
								{
									INT32 pixx = rx >> 8;
									INT32 pixy = ry >> 8;

									if(!(pixx < 0 || pixx >= width || pixy < 0 || pixy >= height || sx >= 240))
									{
										UINT8 color = src[0x10000 + ((((tiledrawindex + (pixy >> 3) * inc) << 5) + ((pixy & 7) << 3) + ((pixx >> 3) << 6) + (pixx & 7)) & 0x7fff)];
										if(color == 0 && priority < ((scanline[sx] >> 25) & 3))
										{
											scanline[sx] = (scanline[sx] & 0xf9ffffff) | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}
										else if(color != 0 && prio < (scanline[sx] & 0xff000000))
										{
											scanline[sx] = palette[256 + color] | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}

										if(attr0 & OBJ_MOSAIC)
										{
											mosaiccnt++;
											if(mosaiccnt == mosaicx)
											{
												mosaiccnt = 0;
											}
										}
									}

									sx++;
									sx &= 0x1ff;

									rx += dx;
									ry += dy;
								}
							}
							else
							{
								INT32 inc = 32;
								INT32 palentry = (attr2 >> 8) & 0xf0;

								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 3;
								}

								for(x = 0; x < fx; x++)
								{
									INT32 pixx = rx >> 8;
									INT32 pixy = ry >> 8;

									if(!(pixx < 0 || pixx >= width || pixy < 0 || pixy >= height || sx >= 240))
									{
										UINT8 color = src[0x10000 + ((((tiledrawindex + (pixy >> 3) * inc) << 5) + ((pixy & 7) << 2) + ((pixx >> 3) << 5) + ((pixx & 7) >> 1)) & 0x7fff)];

										if(pixx & 1)
										{
											color >>= 4;
										}
										else
										{
											color &= 0x0f;
										}

										if(color == 0 && priority < ((scanline[sx] >> 25) & 3))
										{
											scanline[sx] = (scanline[sx] & 0xf9ffffff) | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}
										else if(color != 0 && prio < (scanline[sx] & 0xff000000))
										{
											scanline[sx] = palette[256 + palentry + color] | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}
									}

									if(attr0 & OBJ_MOSAIC)
									{
										mosaiccnt++;
										if(mosaiccnt == mosaicx)
										{
											mosaiccnt = 0;
										}
									}

									sx++;

									sx &= 0x1ff;

									rx += dx;
									ry += dy;
								}
							}
						}
					}
				}
				else
				{
					INT32 sx, sy;
					INT32 vflip = (attr1 & OBJ_VFLIP) ? 1 : 0;
					INT32 hflip = (attr1 & OBJ_HFLIP) ? 1 : 0;

					width *= 8;
					height *= 8;

					if ((attr0 & OBJ_ROZMODE) == OBJ_ROZMODE_DISABLE)
					{
						continue;
					}

					sx = (attr1 & OBJ_X_COORD);
					sy = (attr0 & OBJ_Y_COORD);

					if(sy > 160)
					{
						sy -= 256;
					}

					cury = y - sy;
					if(cury >= 0 && cury < height)
					{
						if(sx < 240 || ((sx + width) & 0x1ff) < 240)
						{
							if((attr0 & OBJ_PALMODE) == OBJ_PALMODE_256)
							{
								INT32 pixx;
								INT32 inc = 32;
								INT32 address = 0;

								if(vflip)
								{
									cury = height - cury - 1;
								}

								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 2;
								}
								else
								{
									tiledrawindex &= 0x3fe;
								}

								pixx = 0;
								if(hflip)
								{
									pixx = width - 1;
								}

								if(attr0 & OBJ_MOSAIC)
								{
									cury -= (cury % mosaicy);
								}

								address = 0x10000 + ((((tiledrawindex + (cury >> 3) * inc) << 5) + ((cury & 7) << 3) + ((pixx >> 3) << 6) + (pixx & 7)) & 0x7fff);

								if(hflip)
								{
									pixx = 7;
								}

								for(x = 0; x < width; x++)
								{
									if(sx < 240)
									{
										UINT8 color = src[address];
										if(color == 0 && priority < ((scanline[sx] >> 25) & 3))
										{
											scanline[sx] = (scanline[sx] & 0xf9ffffff) | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}
										else if(color != 0 && prio < (scanline[sx] & 0xff000000))
										{
											scanline[sx] = palette[256 + color] | prio;
											if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
											{
												scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
											}
										}
									}

									if(attr0 & OBJ_MOSAIC)
									{
										mosaiccnt++;
										if(mosaiccnt == mosaicx)
										{
											mosaiccnt = 0;
										}
									}

									sx++;
									sx &= 0x1ff;

									if(hflip)
									{
										pixx--;
										address--;
										if(pixx == -1)
										{
											address -= 56;
											pixx = 7;
										}
										if(address < 0x10000)
										{
											address += 0x8000;
										}
									}
									else
									{
										pixx++;
										address++;
										if(pixx == 8)
										{
											address += 56;
											pixx = 0;
										}
										if(address > 0x17fff)
										{
											address -= 0x8000;
										}
									}
								}
							}
							else
							{
								INT32 pixx;
								INT32 inc = 32;
								UINT32 address = 0;

								if(vflip)
								{
									cury = height - cury - 1;
								}

								if((state->m_DISPCNT & DISPCNT_MODE) > 2 && tiledrawindex < 0x200)
								{
									continue;
								}

								if((state->m_DISPCNT & DISPCNT_VRAM_MAP) == DISPCNT_VRAM_MAP_1D)
								{
									inc = width >> 3;
								}

								pixx = 0;
								if(hflip)
								{
									pixx = width - 1;
								}

								if(attr0 & OBJ_MOSAIC)
								{
									cury -= (cury % mosaicy);
								}

								address = 0x10000 + ((((tiledrawindex + (cury >> 3) * inc) << 5) + ((cury & 7) << 2) + ((pixx >> 3) << 5) + ((pixx & 7) >> 1)) & 0x7fff);

								if(hflip)
								{
									pixx = 7;
									for(x = width - 1; x >= 0; x--)
									{
										if(sx < 240)
										{
											UINT8 color = src[address];
											UINT8 palentry = (attr2 >> 8) & 0xf0;

											if(x & 1)
											{
												color >>= 4;
											}
											else
											{
												color &= 0x0f;
											}

											if(color == 0 && priority < ((scanline[sx] >> 25) & 3))
											{
												scanline[sx] = (scanline[sx] & 0xf9ffffff) | prio;
												if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
												{
													scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
												}
											}
											else if(color != 0 && prio < (scanline[sx] & 0xff000000))
											{
												scanline[sx] = palette[256 + palentry + color] | prio;
												if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
												{
													scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
												}
											}
										}

										sx++;
										sx &= 0x1ff;

										pixx--;
										if(!(x & 1))
										{
											address--;
										}
										if(pixx == -1)
										{
											address -= 28;
											pixx = 7;
										}
										if(address < 0x10000)
										{
											address += 0x8000;
										}
									}
								}
								else
								{
									for(x = 0; x < width; x++)
									{
										if(sx < 240)
										{
											UINT8 color = src[address];
											UINT8 palentry = (attr2 >> 8) & 0xf0;

											if(x & 1)
											{
												color >>= 4;
											}
											else
											{
												color &= 0x0f;
											}

											if(color == 0 && priority < ((scanline[sx] >> 25) & 3))
											{
												scanline[sx] = (scanline[sx] & 0xf9ffffff) | prio;
												if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
												{
													scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
												}
											}
											else if(color != 0 && prio < (scanline[sx] & 0xff000000))
											{
												scanline[sx] = palette[256 + palentry + color] | prio;
												if((attr0 & OBJ_MOSAIC) != 0 && mosaiccnt != 0)
												{
													scanline[sx] = (scanline[sx - 1] & 0xf9ffffff) | prio;
												}
											}
										}

										if(attr0 & OBJ_MOSAIC)
										{
											mosaiccnt++;
											if(mosaiccnt == mosaicx)
											{
												mosaiccnt = 0;
											}
										}

										sx++;
										sx &= 0x1ff;

										pixx++;
										if(x & 1)
										{
											address++;
										}
										if(pixx == 8)
										{
											address += 28;
											pixx = 0;
										}
										if(address > 0x17fff)
										{
											address -= 0x8000;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

INLINE int is_in_window(gba_state *state, int x, int window)
{
	int x0 = state->m_WIN0H >> 8;
	int x1 = state->m_WIN0H & 0x00ff;

	if(window == 1)
	{
		x0 = state->m_WIN1H >> 8;
		x1 = state->m_WIN1H & 0x00ff;
	}

	if(x0 <= x1)
	{
		if(x >= x0 && x < x1)
		{
			return 1;
		}
	}
	else
	{
		if(x >= x0 || x < x1)
		{
			return 1;
		}
	}

	return 0;
}

INLINE UINT32 alpha_blend_pixel(UINT32 color0, UINT32 color1, int ca, int cb)
{
	if(color0 < 0x80000000)
	{
		int r0 = (color0 >>  0) & 0x1f;
		int g0 = (color0 >>  5) & 0x1f;
		int b0 = (color0 >> 10) & 0x1f;
		int r1 = (color1 >>  0) & 0x1f;
		int g1 = (color1 >>  5) & 0x1f;
		int b1 = (color1 >> 10) & 0x1f;
		int r = ((r0 * ca) >> 4) + ((r1 * cb) >> 4);
		int g = ((g0 * ca) >> 4) + ((g1 * cb) >> 4);
		int b = ((b0 * ca) >> 4) + ((b1 * cb) >> 4);

		if(r > 0x1f) r = 0x1f;
		if(g > 0x1f) g = 0x1f;
		if(b > 0x1f) b = 0x1f;

		return (color0 & 0xffff0000) | (b << 10) | (g << 5) | r;
	}
	return color0;
}

INLINE UINT32 increase_brightness(UINT32 color, int coeff_)
{
	int r = (color >>  0) & 0x1f;
	int g = (color >>  5) & 0x1f;
	int b = (color >> 10) & 0x1f;

	r += ((0x1f - r) * coeff_) >> 4;
	g += ((0x1f - g) * coeff_) >> 4;
	b += ((0x1f - b) * coeff_) >> 4;

	if(r > 0x1f) r = 0x1f;
	if(g > 0x1f) g = 0x1f;
	if(b > 0x1f) b = 0x1f;

	return (color & 0xffff0000) | (b << 10) | (g << 5) | r;
}

INLINE UINT32 decrease_brightness(UINT32 color, int coeff_)
{
	int r = (color >>  0) & 0x1f;
	int g = (color >>  5) & 0x1f;
	int b = (color >> 10) & 0x1f;

	r -= (r * coeff_) >> 4;
	g -= (g * coeff_) >> 4;
	b -= (b * coeff_) >> 4;

	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	return (color & 0xffff0000) | (b << 10) | (g << 5) | r;
}

void gba_draw_scanline(running_machine &machine, int y)
{
	gba_state *state = machine.driver_data<gba_state>();
	bitmap_ind16 &bitmap = state->m_bitmap;
	UINT16 *scanline = &bitmap.pix16(y);
	int i, x;
	UINT8 submode = 0;
	int bpp = 0;

	// forced blank
	if (state->m_DISPCNT & DISPCNT_BLANK)
	{
		// forced blank is white
		for (i = 0; i < 240; i++)
		{
			scanline[i] = 0x7fff;
		}
		return;
	}

	if(!state->m_fxOn && !state->m_windowOn && !(state->m_DISPCNT & DISPCNT_OBJWIN_EN))
	{
		submode = 0;
	}
	else if(state->m_fxOn && !state->m_windowOn && !(state->m_DISPCNT & DISPCNT_OBJWIN_EN))
	{
		submode = 1;
	}
	else
	{
		submode = 2;
	}

	//printf( "mode = %d, %d\n", state->m_DISPCNT & 7, submode );

	switch(state->m_DISPCNT & 7)
	{
		case 3:
			bpp = 16;
			break;
		case 4:
			bpp = 8;
			break;
		case 5:
			bpp = 4;
			break;
	}

	gba_draw_scanline_modes[state->m_DISPCNT & 7][submode](machine, state, y, &state->m_xferscan[0][1024], &state->m_xferscan[1][1024], &state->m_xferscan[2][1024], &state->m_xferscan[3][1024], &state->m_xferscan[4][1024], &state->m_xferscan[5][1024], &state->m_xferscan[6][1024], bpp);

	for(x = 0; x < 240; x++)
	{
		scanline[x] = state->m_xferscan[6][1024 + x] & 0x7fff;
	}

	return;
}

void gba_state::video_start()
{
	machine().primary_screen->register_screen_bitmap(m_bitmap);
}

UINT32 gba_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}
