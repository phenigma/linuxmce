/*************************************************************************

    audio\qix.c

*************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "cpu/m6809/m6809.h"
#include "includes/qix.h"
#include "sound/sn76496.h"
#include "sound/discrete.h"

/* Discrete Sound Input Nodes */
#define QIX_DAC_DATA        NODE_01
#define QIX_VOL_DATA        NODE_02
#define QIX_VOL_DATA_L      NODE_03
#define QIX_VOL_DATA_R      NODE_04






/***************************************************************************
Audio handlers
***************************************************************************/

WRITE8_MEMBER(qix_state::qix_dac_w)
{
	discrete_sound_w(m_discrete, space, QIX_DAC_DATA, data);
}

WRITE8_MEMBER(qix_state::qix_vol_w)
{
	discrete_sound_w(m_discrete, space, QIX_VOL_DATA, data);
}


/************************************************************************/
/* qix Sound System Analog emulation                                    */
/************************************************************************/
/*
 * This hardware is capable of independent L/R volume control,
 * but only sdungeon uses it for a stereo effect.
 * Other games just use it for fixed L/R volume control.
 *
 * This is such a basic sound system that there is only one effect.
 * So I won't bother keeping proper voltages levels, and will just
 * start with the final gain.
 */

static const discrete_comp_adder_table qix_attn_table =
{
	DISC_COMP_P_RESISTOR, 0, 4,
	{RES_K(22)+250, RES_K(10)+250, RES_K(5.6)+250, RES_K(3.3)+250}
};

static DISCRETE_SOUND_START(qix)
	/*                    NODE                      */
	DISCRETE_INPUTX_DATA(QIX_DAC_DATA, 128, -128*128, 128)
	DISCRETE_INPUT_DATA (QIX_VOL_DATA)

	/* Separate the two 4-bit channels. */
	DISCRETE_TRANSFORM3(QIX_VOL_DATA_L, QIX_VOL_DATA, 16, 0x0f, "01/2&")
	DISCRETE_TRANSFORM2(QIX_VOL_DATA_R, QIX_VOL_DATA, 0x0f, "01&")

	/* Work out the parallel resistance of the selected resistors. */
	DISCRETE_COMP_ADDER(NODE_10, QIX_VOL_DATA_L, &qix_attn_table)
	DISCRETE_COMP_ADDER(NODE_20, QIX_VOL_DATA_R, &qix_attn_table)

	/* Then use it for the resistor divider network. */
	DISCRETE_TRANSFORM3(NODE_11, NODE_10, RES_K(10), QIX_DAC_DATA, "001+/2*")
	DISCRETE_TRANSFORM3(NODE_21, NODE_20, RES_K(10), QIX_DAC_DATA, "001+/2*")

	/* If no resistors are selected (0), then the volume is full. */
	DISCRETE_SWITCH(NODE_12, 1, QIX_VOL_DATA_L, QIX_DAC_DATA, NODE_11)
	DISCRETE_SWITCH(NODE_22, 1, QIX_VOL_DATA_R, QIX_DAC_DATA, NODE_21)

	/* Filter the DC using the lowest case filter. */
	DISCRETE_CRFILTER(NODE_13, NODE_12, RES_K(1.5), CAP_U(1))
	DISCRETE_CRFILTER(NODE_23, NODE_22, RES_K(1.5), CAP_U(1))

	DISCRETE_OUTPUT(NODE_13, 1)
	DISCRETE_OUTPUT(NODE_23, 1)
DISCRETE_SOUND_END



/*************************************
 *
 *  PIA handlers
 *
 *************************************/

WRITE8_MEMBER(qix_state::sndpia_2_warning_w)
{
	popmessage("PIA 5 write!!");
}


static TIMER_CALLBACK( deferred_sndpia1_porta_w )
{
	pia6821_device *device = (pia6821_device *)ptr;
	device->porta_w(param);
}


WRITE8_MEMBER(qix_state::sync_sndpia1_porta_w)
{
	device_t *device = machine().device("sndpia1");
	/* we need to synchronize this so the sound CPU doesn't drop anything important */
	machine().scheduler().synchronize(FUNC(deferred_sndpia1_porta_w), data, (void *)downcast<pia6821_device *>(device));
}


WRITE8_MEMBER(qix_state::slither_coinctl_w)
{
	coin_lockout_w(machine(), 0, (~data >> 6) & 1);
	coin_counter_w(machine(), 0, (data >> 5) & 1);
}



/*************************************
 *
 *  IRQ generation
 *
 *************************************/

WRITE_LINE_MEMBER(qix_state::qix_pia_dint)
{
	pia6821_device *pia = downcast<pia6821_device *>(machine().device("sndpia0"));
	int combined_state = pia->irq_a_state() | pia->irq_b_state();

	/* DINT is connected to the data CPU's IRQ line */
	machine().device("maincpu")->execute().set_input_line(M6809_IRQ_LINE, combined_state ? ASSERT_LINE : CLEAR_LINE);
}


WRITE_LINE_MEMBER(qix_state::qix_pia_sint)
{
	pia6821_device *pia = downcast<pia6821_device *>(machine().device("sndpia1"));
	int combined_state = pia->irq_a_state() | pia->irq_b_state();

	/* SINT is connected to the sound CPU's IRQ line */
	machine().device("audiocpu")->execute().set_input_line(M6800_IRQ_LINE, combined_state ? ASSERT_LINE : CLEAR_LINE);
}



