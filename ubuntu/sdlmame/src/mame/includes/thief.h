struct coprocessor_t {
	UINT8 *context_ram;
	UINT8 bank;
	UINT8 *image_ram;
	UINT8 param[0x9];
};

class thief_state : public driver_device
{
public:
	thief_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	UINT8 *m_videoram;
	UINT8 m_input_select;
	UINT8 m_read_mask;
	UINT8 m_write_mask;
	UINT8 m_video_control;
	coprocessor_t m_coprocessor;
	DECLARE_WRITE8_MEMBER(thief_input_select_w);
	DECLARE_READ8_MEMBER(thief_io_r);
	DECLARE_READ8_MEMBER(thief_context_ram_r);
	DECLARE_WRITE8_MEMBER(thief_context_ram_w);
	DECLARE_WRITE8_MEMBER(thief_context_bank_w);
	DECLARE_WRITE8_MEMBER(thief_video_control_w);
	DECLARE_WRITE8_MEMBER(thief_color_map_w);
	DECLARE_WRITE8_MEMBER(thief_color_plane_w);
	DECLARE_READ8_MEMBER(thief_videoram_r);
	DECLARE_WRITE8_MEMBER(thief_videoram_w);
	DECLARE_WRITE8_MEMBER(thief_blit_w);
	DECLARE_READ8_MEMBER(thief_coprocessor_r);
	DECLARE_WRITE8_MEMBER(thief_coprocessor_w);
	DECLARE_WRITE8_MEMBER(tape_control_w);
	DECLARE_DRIVER_INIT(thief);
	virtual void video_start();
	UINT32 screen_update_thief(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(thief_interrupt);
};
