/******************************************************************************

  Exidy Sorcerer machine functions

*******************************************************************************/

#include "includes/sorcerer.h"

#if SORCERER_USING_RS232

/* The serial code (which was never connected to the outside) is disabled for now. */

/* timer for sorcerer serial chip transmit and receive */

TIMER_CALLBACK_MEMBER(sorcerer_state::sorcerer_serial_tc)
{
	/* if rs232 is enabled, uart is connected to clock defined by bit6 of port fe.
	Transmit and receive clocks are connected to the same clock */

	/* if rs232 is disabled, receive clock is linked to cassette hardware */
	if (m_fe & 0x80)
	{
		/* connect to rs232 */
	}
}
#endif


/* timer to read cassette waveforms */


static cassette_image_device *cassette_device_image(running_machine &machine)
{
	sorcerer_state *state = machine.driver_data<sorcerer_state>();
	if (state->m_fe & 0x20)
		return machine.device<cassette_image_device>(CASSETTE2_TAG);
	else
		return machine.device<cassette_image_device>(CASSETTE_TAG);
}



TIMER_CALLBACK_MEMBER(sorcerer_state::sorcerer_cassette_tc)
{
	UINT8 cass_ws = 0;
	switch (m_fe & 0xc0)        /*/ bit 7 low indicates cassette */
	{
		case 0x00:              /* Cassette 300 baud */

			/* loading a tape - this is basically the same as the super80.
			               We convert the 1200/2400 Hz signal to a 0 or 1, and send it to the uart. */

			m_cass_data.input.length++;

			cass_ws = ((cassette_device_image(machine()))->input() > +0.02) ? 1 : 0;

			if (cass_ws != m_cass_data.input.level)
			{
				m_cass_data.input.level = cass_ws;
				m_cass_data.input.bit = ((m_cass_data.input.length < 0x6) || (m_cass_data.input.length > 0x20)) ? 1 : 0;
				m_cass_data.input.length = 0;
				ay31015_set_input_pin( m_uart, AY31015_SI, m_cass_data.input.bit );
			}

			/* saving a tape - convert the serial stream from the uart, into 1200 and 2400 Hz frequencies.
			               Synchronisation of the frequency pulses to the uart is extremely important. */

			m_cass_data.output.length++;
			if (!(m_cass_data.output.length & 0x1f))
			{
				cass_ws = ay31015_get_output_pin( m_uart, AY31015_SO );
				if (cass_ws != m_cass_data.output.bit)
				{
					m_cass_data.output.bit = cass_ws;
					m_cass_data.output.length = 0;
				}
			}

			if (!(m_cass_data.output.length & 3))
			{
				if (!((m_cass_data.output.bit == 0) && (m_cass_data.output.length & 4)))
				{
					m_cass_data.output.level ^= 1;          // toggle output this, except on 2nd half of low bit
					cassette_device_image(machine())->output(m_cass_data.output.level ? -1.0 : +1.0);
				}
			}
			return;

		case 0x40:          /* Cassette 1200 baud */
			/* loading a tape */
			m_cass_data.input.length++;

			cass_ws = ((cassette_device_image(machine()))->input() > +0.02) ? 1 : 0;

			if (cass_ws != m_cass_data.input.level || m_cass_data.input.length == 10)
			{
				m_cass_data.input.bit = ((m_cass_data.input.length < 10) || (m_cass_data.input.length > 0x20)) ? 1 : 0;
				if ( cass_ws != m_cass_data.input.level )
				{
					m_cass_data.input.length = 0;
					m_cass_data.input.level = cass_ws;
				}
				ay31015_set_input_pin( m_uart, AY31015_SI, m_cass_data.input.bit );
			}

			/* saving a tape - convert the serial stream from the uart, into 600 and 1200 Hz frequencies. */

			m_cass_data.output.length++;
			if (!(m_cass_data.output.length & 7))
			{
				cass_ws = ay31015_get_output_pin( m_uart, AY31015_SO );
				if (cass_ws != m_cass_data.output.bit)
				{
					m_cass_data.output.bit = cass_ws;
					m_cass_data.output.length = 0;
				}
			}

			if (!(m_cass_data.output.length & 7))
			{
				if (!((m_cass_data.output.bit == 0) && (m_cass_data.output.length & 8)))
				{
					m_cass_data.output.level ^= 1;          // toggle output this, except on 2nd half of low bit
					cassette_device_image(machine())->output(m_cass_data.output.level ? -1.0 : +1.0);
				}
			}
			return;
	}
}


/* after the first 4 bytes have been read from ROM, switch the ram back in */
TIMER_CALLBACK_MEMBER(sorcerer_state::sorcerer_reset)
{
	membank("boot")->set_entry(0);
}

WRITE8_MEMBER(sorcerer_state::sorcerer_fc_w)
{
	ay31015_set_transmit_data( m_uart, data );
}


