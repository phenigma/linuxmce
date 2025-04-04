/***************************************************************************

    Jailbreak

***************************************************************************/

#define MASTER_CLOCK        XTAL_18_432MHz
#define VOICE_CLOCK         XTAL_3_579545MHz

class jailbrek_state : public driver_device
{
public:
	jailbrek_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_colorram(*this, "colorram"),
		m_videoram(*this, "videoram"),
		m_spriteram(*this, "spriteram"),
		m_scroll_x(*this, "scroll_x"),
		m_scroll_dir(*this, "scroll_dir"){ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_colorram;
	required_shared_ptr<UINT8> m_videoram;
	required_shared_ptr<UINT8> m_spriteram;
	required_shared_ptr<UINT8> m_scroll_x;
	required_shared_ptr<UINT8> m_scroll_dir;

	/* video-related */
	tilemap_t      *m_bg_tilemap;

	/* misc */
	UINT8        m_irq_enable;
	UINT8        m_nmi_enable;
	DECLARE_WRITE8_MEMBER(ctrl_w);
	DECLARE_WRITE8_MEMBER(jailbrek_videoram_w);
	DECLARE_WRITE8_MEMBER(jailbrek_colorram_w);
	DECLARE_READ8_MEMBER(jailbrek_speech_r);
	DECLARE_WRITE8_MEMBER(jailbrek_speech_w);
	DECLARE_DRIVER_INIT(jailbrek);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	virtual void palette_init();
	UINT32 screen_update_jailbrek(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(jb_interrupt);
	INTERRUPT_GEN_MEMBER(jb_interrupt_nmi);
};
