/***************************************************************************

Atari Tank 8 driver

***************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "includes/tank8.h"
#include "sound/discrete.h"



void tank8_set_collision(running_machine &machine, int index)
{
	tank8_state *state = machine.driver_data<tank8_state>();
	machine.device("maincpu")->execute().set_input_line(0, ASSERT_LINE);

	state->m_collision_index = index;
}


void tank8_state::machine_reset()
{
	m_collision_index = 0;
}


READ8_MEMBER(tank8_state::tank8_collision_r)
{
	return m_collision_index;
}

WRITE8_MEMBER(tank8_state::tank8_lockout_w)
{
	coin_lockout_w(machine(), offset, ~data & 1);
}


WRITE8_MEMBER(tank8_state::tank8_int_reset_w)
{
	m_collision_index &= ~0x3f;

	machine().device("maincpu")->execute().set_input_line(0, CLEAR_LINE);
}

WRITE8_MEMBER(tank8_state::tank8_crash_w)
{
	device_t *device = machine().device("discrete");
	discrete_sound_w(device, space, TANK8_CRASH_EN, data);
}

WRITE8_MEMBER(tank8_state::tank8_explosion_w)
{
	device_t *device = machine().device("discrete");
	discrete_sound_w(device, space, TANK8_EXPLOSION_EN, data);
}

WRITE8_MEMBER(tank8_state::tank8_bugle_w)
{
	device_t *device = machine().device("discrete");
	discrete_sound_w(device, space, TANK8_BUGLE_EN, data);
}

WRITE8_MEMBER(tank8_state::tank8_bug_w)
{
	device_t *device = machine().device("discrete");
	/* D0 and D1 determine the on/off time off the square wave */
	switch(data & 3) {
		case 0:
			discrete_sound_w(device, space, TANK8_BUGLE_DATA1,8.0);
			discrete_sound_w(device, space, TANK8_BUGLE_DATA2,4.0);
			break;
		case 1:
			discrete_sound_w(device, space, TANK8_BUGLE_DATA1,8.0);
			discrete_sound_w(device, space, TANK8_BUGLE_DATA2,7.0);
			break;
		case 2:
			discrete_sound_w(device, space, TANK8_BUGLE_DATA1,8.0);
			discrete_sound_w(device, space, TANK8_BUGLE_DATA2,2.0);
			break;
		case 3:
			discrete_sound_w(device, space, TANK8_BUGLE_DATA1,16.0);
			discrete_sound_w(device, space, TANK8_BUGLE_DATA2,4.0);
			break;
	}

}

WRITE8_MEMBER(tank8_state::tank8_attract_w)
{
	device_t *device = machine().device("discrete");
	discrete_sound_w(device, space, TANK8_ATTRACT_EN, data);
}

WRITE8_MEMBER(tank8_state::tank8_motor_w)
{
	device_t *device = machine().device("discrete");
	discrete_sound_w(device, space, NODE_RELATIVE(TANK8_MOTOR1_EN, offset), data);
}

