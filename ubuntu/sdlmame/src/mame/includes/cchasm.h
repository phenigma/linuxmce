/*************************************************************************

    Cinematronics Cosmic Chasm hardware

*************************************************************************/

#include "machine/z80ctc.h"

class cchasm_state : public driver_device
{
public:
	cchasm_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_ram(*this, "ram"){ }

	int m_sound_flags;
	int m_coin_flag;
	z80ctc_device *m_ctc;
	int m_channel_active[2];
	int m_output[2];
	required_shared_ptr<UINT16> m_ram;
	int m_xcenter;
	int m_ycenter;
	DECLARE_WRITE16_MEMBER(cchasm_led_w);
	DECLARE_WRITE16_MEMBER(cchasm_refresh_control_w);
	DECLARE_WRITE8_MEMBER(cchasm_reset_coin_flag_w);
	DECLARE_READ8_MEMBER(cchasm_coin_sound_r);
	DECLARE_READ8_MEMBER(cchasm_soundlatch2_r);
	DECLARE_WRITE8_MEMBER(cchasm_soundlatch4_w);
	DECLARE_WRITE16_MEMBER(cchasm_io_w);
	DECLARE_READ16_MEMBER(cchasm_io_r);
	INPUT_CHANGED_MEMBER(cchasm_set_coin_flag);
	DECLARE_WRITE_LINE_MEMBER(cchasm_6840_irq);
	virtual void video_start();
	TIMER_CALLBACK_MEMBER(cchasm_refresh_end);
	DECLARE_WRITE_LINE_MEMBER(ctc_timer_1_w);
	DECLARE_WRITE_LINE_MEMBER(ctc_timer_2_w);
};

/*----------- defined in audio/cchasm.c -----------*/

extern const z80ctc_interface cchasm_ctc_intf;
SOUND_START( cchasm );
