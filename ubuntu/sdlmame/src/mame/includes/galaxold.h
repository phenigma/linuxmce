/***************************************************************************

  Galaxian hardware family (old)

  This include file is used by the following drivers:
    - dambustr.c
    - galaxold.c
    - scramble.c
    - scobra.c

***************************************************************************/

#ifndef __GALAXOLD_H__
#define __GALAXOLD_H__

/* star circuit */
#define STAR_COUNT  252
struct star
{
	int x, y, color;
};

class galaxold_state : public driver_device
{
public:
	galaxold_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_videoram(*this,"videoram"),
			m_spriteram(*this,"spriteram"),
			m_spriteram2(*this,"spriteram2"),
			m_attributesram(*this,"attributesram"),
			m_bulletsram(*this,"bulletsram"),
			m_rockclim_videoram(*this,"rockclim_vram"),
			m_racknrol_tiles_bank(*this,"racknrol_tbank")
	{ }

	required_shared_ptr<UINT8> m_videoram;
	required_shared_ptr<UINT8> m_spriteram;
	optional_shared_ptr<UINT8> m_spriteram2;
	required_shared_ptr<UINT8> m_attributesram;
	optional_shared_ptr<UINT8> m_bulletsram;
	optional_shared_ptr<UINT8> m_rockclim_videoram;
	optional_shared_ptr<UINT8> m_racknrol_tiles_bank;

	int m_irq_line;
	UINT8 m__4in1_bank;
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_rockclim_tilemap;
	int m_mooncrst_gfxextend;
	int m_spriteram2_present;
	UINT8 m_gfxbank[5];
	UINT8 m_flipscreen_x;
	UINT8 m_flipscreen_y;
	UINT8 m_color_mask;
	tilemap_t *m_dambustr_tilemap2;
	UINT8 *m_dambustr_videoram2;
	void (*m_modify_charcode)(running_machine &machine, UINT16 *code, UINT8 x);     /* function to call to do character banking */
	void (*m_modify_spritecode)(running_machine &machine, UINT8 *spriteram, int*, int*, int*, int); /* function to call to do sprite banking */
	void (*m_modify_color)(UINT8 *color);   /* function to call to do modify how the color codes map to the PROM */
	void (*m_modify_ypos)(UINT8*);  /* function to call to do modify how vertical positioning bits are connected */

	UINT8 m_timer_adjusted;
	UINT8 m_darkplnt_bullet_color;
	void (*m_draw_bullets)(running_machine &,bitmap_ind16 &,const rectangle &, int, int, int);  /* function to call to draw a bullet */

	UINT8 m_background_enable;
	UINT8 m_background_red;
	UINT8 m_background_green;
	UINT8 m_background_blue;
	void (*m_draw_background)(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect);   /* function to call to draw the background */
	UINT16 m_rockclim_v;
	UINT16 m_rockclim_h;
	int m_dambustr_bg_split_line;
	int m_dambustr_bg_color_1;
	int m_dambustr_bg_color_2;
	int m_dambustr_bg_priority;
	int m_dambustr_char_bank;
	bitmap_ind16 *m_dambustr_tmpbitmap;

	void (*m_draw_stars)(running_machine &machine, bitmap_ind16 &, const rectangle &);      /* function to call to draw the star layer */
	int m_stars_colors_start;
	INT32 m_stars_scrollpos;
	UINT8 m_stars_on;
	UINT8 m_stars_blink_state;
	emu_timer *m_stars_blink_timer;
	emu_timer *m_stars_scroll_timer;
	struct star m_stars[STAR_COUNT];