static ADDRESS_MAP_START( tank8_cpu_map, AS_PROGRAM, 8, tank8_state )
	AM_RANGE(0x0000, 0x00ff) AM_RAM
	AM_RANGE(0x0400, 0x17ff) AM_ROM
	AM_RANGE(0xf800, 0xffff) AM_ROM

	AM_RANGE(0x1c00, 0x1c00) AM_READ(tank8_collision_r)

	AM_RANGE(0x1c01, 0x1c01) AM_READ_PORT("P1")
	AM_RANGE(0x1c02, 0x1c02) AM_READ_PORT("P2")
	AM_RANGE(0x1c03, 0x1c03) AM_READ_PORT("P3")
	AM_RANGE(0x1c04, 0x1c04) AM_READ_PORT("P4")
	AM_RANGE(0x1c05, 0x1c05) AM_READ_PORT("P5")
	AM_RANGE(0x1c06, 0x1c06) AM_READ_PORT("P6")
	AM_RANGE(0x1c07, 0x1c07) AM_READ_PORT("P7")
	AM_RANGE(0x1c08, 0x1c08) AM_READ_PORT("P8")
	AM_RANGE(0x1c09, 0x1c09) AM_READ_PORT("DSW1")
	AM_RANGE(0x1c0a, 0x1c0a) AM_READ_PORT("DSW2")
	AM_RANGE(0x1c0b, 0x1c0b) AM_READ_PORT("RC")
	AM_RANGE(0x1c0f, 0x1c0f) AM_READ_PORT("VBLANK")

	AM_RANGE(0x1800, 0x1bff) AM_WRITE(tank8_video_ram_w) AM_SHARE("video_ram")
	AM_RANGE(0x1c00, 0x1c0f) AM_WRITEONLY AM_SHARE("pos_h_ram")
	AM_RANGE(0x1c10, 0x1c1f) AM_WRITEONLY AM_SHARE("pos_v_ram")
	AM_RANGE(0x1c20, 0x1c2f) AM_WRITEONLY AM_SHARE("pos_d_ram")

	AM_RANGE(0x1c30, 0x1c37) AM_WRITE(tank8_lockout_w)
	AM_RANGE(0x1d00, 0x1d00) AM_WRITE(tank8_int_reset_w)
	AM_RANGE(0x1d01, 0x1d01) AM_WRITE(tank8_crash_w)
	AM_RANGE(0x1d02, 0x1d02) AM_WRITE(tank8_explosion_w)
	AM_RANGE(0x1d03, 0x1d03) AM_WRITE(tank8_bugle_w)
	AM_RANGE(0x1d04, 0x1d04) AM_WRITE(tank8_bug_w)
	AM_RANGE(0x1d05, 0x1d05) AM_WRITEONLY AM_SHARE("team")
	AM_RANGE(0x1d06, 0x1d06) AM_WRITE(tank8_attract_w)
	AM_RANGE(0x1e00, 0x1e07) AM_WRITE(tank8_motor_w)

ADDRESS_MAP_END


static INPUT_PORTS_START( tank8 )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)

	PORT_START("P4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)

	PORT_START("P5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN5 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(5)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(5)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(5)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(5)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(5)

	PORT_START("P6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN6 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(6)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(6)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(6)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(6)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(6)

	PORT_START("P7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN7 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(7)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(7)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(7)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(7)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(7)

	PORT_START("P8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN8 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_2WAY PORT_PLAYER(8)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_2WAY PORT_PLAYER(8)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_2WAY PORT_PLAYER(8)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_2WAY PORT_PLAYER(8)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(8)

	/* play time is 4351 + N * 640 frames */
	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x08, "Play Time" )
	PORT_DIPSETTING(    0x0f, "73 seconds" )
	PORT_DIPSETTING(    0x0e, "83 seconds" )
	PORT_DIPSETTING(    0x0d, "94 seconds" )
	PORT_DIPSETTING(    0x0c, "105 seconds" )
	PORT_DIPSETTING(    0x0b, "115 seconds" )
	PORT_DIPSETTING(    0x0a, "126 seconds" )
	PORT_DIPSETTING(    0x09, "137 seconds" )
	PORT_DIPSETTING(    0x08, "147 seconds" )
	PORT_DIPSETTING(    0x07, "158 seconds" )
	PORT_DIPSETTING(    0x06, "169 seconds" )
	PORT_DIPSETTING(    0x05, "179 seconds" )
	PORT_DIPSETTING(    0x04, "190 seconds" )
	PORT_DIPSETTING(    0x03, "201 seconds" )
	PORT_DIPSETTING(    0x02, "211 seconds" )
	PORT_DIPSETTING(    0x01, "222 seconds" )
	PORT_DIPSETTING(    0x00, "233 seconds" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, "Remote" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Team" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Remote Start" )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Remote Team" )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("RC")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 1" )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 2" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 3" )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 4" )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 5" )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 6" )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 7" )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "RC 8" )

	PORT_START("VBLANK")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("CRASH")
	PORT_ADJUSTER( 50, "Crash, Explosion Volume" )
INPUT_PORTS_END


static const gfx_layout tile_layout_1 =
{
	16, 16,
	64,
	1,
	{ 0 },
	{
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0
	},
	{
		0x000, 0x000, 0x200, 0x200, 0x400, 0x400, 0x600, 0x600,
		0x800, 0x800, 0xa00, 0xa00, 0xc00, 0xc00, 0xe00, 0xe00
	},
	8
};