WRITE8_MEMBER(sorcerer_state::sorcerer_fd_w)
{
	/* Translate data to control signals */

	ay31015_set_input_pin( m_uart, AY31015_CS, 0 );
	ay31015_set_input_pin( m_uart, AY31015_NB1, data & 1);
	ay31015_set_input_pin( m_uart, AY31015_NB2, (BIT(data, 1)) ? 1 : 0 );
	ay31015_set_input_pin( m_uart, AY31015_TSB, (BIT(data, 2)) ? 1 : 0 );
	ay31015_set_input_pin( m_uart, AY31015_EPS, (BIT(data, 3)) ? 1 : 0 );
	ay31015_set_input_pin( m_uart, AY31015_NP,  (BIT(data, 4)) ? 1 : 0 );
	ay31015_set_input_pin( m_uart, AY31015_CS, 1 );
}

WRITE8_MEMBER(sorcerer_state::sorcerer_fe_w)
{
	UINT8 changed_bits = (m_fe ^ data) & 0xf0;
	m_fe = data;

	/* bits 0..3 */
	m_keyboard_line = data & 0x0f;

	if (!changed_bits) return;

	/* bits 4..5 */
	/* does user want to hear the sound? */

	if (!BIT(data, 7)) // cassette operations
	{
#if SORCERER_USING_RS232
		m_serial_timer->adjust(attotime::zero);
#endif

		bool sound = BIT(m_iop_config->read(), 3);

		m_cass1->change_state(
			(BIT(data,4) & sound) ? CASSETTE_SPEAKER_ENABLED : CASSETTE_SPEAKER_MUTED, CASSETTE_MASK_SPEAKER);

		m_cass2->change_state(
			(BIT(data,5) & sound) ? CASSETTE_SPEAKER_ENABLED : CASSETTE_SPEAKER_MUTED, CASSETTE_MASK_SPEAKER);

		/* cassette 1 motor */
		m_cass1->change_state(
			(BIT(data,4)) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

		/* cassette 2 motor */
		m_cass2->change_state(
			(BIT(data,5)) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

		if (data & 0x30)
			m_cassette_timer->adjust(attotime::zero, 0, attotime::from_hz(19200));
		else
			m_cassette_timer->adjust(attotime::zero);
	}
#if SORCERER_USING_RS232
	else
	{
		m_serial_timer->adjust(attotime::zero, 0, attotime::from_hz(19200));
		m_cassette_timer->adjust(attotime::zero);
	}
#endif

	// bit 6 baud rate */
	if (BIT(changed_bits, 6))
	{
		ay31015_set_receiver_clock( m_uart, (BIT(data, 6)) ? 19200.0 : 4800.0);
		ay31015_set_transmitter_clock( m_uart, (BIT(data, 6)) ? 19200.0 : 4800.0);
	}
}

WRITE8_MEMBER(sorcerer_state::sorcerer_ff_w)
{
	/* reading the config switch */
	switch (m_iop_config->read() & 0x06)
	{
		case 0: /* speaker */
			m_dac->write_unsigned8(data);
			break;

		case 2: /* Centronics 7-bit printer */
			/* bit 7 = strobe, bit 6..0 = data */
			m_centronics->strobe_w((~data>>7) & 0x01);
			m_centronics->write(space, 0, data & 0x7f);
			break;

		case 4: /* 8-bit parallel output */
			/* hardware strobe driven from port select, bit 7..0 = data */
			m_centronics->strobe_w(1);
			m_centronics->write(space, 0, data);
			m_centronics->strobe_w(0);
			break;
	}
}

READ8_MEMBER(sorcerer_state::sorcerer_fc_r)
{
	UINT8 data = ay31015_get_received_data( m_uart );
	ay31015_set_input_pin( m_uart, AY31015_RDAV, 0 );
	ay31015_set_input_pin( m_uart, AY31015_RDAV, 1 );
	return data;
}

READ8_MEMBER(sorcerer_state::sorcerer_fd_r)
{
	/* set unused bits high */
	UINT8 data = 0xe0;

	ay31015_set_input_pin( m_uart, AY31015_SWE, 0 );
	data |= ay31015_get_output_pin( m_uart, AY31015_TBMT ) ? 0x01 : 0;
	data |= ay31015_get_output_pin( m_uart, AY31015_DAV  ) ? 0x02 : 0;
	data |= ay31015_get_output_pin( m_uart, AY31015_OR   ) ? 0x04 : 0;
	data |= ay31015_get_output_pin( m_uart, AY31015_FE   ) ? 0x08 : 0;
	data |= ay31015_get_output_pin( m_uart, AY31015_PE   ) ? 0x10 : 0;
	ay31015_set_input_pin( m_uart, AY31015_SWE, 1 );

	return data;
}

READ8_MEMBER(sorcerer_state::sorcerer_fe_r)
{
	/* bits 6..7
	 - hardware handshakes from user port
	 - not emulated
	 - tied high, allowing PARIN and PAROUT bios routines to run */

	UINT8 data = 0xc0;
	char kbdrow[6];

	sprintf(kbdrow,"X%X",m_keyboard_line);

	/* bit 5 - vsync */
	data |= m_iop_vs->read();

	/* bits 4..0 - keyboard data */
	data |= ioport(kbdrow)->read();

	return data;
}

READ8_MEMBER(sorcerer_state::sorcerer_ff_r)
{
	/* The use of the parallel port as a general purpose port is not emulated.
	Currently the only use is to read the printer status in the Centronics CENDRV bios routine.
	This uses bit 7. The other bits have been set high (=nothing plugged in).
	This fixes those games that use a joystick. */

	UINT8 data=0x7f;

	/* bit 7 = printer busy   0 = printer is not busy */

	data |= m_centronics->busy_r() << 7;

	return data;
}

/******************************************************************************
 Snapshot Handling
******************************************************************************/

SNAPSHOT_LOAD(sorcerer)
{
	device_t *cpu = image.device().machine().device("maincpu");
	UINT8 *RAM = image.device().machine().root_device().memregion(cpu->tag())->base();
	address_space &space = cpu->memory().space(AS_PROGRAM);
	UINT8 header[28];
	unsigned char s_byte;

	/* check size */
	if (snapshot_size != 0x1001c)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Snapshot must be 65564 bytes");
		image.message("Snapshot must be 65564 bytes");
		return IMAGE_INIT_FAIL;
	}

	/* get the header */
	image.fread( &header, sizeof(header));

	/* write it to ram */
	for (int i = 0; i < 0xc000; i++)
	{
		image.fread( &s_byte, 1);
		space.write_byte(i, s_byte);
	}
	image.fread( RAM+0xc000, 0x4000);

	/* patch CPU registers */
	cpu->state().set_state_int(Z80_I, header[0]);
	cpu->state().set_state_int(Z80_HL2, header[1] | (header[2] << 8));
	cpu->state().set_state_int(Z80_DE2, header[3] | (header[4] << 8));
	cpu->state().set_state_int(Z80_BC2, header[5] | (header[6] << 8));
	cpu->state().set_state_int(Z80_AF2, header[7] | (header[8] << 8));
	cpu->state().set_state_int(Z80_HL, header[9] | (header[10] << 8));
	cpu->state().set_state_int(Z80_DE, header[11] | (header[12] << 8));
	cpu->state().set_state_int(Z80_BC, header[13] | (header[14] << 8));
	cpu->state().set_state_int(Z80_IY, header[15] | (header[16] << 8));
	cpu->state().set_state_int(Z80_IX, header[17] | (header[18] << 8));
	cpu->state().set_state_int(Z80_IFF1, header[19]&2 ? 1 : 0);
	cpu->state().set_state_int(Z80_IFF2, header[19]&4 ? 1 : 0);
	cpu->state().set_state_int(Z80_R, header[20]);
	cpu->state().set_state_int(Z80_AF, header[21] | (header[22] << 8));
	cpu->state().set_state_int(STATE_GENSP, header[23] | (header[24] << 8));
	cpu->state().set_state_int(Z80_IM, header[25]);
	cpu->state().set_pc(header[26] | (header[27] << 8));

	return IMAGE_INIT_PASS;
}

void sorcerer_state::machine_start()
{
	m_cassette_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(sorcerer_state::sorcerer_cassette_tc),this));