	DECLARE_READ8_MEMBER(drivfrcg_port0_r);
	DECLARE_READ8_MEMBER(scramb2_protection_r);
	DECLARE_READ8_MEMBER(scramb2_port0_r);
	DECLARE_READ8_MEMBER(scramb2_port1_r);
	DECLARE_READ8_MEMBER(scramb2_port2_r);
	DECLARE_READ8_MEMBER(hexpoola_data_port_r);
	DECLARE_READ8_MEMBER(bullsdrtg_data_port_r);
	DECLARE_WRITE8_MEMBER(galaxold_nmi_enable_w);
	DECLARE_WRITE8_MEMBER(galaxold_coin_lockout_w);
	DECLARE_WRITE8_MEMBER(galaxold_coin_counter_w);
	DECLARE_WRITE8_MEMBER(galaxold_coin_counter_1_w);
	DECLARE_WRITE8_MEMBER(galaxold_coin_counter_2_w);
	DECLARE_WRITE8_MEMBER(galaxold_leds_w);
	DECLARE_WRITE8_MEMBER(zigzag_sillyprotection_w);
	DECLARE_READ8_MEMBER(scramblb_protection_1_r);
	DECLARE_READ8_MEMBER(scramblb_protection_2_r);
	DECLARE_WRITE8_MEMBER(_4in1_bank_w);
	DECLARE_READ8_MEMBER(checkmaj_protection_r);
	DECLARE_READ8_MEMBER(dingo_3000_r);
	DECLARE_READ8_MEMBER(dingo_3035_r);
	DECLARE_READ8_MEMBER(dingoe_3001_r);
	DECLARE_WRITE8_MEMBER(racknrol_tiles_bank_w);
	DECLARE_WRITE8_MEMBER(galaxold_videoram_w);
	DECLARE_READ8_MEMBER(galaxold_videoram_r);
	DECLARE_WRITE8_MEMBER(galaxold_attributesram_w);
	DECLARE_WRITE8_MEMBER(galaxold_flip_screen_x_w);
	DECLARE_WRITE8_MEMBER(galaxold_flip_screen_y_w);
	DECLARE_WRITE8_MEMBER(gteikob2_flip_screen_x_w);
	DECLARE_WRITE8_MEMBER(gteikob2_flip_screen_y_w);
	DECLARE_WRITE8_MEMBER(hotshock_flip_screen_w);
	DECLARE_WRITE8_MEMBER(scrambold_background_enable_w);
	DECLARE_WRITE8_MEMBER(scrambold_background_red_w);
	DECLARE_WRITE8_MEMBER(scrambold_background_green_w);
	DECLARE_WRITE8_MEMBER(scrambold_background_blue_w);
	DECLARE_WRITE8_MEMBER(galaxold_stars_enable_w);
	DECLARE_WRITE8_MEMBER(darkplnt_bullet_color_w);
	DECLARE_WRITE8_MEMBER(galaxold_gfxbank_w);
	DECLARE_WRITE8_MEMBER(rockclim_videoram_w);
	DECLARE_WRITE8_MEMBER(rockclim_scroll_w);
	DECLARE_READ8_MEMBER(rockclim_videoram_r);
	DECLARE_WRITE8_MEMBER(dambustr_bg_split_line_w);
	DECLARE_WRITE8_MEMBER(dambustr_bg_color_w);
	DECLARE_CUSTOM_INPUT_MEMBER(_4in1_fake_port_r);
	DECLARE_CUSTOM_INPUT_MEMBER(vpool_lives_r);
	DECLARE_CUSTOM_INPUT_MEMBER(ckongg_coinage_r);
	DECLARE_CUSTOM_INPUT_MEMBER(dkongjrm_coinage_r);
	DECLARE_READ8_MEMBER(ttl7474_trampoline);
	DECLARE_DRIVER_INIT(bullsdrtg);
	DECLARE_DRIVER_INIT(ladybugg);
	DECLARE_DRIVER_INIT(4in1);
	TILE_GET_INFO_MEMBER(drivfrcg_get_tile_info);
	TILE_GET_INFO_MEMBER(racknrol_get_tile_info);
	TILE_GET_INFO_MEMBER(dambustr_get_tile_info2);
	TILE_GET_INFO_MEMBER(get_tile_info);
	TILE_GET_INFO_MEMBER(rockclim_get_tile_info);
	TILE_GET_INFO_MEMBER(harem_get_tile_info);
	DECLARE_MACHINE_RESET(galaxold);
	DECLARE_VIDEO_START(galaxold);
	DECLARE_PALETTE_INIT(galaxold);
	DECLARE_VIDEO_START(drivfrcg);
	DECLARE_PALETTE_INIT(rockclim);
	DECLARE_VIDEO_START(racknrol);
	DECLARE_VIDEO_START(batman2);
	DECLARE_VIDEO_START(mooncrst);
	DECLARE_VIDEO_START(scrambold);
	DECLARE_PALETTE_INIT(scrambold);
	DECLARE_VIDEO_START(pisces);
	DECLARE_MACHINE_RESET(devilfsg);
	DECLARE_VIDEO_START(dkongjrm);
	DECLARE_VIDEO_START(rockclim);
	DECLARE_VIDEO_START(galaxold_plain);
	DECLARE_VIDEO_START(ozon1);
	DECLARE_VIDEO_START(bongo);
	DECLARE_MACHINE_RESET(hunchbkg);
	DECLARE_VIDEO_START(ckongs);
	DECLARE_PALETTE_INIT(stratgyx);
	DECLARE_PALETTE_INIT(darkplnt);
	DECLARE_PALETTE_INIT(minefld);
	DECLARE_PALETTE_INIT(rescue);
	DECLARE_PALETTE_INIT(mariner);
	DECLARE_PALETTE_INIT(dambustr);
	DECLARE_PALETTE_INIT(turtles);
	DECLARE_VIDEO_START(darkplnt);
	DECLARE_VIDEO_START(rescue);
	DECLARE_VIDEO_START(minefld);
	DECLARE_VIDEO_START(stratgyx);
	DECLARE_VIDEO_START(mariner);
	DECLARE_VIDEO_START(mimonkey);
	DECLARE_VIDEO_START(scorpion);
	DECLARE_VIDEO_START(ad2083);
	DECLARE_VIDEO_START(dambustr);
	DECLARE_VIDEO_START(harem);
	UINT32 screen_update_galaxold(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_dambustr(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(hunchbks_vh_interrupt);
	TIMER_CALLBACK_MEMBER(stars_blink_callback);
	TIMER_CALLBACK_MEMBER(stars_scroll_callback);
	TIMER_DEVICE_CALLBACK_MEMBER(galaxold_interrupt_timer);
	DECLARE_WRITE_LINE_MEMBER(galaxold_7474_9m_2_q_callback);
	DECLARE_WRITE_LINE_MEMBER(galaxold_7474_9m_1_callback);
	DECLARE_VIDEO_START(bagmanmc);
};

/*----------- defined in video/galaxold.c -----------*/
void galaxold_init_stars(running_machine &machine, int colors_offset);
void galaxold_draw_stars(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect);

#define galaxold_coin_counter_0_w galaxold_coin_counter_w

#endif