static const gfx_layout tile_layout_2 =
{
	16, 16,
	64,
	1,
	{ 0 },
	{
		0x000, 0x000, 0x200, 0x200, 0x400, 0x400, 0x600, 0x600,
		0x800, 0x800, 0xa00, 0xa00, 0xc00, 0xc00, 0xe00, 0xe00

	},
	{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7
	},
	8
};


static const gfx_layout tank_layout =
{
	16, 16,
	4,
	1,
	{ 0 },
	{
		0x07, 0x06, 0x05, 0x04, 0x0f, 0x0e, 0x0d, 0x0c,
		0x17, 0x16, 0x15, 0x14, 0x1f, 0x1e, 0x1d, 0x1c
	},
	{
		0x000, 0x400, 0x020, 0x420, 0x040, 0x440, 0x060, 0x460,
		0x080, 0x480, 0x0a0, 0x4a0, 0x0c0, 0x4c0, 0x0e0, 0x4e0
	},
	0x100
};


static GFXDECODE_START( tank8 )
	GFXDECODE_ENTRY( "gfx1", 0, tile_layout_1, 0, 10 )
	GFXDECODE_ENTRY( "gfx1", 0, tile_layout_2, 0, 10 )
	GFXDECODE_ENTRY( "gfx2", 0, tank_layout,   0, 8 )
	GFXDECODE_ENTRY( "gfx3", 0, tank_layout,   0, 8 )
GFXDECODE_END


static MACHINE_CONFIG_START( tank8, tank8_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, 11055000 / 10) /* ? */
	MCFG_CPU_PROGRAM_MAP(tank8_cpu_map)


	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_UPDATE_AFTER_VBLANK)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(30 * 1000000 / 15681))
	MCFG_SCREEN_SIZE(512, 524)
	MCFG_SCREEN_VISIBLE_AREA(16, 495, 0, 463)
	MCFG_SCREEN_UPDATE_DRIVER(tank8_state, screen_update_tank8)
	MCFG_SCREEN_VBLANK_DRIVER(tank8_state, screen_eof_tank8)

	MCFG_GFXDECODE(tank8)
	MCFG_PALETTE_LENGTH(20)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("discrete", DISCRETE, 0)
	MCFG_SOUND_CONFIG_DISCRETE(tank8)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_CONFIG_END


ROM_START( tank8a )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "5071.c1",     0x10400, 0x0800, CRC(8e756e9e) BAD_DUMP SHA1(02ba64cae708967c39070b7e96085f41ed381fd4) )
	ROM_LOAD( "5072.e1",     0x10c00, 0x0800, CRC(88d65e48) BAD_DUMP SHA1(377b3c9f838bd515f8ddb55feda44398e1922521) )
	ROM_RELOAD(              0x1f800, 0x0800 )
	ROM_LOAD( "5073.f1",     0x11400, 0x0200, CRC(deedbe7c) SHA1(686c498d4c87054e00a9cfc0588e49f72470e1f9) )
	ROM_LOAD( "5074.j1",     0x11600, 0x0200, CRC(e49098b4) SHA1(5019ab0d4f3cd22733a60f15a54e4b772f534430) )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "5075.n6",     0x0000, 0x0200, CRC(2d6519b3) SHA1(3837687893d0fca683ff9b86b335a77d98fd4230) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* tanks #1 */
	ROM_LOAD( "5079.h5",     0x0000, 0x0100, CRC(5c32d471) SHA1(983c7f15ad3a50ab87157b6894b9c292358de5a1) )

	ROM_REGION( 0x0100, "gfx3", 0 ) /* tanks #2 */
	ROM_LOAD( "5078.j5",     0x0000, 0x0100, CRC(ab083245) SHA1(e084627a4a17dd274d31638c938a04aa5049359b) )

	ROM_REGION( 0x0100, "user1", 0 ) /* decode PROMs */
	ROM_LOAD_NIB_LOW ( "5077.k1", 0x0000, 0x0100, CRC(343fc116) SHA1(ac9e95fc2a5dc115e8158d69482882072bea972b) )
	ROM_LOAD_NIB_HIGH( "5076.l1", 0x0000, 0x0100, CRC(4e1d9dad) SHA1(dc467914cbc8fd1add2d5e3b988e9596037b3a1e) )
