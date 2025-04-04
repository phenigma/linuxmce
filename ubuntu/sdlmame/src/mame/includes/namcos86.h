class namcos86_state : public driver_device
{
public:
	namcos86_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_rthunder_videoram1(*this, "videoram1"),
		m_rthunder_videoram2(*this, "videoram2"),
		m_rthunder_spriteram(*this, "spriteram"){ }

	UINT8 *m_spriteram;
	int m_wdog;
	required_shared_ptr<UINT8> m_rthunder_videoram1;
	required_shared_ptr<UINT8> m_rthunder_videoram2;
	required_shared_ptr<UINT8> m_rthunder_spriteram;
	int m_tilebank;
	int m_xscroll[4];
	int m_yscroll[4];
	tilemap_t *m_bg_tilemap[4];
	int m_backcolor;
	const UINT8 *m_tile_address_prom;
	int m_copy_sprites;
	DECLARE_WRITE8_MEMBER(bankswitch1_w);
	DECLARE_WRITE8_MEMBER(bankswitch1_ext_w);
	DECLARE_WRITE8_MEMBER(bankswitch2_w);
	DECLARE_READ8_MEMBER(dsw0_r);
	DECLARE_READ8_MEMBER(dsw1_r);
	DECLARE_WRITE8_MEMBER(int_ack1_w);
	DECLARE_WRITE8_MEMBER(int_ack2_w);
	DECLARE_WRITE8_MEMBER(watchdog1_w);
	DECLARE_WRITE8_MEMBER(watchdog2_w);
	DECLARE_WRITE8_MEMBER(namcos86_coin_w);
	DECLARE_WRITE8_MEMBER(namcos86_led_w);
	DECLARE_WRITE8_MEMBER(cus115_w);
	DECLARE_READ8_MEMBER(readFF);
	DECLARE_READ8_MEMBER(rthunder_videoram1_r);
	DECLARE_WRITE8_MEMBER(rthunder_videoram1_w);
	DECLARE_READ8_MEMBER(rthunder_videoram2_r);
	DECLARE_WRITE8_MEMBER(rthunder_videoram2_w);
	DECLARE_WRITE8_MEMBER(rthunder_tilebank_select_w);
	DECLARE_WRITE8_MEMBER(rthunder_scroll0_w);
	DECLARE_WRITE8_MEMBER(rthunder_scroll1_w);
	DECLARE_WRITE8_MEMBER(rthunder_scroll2_w);
	DECLARE_WRITE8_MEMBER(rthunder_scroll3_w);
	DECLARE_WRITE8_MEMBER(rthunder_backcolor_w);
	DECLARE_READ8_MEMBER(rthunder_spriteram_r);
	DECLARE_WRITE8_MEMBER(rthunder_spriteram_w);
	DECLARE_DRIVER_INIT(namco86);
	TILE_GET_INFO_MEMBER(get_tile_info0);
	TILE_GET_INFO_MEMBER(get_tile_info1);
	TILE_GET_INFO_MEMBER(get_tile_info2);
	TILE_GET_INFO_MEMBER(get_tile_info3);
	virtual void machine_reset();
	virtual void video_start();
	virtual void palette_init();
	UINT32 screen_update_namcos86(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_eof_namcos86(screen_device &screen, bool state);
};