#if SORCERER_USING_RS232
	m_serial_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(sorcerer_state::sorcerer_serial_tc),this));
#endif

	UINT16 endmem = 0xbfff;

	address_space &space = m_maincpu->space(AS_PROGRAM);
	/* configure RAM */
	switch (m_ram->size())
	{
	case 8*1024:
		space.unmap_readwrite(0x2000, endmem);
		break;

	case 16*1024:
		space.unmap_readwrite(0x4000, endmem);
		break;

	case 32*1024:
		space.unmap_readwrite(0x8000, endmem);
		break;
	}
}

MACHINE_START_MEMBER(sorcerer_state,sorcererd)
{
	m_cassette_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(sorcerer_state::sorcerer_cassette_tc),this));
#if SORCERER_USING_RS232
	m_serial_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(sorcerer_state::sorcerer_serial_tc),this));
#endif

	UINT16 endmem = 0xbbff;

	address_space &space = m_maincpu->space(AS_PROGRAM);
	/* configure RAM */
	switch (m_ram->size())
	{
	case 8*1024:
		space.unmap_readwrite(0x2000, endmem);
		break;

	case 16*1024:
		space.unmap_readwrite(0x4000, endmem);
		break;

	case 32*1024:
		space.unmap_readwrite(0x8000, endmem);
		break;
	}
}

void sorcerer_state::machine_reset()
{
	address_space &space = machine().device("maincpu")->memory().space(AS_PROGRAM);

	/* Initialize cassette interface */
	m_cass_data.output.length = 0;
	m_cass_data.output.level = 1;
	m_cass_data.input.length = 0;
	m_cass_data.input.bit = 1;

	m_fe = 0xff;
	sorcerer_fe_w(space, 0, 0, 0xff);

	membank("boot")->set_entry(1);
	machine().scheduler().timer_set(attotime::from_usec(10), timer_expired_delegate(FUNC(sorcerer_state::sorcerer_reset),this));
}