ROM_END


ROM_START( tank8b )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "5071.c1",     0x10400, 0x0800, CRC(8e756e9e) BAD_DUMP SHA1(02ba64cae708967c39070b7e96085f41ed381fd4) )
	ROM_LOAD( "5072.e1",     0x10c00, 0x0800, CRC(88d65e48) BAD_DUMP SHA1(377b3c9f838bd515f8ddb55feda44398e1922521) )
	ROM_RELOAD(              0x1f800, 0x0800 )
	ROM_LOAD( "5764.f1",     0x11400, 0x0200, CRC(da4e81e0) SHA1(7e504b7064a8fed9dd7a6fe013f22427edc52384) )
	ROM_LOAD( "5765.j1",     0x11600, 0x0200, CRC(537e0c75) SHA1(b35db8d316cef9a2603b64868023e72e21d03143) )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "5075.n6",     0x0000, 0x0200, CRC(2d6519b3) SHA1(3837687893d0fca683ff9b86b335a77d98fd4230) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* tanks #1 */
	ROM_LOAD( "5079.h5",     0x0000, 0x0100, CRC(5c32d471) SHA1(983c7f15ad3a50ab87157b6894b9c292358de5a1) )

	ROM_REGION( 0x0100, "gfx3", 0 ) /* tanks #2 */
	ROM_LOAD( "5078.j5",     0x0000, 0x0100, CRC(ab083245) SHA1(e084627a4a17dd274d31638c938a04aa5049359b) )

	ROM_REGION( 0x0100, "user1", 0 ) /* decode PROMs */
	ROM_LOAD_NIB_LOW ( "5077.k1", 0x0000, 0x0100, CRC(343fc116) SHA1(ac9e95fc2a5dc115e8158d69482882072bea972b) )
	ROM_LOAD_NIB_HIGH( "5076.l1", 0x0000, 0x0100, CRC(4e1d9dad) SHA1(dc467914cbc8fd1add2d5e3b988e9596037b3a1e) )
ROM_END


ROM_START( tank8c )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "a05071.c1",   0x0400, 0x0800, /* wrong revision */ CRC(2211fb2c) SHA1(6d44d1e9c94a7cb364c8809445c015a6510b8c5f) )
	ROM_LOAD( "a05072.e1",   0x0c00, 0x0800, CRC(d907b116) SHA1(290a77e6095d4ffc2365d784e74e115fe90617fb) )
	ROM_RELOAD(              0xf800, 0x0800 )
	ROM_LOAD( "a05473.f1",   0x1400, 0x0200, CRC(109020db) SHA1(c8517a09c1ff36a1b08c4a71acaae55dc48fc45b) )
	ROM_LOAD( "a05474.j1",   0x1600, 0x0200, CRC(64d8a386) SHA1(fdb6669762ddebbd775c66d67d82876428dfe009) )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "5075.n6",     0x0000, 0x0200, CRC(2d6519b3) SHA1(3837687893d0fca683ff9b86b335a77d98fd4230) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* tanks #1 */
	ROM_LOAD( "5079.h5",     0x0000, 0x0100, CRC(5c32d471) SHA1(983c7f15ad3a50ab87157b6894b9c292358de5a1) )

	ROM_REGION( 0x0100, "gfx3", 0 ) /* tanks #2 */
	ROM_LOAD( "5078.j5",     0x0000, 0x0100, CRC(ab083245) SHA1(e084627a4a17dd274d31638c938a04aa5049359b) )
ROM_END


