/* This it the best way to allow game specific kludges until the system is fully understood */
enum {
	ARKUNK = 0,  /* unknown bootlegs for inclusion of possible new sets */
	ARKANGC,
	ARKANGC2,
	BLOCK2,
	ARKBLOCK,
	ARKBLOC2,
	ARKGCBL,
	PADDLE2
};

class arkanoid_state : public driver_device
{
public:
	arkanoid_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this,"videoram"),
		m_spriteram(*this,"spriteram") { }

	/* memory pointers */
	required_shared_ptr<UINT8> m_videoram;
	optional_shared_ptr<UINT8> m_spriteram;

	/* video-related */
	tilemap_t  *m_bg_tilemap;
	UINT8    m_gfxbank;
	UINT8    m_palettebank;

	/* input-related */
	UINT8    m_paddle_select;

	/* misc */
	int      m_bootleg_id;
	UINT8    m_z80write;
	UINT8    m_fromz80;
	UINT8    m_m68705write;
	UINT8    m_toz80;
	UINT8    m_port_a_in;
	UINT8    m_port_a_out;
	UINT8    m_ddr_a;
	UINT8    m_port_c_out;
	UINT8    m_ddr_c;
	UINT8    m_bootleg_cmd;

	/* devices */
	device_t *m_mcu;
	DECLARE_READ8_MEMBER(arkanoid_Z80_mcu_r);
	DECLARE_WRITE8_MEMBER(arkanoid_Z80_mcu_w);
	DECLARE_READ8_MEMBER(arkanoid_68705_port_a_r);
	DECLARE_WRITE8_MEMBER(arkanoid_68705_port_a_w);
	DECLARE_WRITE8_MEMBER(arkanoid_68705_ddr_a_w);
	DECLARE_READ8_MEMBER(arkanoid_68705_port_c_r);
	DECLARE_WRITE8_MEMBER(arkanoid_68705_port_c_w);
	DECLARE_WRITE8_MEMBER(arkanoid_68705_ddr_c_w);
	DECLARE_READ8_MEMBER(arkanoid_bootleg_f000_r);
	DECLARE_READ8_MEMBER(arkanoid_bootleg_f002_r);
	DECLARE_WRITE8_MEMBER(arkanoid_bootleg_d018_w);
	DECLARE_READ8_MEMBER(block2_bootleg_f000_r);
	DECLARE_READ8_MEMBER(arkanoid_bootleg_d008_r);
	DECLARE_WRITE8_MEMBER(arkanoid_videoram_w);
	DECLARE_WRITE8_MEMBER(arkanoid_d008_w);
	DECLARE_WRITE8_MEMBER(tetrsark_d008_w);
	DECLARE_WRITE8_MEMBER(hexa_d008_w);
	DECLARE_CUSTOM_INPUT_MEMBER(arkanoid_68705_input_r);
	DECLARE_CUSTOM_INPUT_MEMBER(arkanoid_input_mux);
	DECLARE_DRIVER_INIT(block2);
	DECLARE_DRIVER_INIT(arkblock);
	DECLARE_DRIVER_INIT(hexa);
	DECLARE_DRIVER_INIT(paddle2);
	DECLARE_DRIVER_INIT(tetrsark);
	DECLARE_DRIVER_INIT(arkgcbl);
	DECLARE_DRIVER_INIT(arkangc2);
	DECLARE_DRIVER_INIT(arkbloc2);
	DECLARE_DRIVER_INIT(arkangc);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	DECLARE_MACHINE_START(arkanoid);
	DECLARE_MACHINE_RESET(arkanoid);
	DECLARE_VIDEO_START(arkanoid);
	UINT32 screen_update_arkanoid(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_hexa(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(test);
};