/*************************************
 *
 *  Audio CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( audio_map, AS_PROGRAM, 8, driver_device )
	AM_RANGE(0x0000, 0x007f) AM_RAM
	AM_RANGE(0x2000, 0x2003) AM_MIRROR(0x5ffc) AM_DEVREADWRITE("sndpia2", pia6821_device, read, write)
	AM_RANGE(0x4000, 0x4003) AM_MIRROR(0x3ffc) AM_DEVREADWRITE("sndpia1", pia6821_device, read, write)
	AM_RANGE(0xd000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  PIA interfaces
 *
 *************************************/

static const pia6821_interface qixsnd_pia_0_intf =
{
	DEVCB_NULL,     /* port A in */
	DEVCB_NULL,     /* port B in */
	DEVCB_NULL,     /* line CA1 in */
	DEVCB_NULL,     /* line CB1 in */
	DEVCB_NULL,     /* line CA2 in */
	DEVCB_NULL,     /* line CB2 in */
	DEVCB_DRIVER_MEMBER(qix_state,sync_sndpia1_porta_w),            /* port A out */
	DEVCB_DRIVER_MEMBER(qix_state,qix_vol_w),                   /* port B out */
	DEVCB_DEVICE_LINE_MEMBER("sndpia1", pia6821_device, ca1_w),     /* line CA2 out */
	DEVCB_DRIVER_MEMBER(qix_state,qix_flip_screen_w),                               /* port CB2 out */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_dint),                                       /* IRQA */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_dint)                                        /* IRQB */
};

static const pia6821_interface qixsnd_pia_1_intf =
{
	DEVCB_NULL,     /* port A in */
	DEVCB_NULL,     /* port B in */
	DEVCB_NULL,     /* line CA1 in */
	DEVCB_NULL,     /* line CB1 in */
	DEVCB_NULL,     /* line CA2 in */
	DEVCB_NULL,     /* line CB2 in */
	DEVCB_DEVICE_MEMBER("sndpia0", pia6821_device, porta_w),            /* port A out */
	DEVCB_DRIVER_MEMBER(qix_state,qix_dac_w),               /* port B out */
	DEVCB_DEVICE_LINE_MEMBER("sndpia0", pia6821_device, ca1_w), /* line CA2 out */
	DEVCB_NULL,     /* line CB2 out */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_sint),                               /* IRQA */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_sint)                                /* IRQB */
};

static const pia6821_interface qixsnd_pia_2_intf =
{
	DEVCB_NULL,     /* port A in */
	DEVCB_NULL,     /* port B in */
	DEVCB_NULL,     /* line CA1 in */
	DEVCB_NULL,     /* line CB1 in */
	DEVCB_NULL,     /* line CA2 in */
	DEVCB_NULL,     /* line CB2 in */
	DEVCB_DRIVER_MEMBER(qix_state,sndpia_2_warning_w),  /* port A out */
	DEVCB_DRIVER_MEMBER(qix_state,sndpia_2_warning_w),  /* port B out */
	DEVCB_DRIVER_MEMBER(qix_state,sndpia_2_warning_w),  /* line CA2 out */
	DEVCB_DRIVER_MEMBER(qix_state,sndpia_2_warning_w),  /* line CB2 out */
	DEVCB_NULL,
	DEVCB_NULL
};

static const pia6821_interface slithersnd_pia_0_intf =
{
	DEVCB_INPUT_PORT("P2"),     /* port A in */
	DEVCB_NULL,     /* port B in */
	DEVCB_NULL,     /* line CA1 in */
	DEVCB_NULL,     /* line CB1 in */
	DEVCB_NULL,     /* line CA2 in */
	DEVCB_NULL,     /* line CB2 in */
	DEVCB_NULL,     /* port A out */
	DEVCB_DRIVER_MEMBER(qix_state,slither_coinctl_w),   /* port B out */
	DEVCB_NULL,     /* line CA2 out */
	DEVCB_DRIVER_MEMBER(qix_state,qix_flip_screen_w),   /* port CB2 out */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_dint),           /* IRQA */
	DEVCB_DRIVER_LINE_MEMBER(qix_state,qix_pia_dint)            /* IRQB */
};


//-------------------------------------------------
//  sn76496_config psg_intf
//-------------------------------------------------

static const sn76496_config psg_intf =
{
	DEVCB_NULL
};


/*************************************
 *
 *  Machine drivers
 *
 *************************************/

MACHINE_CONFIG_FRAGMENT( qix_audio )
	MCFG_CPU_ADD("audiocpu", M6802, SOUND_CLOCK_OSC/2)      /* 0.92 MHz */
	MCFG_CPU_PROGRAM_MAP(audio_map)

	MCFG_PIA6821_ADD("sndpia0", qixsnd_pia_0_intf)
	MCFG_PIA6821_ADD("sndpia1", qixsnd_pia_1_intf)
	MCFG_PIA6821_ADD("sndpia2", qixsnd_pia_2_intf)

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("discrete", DISCRETE, 0)
	MCFG_SOUND_CONFIG_DISCRETE(qix)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END


MACHINE_CONFIG_FRAGMENT( slither_audio )
	MCFG_PIA6821_ADD("sndpia0", slithersnd_pia_0_intf)

	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("sn1", SN76489, SLITHER_CLOCK_OSC/4/4)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
	MCFG_SOUND_CONFIG(psg_intf)

	MCFG_SOUND_ADD("sn2", SN76489, SLITHER_CLOCK_OSC/4/4)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
	MCFG_SOUND_CONFIG(psg_intf)
MACHINE_CONFIG_END