ROM_START( tank8d )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "a05071.c1",   0x0400, 0x0800, /* wrong revision */ CRC(2211fb2c) SHA1(6d44d1e9c94a7cb364c8809445c015a6510b8c5f) )
	ROM_LOAD( "a05072.e1",   0x0c00, 0x0800, CRC(d907b116) SHA1(290a77e6095d4ffc2365d784e74e115fe90617fb) )
	ROM_RELOAD(              0xf800, 0x0800 )
	ROM_LOAD( "a05918.f1",   0x1400, 0x0200, CRC(c2fca931) SHA1(a5aea79ef560d4498ba38482260f49a09cfe59d6) )
	ROM_LOAD( "a05919.j1",   0x1600, 0x0200, CRC(47204dc0) SHA1(b602e26b615e5e8bb747e9bd3879e4b95d923dc1) )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "5075.n6",     0x0000, 0x0200, CRC(2d6519b3) SHA1(3837687893d0fca683ff9b86b335a77d98fd4230) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* tanks #1 */
	ROM_LOAD( "5079.h5",     0x0000, 0x0100, CRC(5c32d471) SHA1(983c7f15ad3a50ab87157b6894b9c292358de5a1) )

	ROM_REGION( 0x0100, "gfx3", 0 ) /* tanks #2 */
	ROM_LOAD( "5078.j5",     0x0000, 0x0100, CRC(ab083245) SHA1(e084627a4a17dd274d31638c938a04aa5049359b) )
ROM_END


ROM_START( tank8 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b05475.c1",   0x0400, 0x0800, CRC(62a00e75) SHA1(58d80dc58bc2a4503348807db578348fc76a5349) )
	ROM_LOAD( "a05072.e1",   0x0c00, 0x0800, CRC(d907b116) SHA1(290a77e6095d4ffc2365d784e74e115fe90617fb) )
	ROM_RELOAD(              0xf800, 0x0800 )
	ROM_LOAD( "b05476.f1",   0x1400, 0x0200, CRC(98754edd) SHA1(56eb017bad9c29649573875a6b13189f2ba69b0e) )
	ROM_LOAD( "b05477.j1",   0x1600, 0x0200, CRC(5087223b) SHA1(fea032e6d0b3e0730a1180e57118e1765693f67e) )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "5075.n6",     0x0000, 0x0200, CRC(2d6519b3) SHA1(3837687893d0fca683ff9b86b335a77d98fd4230) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* tanks #1 */
	ROM_LOAD( "5079.h5",     0x0000, 0x0100, CRC(5c32d471) SHA1(983c7f15ad3a50ab87157b6894b9c292358de5a1) )

	ROM_REGION( 0x0100, "gfx3", 0 ) /* tanks #2 */
	ROM_LOAD( "5078.j5",     0x0000, 0x0100, CRC(ab083245) SHA1(e084627a4a17dd274d31638c938a04aa5049359b) )
ROM_END


DRIVER_INIT_MEMBER(tank8_state,decode)
{
	const UINT8* DECODE = machine().root_device().memregion("user1")->base();

	UINT8* p1 = machine().root_device().memregion("maincpu")->base() + 0x00000;
	UINT8* p2 = machine().root_device().memregion("maincpu")->base() + 0x10000;

	int i;

	for (i = 0x0400; i <= 0x17ff; i++)
	{
		p1[i] = DECODE[p2[i]];
	}
	for (i = 0xf800; i <= 0xffff; i++)
	{
		p1[i] = DECODE[p2[i]];
	}
}


GAME( 1976, tank8,    0,        tank8,    tank8, driver_device,    0,         ROT0, "Atari (Kee Games)", "Tank 8 (set 1)", 0)
GAME( 1976, tank8a,   tank8,    tank8,    tank8, tank8_state,    decode,   ROT0, "Atari (Kee Games)", "Tank 8 (set 2)",  GAME_NOT_WORKING )
GAME( 1976, tank8b,   tank8,    tank8,    tank8, tank8_state,    decode,   ROT0, "Atari (Kee Games)", "Tank 8 (set 3)",  GAME_NOT_WORKING )
GAME( 1976, tank8c,   tank8,    tank8,    tank8, driver_device,    0,        ROT0, "Atari (Kee Games)", "Tank 8 (set 4)",  GAME_NOT_WORKING )
GAME( 1976, tank8d,   tank8,    tank8,    tank8, driver_device,    0,        ROT0, "Atari (Kee Games)", "Tank 8 (set 5)",  GAME_NOT_WORKING )
