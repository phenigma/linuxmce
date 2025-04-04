/*************************************************************************

  Taito JC System

  Driver by Ville Linde, based on the preliminary driver by David Haywood

Taito custom chips on this hardware:
- TC0640FIO      : I/O
- TC0770CMU      : Math co-processor?
- TC0780FPA x 2  : Polygon/Texture renderer?
- TC0840GLU      : 2D graphics?
- TC0870HVP      : Vertex processor?

TODO:
- dendego intro object RAM usage has various gfx bugs (check video file)
- dendego title screen builds up and it shouldn't
- dendego attract mode train doesn't ride
- landgear has some weird crashes (after playing one round, after a couple of loops in attract mode) (needs testing -AS)
- landgear has huge 3d problems on gameplay (CPU comms?)
- dangcurv DSP program crashes very soon due to undumped rom, so no 3d is currently shown.
- add idle skips if possible
- POST has a PCB ID (shown at top of screen) that can't be faked without a proper reference.

--------------------------------------------------------------------------

PCB notes:


Side By Side 2
Taito, 1997

This game runs on Taito JC System hardware

PCB Layout
----------

Top board: MOTHER PCB-C K11X0838A  M43E0325A
|---------------------------------------------------------------------------------|
|                                                                 43256           |
|54MHz                                   E23-32-1.51              43256           |
|         424210 424210                                           43256           |
|                      E23-29.39                                                  |
|E23-25-1.3 TC0870HVP             E23-31.46                                       |
|           (QFP208)   E23-30.40             E23-34.72      93C46.87              |
|E23-26.4                                                                         |
|                              MC68040RC25            CXD1178Q    TC0640FIO       |
|424260     TMS320C51          (PGA TYPE)                         (QFP120)        |
|           (QFP132)                                                              |
|           labelled                                              TEST_SW         |
|424260     "Taito E07-11"                                      MB3771   RESET_SW |
|                                  E23-33.53                                      |
|                                             CY7B991       MB8421-90             |
|4218160    43256                    CY7B991                                      |
|                                                                                 |
|           43256        TC0770CMU                          E23-35.110            |
|4218160                 (QFP208)                                                 |
|                                    10MHz    MC68EC000     LC321664AJ-80         |
|E23-27.13  TC0780FPA                                                             |
|           (QFP240)                                        ENSONIC               |
|                      D482445                TC51832       ESPR6 ES5510          |
|                                             TC51832                             |
|4218160               D482445                                                    |
|                                 TC0840GLU                 MC33274   TDA1543     |
|                      D482445    (QFP144)                                        |
|4218160                                      16MHz         MB87078               |
|           TC0780FPA  D482445           30.4761MHz                               |
|           (QFP240)              ENSONIC                                         |
|E23-28.18                        OTISR2                                          |
|                                                                                 |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.


Bottom board: JCG DAUGHTERL PCB-C K91E0677A
|---------------------------------------------------------------------------------|
|                                                                                 |
|                                                                                 |
|                    E38-19.30  E38-20.31                                         |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|  E38-01.5   E38-09.18   E23-15.32                                               |
|                                                                                 |
|  E38-02.6   E38-10.19   E38-17.33                                               |
|                                                                                 |
|  E38-03.7   E38-11.20   E38-18.34                     E17-23.65                 |
|                                                                                 |
|  E38-04.8   E38-12.21   E38-21.35                     TC5563                    |
|                                                                                 |
|  E38-05.9   E38-13.22   SBS2_P0.36                       MC68HC11M0             |
|                                                          (QFP80)                |
|  E38-06.10  E38-14.23   SBS2_P1.37                                              |
|                                                                                 |
|  E38-07.11  E38-15.24   SBS2_P2.38                                              |
|                                                          E23-37.69              |
|  E38-08.12  E38-16.25   SBS2_P3.39                             SMC_COM20020I    |
|                                                                                 |
|                                                                                 |
|                                                      E23-38.73                  |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.
      ROMs .36-.39 are 27C4001, main program.
      ROMs .5-.7, .9-.12, .18-.20, .22-.25 are 16M MASK, graphics.
      ROMs .32-.35 are 16M MASK, ES5505 samples.
      ROMs .8 and .21 are 4M MASK, graphics.
      ROMs .30-.31 are 27C2001, 68000 sound program.
      ROM .65 is 27C512, MC68HC11 program.

----

--------------------------------------------------------------------------
SIDE BY SIDE  / 2       JC-SYSTEM TYPE-C
--------------------------------------------------------------------------
 SIDE BY SIDE (E23) (C)TAITO 1996 VER 2.5J 1996/6/20 18:13:14
 E23-01 to 24 + E17-23 (BIOS ?)   E17:LANDING GEAR

 SIDE BY SIDE 2 (E38)   Not dumped
 ROMKIT : E38-01 to 21 , 23* to 26*

--------------------------------------------------------------------------
DENSHA DE GO! (E35)     JC-SYSTEM TYPE-C with TRAIN BOARD (Ext.Sound)
--------------------------------------------------------------------------
 DENSHA DE GO! VER 2.2J 1997/2/4
 E35-01 to 26 + E35-28(TRAIN BOARD) + E17-23(BIOS?)

 DENSHA DE GO! EX VER 2.4J 1997/4/18 13:38:34
 ROMKIT : E35-30 to 33

--------------------------------------------------------------------------
DENSHA DE GO! 2 (E52)   JC-SYSTEM TYPE-C with TRAIN BOARD (Ext.Sound)
--------------------------------------------------------------------------
 DENSHA DE GO! 2 (KOUSOKUHEN RYOUSANSYA) VER 2.5 J 1998/3/2 15:30:55
 E52-01 to 24 , 25-1 to 28-1, 29, 30 + E35-28(TRAIN BOARD) + E17-23(BIOS?)

 DENSHA DE GO! 2 (3000BANDAI KOUSOKUHEN) VER 2.20 J 1998/7/15 17:42:38
 ROMKIT :  E52-31 to 38

----

Landing Gear
Taito, 1995

This is a flight simulator game running on Taito JC System hardware.
The system comprises two boards plugged together back-to-back via five high density connectors.
The top board contains the main CPU, all RAMs, graphics and sound hardware and the bottom
board contains the game ROMs, communication devices and a MC68HC11 MCU. Landing Gear
is a single player game so the board does not contain any communication hardware.
This hardware seems to be the earliest revision of the JC board. This is the only JC game that
had a 28-way edge connector, which is actually almost JAMMA. The power/GND, video and
most buttons (test/service/coin/start etc) work fine using any standard JAMMA cab. However the
analog controls are wired to another connector on the bottom PCB via an interface board.
When other JC bottom boards are swapped to this top board, they will run fine, but some of the
graphics are messed up. This is probably due to the different PALs and their changed locations.
This seems to mostly affect the texture mapping (tested with swapping Side By Side 2) as the title
and most of the background graphics are ok but the car textures are just a mess of pixels.


PCB Layout
----------

Top board: MOTHER PCB  K11X0835A  M43E0304A
|---------------------------------------------------------------------------------|
|                                                                 43256  43256    |
|54MHz                                                            43256           |
|                                                                                 |
|TC514260                                                                         |
|         TC0870HVP  uPD424210                                    TC0640FIO       |
|TC514260 (QFP208)   uPD424210             E07-08.65              (QFP120)        |
|E07-02.4                                                                         |
|                                                                                 |
|TMS320C51  43256                                                                 |
|(QFP132)   43256              MC68040RC25          E07-10.116  93C46.91          |
|labelled          TC0770CMU   (PGA TYPE)                    E07-04.115  TEST_SW  |
|"Taito E07-11"    (QFP208)                         E07-09.82      MB3771         |
|                                                                                 |
|                                                            MB8421-90            |
|TC528257   E07-06.37                                                             |
|                                                                                 |
|TC528257   TC514260                                                              |
|                              E07-07.49    CY7B991                               |
|TC528257   TC514260           E07-03.50                     TC511664             |
|                     TC0780FPA                                                   |
|TC528257   TC514260  (QFP240)           10MHz   MC68EC000   ENSONIC              |
|                                 CY7B991                    ESPR6 ES5510         |
|TC528257   TC514260                                                              |
|                                                                                 |
|TC528257   TC514260              TC0840GLU   TC51832       MC33274   TDA1543     |
|                                 (QFP144)    TC51832                             |
|TC528257   TC514260                             16MHz      MB87078               |
|                     TC0780FPA          30.4761MHz                               |
|TC528257   TC514260  (QFP240)                                                    |
|                                           ENSONIC                               |
|TC528257             E07-05.22             OTISR2                                |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.


Bottom board: JCG DAUGHTER PCB-L K91E0603A
|---------------------------------------------------------------------------------|
|                                                                                 |
|                                                                                 |
|                    E17-21.30  E17-22.31                                         |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|  E17-01.5   E17-07.18   E17-13.32                                               |
|                                                                                 |
|  *          E17-08.19   E17-14.33                                               |
|                                                                                 |
|  *          *           E17-15.34                     E17-23.65                 |
|                                                                                 |
|  E17-02.8   *           E17-16.35                     TC5563                    |
|                                                                                 |
|  E17-03.9   E17-09.22   E17-37.36                        MC68HC11M0             |
|                                                          (QFP80)                |
|  E17-04.10  E17-10.23   E17-38.37                                               |
|                                                                                 |
|  E17-05.11  E17-11.24   E17-39.38                                               |
|                                                          E09-21.69              |
|  E17-06.12  E17-12.25   E17-40.39                                               |
|                                                                                 |
|                                                                                 |
|                                                      E09-22.73                  |
|                                                                                 |
|                                                                                 |
|                                                      E17-32.96                  |
|                                                                                 |
|                                                                                 |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.
      ROMs .36-.39 are 27C4001, main program.
      ROM .65 is 27C512, 68HC11 MCU program.
      ROMs .30-.31 are 27C2001, sound program.
      ROMs .32-.35 are 16M MASK, sound.
      ROMs .5-.25 are 16M MASK, graphics.
      * Unpopulated ROM socket

----

Side By Side
Taito, 1996

This game runs on Taito JC System hardware and uses a 24kHz monitor.

PCB Layout
----------

Top board: MOTHER PCB-C K11X0838A  M43E0325A
|---------------------------------------------------------------------------------|
|                                                                 43256           |
|54MHz                                   E23-32-1.51              43256           |
|         424210 424210                                           43256           |
|                      E23-29.39                                                  |
|E23-25-1.3 TC0870HVP             E23-31.46                                       |
|           (QFP208)   E23-30.40             E23-34.72      93C46.87              |
|E23-26.4                                                                         |
|                              MC68040RC25            CXD1178Q    TC0640FIO       |
|424260     TMS320C51          (PGA TYPE)                         (QFP120)        |
|           (QFP132)                                                              |
|           labelled                                              TEST_SW         |
|424260     "Taito E07-11"                                      MB3771   RESET_SW |
|                                  E23-33.53                                      |
|                                             CY7B991       MB8421-90             |
|4218160    43256                    CY7B991                                      |
|                                                                                 |
|           43256        TC0770CMU                          E23-35.110            |
|4218160                 (QFP208)                                                 |
|                                    10MHz    MC68EC000     LC321664AJ-80         |
|E23-27.13  TC0780FPA                                                             |
|           (QFP240)                                        ENSONIC               |
|                      D482445                TC51832       ESPR6 ES5510          |
|                                             TC51832                             |
|4218160               D482445                                                    |
|                                 TC0840GLU                 MC33274   TDA1543     |
|                      D482445    (QFP144)                                        |
|4218160                                      16MHz         MB87078               |
|           TC0780FPA  D482445           30.4761MHz                               |
|           (QFP240)              ENSONIC                                         |
|E23-28.18                        OTISR2                                          |
|                                                                                 |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.
      CY7B991 : Programmable Skew Clock Buffer (PLCC32)
      4218160 : 2M x8 / 1M x16 DRAM
      424210  : 256K x16 DRAM
      424260  : 256K x16 DRAM
      43256   : 32K x8 SRAM
      D482445 : DRAM?
      LC321664: 64K x16 DRAM
      TC51832 : 32K x8 SRAM


Bottom board: JCG DAUGHTERL PCB-C K9100633A J9100434A (Sticker K91J0633A)
|---------------------------------------------------------------------------------|
|                                                                                 |
|                                                                                 |
|                    E23-23.30  E23-24.31                                         |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|  E23-01.5   E23-08.18   E23-15.32                                               |
|                                                                                 |
|  E23-02.6   E23-09.19   E23-16.33                                               |
|                                                                                 |
|  E23-03.7   E23-10.20   E23-17.34                     E17-23.65                 |
|                                                                                 |
|  E23-04.8   E23-11.21   E23-18.35                     6264                      |
|                                                                                 |
|  E23-05.9   E23-12.22   E23-19.36                        MC68HC11M0             |
|                                                          (QFP80)                |
|  E23-06.10  E23-13.23   E23-20.37                                               |
|                                                                                 |
|  *          *           E23-21.38                                               |
|                                                          E23-37.69              |
|  E23-07.12  E23-14.25   E23-22.39                              SMC_COM20020I    |
|                                                                                 |
|                                                                                 |
|                                                      E23-38.73                  |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|                                                                                 |
|---------------------------------------------------------------------------------|

Notes:
      All 3K files are PALs type PALCE 16V8 and saved in Jedec format.
      6264: 8K x8 SRAM
      SMC_COM20020I: Network communmication IC
      ROMs .36-.39 are 27C4001, main program.
      ROMs .5-.12, .18-.25 are 16M MASK, graphics.
      ROMs .32-.35 are 16M MASK, sound data.
      ROMs .30-.31 are 27C2001, sound program.
      ROM  .65 is 27C512, linked to 68HC11 MCU
      *    Unpopulated socket.

*/

#include "emu.h"
#include "cpu/tms32051/tms32051.h"
#include "cpu/m68000/m68000.h"
#include "cpu/mc68hc11/mc68hc11.h"
#include "sound/es5506.h"
#include "sound/okim6295.h"
#include "machine/eeprom.h"
#include "machine/taitoio.h"
#include "audio/taito_en.h"
#include "includes/taitojc.h"

#include "dendego.lh"


/***************************************************************************

  maincpu I/O

***************************************************************************/

#define DEBUG_DSP               0
#define DEBUG_BLOCK_MOVES       0

READ32_MEMBER(taitojc_state::dsp_shared_r)
{
	return m_dsp_shared_ram[offset] << 16;
}

#if DEBUG_DSP

static void debug_dsp_command(running_machine &machine)
{
	taitojc_state *state = machine.driver_data<taitojc_state>();
	UINT16 *cmd = &state->m_dsp_shared_ram[0x1fc0/2];

	switch (cmd[0])
	{
		case 0x00:
		{
			printf("DSP: NOP\n");
			break;
		}
		case 0x01:
		{
			printf("DSP: Move to Shared RAM: %04X\n", cmd[1]);
			break;
		}
		case 0x02:
		{
			printf("DSP: Move from Shared RAM: %04X\n", cmd[1]);
			break;
		}
		case 0x03:
		{
			printf("DSP: Block move DM to DM: %04X, %04X, %04X\n", cmd[1], cmd[2], cmd[3]+1);
			if (cmd[2] >= 0x7800 && cmd[2] < 0x8000)
			{
				int i, j;
				int saddr = cmd[2] - 0x7800;
				int daddr = cmd[1];
				int length = cmd[3]+1;

				for (j=0; j < length; j+=16)
				{
					int ll = (length - j);
					if (ll > 16) ll = 16;

#if DEBUG_BLOCK_MOVES
					printf("   %04X: ", daddr);
#endif
					for (i=0; i < ll; i++)
					{
						UINT16 d = state->m_dsp_shared_ram[saddr++];
						if (daddr >= 0x8000 && daddr < 0x10000)
						{
							state->m_debug_dsp_ram[daddr-0x8000] = d;
						}
						daddr++;

#if DEBUG_BLOCK_MOVES
						printf("%04X ", d);
#endif
					}
#if DEBUG_BLOCK_MOVES
					printf("\n");
#endif
				}
			}
			break;
		}
		case 0x04:
		{
			printf("DSP: Block move PM to DM: %04X, %04X, %04X\n", cmd[1], cmd[2], cmd[3]+1);
			break;
		}
		case 0x05:
		{
			printf("DSP: Block move DM to PM: %04X, %04X, %04X\n", cmd[1], cmd[2], cmd[3]+1);
			break;
		}
		case 0x08:
		{
			printf("DSP: Jump to address: %04X, %04X, %04X\n", cmd[1], cmd[3], cmd[4]);
			break;
		}
		case 0x09:
		{
			printf("DSP: Call Sub operation: %04X\n", cmd[1]);
			if (cmd[1] == 0x8000)
			{
				int addr = 0;
				int end = 0;
				while (!end)
				{
					int i;
					UINT16 cmd = state->m_debug_dsp_ram[addr++];
					int length = cmd & 0xff;

					if ((cmd >> 11) == 6)
						end = 1;

					printf("   %04X (%02X): ", cmd, cmd >> 11);
					for (i=0; i < length; i++)
					{
						printf("%04X ", state->m_debug_dsp_ram[addr+i]);
					}
					printf("\n");

					addr += length;
				};
			}
			break;
		}
		case 0x0d:
		{
			printf("DSP: Calculate ROM checksum\n");
			break;
		}
		case 0x10:
		{
			printf("DSP: Test RAM\n");
			break;
		}
		case 0x11:
		{
			printf("DSP: Test Program Checksum: %04X, %04X\n", cmd[1], cmd[2]+1);
			break;
		}
		default:
		{
			printf("DSP: Unknown command %04X\n", cmd[0]);
			break;
		}
	}

	printf("\n");
}
#endif // DEBUG_DSP

WRITE32_MEMBER(taitojc_state::dsp_shared_w)
{
	//mame_printf_debug("dsp_shared: %08X, %04X at %08X\n", offset, data >> 16, space.device().safe_pc());
	if (ACCESSING_BITS_24_31)
	{
		m_dsp_shared_ram[offset] &= 0x00ff;
		m_dsp_shared_ram[offset] |= (data >> 16) & 0xff00;
	}
	if (ACCESSING_BITS_16_23)
	{
		m_dsp_shared_ram[offset] &= 0xff00;
		m_dsp_shared_ram[offset] |= (data >> 16) & 0x00ff;
	}

#if DEBUG_DSP
	if (offset == 0x1fc0/4)
	{
		debug_dsp_command(machine());
	}
#endif

	if (offset == 0x1ff8/4)
		m_maincpu->set_input_line(6, CLEAR_LINE);

	if (offset == 0x1ffc/4)
	{
		if ((data & 0x80000) == 0)
		{
			/*
			All games minus Dangerous Curves tests if the DSP is alive with this code snippet:

			0008C370: 4A79 1000 1FC0                                      tst.w   $10001fc0.l
			0008C376: 33FC 0000 0660 0000                                 move.w  #$0, $6600000.l
			0008C37E: 66F0                                                bne     $8c370

			Problem is: that move.w in the middle makes the SR to always return a zero flag result,
			hence it never branches like it should. CPU bug?
			*/
			if (!m_first_dsp_reset || !m_has_dsp_hack)
			{
				m_dsp->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
			}
			m_first_dsp_reset = 0;
		}
		else
		{
			m_dsp->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
		}
	}
}



static UINT8 mcu_comm_reg_r(address_space &space, int reg)
{
	taitojc_state *state = space.machine().driver_data<taitojc_state>();
	UINT8 r = 0;

	switch (reg)
	{
		case 0x03:
		{
			r = state->m_mcu_data_main;
			break;
		}
		case 0x04:
		{
			r = state->m_mcu_comm_main | 0x14;
			break;
		}
		default:
		{
			//mame_printf_debug("hc11_reg_r: %02X at %08X\n", reg, space.device().safe_pc());
			break;
		}
	}

	return r;
}

static void mcu_comm_reg_w(address_space &space, int reg, UINT8 data)
{
	taitojc_state *state = space.machine().driver_data<taitojc_state>();

	switch (reg)
	{
		case 0x00:
		{
			state->m_mcu_data_hc11 = data;
			state->m_mcu_comm_hc11 &= ~0x04;
			state->m_mcu_comm_main &= ~0x20;
			break;
		}
		case 0x04:
		{
			break;
		}
		default:
		{
			//mame_printf_debug("hc11_reg_w: %02X, %02X at %08X\n", reg, data, space.device().safe_pc());
			break;
		}
	}
}

READ32_MEMBER(taitojc_state::mcu_comm_r)
{
	UINT32 r = 0;
	int reg = offset * 4;

	if (ACCESSING_BITS_24_31)
	{
		r |= mcu_comm_reg_r(space, reg + 0) << 24;
	}
	if (ACCESSING_BITS_16_23)
	{
		r |= mcu_comm_reg_r(space, reg + 1) << 16;
	}
	if (ACCESSING_BITS_8_15)
	{
		r |= mcu_comm_reg_r(space, reg + 2) << 8;
	}
	if (ACCESSING_BITS_0_7)
	{
		r |= mcu_comm_reg_r(space, reg + 3) << 0;
	}

	return r;
}

WRITE32_MEMBER(taitojc_state::mcu_comm_w)
{
	int reg = offset * 4;

	if (ACCESSING_BITS_24_31)
	{
		mcu_comm_reg_w(space, reg + 0, (data >> 24) & 0xff);
	}
	if (ACCESSING_BITS_16_23)
	{
		mcu_comm_reg_w(space, reg + 1, (data >> 16) & 0xff);
	}
	if (ACCESSING_BITS_8_15)
	{
		mcu_comm_reg_w(space, reg + 2, (data >> 8) & 0xff);
	}
	if (ACCESSING_BITS_0_7)
	{
		mcu_comm_reg_w(space, reg + 3, (data >> 0) & 0xff);
	}
}

READ32_MEMBER(taitojc_state::snd_share_r)
{
	switch (offset & 3)
	{
		case 0: return (m_snd_shared_ram[(offset/4)] <<  0) & 0xff000000;
		case 1: return (m_snd_shared_ram[(offset/4)] <<  8) & 0xff000000;
		case 2: return (m_snd_shared_ram[(offset/4)] << 16) & 0xff000000;
		case 3: return (m_snd_shared_ram[(offset/4)] << 24) & 0xff000000;
	}

	return 0;
}

WRITE32_MEMBER(taitojc_state::snd_share_w)
{
	if (ACCESSING_BITS_24_31)
	{
		switch (offset & 3)
		{
			case 0: m_snd_shared_ram[(offset/4)] &= ~0xff000000; m_snd_shared_ram[(offset/4)] |= (data >>  0 & 0xff000000); break;
			case 1: m_snd_shared_ram[(offset/4)] &= ~0x00ff0000; m_snd_shared_ram[(offset/4)] |= (data >>  8 & 0x00ff0000); break;
			case 2: m_snd_shared_ram[(offset/4)] &= ~0x0000ff00; m_snd_shared_ram[(offset/4)] |= (data >> 16 & 0x0000ff00); break;
			case 3: m_snd_shared_ram[(offset/4)] &= ~0x000000ff; m_snd_shared_ram[(offset/4)] |= (data >> 24 & 0x000000ff); break;
		}
	}
}


READ8_MEMBER(taitojc_state::jc_pcbid_r)
{
	static const char pcb_id[0x40] =
	{ "Needs proper PCB ID here!"};

	return pcb_id[offset];
}


/*

Some games (Dangerous Curves, Side by Side, Side by Side 2) were released as Twin cabinets,
allowing 2 players to compete eachother via a SMSC COM20020I network IC

Not emulated yet...

*/

READ8_MEMBER(taitojc_state::jc_lan_r)
{
	return 0xff;
}

WRITE8_MEMBER(taitojc_state::jc_lan_w)
{
}


static ADDRESS_MAP_START( taitojc_map, AS_PROGRAM, 32, taitojc_state )
	AM_RANGE(0x00000000, 0x001fffff) AM_ROM AM_MIRROR(0x200000)
	AM_RANGE(0x00400000, 0x01bfffff) AM_ROM AM_REGION("gfx1", 0)
	AM_RANGE(0x04000000, 0x040f7fff) AM_RAM AM_SHARE("vram")
	AM_RANGE(0x040f8000, 0x040fbfff) AM_READWRITE(taitojc_tile_r, taitojc_tile_w)
	AM_RANGE(0x040fc000, 0x040fefff) AM_READWRITE(taitojc_char_r, taitojc_char_w)
	AM_RANGE(0x040ff000, 0x040fffff) AM_RAM AM_SHARE("objlist")
	AM_RANGE(0x05800000, 0x0580003f) AM_READ8(jc_pcbid_r, 0xffffffff)
	AM_RANGE(0x05900000, 0x05900007) AM_READWRITE(mcu_comm_r, mcu_comm_w)
	AM_RANGE(0x06400000, 0x0641ffff) AM_READWRITE(taitojc_palette_r, taitojc_palette_w) AM_SHARE("palette_ram")
	AM_RANGE(0x06600000, 0x0660001f) AM_DEVREADWRITE8_LEGACY("tc0640fio", tc0640fio_r, tc0640fio_w, 0xff000000)
	AM_RANGE(0x0660004c, 0x0660004f) AM_WRITE_PORT("EEPROMOUT")
	AM_RANGE(0x06800000, 0x06800003) AM_WRITENOP // irq mask/ack? a watchdog?
	AM_RANGE(0x06a00000, 0x06a01fff) AM_READWRITE(snd_share_r, snd_share_w) AM_SHARE("snd_shared")
	AM_RANGE(0x06c00000, 0x06c0001f) AM_READWRITE8(jc_lan_r, jc_lan_w, 0x00ff0000)
	AM_RANGE(0x08000000, 0x080fffff) AM_RAM AM_SHARE("main_ram")
	AM_RANGE(0x10000000, 0x10001fff) AM_READWRITE(dsp_shared_r, dsp_shared_w)
ADDRESS_MAP_END


/*

Densha de Go games have odometers for speed and brakepressure.
There's a voltmeter too, but seems to be a dummy (always stuck on 1.5kV)

The OKI is used for seat vibration effects.

*/

WRITE8_MEMBER(taitojc_state::dendego_speedmeter_w)
{
	if (m_speed_meter != dendego_odometer_table[data])
	{
		m_speed_meter = dendego_odometer_table[data];
		output_set_value("counter2", m_speed_meter / 10);
		output_set_value("counter3", m_speed_meter % 10);
	}
}

WRITE8_MEMBER(taitojc_state::dendego_brakemeter_w)
{
	if (m_brake_meter != dendego_pressure_table[data])
	{
		m_brake_meter = dendego_pressure_table[data];
		output_set_value("counter4", m_brake_meter);
	}
}

static ADDRESS_MAP_START( dendego_map, AS_PROGRAM, 32, taitojc_state )
	AM_RANGE(0x06e00000, 0x06e00003) AM_WRITE8(dendego_speedmeter_w, 0x00ff0000)
	AM_RANGE(0x06e00004, 0x06e00007) AM_WRITE8(dendego_brakemeter_w, 0x00ff0000)
	AM_RANGE(0x06e0000c, 0x06e0000f) AM_DEVREADWRITE8("oki", okim6295_device, read, write, 0x00ff0000)
	AM_IMPORT_FROM( taitojc_map )
ADDRESS_MAP_END




/***************************************************************************

  MCU I/O

***************************************************************************/

READ8_MEMBER(taitojc_state::hc11_comm_r)
{
	return m_mcu_comm_hc11;
}

WRITE8_MEMBER(taitojc_state::hc11_comm_w)
{
}

READ8_MEMBER(taitojc_state::hc11_data_r)
{
	m_mcu_comm_hc11 |= 0x04;
	m_mcu_comm_main |= 0x20;
	return m_mcu_data_hc11;
}

WRITE8_MEMBER(taitojc_state::hc11_data_w)
{
	m_mcu_data_main = data;
}

READ8_MEMBER(taitojc_state::hc11_output_r)
{
	return m_mcu_output;
}

WRITE8_MEMBER(taitojc_state::hc11_output_w)
{
/*
    cabinet lamps, active high

    dendego/dendego2:
    d0: START
    d1: DOOR
    d2: JYOUYO (normal)
    d3: HIJYOU (emergency)
    d4: DENSEI (three-stage power)
    d5: POP L
    d6: POP R
    d7: ------- (?)

    landgear:
    unused?

    sidebs/sidebs2:
    ?
*/
	for (int i = 0; i < 8; i++)
		output_set_lamp_value(i, data >> i & 1);

	m_mcu_output = data;
}

READ8_MEMBER(taitojc_state::hc11_analog_r)
{
	static const char *const portnames[] = { "ANALOG1", "ANALOG2", "ANALOG3", "ANALOG4",
										"ANALOG5", "ANALOG6", "ANALOG7", "ANALOG8" };

	return ioport(portnames[offset])->read_safe(0);
}


static ADDRESS_MAP_START( hc11_pgm_map, AS_PROGRAM, 8, taitojc_state )
	AM_RANGE(0x4000, 0x5fff) AM_RAM
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( hc11_io_map, AS_IO, 8, taitojc_state )
	AM_RANGE(MC68HC11_IO_PORTA,     MC68HC11_IO_PORTA    ) AM_READNOP // ?
	AM_RANGE(MC68HC11_IO_PORTG,     MC68HC11_IO_PORTG    ) AM_READWRITE(hc11_comm_r, hc11_comm_w)
	AM_RANGE(MC68HC11_IO_PORTH,     MC68HC11_IO_PORTH    ) AM_READWRITE(hc11_output_r, hc11_output_w)
	AM_RANGE(MC68HC11_IO_SPI2_DATA, MC68HC11_IO_SPI2_DATA) AM_READWRITE(hc11_data_r, hc11_data_w)
	AM_RANGE(MC68HC11_IO_AD0,       MC68HC11_IO_AD7      ) AM_READ(hc11_analog_r)
ADDRESS_MAP_END




/***************************************************************************

  DSP I/O

***************************************************************************/

/*
    Math co-processor memory map

    0x7000: Projection point Y
    0x7001: Projection point X
    0x7002: Projection point Z
    0x7003: Frustum Min Z(?)
    0x7004: Frustum Max Z(?)
    0x7010: Line intersection, parameter length
    0x7011: Line intersection, intersection point
    0x7012: Line intersection, line length
    0x7013: Viewport Width / 2
    0x7014: Viewport Height / 2
    0x7015: Viewport Z / 2 (?)
    0x701b: Line intersection, parameter interpolation read
    0x701d: Projected point Y read
    0x701f: Projected point X read
    0x7022: Unknown read
    0x7030: Unknown write
    0x703b: Unknown read/write
*/

WRITE16_MEMBER(taitojc_state::dsp_math_projection_w)
{
	m_projection_data[offset] = data;
}

WRITE16_MEMBER(taitojc_state::dsp_math_viewport_w)
{
	m_viewport_data[offset] = data;
}

READ16_MEMBER(taitojc_state::dsp_math_projection_y_r)
{
	return (m_projection_data[2] != 0) ? (m_projection_data[0] * m_viewport_data[0]) / m_projection_data[2] : 0;
}

READ16_MEMBER(taitojc_state::dsp_math_projection_x_r)
{
	return (m_projection_data[2] != 0) ? (m_projection_data[1] * m_viewport_data[1]) / m_projection_data[2] : 0;
}

WRITE16_MEMBER(taitojc_state::dsp_math_intersection_w)
{
	m_intersection_data[offset] = data;
}

READ16_MEMBER(taitojc_state::dsp_math_intersection_r)
{
	return (m_intersection_data[2] != 0) ? (m_intersection_data[0] * m_intersection_data[1]) / m_intersection_data[2] : 0;
}

READ16_MEMBER(taitojc_state::dsp_math_unk_r)
{
	return 0x7fff;
}


/**************************************************************************/

READ16_MEMBER(taitojc_state::dsp_rom_r)
{
	//assert (m_dsp_rom_pos < 0x800000); // never happens
	return ((UINT16*)m_gfx2->base())[m_dsp_rom_pos++];
}

WRITE16_MEMBER(taitojc_state::dsp_rom_w)
{
	if (offset == 0)
	{
		m_dsp_rom_pos &= 0xffff;
		m_dsp_rom_pos |= data << 16;
	}
	else
	{
		m_dsp_rom_pos &= 0xffff0000;
		m_dsp_rom_pos |= data;
	}
}

WRITE16_MEMBER(taitojc_state::dsp_texture_w)
{
	int index;
	int x, y;
	//mame_printf_debug("texture write %08X, %04X\n", dsp_addr1, data);

	x = (m_dsp_tex_offset >> 0) & 0x1f;
	y = (m_dsp_tex_offset >> 5) & 0x1f;

	x += (m_dsp_tex_offset & 0x400) ? 0x20 : 0;
	y += (m_dsp_tex_offset & 0x800) ? 0x20 : 0;

	index = (((m_texture_y * 32) + y) * 2048) + ((m_texture_x * 32) + x);
	m_texture[index] = data & 0xff;

	m_dsp_tex_offset++;
}

READ16_MEMBER(taitojc_state::dsp_texaddr_r)
{
	return m_dsp_tex_address;
}

WRITE16_MEMBER(taitojc_state::dsp_texaddr_w)
{
	m_dsp_tex_address = data;
//  mame_printf_debug("texaddr = %08X at %08X\n", data, space.device().safe_pc());

	m_texture_x = (((data >> 0) & 0x1f) << 1) | ((data >> 12) & 0x1);
	m_texture_y = (((data >> 5) & 0x1f) << 1) | ((data >> 13) & 0x1);

	m_dsp_tex_offset = 0;
}

WRITE16_MEMBER(taitojc_state::dsp_polygon_fifo_w)
{
	//assert (m_polygon_fifo_ptr < TAITOJC_POLYGON_FIFO_SIZE); // never happens
	m_polygon_fifo[m_polygon_fifo_ptr++] = data;
}

WRITE16_MEMBER(taitojc_state::dsp_unk2_w)
{
	if (offset == 0)
	{
		taitojc_clear_frame(machine());
		m_renderer->render_polygons(machine(), m_polygon_fifo, m_polygon_fifo_ptr);

		m_polygon_fifo_ptr = 0;
	}
}

READ16_MEMBER(taitojc_state::dsp_to_main_r)
{
	return m_dsp_shared_ram[0x7fe];
}

WRITE16_MEMBER(taitojc_state::dsp_to_main_w)
{
	m_maincpu->set_input_line(6, ASSERT_LINE);

	COMBINE_DATA(&m_dsp_shared_ram[0x7fe]);
}


static ADDRESS_MAP_START( tms_program_map, AS_PROGRAM, 16, taitojc_state )
	AM_RANGE(0x4000, 0x7fff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( tms_data_map, AS_DATA, 16, taitojc_state )
	AM_RANGE(0x6a01, 0x6a02) AM_WRITE(dsp_unk2_w)
	AM_RANGE(0x6a11, 0x6a12) AM_NOP     // same as 0x6a01..02 for the second renderer chip?
	AM_RANGE(0x6b20, 0x6b20) AM_WRITE(dsp_polygon_fifo_w)
	AM_RANGE(0x6b22, 0x6b22) AM_WRITE(dsp_texture_w)
	AM_RANGE(0x6b23, 0x6b23) AM_READWRITE(dsp_texaddr_r, dsp_texaddr_w)
	AM_RANGE(0x6c00, 0x6c01) AM_READWRITE(dsp_rom_r, dsp_rom_w)
	AM_RANGE(0x7000, 0x7002) AM_WRITE(dsp_math_projection_w)
	AM_RANGE(0x7010, 0x7012) AM_WRITE(dsp_math_intersection_w)
	AM_RANGE(0x7013, 0x7015) AM_WRITE(dsp_math_viewport_w)
	AM_RANGE(0x701b, 0x701b) AM_READ(dsp_math_intersection_r)
	AM_RANGE(0x701d, 0x701d) AM_READ(dsp_math_projection_y_r)
	AM_RANGE(0x701f, 0x701f) AM_READ(dsp_math_projection_x_r)
	AM_RANGE(0x7022, 0x7022) AM_READ(dsp_math_unk_r)
	AM_RANGE(0x7ffe, 0x7ffe) AM_READWRITE(dsp_to_main_r, dsp_to_main_w)
	AM_RANGE(0x7800, 0x7fff) AM_RAM AM_SHARE("dsp_shared")
	AM_RANGE(0x8000, 0xffff) AM_RAM
ADDRESS_MAP_END




/***************************************************************************

  Inputs

***************************************************************************/

static INPUT_PORTS_START( common )
	PORT_START("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE_NO_TOGGLE( 0x08, IP_ACTIVE_LOW )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_device, read_bit)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("START")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("UNUSED")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED ) // debug related in dendego/sidebs
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("BUTTONS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("EEPROMOUT")
	PORT_BIT( 0x04000000, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, write_bit)
	PORT_BIT( 0x08000000, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, set_clock_line)
	PORT_BIT( 0x10000000, IP_ACTIVE_LOW,  IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, set_cs_line)
INPUT_PORTS_END

static INPUT_PORTS_START( dendego )
	PORT_INCLUDE( common )

	PORT_MODIFY("UNUSED")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Horn Pedal")

	PORT_MODIFY("BUTTONS")  // Throttle Lever at left, move down to speed up, 6 positions
	PORT_BIT( 0x77, 0x00, IPT_POSITIONAL_V ) PORT_POSITIONS(6) PORT_REMAP_TABLE(dendego_mascon_table) PORT_SENSITIVITY(10) PORT_KEYDELTA(1) PORT_CENTERDELTA(0) PORT_NAME("Throttle Lever")
	PORT_BIT( 0x88, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")   // Brake Lever at right, rotate handle right (anti clockwise) to increase pressure, 11 positions but not at constant intervals like the throttle lever
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x00, 0xef) PORT_SENSITIVITY(35) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_NAME("Brake Lever")
INPUT_PORTS_END

static INPUT_PORTS_START( landgear )
	PORT_INCLUDE( common )

	PORT_MODIFY("UNUSED")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("View Switch")

	PORT_START("ANALOG1")       // Lever X
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(5) PORT_REVERSE

	PORT_START("ANALOG2")       // Lever Y
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y )  PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(5)

	PORT_START("ANALOG3")       // Throttle
	PORT_BIT( 0xff, 0x00, IPT_PEDAL )  PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(5) PORT_REVERSE
INPUT_PORTS_END

static INPUT_PORTS_START( sidebs )
	PORT_INCLUDE( common )

	PORT_MODIFY("START")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) // no start button

	PORT_MODIFY("UNUSED")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("View Switch")

	PORT_MODIFY("BUTTONS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Shift Up")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP   ) PORT_NAME("Shift Down")

	PORT_START("ANALOG1")       // Steering
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(10) PORT_NAME("Steering Wheel")

	PORT_START("ANALOG2")       // Acceleration
	PORT_BIT( 0xff, 0x00, IPT_PEDAL )  PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(25) PORT_NAME("Gas Pedal")

	PORT_START("ANALOG3")       // Brake
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(25) PORT_NAME("Brake Pedal")
INPUT_PORTS_END

static INPUT_PORTS_START( dangcurv )
	PORT_INCLUDE( common )

	PORT_MODIFY("START")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) // no start button

	PORT_MODIFY("UNUSED")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("View Switch")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Rear Switch")

	PORT_MODIFY("BUTTONS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Shift Up")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP   ) PORT_NAME("Shift Down")

	PORT_START("ANALOG1")       // Steering
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(35) PORT_KEYDELTA(10) PORT_REVERSE PORT_NAME("Steering Wheel")

	PORT_START("ANALOG2")       // Acceleration
	PORT_BIT( 0xff, 0x00, IPT_PEDAL )  PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(25) PORT_REVERSE PORT_NAME("Gas Pedal")

	PORT_START("ANALOG3")       // Brake
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00, 0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(25) PORT_REVERSE PORT_NAME("Brake Pedal")
INPUT_PORTS_END




/***************************************************************************

  Machine Config

***************************************************************************/

void taitojc_state::machine_reset()
{

	m_first_dsp_reset = 1;

	m_mcu_comm_main = 0;
	m_mcu_comm_hc11 = 0;
	m_mcu_data_main = 0;
	m_mcu_data_hc11 = 0;

	m_texture_x = 0;
	m_texture_y = 0;

	m_dsp_rom_pos = 0;
	m_dsp_tex_address = 0;
	m_dsp_tex_offset = 0;
	m_polygon_fifo_ptr = 0;

	memset(m_viewport_data, 0, sizeof(m_viewport_data));
	memset(m_projection_data, 0, sizeof(m_projection_data));
	memset(m_intersection_data, 0, sizeof(m_intersection_data));

	// hold the TMS in reset until we have code
	m_dsp->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
}

INTERRUPT_GEN_MEMBER(taitojc_state::taitojc_vblank)
{
	device.execute().set_input_line_and_vector(2, HOLD_LINE, 130);
}

static const tc0640fio_interface taitojc_io_intf =
{
	DEVCB_INPUT_PORT("SERVICE"), DEVCB_INPUT_PORT("COINS"),
	DEVCB_INPUT_PORT("START"), DEVCB_INPUT_PORT("UNUSED"), DEVCB_INPUT_PORT("BUTTONS")
};

static const hc11_config taitojc_hc11_config =
{
	1,      // has extended I/O
	1280,   // internal RAM size
	0x00    // INIT defaults to 0x00
};

static MACHINE_CONFIG_START( taitojc, taitojc_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68040, 25000000)
	MCFG_CPU_PROGRAM_MAP(taitojc_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", taitojc_state,  taitojc_vblank)

	MCFG_CPU_ADD("sub", MC68HC11, 4000000) // MC68HC11M0
	MCFG_CPU_PROGRAM_MAP(hc11_pgm_map)
	MCFG_CPU_IO_MAP(hc11_io_map)
	MCFG_CPU_CONFIG(taitojc_hc11_config)

	MCFG_CPU_ADD("dsp", TMS32051, 50000000)
	MCFG_CPU_PROGRAM_MAP(tms_program_map)
	MCFG_CPU_DATA_MAP(tms_data_map)

	MCFG_QUANTUM_TIME(attotime::from_hz(6000))


	MCFG_EEPROM_93C46_ADD("eeprom")

	MCFG_TC0640FIO_ADD("tc0640fio", taitojc_io_intf)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MCFG_SCREEN_SIZE(512, 400)
	MCFG_SCREEN_VISIBLE_AREA(0, 511, 0, 399)
	MCFG_SCREEN_UPDATE_DRIVER(taitojc_state, screen_update_taitojc)

	MCFG_PALETTE_LENGTH(32768)


	/* sound hardware */
	MCFG_FRAGMENT_ADD(taito_en_sound)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( dendego, taitojc )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(dendego_map)

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_UPDATE_DRIVER(taitojc_state, screen_update_dendego)

	/* sound hardware */
	MCFG_SPEAKER_ADD("subwoofer", 0.0, 0.0, 1.0)
	MCFG_OKIM6295_ADD("oki", 1056000, OKIM6295_PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "subwoofer", 0.20)
MACHINE_CONFIG_END




/***************************************************************************

  Game Drivers

***************************************************************************/

READ16_MEMBER(taitojc_state::taitojc_dsp_idle_skip_r)
{
	if(space.device().safe_pc()==0x404c)
		space.device().execute().spin_until_time(attotime::from_usec(500));

	return m_dsp_shared_ram[0x7f0];
}

READ16_MEMBER(taitojc_state::dendego2_dsp_idle_skip_r)
{
	if(space.device().safe_pc()==0x402e)
		space.device().execute().spin_until_time(attotime::from_usec(500));

	return m_dsp_shared_ram[0x7f0];
}

WRITE16_MEMBER(taitojc_state::dsp_idle_skip_w)
{
	COMBINE_DATA(&m_dsp_shared_ram[0x7f0]);
}


DRIVER_INIT_MEMBER(taitojc_state,taitojc)
{

	m_polygon_fifo = auto_alloc_array(machine(), UINT16, TAITOJC_POLYGON_FIFO_SIZE);

	m_has_dsp_hack = 1;

	machine().device("dsp")->memory().space(AS_DATA).install_readwrite_handler(0x7ff0, 0x7ff0, read16_delegate(FUNC(taitojc_state::taitojc_dsp_idle_skip_r),this), write16_delegate(FUNC(taitojc_state::dsp_idle_skip_w),this));
}

DRIVER_INIT_MEMBER(taitojc_state,dendego2)
{

	DRIVER_INIT_CALL(taitojc);

	machine().device("dsp")->memory().space(AS_DATA).install_readwrite_handler(0x7ff0, 0x7ff0, read16_delegate(FUNC(taitojc_state::dendego2_dsp_idle_skip_r),this), write16_delegate(FUNC(taitojc_state::dsp_idle_skip_w),this));
}

DRIVER_INIT_MEMBER(taitojc_state,dangcurv)
{

	DRIVER_INIT_CALL(taitojc);

	m_has_dsp_hack = 0;
}


/**************************************************************************/

ROM_START( sidebs )
	ROM_REGION(0x200000, "maincpu", 0)      /* 68040 code */
	ROM_LOAD32_BYTE( "e23-19.ic36", 0x000000, 0x80000, CRC(7b75481b) SHA1(47332e045f92b31e4f35c38e6880a7287b9a5c2c) )
	ROM_LOAD32_BYTE( "e23-20.ic37", 0x000001, 0x80000, CRC(cbd857dd) SHA1(ae33ad8b0c3559a3a9096351e9aa07782d3cb841) )
	ROM_LOAD32_BYTE( "e23-21.ic38", 0x000002, 0x80000, CRC(357f2e10) SHA1(226922f2649d9ac78d253200f5bbff4fb3ac74c8) )
	ROM_LOAD32_BYTE( "e23-22.ic39", 0x000003, 0x80000, CRC(c793ba43) SHA1(0ddbf625320968b4e18309d8e732ce4a2b9f4bce) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e23-23.ic30", 0x100001, 0x40000, CRC(cffbffe5) SHA1(c01ac44390dacab4b49bb066a46d81a184b07a1e) )
	ROM_LOAD16_BYTE( "e23-24.ic31", 0x100000, 0x40000, CRC(64bae246) SHA1(f929f664881487615b1259db43a0721135830274) )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e23-05.ic9",   0x0800002, 0x200000, CRC(6e5d11ec) SHA1(e5c39d80577bf8ae9fc6162dc54571c6c8421160) )
	ROM_LOAD32_WORD( "e23-12.ic22",  0x0800000, 0x200000, CRC(7365333c) SHA1(4f7b75088799ea37f714bc7e5c5b276a7e5d933f) )
	ROM_LOAD32_WORD( "e23-06.ic10",  0x0c00002, 0x200000, CRC(ffcfd153) SHA1(65fa486cf0156e2988bd6e7060d66f87f765a123) )
	ROM_LOAD32_WORD( "e23-13.ic23",  0x0c00000, 0x200000, CRC(16982d37) SHA1(134370f7dfadb1886f1e5e5dd16f8b72ad08fc68) )
	ROM_LOAD32_WORD( "e23-07.ic12",  0x1400002, 0x200000, CRC(90f2a87c) SHA1(770bb89fa42cb2a1d5a58525b8d72ed7df3f93ed) )
	ROM_LOAD32_WORD( "e23-14.ic25",  0x1400000, 0x200000, CRC(1bc5a914) SHA1(92f82a4e2fbac73dbb3293726fc09022bd11a8fe) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e23-01.ic5",   0x0000000, 0x200000, CRC(2cbe4bbd) SHA1(ed6fe4344c86d50914b5ddbc720dd15544f4d07f) )
	ROM_LOAD( "e23-02.ic6",   0x0200000, 0x200000, CRC(7ebada03) SHA1(d75c992aa33dd7f71de6a6d09aac471012b0daa3) )
	ROM_LOAD( "e23-03.ic7",   0x0400000, 0x200000, CRC(5bf1f30b) SHA1(6e0c07b9f92962eec55ee444732a10ac78f8b050) )
	ROM_LOAD( "e23-04.ic8",   0x0600000, 0x200000, CRC(0f860fb5) SHA1(47c0db4ec6d02e10d8abfacd1fa332f7af3976dd) )
	ROM_LOAD( "e23-08.ic18",  0x0800000, 0x200000, CRC(bceea63e) SHA1(eeec1e2306aa37431c5ba69bdb9c5524ab7b7ba4) )
	ROM_LOAD( "e23-09.ic19",  0x0a00000, 0x200000, CRC(3917c12f) SHA1(e3a568f638bb6b0cd6237c9fee5fc350983ea1e7) )
	ROM_LOAD( "e23-10.ic20",  0x0c00000, 0x200000, CRC(038370d9) SHA1(c02f68a25121d2d5aae62c419522b25cd6ec32b6) )
	ROM_LOAD( "e23-11.ic21",  0x0e00000, 0x200000, CRC(91fab03d) SHA1(1865d8b679faa6f2b3c14db2c6c461c00afd547c) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e23-15.ic32",  0x000000, 0x200000, CRC(8955b7c7) SHA1(767626bd5cf6810b0368ee85e487c12ef7e8a23d) )
	ROM_LOAD16_BYTE( "e23-16.ic33",  0x400000, 0x200000, CRC(1d63d785) SHA1(0cf74bb433e9c453e35f7a552fdf9e22084b2f49) )
	ROM_LOAD16_BYTE( "e23-17.ic34",  0x800000, 0x200000, CRC(1c54021a) SHA1(a1efbdb02d23a5d32ebd25cb8e99dface3178ebd) )
	ROM_LOAD16_BYTE( "e23-18.ic35",  0xc00000, 0x200000, CRC(1816f38a) SHA1(6451bdb4b4297aaf4987451bc0ddd97b0072e113) )
ROM_END

ROM_START( sidebs2 )
	ROM_REGION(0x200000, "maincpu", 0)      /* 68040 code */
	ROM_LOAD32_BYTE( "sbs2_p0.ic36", 0x000000, 0x80000, CRC(2dd78d09) SHA1(f0a0105c3f2827c8b55d1bc58ebeea0f71150fed) )
	ROM_LOAD32_BYTE( "sbs2_p1.ic37", 0x000001, 0x80000, CRC(befeda1d) SHA1(3171c87b0872f3206653900e3dbd210ea9beba61) )
	ROM_LOAD32_BYTE( "sbs2_p2.ic38", 0x000002, 0x80000, CRC(ade07d7e) SHA1(a5200ea3ddbfef37d302e7cb27015b6f6aa8a7c1) )
	ROM_LOAD32_BYTE( "sbs2_p3.ic39", 0x000003, 0x80000, CRC(94e943d6) SHA1(2bc7332526b969e5084b9d73063f1c0d18ec5181) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e38-19.ic30",  0x100001, 0x40000, CRC(3f50cb7b) SHA1(76af65c9b74ede843a3182f79cecda8c3e3febe6) )
	ROM_LOAD16_BYTE( "e38-20.ic31",  0x100000, 0x40000, CRC(d01340e7) SHA1(76ee48d644dc1ec415d47e0df4864c64ac928b9d) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e38-05.ic9",  0x0800002, 0x200000, CRC(bda366bf) SHA1(a7558e6d5e4583a2d8e3d2bfa8cabcc459d3ee83) )
	ROM_LOAD32_WORD( "e38-13.ic22", 0x0800000, 0x200000, CRC(1bd7582b) SHA1(35763b9489f995433f66ef72d4f6b6ac67df5480) )
	ROM_LOAD32_WORD( "e38-06.ic10", 0x0c00002, 0x200000, CRC(308d2783) SHA1(22c309273444bc6c1df78069850958a739664998) )
	ROM_LOAD32_WORD( "e38-14.ic23", 0x0c00000, 0x200000, CRC(f1699f27) SHA1(3c9a9cefe6f215fd9f0a9746da97147d14df1da4) )
	ROM_LOAD32_WORD( "e38-07.ic11", 0x1000002, 0x200000, CRC(226ba93d) SHA1(98e6342d070ddd988c1e9bff21afcfb28df86844) )
	ROM_LOAD32_WORD( "e38-15.ic24", 0x1000000, 0x200000, CRC(2853c2e3) SHA1(046dbbd4bcb3b07cddab19a284fee9fe736f8def) )
	ROM_LOAD32_WORD( "e38-08.ic12", 0x1400002, 0x200000, CRC(9c513b32) SHA1(fe26e39d3d65073d23d525bc17771f0c244a38c2) )
	ROM_LOAD32_WORD( "e38-16.ic25", 0x1400000, 0x200000, CRC(fceafae2) SHA1(540ecd5d1aa64c0428a08ea1e4e634e00f7e6bd6) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e38-01.ic5",  0x0000000, 0x200000, CRC(a3c2e2c7) SHA1(538208534f996782167e4cf0d157ad93ce2937bd) )
	ROM_LOAD( "e38-02.ic6",  0x0200000, 0x200000, CRC(ecdfb75a) SHA1(85e7afa321846816fa3bd9074ad9dec95abe23fe) )
	ROM_LOAD( "e38-03.ic7",  0x0400000, 0x200000, CRC(28e9cb59) SHA1(a2651fd81a1263573f868864ee049f8fc4177ffa) )
	ROM_LOAD( "e38-04.ic8",  0x0600000, 0x080000, CRC(26cab05b) SHA1(d5bcf021646dade834840051e0ce083319c53985) )
	ROM_RELOAD(              0x0680000, 0x080000 )
	ROM_RELOAD(              0x0700000, 0x080000 )
	ROM_RELOAD(              0x0780000, 0x080000 )
	ROM_LOAD( "e38-09.ic18", 0x0800000, 0x200000, CRC(03c95a7f) SHA1(fc046cf5fcfcf5648f68af4bed78576f6d67b32f) )
	ROM_LOAD( "e38-10.ic19", 0x0a00000, 0x200000, CRC(0fb06794) SHA1(2d0e3b07ded698235572fe9e907a84d5779ac2c5) )
	ROM_LOAD( "e38-11.ic20", 0x0c00000, 0x200000, CRC(6a312351) SHA1(8037e377f8eef4cc6dd84aec9c829106f0bb130c) )
	ROM_LOAD( "e38-12.ic21", 0x0e00000, 0x080000, CRC(193a5774) SHA1(aa017ae4fec92bb87c0eb94f59d093853ddbabc9) )
	ROM_RELOAD(              0x0e80000, 0x080000 )
	ROM_RELOAD(              0x0f00000, 0x080000 )
	ROM_RELOAD(              0x0f80000, 0x080000 )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e23-15.ic32", 0x000000, 0x200000, CRC(8955b7c7) SHA1(767626bd5cf6810b0368ee85e487c12ef7e8a23d) ) // from sidebs (redump)
	ROM_LOAD16_BYTE( "e38-17.ic33", 0x400000, 0x200000, CRC(61e81c7f) SHA1(aa650bc139685ad456c233b79aa60005a8fd6d79) )
	ROM_LOAD16_BYTE( "e38-18.ic34", 0x800000, 0x200000, CRC(43e2f149) SHA1(32ea9156948a886dda5bd071e9f493ddc2b06212) )
	ROM_LOAD16_BYTE( "e38-21.ic35", 0xc00000, 0x200000, CRC(25373c5f) SHA1(ab9f917dbde7c808be2cd836ce2d3fc558e290f1) )

	/* PALS
	e23-28.ic18    NOT A ROM
	e23-27.ic13    NOT A ROM
	e23-26.ic4     NOT A ROM
	e23-25-1.ic3   NOT A ROM
	e23-30.ic40    NOT A ROM
	e23-29.ic39    NOT A ROM
	e23-31.ic46    NOT A ROM
	e23-32-1.ic51  NOT A ROM
	e23-34.ic72    NOT A ROM
	e23-33.ic53    NOT A ROM
	e23-35.ic110   NOT A ROM
	e23-38.ic73    NOT A ROM
	e23-37.ic69    NOT A ROM
	*/
ROM_END

ROM_START( sidebs2j )
	ROM_REGION(0x200000, "maincpu", 0)      /* 68040 code */
	ROM_LOAD32_BYTE( "e38-23.ic36", 0x000000, 0x80000, CRC(b3d8e2d9) SHA1(6de6a51c3d9ace532fa03517bab93101b5a3eaae) )
	ROM_LOAD32_BYTE( "e38-24.ic37", 0x000001, 0x80000, CRC(2a47d80d) SHA1(41b889e4a1397c7f0d4f6ef136ed8abfd7e1ed86) )
	ROM_LOAD32_BYTE( "e38-25.ic38", 0x000002, 0x80000, CRC(f1a8a4df) SHA1(e4cf75969fb0503df2290522194b097f5cb983a3) )
	ROM_LOAD32_BYTE( "e38-26.ic39", 0x000003, 0x80000, CRC(b550fbf2) SHA1(a0a461af7e71c6ad6468cfdee2bc7161ae31bbfb) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e38-19.ic30", 0x100001, 0x040000, CRC(3f50cb7b) SHA1(76af65c9b74ede843a3182f79cecda8c3e3febe6) )
	ROM_LOAD16_BYTE( "e38-20.ic31", 0x100000, 0x040000, CRC(d01340e7) SHA1(76ee48d644dc1ec415d47e0df4864c64ac928b9d) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e38-05.ic9",  0x0800002, 0x200000, CRC(bda366bf) SHA1(a7558e6d5e4583a2d8e3d2bfa8cabcc459d3ee83) )
	ROM_LOAD32_WORD( "e38-13.ic22", 0x0800000, 0x200000, CRC(1bd7582b) SHA1(35763b9489f995433f66ef72d4f6b6ac67df5480) )
	ROM_LOAD32_WORD( "e38-06.ic10", 0x0c00002, 0x200000, CRC(308d2783) SHA1(22c309273444bc6c1df78069850958a739664998) )
	ROM_LOAD32_WORD( "e38-14.ic23", 0x0c00000, 0x200000, CRC(f1699f27) SHA1(3c9a9cefe6f215fd9f0a9746da97147d14df1da4) )
	ROM_LOAD32_WORD( "e38-07.ic11", 0x1000002, 0x200000, CRC(226ba93d) SHA1(98e6342d070ddd988c1e9bff21afcfb28df86844) )
	ROM_LOAD32_WORD( "e38-15.ic24", 0x1000000, 0x200000, CRC(2853c2e3) SHA1(046dbbd4bcb3b07cddab19a284fee9fe736f8def) )
	ROM_LOAD32_WORD( "e38-08.ic12", 0x1400002, 0x200000, CRC(9c513b32) SHA1(fe26e39d3d65073d23d525bc17771f0c244a38c2) )
	ROM_LOAD32_WORD( "e38-16.ic25", 0x1400000, 0x200000, CRC(fceafae2) SHA1(540ecd5d1aa64c0428a08ea1e4e634e00f7e6bd6) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e38-01.ic5",  0x0000000, 0x200000, CRC(a3c2e2c7) SHA1(538208534f996782167e4cf0d157ad93ce2937bd) )
	ROM_LOAD( "e38-02.ic6",  0x0200000, 0x200000, CRC(ecdfb75a) SHA1(85e7afa321846816fa3bd9074ad9dec95abe23fe) )
	ROM_LOAD( "e38-03.ic7",  0x0400000, 0x200000, CRC(28e9cb59) SHA1(a2651fd81a1263573f868864ee049f8fc4177ffa) )
	ROM_LOAD( "e38-04.ic8",  0x0600000, 0x080000, CRC(26cab05b) SHA1(d5bcf021646dade834840051e0ce083319c53985) )
	ROM_RELOAD(              0x0680000, 0x080000 )
	ROM_RELOAD(              0x0700000, 0x080000 )
	ROM_RELOAD(              0x0780000, 0x080000 )
	ROM_LOAD( "e38-09.ic18", 0x0800000, 0x200000, CRC(03c95a7f) SHA1(fc046cf5fcfcf5648f68af4bed78576f6d67b32f) )
	ROM_LOAD( "e38-10.ic19", 0x0a00000, 0x200000, CRC(0fb06794) SHA1(2d0e3b07ded698235572fe9e907a84d5779ac2c5) )
	ROM_LOAD( "e38-11.ic20", 0x0c00000, 0x200000, CRC(6a312351) SHA1(8037e377f8eef4cc6dd84aec9c829106f0bb130c) )
	ROM_LOAD( "e38-12.ic21", 0x0e00000, 0x080000, CRC(193a5774) SHA1(aa017ae4fec92bb87c0eb94f59d093853ddbabc9) )
	ROM_RELOAD(              0x0e80000, 0x080000 )
	ROM_RELOAD(              0x0f00000, 0x080000 )
	ROM_RELOAD(              0x0f80000, 0x080000 )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e23-15.ic32", 0x000000, 0x200000, CRC(8955b7c7) SHA1(767626bd5cf6810b0368ee85e487c12ef7e8a23d) ) // from sidebs (redump)
	ROM_LOAD16_BYTE( "e38-17.ic33", 0x400000, 0x200000, CRC(61e81c7f) SHA1(aa650bc139685ad456c233b79aa60005a8fd6d79) )
	ROM_LOAD16_BYTE( "e38-18.ic34", 0x800000, 0x200000, CRC(43e2f149) SHA1(32ea9156948a886dda5bd071e9f493ddc2b06212) )
	ROM_LOAD16_BYTE( "e38-21.ic35", 0xc00000, 0x200000, CRC(25373c5f) SHA1(ab9f917dbde7c808be2cd836ce2d3fc558e290f1) )

	/* PALS
	e23-28.ic18    NOT A ROM
	e23-27.ic13    NOT A ROM
	e23-26.ic4     NOT A ROM
	e23-25-1.ic3   NOT A ROM
	e23-30.ic40    NOT A ROM
	e23-29.ic39    NOT A ROM
	e23-31.ic46    NOT A ROM
	e23-32-1.ic51  NOT A ROM
	e23-34.ic72    NOT A ROM
	e23-33.ic53    NOT A ROM
	e23-35.ic110   NOT A ROM
	e23-38.ic73    NOT A ROM
	e23-37.ic69    NOT A ROM
	*/
ROM_END

ROM_START( dendego )
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e35-21.ic36", 0x000000, 0x80000, CRC(bc70ca97) SHA1(724a24da9d6f163c26e7528ee2c15bd06f2c4382) )
	ROM_LOAD32_BYTE( "e35-22.ic37", 0x000001, 0x80000, CRC(83b17de8) SHA1(538ddc16727e08e9a2a8ff6b4f030dc044993aa0) )
	ROM_LOAD32_BYTE( "e35-23.ic38", 0x000002, 0x80000, CRC(1da4acd6) SHA1(2ce11c5f37287526bb1d39185f793d79fc73d5b5) )
	ROM_LOAD32_BYTE( "e35-24.ic39", 0x000003, 0x80000, CRC(0318afb0) SHA1(9c86330c85536fb1a093ed40610b1c3ddb7813c3) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e35-25.ic30", 0x100001, 0x40000, CRC(8104de13) SHA1(e518fbaf91704cf5cb8ffbb4833e3adba8c18658) )
	ROM_LOAD16_BYTE( "e35-26.ic31", 0x100000, 0x40000, CRC(61821cc9) SHA1(87cd5bd3bb22c9f4ca4b6d96f75434d48418321b) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )    /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e35-05.ic9",   0x0800002, 0x200000, CRC(a94486c5) SHA1(c3f869aa0557411f747038a1e0ed6eedcf91fda5) )
	ROM_LOAD32_WORD( "e35-13.ic22",  0x0800000, 0x200000, CRC(2dc9dff1) SHA1(bc7ad64bc359f18a065e36749cc29c75e52202e2) )
	ROM_LOAD32_WORD( "e35-06.ic10",  0x0c00002, 0x200000, CRC(cf328021) SHA1(709ce80f9338637172dbf4e9adcacdb3e5b4ccdb) )
	ROM_LOAD32_WORD( "e35-14.ic23",  0x0c00000, 0x200000, CRC(cab50364) SHA1(e3272f844ecadfd63a1e3c965526a851f8cde94d) )
	ROM_LOAD32_WORD( "e35-07.ic11",  0x1000002, 0x200000, CRC(a88a5a86) SHA1(a4a393fe9df3654e41d32e015ca3977d13206dfe) )
	ROM_LOAD32_WORD( "e35-15.ic24",  0x1000000, 0x200000, CRC(aea86b35) SHA1(092f34f844fc6a779a6f18c03d44a9d39a101373) )
	ROM_LOAD32_WORD( "e35-08.ic12",  0x1400002, 0x200000, CRC(99425ff6) SHA1(3bd6fe7204dece55459392170b42d4c6a9d3ef5b) )
	ROM_LOAD32_WORD( "e35-16.ic25",  0x1400000, 0x200000, CRC(161481b6) SHA1(cc3c2939ac8911c197e9930580d316066f345772) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e35-01.ic5",   0x0000000, 0x200000, CRC(bd1975cb) SHA1(a08c6f4a84f9d4c2a5aa67cc2045aedd4580b8dc) )
	ROM_LOAD( "e35-02.ic6",   0x0200000, 0x200000, CRC(e5caa459) SHA1(c38d795b96fff193223cd3df9f51ebdc2971b719) )
	ROM_LOAD( "e35-03.ic7",   0x0400000, 0x200000, CRC(86ea5bcf) SHA1(1cee7f677b786b2fa9f50e723decd08cd69fbdef) )
	ROM_LOAD( "e35-04.ic8",   0x0600000, 0x200000, CRC(afc07c36) SHA1(f3f7b04766a9a2c8b298e1692aea24abc7001432) )
	ROM_LOAD( "e35-09.ic18",  0x0800000, 0x200000, CRC(be18ddf1) SHA1(d4fe26e427244c5b421b6421ff3bf9af303673d5) )
	ROM_LOAD( "e35-10.ic19",  0x0a00000, 0x200000, CRC(44ea9474) SHA1(161ff94b31c3dc2fa4b1e556ed62624147687443) )
	ROM_LOAD( "e35-11.ic20",  0x0c00000, 0x200000, CRC(dc8f5e88) SHA1(e311252db8a7232a5325a3eff5c1890d20bd3f8f) )
	ROM_LOAD( "e35-12.ic21",  0x0e00000, 0x200000, CRC(039b604c) SHA1(7e394e7cddc6bf42f3834d5331203e8496597a90) )

	ROM_REGION( 0x40000, "oki", 0 )     /* train board, OKI6295 sound samples */
	ROM_LOAD( "e35-28.trn",  0x000000, 0x040000, CRC(d1b571c1) SHA1(cac7d3f0285544fe36b8b744edfbac0190cdecab) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e35-17.ic32",  0x000000, 0x200000, CRC(3bc23aa1) SHA1(621e0f2f5f3dbd7d7ce05c9cd317958c361c1c26) )
	ROM_LOAD16_BYTE( "e35-18.ic33",  0x400000, 0x200000, CRC(a084d3aa) SHA1(18ea39366006e362e16d6732ce3e79cd3bfa041e) )
	ROM_LOAD16_BYTE( "e35-19.ic34",  0x800000, 0x200000, CRC(ebe2dcef) SHA1(16ae41e0f3bb242cbc2922f53cacbd99961a3f97) )
	ROM_LOAD16_BYTE( "e35-20.ic35",  0xc00000, 0x200000, CRC(a1d4b30d) SHA1(e02f613b93d3b3ee1eb23f5b7f62c5448ed3966d) )
ROM_END

ROM_START( dendegox )
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e35-30.ic36", 0x000000, 0x80000, CRC(57ee0975) SHA1(c7741a7e0e9c1fdebc6b942587d7ac5a6f26f66d) ) //ex
	ROM_LOAD32_BYTE( "e35-31.ic37", 0x000001, 0x80000, CRC(bd5f2651) SHA1(73b760df351170ace019e4b61c82d8c6296a3632) ) //ex
	ROM_LOAD32_BYTE( "e35-32.ic38", 0x000002, 0x80000, CRC(66be29d5) SHA1(e73937f5bda709a606d5cdf7316b26051317c22f) ) //ex
	ROM_LOAD32_BYTE( "e35-33.ic39", 0x000003, 0x80000, CRC(76a6bde2) SHA1(ca456ec3f0410777362e3eb977ae156866271bd5) ) //ex

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e35-25.ic30", 0x100001, 0x40000, CRC(8104de13) SHA1(e518fbaf91704cf5cb8ffbb4833e3adba8c18658) )
	ROM_LOAD16_BYTE( "e35-26.ic31", 0x100000, 0x40000, CRC(61821cc9) SHA1(87cd5bd3bb22c9f4ca4b6d96f75434d48418321b) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e35-05.ic9",   0x0800002, 0x200000, CRC(a94486c5) SHA1(c3f869aa0557411f747038a1e0ed6eedcf91fda5) )
	ROM_LOAD32_WORD( "e35-13.ic22",  0x0800000, 0x200000, CRC(2dc9dff1) SHA1(bc7ad64bc359f18a065e36749cc29c75e52202e2) )
	ROM_LOAD32_WORD( "e35-06.ic10",  0x0c00002, 0x200000, CRC(cf328021) SHA1(709ce80f9338637172dbf4e9adcacdb3e5b4ccdb) )
	ROM_LOAD32_WORD( "e35-14.ic23",  0x0c00000, 0x200000, CRC(cab50364) SHA1(e3272f844ecadfd63a1e3c965526a851f8cde94d) )
	ROM_LOAD32_WORD( "e35-07.ic11",  0x1000002, 0x200000, CRC(a88a5a86) SHA1(a4a393fe9df3654e41d32e015ca3977d13206dfe) )
	ROM_LOAD32_WORD( "e35-15.ic24",  0x1000000, 0x200000, CRC(aea86b35) SHA1(092f34f844fc6a779a6f18c03d44a9d39a101373) )
	ROM_LOAD32_WORD( "e35-08.ic12",  0x1400002, 0x200000, CRC(99425ff6) SHA1(3bd6fe7204dece55459392170b42d4c6a9d3ef5b) )
	ROM_LOAD32_WORD( "e35-16.ic25",  0x1400000, 0x200000, CRC(161481b6) SHA1(cc3c2939ac8911c197e9930580d316066f345772) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e35-01.ic5",   0x0000000, 0x200000, CRC(bd1975cb) SHA1(a08c6f4a84f9d4c2a5aa67cc2045aedd4580b8dc) )
	ROM_LOAD( "e35-02.ic6",   0x0200000, 0x200000, CRC(e5caa459) SHA1(c38d795b96fff193223cd3df9f51ebdc2971b719) )
	ROM_LOAD( "e35-03.ic7",   0x0400000, 0x200000, CRC(86ea5bcf) SHA1(1cee7f677b786b2fa9f50e723decd08cd69fbdef) )
	ROM_LOAD( "e35-04.ic8",   0x0600000, 0x200000, CRC(afc07c36) SHA1(f3f7b04766a9a2c8b298e1692aea24abc7001432) )
	ROM_LOAD( "e35-09.ic18",  0x0800000, 0x200000, CRC(be18ddf1) SHA1(d4fe26e427244c5b421b6421ff3bf9af303673d5) )
	ROM_LOAD( "e35-10.ic19",  0x0a00000, 0x200000, CRC(44ea9474) SHA1(161ff94b31c3dc2fa4b1e556ed62624147687443) )
	ROM_LOAD( "e35-11.ic20",  0x0c00000, 0x200000, CRC(dc8f5e88) SHA1(e311252db8a7232a5325a3eff5c1890d20bd3f8f) )
	ROM_LOAD( "e35-12.ic21",  0x0e00000, 0x200000, CRC(039b604c) SHA1(7e394e7cddc6bf42f3834d5331203e8496597a90) )

	ROM_REGION( 0x40000, "oki", 0 )     /* train board, OKI6295 sound samples */
	ROM_LOAD( "e35-28.trn",  0x000000, 0x040000, CRC(d1b571c1) SHA1(cac7d3f0285544fe36b8b744edfbac0190cdecab) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e35-17.ic32",  0x000000, 0x200000, CRC(3bc23aa1) SHA1(621e0f2f5f3dbd7d7ce05c9cd317958c361c1c26) )
	ROM_LOAD16_BYTE( "e35-18.ic33",  0x400000, 0x200000, CRC(a084d3aa) SHA1(18ea39366006e362e16d6732ce3e79cd3bfa041e) )
	ROM_LOAD16_BYTE( "e35-19.ic34",  0x800000, 0x200000, CRC(ebe2dcef) SHA1(16ae41e0f3bb242cbc2922f53cacbd99961a3f97) )
	ROM_LOAD16_BYTE( "e35-20.ic35",  0xc00000, 0x200000, CRC(a1d4b30d) SHA1(e02f613b93d3b3ee1eb23f5b7f62c5448ed3966d) )
ROM_END

ROM_START( dendego2 )
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e52-25-1.ic36", 0x000000, 0x80000, CRC(fadf5b4c) SHA1(48f3e1425bb9552d472a2720e1c9a752db2b43ed) )
	ROM_LOAD32_BYTE( "e52-26-1.ic37", 0x000001, 0x80000, CRC(7cf5230d) SHA1(b3416886d7cfc88520f6bf378529086bf0095db5) )
	ROM_LOAD32_BYTE( "e52-27-1.ic38", 0x000002, 0x80000, CRC(25f0d81d) SHA1(c33c3e6b1ad49b63b31a2f1227d43141faef4eab) )
	ROM_LOAD32_BYTE( "e52-28-1.ic39", 0x000003, 0x80000, CRC(e76ff6a1) SHA1(674c00f19df034de8134d48a8c2d2e42f7eb1be7) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e52-29.ic30",   0x100001, 0x40000, CRC(6010162a) SHA1(f14920b26887f5387b3e261b63573d850195982a) )
	ROM_LOAD16_BYTE( "e52-30.ic31",   0x100000, 0x40000, CRC(2881af4a) SHA1(5918f6508b3cd3bef3751e3bda2a48152569c1cd) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e52-17.ic52",  0x0000002, 0x200000, CRC(4ac11921) SHA1(c4816e1d68bb52ee59e7a2e6de617c1093020944) )
	ROM_LOAD32_WORD( "e52-18.ic53",  0x0000000, 0x200000, CRC(7f3e4af7) SHA1(ab35744014175a802e73c8b70de4e7508f0a1cd1) )
	ROM_LOAD32_WORD( "e52-19.ic54",  0x0400002, 0x200000, CRC(2e5ff408) SHA1(91f95721b98198082e950c50f33324820719e9ed) )
	ROM_LOAD32_WORD( "e52-20.ic55",  0x0400000, 0x200000, CRC(e90eb71e) SHA1(f07518c718f773e20412393c0ebb3243f9b1d96c) )
	ROM_LOAD32_WORD( "e52-05.ic9",   0x0800002, 0x200000, CRC(1ad0c612) SHA1(4ffc373fca8c1e1a5edbad3305b08f0867e9809c) )
	ROM_LOAD32_WORD( "e52-13.ic22",  0x0800000, 0x200000, CRC(943af3f4) SHA1(bfc81aa5e5c22e44601428b9e980f09d0c65e38e) )
	ROM_LOAD32_WORD( "e52-06.ic10",  0x0c00002, 0x200000, CRC(aa35e536) SHA1(2c1b2ee0d2587db6d6dd60b081bfcef3bb0dd9fa) )
	ROM_LOAD32_WORD( "e52-14.ic23",  0x0c00000, 0x200000, CRC(f311a9b4) SHA1(1f25571ac9468d453e92886e003400fffdc149f2) )
	ROM_LOAD32_WORD( "e52-07.ic11",  0x1000002, 0x200000, CRC(80a27984) SHA1(57b8c41809de09582600b8ff30c135d5abd044b8) )
	ROM_LOAD32_WORD( "e52-15.ic24",  0x1000000, 0x200000, CRC(b45a6199) SHA1(9339a1520b38d1f6538171a0b01df87de3e9c2d1) )
	ROM_LOAD32_WORD( "e52-08.ic12",  0x1400002, 0x200000, CRC(d52e6b9c) SHA1(382a5fd4533ab641a09321208464d83f72e161e3) )
	ROM_LOAD32_WORD( "e52-16.ic25",  0x1400000, 0x200000, CRC(db6dd6e2) SHA1(d345dbd745514d4777d52c4360787ea8c462ffb1) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e52-01.ic5",   0x0000000, 0x200000, CRC(8db39c3c) SHA1(74b3305ebdf679ae274c73b7b32d2adea602bedc) )
	ROM_LOAD( "e52-02.ic6",   0x0200000, 0x200000, CRC(b8d6f066) SHA1(99553ad66643ebf7fc71a9aee526d8f206b41dcc) )
	ROM_LOAD( "e52-03.ic7",   0x0400000, 0x200000, CRC(a37d164b) SHA1(767a7d2de8b91a00c5fe74710937457e8568a422) )
	ROM_LOAD( "e52-04.ic8",   0x0600000, 0x200000, CRC(403a4142) SHA1(aa5fea79a7a838642152586689d0f9b33311252c) )
	ROM_LOAD( "e52-09.ic18",  0x0800000, 0x200000, CRC(830e0a3c) SHA1(ab96f380e53f72945f73a6cfcc80d12e1b1afce8) )
	ROM_LOAD( "e52-10.ic19",  0x0a00000, 0x200000, CRC(671e41c6) SHA1(281899d83dba104daf7d7c2bd0834cab4c022472) )
	ROM_LOAD( "e52-11.ic20",  0x0c00000, 0x200000, CRC(1bc22680) SHA1(1f71db88d6df3b4bdf090b77bc83a67906bb31da) )
	ROM_LOAD( "e52-12.ic21",  0x0e00000, 0x200000, CRC(a8bb91c5) SHA1(959a9fedb7839e1e4e7658d920bd5da4fd8cae48) )

	ROM_REGION( 0x40000, "oki", 0 )     /* train board, OKI6295 sound samples */
	ROM_LOAD( "e35-28.trn",  0x000000, 0x040000, CRC(d1b571c1) SHA1(cac7d3f0285544fe36b8b744edfbac0190cdecab) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e52-21.ic32",  0x000000, 0x200000, CRC(ba58081d) SHA1(bcb6c8781191d48f906ed404a3e7388097a64781) )
	ROM_LOAD16_BYTE( "e52-22.ic33",  0x400000, 0x200000, CRC(dda281b1) SHA1(4851a6bf7902548c5033090a0e5c15f74c00ef58) )
	ROM_LOAD16_BYTE( "e52-23.ic34",  0x800000, 0x200000, CRC(ebe2dcef) SHA1(16ae41e0f3bb242cbc2922f53cacbd99961a3f97) ) // same as e35-19.ic34 from dendego
	ROM_LOAD16_BYTE( "e52-24.ic35",  0xc00000, 0x200000, CRC(a9a678da) SHA1(b980ae644ef0312acd63b017028af9bf2b084c29) )
ROM_END

ROM_START( dendego23k )
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e52-35.ic36", 0x000000, 0x80000, CRC(d5b33eb8) SHA1(e05ad73986741827b7bbeac72af0a8324384bf6b) ) //2ex
	ROM_LOAD32_BYTE( "e52-36.ic37", 0x000001, 0x80000, CRC(f3f3fabd) SHA1(4f88080091af2208d671c491284d992b5036908c) ) //2ex
	ROM_LOAD32_BYTE( "e52-37.ic38", 0x000002, 0x80000, CRC(65b8ef31) SHA1(b61b391b160e81715ff355aeef65026d7e4dd9af) ) //2ex
	ROM_LOAD32_BYTE( "e52-38.ic39", 0x000003, 0x80000, CRC(cf61f321) SHA1(c8493d2499afba673174b26044aca537e384916c) ) //2ex

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e52-29.ic30", 0x100001, 0x40000, CRC(6010162a) SHA1(f14920b26887f5387b3e261b63573d850195982a) )
	ROM_LOAD16_BYTE( "e52-30.ic31", 0x100000, 0x40000, CRC(2881af4a) SHA1(5918f6508b3cd3bef3751e3bda2a48152569c1cd) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e52-17.ic52",  0x0000002, 0x200000, CRC(4ac11921) SHA1(c4816e1d68bb52ee59e7a2e6de617c1093020944) )
	ROM_LOAD32_WORD( "e52-18.ic53",  0x0000000, 0x200000, CRC(7f3e4af7) SHA1(ab35744014175a802e73c8b70de4e7508f0a1cd1) )
	ROM_LOAD32_WORD( "e52-19.ic54",  0x0400002, 0x200000, CRC(2e5ff408) SHA1(91f95721b98198082e950c50f33324820719e9ed) )
	ROM_LOAD32_WORD( "e52-20.ic55",  0x0400000, 0x200000, CRC(e90eb71e) SHA1(f07518c718f773e20412393c0ebb3243f9b1d96c) )
	ROM_LOAD32_WORD( "e52-05.ic9",   0x0800002, 0x200000, CRC(1ad0c612) SHA1(4ffc373fca8c1e1a5edbad3305b08f0867e9809c) )
	ROM_LOAD32_WORD( "e52-13.ic22",  0x0800000, 0x200000, CRC(943af3f4) SHA1(bfc81aa5e5c22e44601428b9e980f09d0c65e38e) )
	ROM_LOAD32_WORD( "e52-31.ic10",  0x0c00002, 0x200000, CRC(e9e2eb3d) SHA1(97e2dc907faa512d3fb140dafe3250f04991ff07) ) //2ex
	ROM_LOAD32_WORD( "e52-33.ic23",  0x0c00000, 0x200000, CRC(6906f41f) SHA1(0d3f6fc4772417190d123ad38b0b8a8a532159c6) ) //2ex
	ROM_LOAD32_WORD( "e52-32.ic11",  0x1000002, 0x200000, CRC(43d452fd) SHA1(20a4064904cf2f21d5f03236c99c9e21a49a1206) ) //2ex
	ROM_LOAD32_WORD( "e52-34.ic24",  0x1000000, 0x200000, CRC(4ae1064b) SHA1(a5f1ad3262f8ffd09e9063a6dbe98d883b11a156) ) //2ex
	ROM_LOAD32_WORD( "e52-08.ic12",  0x1400002, 0x200000, CRC(d52e6b9c) SHA1(382a5fd4533ab641a09321208464d83f72e161e3) )
	ROM_LOAD32_WORD( "e52-16.ic25",  0x1400000, 0x200000, CRC(db6dd6e2) SHA1(d345dbd745514d4777d52c4360787ea8c462ffb1) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e52-01.ic5",   0x0000000, 0x200000, CRC(8db39c3c) SHA1(74b3305ebdf679ae274c73b7b32d2adea602bedc) )
	ROM_LOAD( "e52-02.ic6",   0x0200000, 0x200000, CRC(b8d6f066) SHA1(99553ad66643ebf7fc71a9aee526d8f206b41dcc) )
	ROM_LOAD( "e52-03.ic7",   0x0400000, 0x200000, CRC(a37d164b) SHA1(767a7d2de8b91a00c5fe74710937457e8568a422) )
	ROM_LOAD( "e52-04.ic8",   0x0600000, 0x200000, CRC(403a4142) SHA1(aa5fea79a7a838642152586689d0f9b33311252c) )
	ROM_LOAD( "e52-09.ic18",  0x0800000, 0x200000, CRC(830e0a3c) SHA1(ab96f380e53f72945f73a6cfcc80d12e1b1afce8) )
	ROM_LOAD( "e52-10.ic19",  0x0a00000, 0x200000, CRC(671e41c6) SHA1(281899d83dba104daf7d7c2bd0834cab4c022472) )
	ROM_LOAD( "e52-11.ic20",  0x0c00000, 0x200000, CRC(1bc22680) SHA1(1f71db88d6df3b4bdf090b77bc83a67906bb31da) )
	ROM_LOAD( "e52-12.ic21",  0x0e00000, 0x200000, CRC(a8bb91c5) SHA1(959a9fedb7839e1e4e7658d920bd5da4fd8cae48) )

	ROM_REGION( 0x40000, "oki", 0 )     /* train board, OKI6295 sound samples */
	ROM_LOAD( "e35-28.trn",  0x000000, 0x040000, CRC(d1b571c1) SHA1(cac7d3f0285544fe36b8b744edfbac0190cdecab) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e52-21.ic32",  0x000000, 0x200000, CRC(ba58081d) SHA1(bcb6c8781191d48f906ed404a3e7388097a64781) )
	ROM_LOAD16_BYTE( "e52-22.ic33",  0x400000, 0x200000, CRC(dda281b1) SHA1(4851a6bf7902548c5033090a0e5c15f74c00ef58) )
	ROM_LOAD16_BYTE( "e52-23.ic34",  0x800000, 0x200000, CRC(ebe2dcef) SHA1(16ae41e0f3bb242cbc2922f53cacbd99961a3f97) ) // same as e35-19.ic34 from dendego
	ROM_LOAD16_BYTE( "e52-24.ic35",  0xc00000, 0x200000, CRC(a9a678da) SHA1(b980ae644ef0312acd63b017028af9bf2b084c29) )
ROM_END

/* Landing Gear known program rom sets not dumped:

E17-28 through E17-31 (E17-32 is a PAL)

*/

ROM_START( landgear ) /* Landing Gear Ver 4.2 O */
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e17-37.ic36", 0x000000, 0x80000, CRC(e6dda113) SHA1(786cbfae420b6ee820a93731e59da3442245b6b8) )
	ROM_LOAD32_BYTE( "e17-38.ic37", 0x000001, 0x80000, CRC(86fa29bd) SHA1(f711528143c042cdc4a26d9e6965a882a73f397c) )
	ROM_LOAD32_BYTE( "e17-39.ic38", 0x000002, 0x80000, CRC(ccbbcc7b) SHA1(52d91fcaa1683d2679ed4f14ebc11dc487527898) )
	ROM_LOAD32_BYTE( "e17-40.ic39", 0x000003, 0x80000, CRC(ce9231d2) SHA1(d2c3955d910dbd0cac95862047c58791af626722) ) /* 0x7ffff == 03 - One byte difference from E17-36.ic39 */

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e17-21.ic30", 0x100001, 0x40000, CRC(8b54f46c) SHA1(c6d16197ab7768945becf9b49b6d286113b4d1cc) )
	ROM_LOAD16_BYTE( "e17-22.ic31", 0x100000, 0x40000, CRC(b96f6cd7) SHA1(0bf086e5dc6d524cd00e33df3e3d2a8b9231eb72) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e17-03.ic9",   0x0800002, 0x200000, CRC(64820c4f) SHA1(ee18e4e2b01ec21c33ec1f0eb43f6d0cd48d7225) )
	ROM_LOAD32_WORD( "e17-09.ic22",  0x0800000, 0x200000, CRC(19e9a1d1) SHA1(26f1a91e3757da510d685a11add08e3e00317796) )
	ROM_LOAD32_WORD( "e17-04.ic10",  0x0c00002, 0x200000, CRC(7dc2cae3) SHA1(90638a1efc353428ce4155ca29f67accaf0499cd) )
	ROM_LOAD32_WORD( "e17-10.ic23",  0x0c00000, 0x200000, CRC(a6bdf6b8) SHA1(e8d76d38f2c7e428a3c2f555571e314351d74a69) )
	ROM_LOAD32_WORD( "e17-05.ic11",  0x1000002, 0x200000, CRC(3f70acd4) SHA1(e8c1c6214631e3e39d1fc9df13d1862442a47e5d) )
	ROM_LOAD32_WORD( "e17-11.ic24",  0x1000000, 0x200000, CRC(4e986d93) SHA1(b218a0360c1d0eca5a907f2b402f352e0329fe41) )
	ROM_LOAD32_WORD( "e17-06.ic12",  0x1400002, 0x200000, CRC(107ff481) SHA1(2a48cedec9641ff08776e5d8b1bf1f5b250d4179) )
	ROM_LOAD32_WORD( "e17-12.ic25",  0x1400000, 0x200000, CRC(0727ddfa) SHA1(68bf83a3c46cd042a7ad27a530c8bed6360d8492) )

	ROM_REGION( 0x1000000, "gfx2", ROMREGION_ERASE00 )      /* only accessible to the TMS */
	ROM_LOAD( "e17-01.ic5",   0x0000000, 0x200000, CRC(42aa56a6) SHA1(945c338515ceb946c01480919546869bb8c3d323) )
	ROM_LOAD( "e17-02.ic8",   0x0600000, 0x200000, CRC(df7e2405) SHA1(684d6fc398791c48101e6cb63acbf0d691ed863c) )
	ROM_LOAD( "e17-07.ic18",  0x0800000, 0x200000, CRC(0f180eb0) SHA1(5e1dd920f110a62a029bace6f4cb80fee0fdaf03) )
	ROM_LOAD( "e17-08.ic19",  0x0a00000, 0x200000, CRC(3107e154) SHA1(59a99770c2aa535cac6569f41b03be1554e0e800) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e17-13.ic32",  0x000000, 0x200000, CRC(6cf238e7) SHA1(0745d2dcfea26178adde3ad08650156e8e30651f) )
	ROM_LOAD16_BYTE( "e17-14.ic33",  0x400000, 0x200000, CRC(5efec311) SHA1(f253bc40f2567f59ddfb617fddb8b9a389bfac89) )
	ROM_LOAD16_BYTE( "e17-15.ic34",  0x800000, 0x200000, CRC(41d7a7d0) SHA1(f5a8b79c1d47611e93d46aaf921107b52090bb5f) )
	ROM_LOAD16_BYTE( "e17-16.ic35",  0xc00000, 0x200000, CRC(6cf9f277) SHA1(03ca51fadc6b0b6502804346f18eeb55ab87b0e7) )

	ROM_REGION( 0x1000, "pals", 0 ) /* PALCE 16V8, saved in Jedec format (unused now) */
	ROM_LOAD( "e07-02.ic4",    0x0000, 0x0bac, CRC(b10110e0) SHA1(574dfa70cbdc910973f4b47a9534f22839baf76d) )
	ROM_LOAD( "e07-03.ic50",   0x0000, 0x0bac, CRC(3fe03710) SHA1(bbccddea0cccb50ea361721e51a0489f6686312c) )
	ROM_LOAD( "e07-04.ic115",  0x0000, 0x0bac, CRC(6c83e648) SHA1(7ed4001d8f27933b31c09d98421dac5bdc265ff4) )
	ROM_LOAD( "e07-05.ic22",   0x0000, 0x0bac, CRC(d2530a9d) SHA1(2f5cafe9ac4e5b3121981dc7498a82b54b2aee08) )
	ROM_LOAD( "e07-06.ic37",   0x0000, 0x0bac, CRC(ade68194) SHA1(95322d49b1f6edac03a7a00bf8876e556ccc2581) )
	ROM_LOAD( "e07-07.ic49",   0x0000, 0x0bac, CRC(76136aac) SHA1(5f28de747e32a4e2622603a7d35e6c979e56977d) )
	ROM_LOAD( "e07-08.ic65",   0x0000, 0x0bac, CRC(33aa1678) SHA1(e0d941c048bec25994dea30fed989d9f9e1af6a6) )
	ROM_LOAD( "e07-09.ic82",   0x0000, 0x0bac, CRC(62e2bf9e) SHA1(fe7055f5f6292f4c613cc3a47245340722299b45) )
	ROM_LOAD( "e07-10.ic116",  0x0000, 0x0bac, CRC(97e36b54) SHA1(2181097bfd7d09a537cbcc4e7ead11532f0ddb20) )
	ROM_LOAD( "e09-21.ic69",   0x0000, 0x0bac, CRC(74b06310) SHA1(0e1f559247a43fbb3852b1b0b92753ed993b876d) )
	ROM_LOAD( "e09-22.ic73",   0x0000, 0x0bac, CRC(58aa9c49) SHA1(7c9b463ea1adc701f326773cc556ab11f290f87e) )
	ROM_LOAD( "e17-32.ic96",   0x0000, 0x0bac, CRC(1d590ca3) SHA1(733e9e34642c1e03fd880fda198b98927c1bb81d) )
ROM_END

ROM_START( landgearj ) /* Landing Gear Ver 4.2 J */
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e17-33.ic36", 0x000000, 0x80000, CRC(e6dda113) SHA1(786cbfae420b6ee820a93731e59da3442245b6b8) ) /* matches E17-37.ic36, verified correct */
	ROM_LOAD32_BYTE( "e17-34.ic37", 0x000001, 0x80000, CRC(86fa29bd) SHA1(f711528143c042cdc4a26d9e6965a882a73f397c) ) /* matches E17-38.ic37, verified correct */
	ROM_LOAD32_BYTE( "e17-35.ic38", 0x000002, 0x80000, CRC(ccbbcc7b) SHA1(52d91fcaa1683d2679ed4f14ebc11dc487527898) ) /* matches E17-39.ic38, verified correct */
	ROM_LOAD32_BYTE( "e17-36.ic39", 0x000003, 0x80000, CRC(209c50fe) SHA1(42e0eaa182730e260ee4361d936b133ed85f8221) ) /* 0x7ffff == 01 - One byte difference from E17-40.ic39 */

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e17-21.ic30", 0x100001, 0x40000, CRC(8b54f46c) SHA1(c6d16197ab7768945becf9b49b6d286113b4d1cc) )
	ROM_LOAD16_BYTE( "e17-22.ic31", 0x100000, 0x40000, CRC(b96f6cd7) SHA1(0bf086e5dc6d524cd00e33df3e3d2a8b9231eb72) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e17-03.ic9",   0x0800002, 0x200000, CRC(64820c4f) SHA1(ee18e4e2b01ec21c33ec1f0eb43f6d0cd48d7225) )
	ROM_LOAD32_WORD( "e17-09.ic22",  0x0800000, 0x200000, CRC(19e9a1d1) SHA1(26f1a91e3757da510d685a11add08e3e00317796) )
	ROM_LOAD32_WORD( "e17-04.ic10",  0x0c00002, 0x200000, CRC(7dc2cae3) SHA1(90638a1efc353428ce4155ca29f67accaf0499cd) )
	ROM_LOAD32_WORD( "e17-10.ic23",  0x0c00000, 0x200000, CRC(a6bdf6b8) SHA1(e8d76d38f2c7e428a3c2f555571e314351d74a69) )
	ROM_LOAD32_WORD( "e17-05.ic11",  0x1000002, 0x200000, CRC(3f70acd4) SHA1(e8c1c6214631e3e39d1fc9df13d1862442a47e5d) )
	ROM_LOAD32_WORD( "e17-11.ic24",  0x1000000, 0x200000, CRC(4e986d93) SHA1(b218a0360c1d0eca5a907f2b402f352e0329fe41) )
	ROM_LOAD32_WORD( "e17-06.ic12",  0x1400002, 0x200000, CRC(107ff481) SHA1(2a48cedec9641ff08776e5d8b1bf1f5b250d4179) )
	ROM_LOAD32_WORD( "e17-12.ic25",  0x1400000, 0x200000, CRC(0727ddfa) SHA1(68bf83a3c46cd042a7ad27a530c8bed6360d8492) )

	ROM_REGION( 0x1000000, "gfx2", ROMREGION_ERASE00 )      /* only accessible to the TMS */
	ROM_LOAD( "e17-01.ic5",   0x0000000, 0x200000, CRC(42aa56a6) SHA1(945c338515ceb946c01480919546869bb8c3d323) )
	ROM_LOAD( "e17-02.ic8",   0x0600000, 0x200000, CRC(df7e2405) SHA1(684d6fc398791c48101e6cb63acbf0d691ed863c) )
	ROM_LOAD( "e17-07.ic18",  0x0800000, 0x200000, CRC(0f180eb0) SHA1(5e1dd920f110a62a029bace6f4cb80fee0fdaf03) )
	ROM_LOAD( "e17-08.ic19",  0x0a00000, 0x200000, CRC(3107e154) SHA1(59a99770c2aa535cac6569f41b03be1554e0e800) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e17-13.ic32",  0x000000, 0x200000, CRC(6cf238e7) SHA1(0745d2dcfea26178adde3ad08650156e8e30651f) )
	ROM_LOAD16_BYTE( "e17-14.ic33",  0x400000, 0x200000, CRC(5efec311) SHA1(f253bc40f2567f59ddfb617fddb8b9a389bfac89) )
	ROM_LOAD16_BYTE( "e17-15.ic34",  0x800000, 0x200000, CRC(41d7a7d0) SHA1(f5a8b79c1d47611e93d46aaf921107b52090bb5f) )
	ROM_LOAD16_BYTE( "e17-16.ic35",  0xc00000, 0x200000, CRC(6cf9f277) SHA1(03ca51fadc6b0b6502804346f18eeb55ab87b0e7) )

	ROM_REGION( 0x1000, "pals", 0 ) /* PALCE 16V8, saved in Jedec format (unused now) */
	ROM_LOAD( "e07-02.ic4",    0x0000, 0x0bac, CRC(b10110e0) SHA1(574dfa70cbdc910973f4b47a9534f22839baf76d) )
	ROM_LOAD( "e07-03.ic50",   0x0000, 0x0bac, CRC(3fe03710) SHA1(bbccddea0cccb50ea361721e51a0489f6686312c) )
	ROM_LOAD( "e07-04.ic115",  0x0000, 0x0bac, CRC(6c83e648) SHA1(7ed4001d8f27933b31c09d98421dac5bdc265ff4) )
	ROM_LOAD( "e07-05.ic22",   0x0000, 0x0bac, CRC(d2530a9d) SHA1(2f5cafe9ac4e5b3121981dc7498a82b54b2aee08) )
	ROM_LOAD( "e07-06.ic37",   0x0000, 0x0bac, CRC(ade68194) SHA1(95322d49b1f6edac03a7a00bf8876e556ccc2581) )
	ROM_LOAD( "e07-07.ic49",   0x0000, 0x0bac, CRC(76136aac) SHA1(5f28de747e32a4e2622603a7d35e6c979e56977d) )
	ROM_LOAD( "e07-08.ic65",   0x0000, 0x0bac, CRC(33aa1678) SHA1(e0d941c048bec25994dea30fed989d9f9e1af6a6) )
	ROM_LOAD( "e07-09.ic82",   0x0000, 0x0bac, CRC(62e2bf9e) SHA1(fe7055f5f6292f4c613cc3a47245340722299b45) )
	ROM_LOAD( "e07-10.ic116",  0x0000, 0x0bac, CRC(97e36b54) SHA1(2181097bfd7d09a537cbcc4e7ead11532f0ddb20) )
	ROM_LOAD( "e09-21.ic69",   0x0000, 0x0bac, CRC(74b06310) SHA1(0e1f559247a43fbb3852b1b0b92753ed993b876d) )
	ROM_LOAD( "e09-22.ic73",   0x0000, 0x0bac, CRC(58aa9c49) SHA1(7c9b463ea1adc701f326773cc556ab11f290f87e) )
	ROM_LOAD( "e17-32.ic96",   0x0000, 0x0bac, CRC(1d590ca3) SHA1(733e9e34642c1e03fd880fda198b98927c1bb81d) )
ROM_END

ROM_START( landgeara ) /* Landing Gear Ver 3.1 O, is there an alternate set without the "*" on the labels? */
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e17-24+.ic36", 0x000000, 0x80000, CRC(6907e451) SHA1(330eecb5898942514b40e67cf3c9dcb82d4cafab) ) /* Actually labeled E17-24* */
	ROM_LOAD32_BYTE( "e17-25+.ic37", 0x000001, 0x80000, CRC(ecbc8875) SHA1(5f5e4850cbdbdfff4a7f0b781edb2e983c166962) ) /* Actually labeled E17-25* */
	ROM_LOAD32_BYTE( "e17-26+.ic38", 0x000002, 0x80000, CRC(3032bbe7) SHA1(201c61f236c81928f50815d8ad12e312a3c7427b) ) /* Actually labeled E17-26* */
	ROM_LOAD32_BYTE( "e17-27+.ic39", 0x000003, 0x80000, CRC(e936224c) SHA1(8699cbb756844d12b7585e66198b7faed2af8e24) ) /* Actually labeled E17-27* */

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e17-21.ic30",  0x100001, 0x40000, CRC(8b54f46c) SHA1(c6d16197ab7768945becf9b49b6d286113b4d1cc) )
	ROM_LOAD16_BYTE( "e17-22.ic31",  0x100000, 0x40000, CRC(b96f6cd7) SHA1(0bf086e5dc6d524cd00e33df3e3d2a8b9231eb72) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e17-03.ic9",   0x0800002, 0x200000, CRC(64820c4f) SHA1(ee18e4e2b01ec21c33ec1f0eb43f6d0cd48d7225) )
	ROM_LOAD32_WORD( "e17-09.ic22",  0x0800000, 0x200000, CRC(19e9a1d1) SHA1(26f1a91e3757da510d685a11add08e3e00317796) )
	ROM_LOAD32_WORD( "e17-04.ic10",  0x0c00002, 0x200000, CRC(7dc2cae3) SHA1(90638a1efc353428ce4155ca29f67accaf0499cd) )
	ROM_LOAD32_WORD( "e17-10.ic23",  0x0c00000, 0x200000, CRC(a6bdf6b8) SHA1(e8d76d38f2c7e428a3c2f555571e314351d74a69) )
	ROM_LOAD32_WORD( "e17-05.ic11",  0x1000002, 0x200000, CRC(3f70acd4) SHA1(e8c1c6214631e3e39d1fc9df13d1862442a47e5d) )
	ROM_LOAD32_WORD( "e17-11.ic24",  0x1000000, 0x200000, CRC(4e986d93) SHA1(b218a0360c1d0eca5a907f2b402f352e0329fe41) )
	ROM_LOAD32_WORD( "e17-06.ic12",  0x1400002, 0x200000, CRC(107ff481) SHA1(2a48cedec9641ff08776e5d8b1bf1f5b250d4179) )
	ROM_LOAD32_WORD( "e17-12.ic25",  0x1400000, 0x200000, CRC(0727ddfa) SHA1(68bf83a3c46cd042a7ad27a530c8bed6360d8492) )

	ROM_REGION( 0x1000000, "gfx2", ROMREGION_ERASE00 )      /* only accessible to the TMS */
	ROM_LOAD( "e17-01.ic5",   0x0000000, 0x200000, CRC(42aa56a6) SHA1(945c338515ceb946c01480919546869bb8c3d323) )
	ROM_LOAD( "e17-02.ic8",   0x0600000, 0x200000, CRC(df7e2405) SHA1(684d6fc398791c48101e6cb63acbf0d691ed863c) )
	ROM_LOAD( "e17-07.ic18",  0x0800000, 0x200000, CRC(0f180eb0) SHA1(5e1dd920f110a62a029bace6f4cb80fee0fdaf03) )
	ROM_LOAD( "e17-08.ic19",  0x0a00000, 0x200000, CRC(3107e154) SHA1(59a99770c2aa535cac6569f41b03be1554e0e800) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e17-13.ic32",  0x000000, 0x200000, CRC(6cf238e7) SHA1(0745d2dcfea26178adde3ad08650156e8e30651f) )
	ROM_LOAD16_BYTE( "e17-14.ic33",  0x400000, 0x200000, CRC(5efec311) SHA1(f253bc40f2567f59ddfb617fddb8b9a389bfac89) )
	ROM_LOAD16_BYTE( "e17-15.ic34",  0x800000, 0x200000, CRC(41d7a7d0) SHA1(f5a8b79c1d47611e93d46aaf921107b52090bb5f) )
	ROM_LOAD16_BYTE( "e17-16.ic35",  0xc00000, 0x200000, CRC(6cf9f277) SHA1(03ca51fadc6b0b6502804346f18eeb55ab87b0e7) )

	ROM_REGION( 0x1000, "pals", 0 ) /* PALCE 16V8, saved in Jedec format (unused now) */
	ROM_LOAD( "e07-02.ic4",    0x0000, 0x0bac, CRC(b10110e0) SHA1(574dfa70cbdc910973f4b47a9534f22839baf76d) )
	ROM_LOAD( "e07-03.ic50",   0x0000, 0x0bac, CRC(3fe03710) SHA1(bbccddea0cccb50ea361721e51a0489f6686312c) )
	ROM_LOAD( "e07-04.ic115",  0x0000, 0x0bac, CRC(6c83e648) SHA1(7ed4001d8f27933b31c09d98421dac5bdc265ff4) )
	ROM_LOAD( "e07-05.ic22",   0x0000, 0x0bac, CRC(d2530a9d) SHA1(2f5cafe9ac4e5b3121981dc7498a82b54b2aee08) )
	ROM_LOAD( "e07-06.ic37",   0x0000, 0x0bac, CRC(ade68194) SHA1(95322d49b1f6edac03a7a00bf8876e556ccc2581) )
	ROM_LOAD( "e07-07.ic49",   0x0000, 0x0bac, CRC(76136aac) SHA1(5f28de747e32a4e2622603a7d35e6c979e56977d) )
	ROM_LOAD( "e07-08.ic65",   0x0000, 0x0bac, CRC(33aa1678) SHA1(e0d941c048bec25994dea30fed989d9f9e1af6a6) )
	ROM_LOAD( "e07-09.ic82",   0x0000, 0x0bac, CRC(62e2bf9e) SHA1(fe7055f5f6292f4c613cc3a47245340722299b45) )
	ROM_LOAD( "e07-10.ic116",  0x0000, 0x0bac, CRC(97e36b54) SHA1(2181097bfd7d09a537cbcc4e7ead11532f0ddb20) )
	ROM_LOAD( "e09-21.ic69",   0x0000, 0x0bac, CRC(74b06310) SHA1(0e1f559247a43fbb3852b1b0b92753ed993b876d) )
	ROM_LOAD( "e09-22.ic73",   0x0000, 0x0bac, CRC(58aa9c49) SHA1(7c9b463ea1adc701f326773cc556ab11f290f87e) )
	ROM_LOAD( "e17-32.ic96",   0x0000, 0x0bac, CRC(1d590ca3) SHA1(733e9e34642c1e03fd880fda198b98927c1bb81d) )
ROM_END

ROM_START( landgearja ) /* Landing Gear Ver 3.0 J, is there an alternate set without the "*" on the labels? */
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e17-17+.ic36", 0x000000, 0x80000, CRC(653e9c43) SHA1(b43c4baf1b3114977faa310c0815ea0940d548b3) ) /* Actually labeled E17-17* */
	ROM_LOAD32_BYTE( "e17-18+.ic37", 0x000001, 0x80000, CRC(4d90b321) SHA1(a919f15dcc105eaa12d7c9816aff4f0daffbb7a1) ) /* Actually labeled E17-18* */
	ROM_LOAD32_BYTE( "e17-19+.ic38", 0x000002, 0x80000, CRC(1c487204) SHA1(f6c8ddd80c57ed63b0785b240c4b00416a1a87f3) ) /* Actually labeled E17-19* */
	ROM_LOAD32_BYTE( "e17-20+.ic39", 0x000003, 0x80000, CRC(1311234f) SHA1(5211cae0d6dc1710bc669bcf81a247b01f8aebff) ) /* Actually labeled E17-20* */

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e17-21.ic30",  0x100001, 0x40000, CRC(8b54f46c) SHA1(c6d16197ab7768945becf9b49b6d286113b4d1cc) )
	ROM_LOAD16_BYTE( "e17-22.ic31",  0x100000, 0x40000, CRC(b96f6cd7) SHA1(0bf086e5dc6d524cd00e33df3e3d2a8b9231eb72) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e17-23.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e17-03.ic9",   0x0800002, 0x200000, CRC(64820c4f) SHA1(ee18e4e2b01ec21c33ec1f0eb43f6d0cd48d7225) )
	ROM_LOAD32_WORD( "e17-09.ic22",  0x0800000, 0x200000, CRC(19e9a1d1) SHA1(26f1a91e3757da510d685a11add08e3e00317796) )
	ROM_LOAD32_WORD( "e17-04.ic10",  0x0c00002, 0x200000, CRC(7dc2cae3) SHA1(90638a1efc353428ce4155ca29f67accaf0499cd) )
	ROM_LOAD32_WORD( "e17-10.ic23",  0x0c00000, 0x200000, CRC(a6bdf6b8) SHA1(e8d76d38f2c7e428a3c2f555571e314351d74a69) )
	ROM_LOAD32_WORD( "e17-05.ic11",  0x1000002, 0x200000, CRC(3f70acd4) SHA1(e8c1c6214631e3e39d1fc9df13d1862442a47e5d) )
	ROM_LOAD32_WORD( "e17-11.ic24",  0x1000000, 0x200000, CRC(4e986d93) SHA1(b218a0360c1d0eca5a907f2b402f352e0329fe41) )
	ROM_LOAD32_WORD( "e17-06.ic12",  0x1400002, 0x200000, CRC(107ff481) SHA1(2a48cedec9641ff08776e5d8b1bf1f5b250d4179) )
	ROM_LOAD32_WORD( "e17-12.ic25",  0x1400000, 0x200000, CRC(0727ddfa) SHA1(68bf83a3c46cd042a7ad27a530c8bed6360d8492) )

	ROM_REGION( 0x1000000, "gfx2", ROMREGION_ERASE00 )      /* only accessible to the TMS */
	ROM_LOAD( "e17-01.ic5",   0x0000000, 0x200000, CRC(42aa56a6) SHA1(945c338515ceb946c01480919546869bb8c3d323) )
	ROM_LOAD( "e17-02.ic8",   0x0600000, 0x200000, CRC(df7e2405) SHA1(684d6fc398791c48101e6cb63acbf0d691ed863c) )
	ROM_LOAD( "e17-07.ic18",  0x0800000, 0x200000, CRC(0f180eb0) SHA1(5e1dd920f110a62a029bace6f4cb80fee0fdaf03) )
	ROM_LOAD( "e17-08.ic19",  0x0a00000, 0x200000, CRC(3107e154) SHA1(59a99770c2aa535cac6569f41b03be1554e0e800) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e17-13.ic32",  0x000000, 0x200000, CRC(6cf238e7) SHA1(0745d2dcfea26178adde3ad08650156e8e30651f) )
	ROM_LOAD16_BYTE( "e17-14.ic33",  0x400000, 0x200000, CRC(5efec311) SHA1(f253bc40f2567f59ddfb617fddb8b9a389bfac89) )
	ROM_LOAD16_BYTE( "e17-15.ic34",  0x800000, 0x200000, CRC(41d7a7d0) SHA1(f5a8b79c1d47611e93d46aaf921107b52090bb5f) )
	ROM_LOAD16_BYTE( "e17-16.ic35",  0xc00000, 0x200000, CRC(6cf9f277) SHA1(03ca51fadc6b0b6502804346f18eeb55ab87b0e7) )

	ROM_REGION( 0x1000, "pals", 0 ) /* PALCE 16V8, saved in Jedec format (unused now) */
	ROM_LOAD( "e07-02.ic4",    0x0000, 0x0bac, CRC(b10110e0) SHA1(574dfa70cbdc910973f4b47a9534f22839baf76d) )
	ROM_LOAD( "e07-03.ic50",   0x0000, 0x0bac, CRC(3fe03710) SHA1(bbccddea0cccb50ea361721e51a0489f6686312c) )
	ROM_LOAD( "e07-04.ic115",  0x0000, 0x0bac, CRC(6c83e648) SHA1(7ed4001d8f27933b31c09d98421dac5bdc265ff4) )
	ROM_LOAD( "e07-05.ic22",   0x0000, 0x0bac, CRC(d2530a9d) SHA1(2f5cafe9ac4e5b3121981dc7498a82b54b2aee08) )
	ROM_LOAD( "e07-06.ic37",   0x0000, 0x0bac, CRC(ade68194) SHA1(95322d49b1f6edac03a7a00bf8876e556ccc2581) )
	ROM_LOAD( "e07-07.ic49",   0x0000, 0x0bac, CRC(76136aac) SHA1(5f28de747e32a4e2622603a7d35e6c979e56977d) )
	ROM_LOAD( "e07-08.ic65",   0x0000, 0x0bac, CRC(33aa1678) SHA1(e0d941c048bec25994dea30fed989d9f9e1af6a6) )
	ROM_LOAD( "e07-09.ic82",   0x0000, 0x0bac, CRC(62e2bf9e) SHA1(fe7055f5f6292f4c613cc3a47245340722299b45) )
	ROM_LOAD( "e07-10.ic116",  0x0000, 0x0bac, CRC(97e36b54) SHA1(2181097bfd7d09a537cbcc4e7ead11532f0ddb20) )
	ROM_LOAD( "e09-21.ic69",   0x0000, 0x0bac, CRC(74b06310) SHA1(0e1f559247a43fbb3852b1b0b92753ed993b876d) )
	ROM_LOAD( "e09-22.ic73",   0x0000, 0x0bac, CRC(58aa9c49) SHA1(7c9b463ea1adc701f326773cc556ab11f290f87e) )
	ROM_LOAD( "e17-32.ic96",   0x0000, 0x0bac, CRC(1d590ca3) SHA1(733e9e34642c1e03fd880fda198b98927c1bb81d) )
ROM_END

ROM_START( dangcurv )
	ROM_REGION( 0x200000, "maincpu", 0 )        /* 68040 code */
	ROM_LOAD32_BYTE( "e09-23.ic36", 0x000000, 0x80000, CRC(b4cdadd6) SHA1(84bd1d055ff15afb5438cd5151abf78b0000cebc) )
	ROM_LOAD32_BYTE( "e09-24.ic37", 0x000001, 0x80000, CRC(fb2fc795) SHA1(2f58d043ab9fc0269a5b6827009777cd7ab832fc) )
	ROM_LOAD32_BYTE( "e09-25.ic38", 0x000002, 0x80000, CRC(aa233404) SHA1(a2b14e54eb1b5f6d4ed9f289b30ecfa654f21c87) )
	ROM_LOAD32_BYTE( "e09-26.ic39", 0x000003, 0x80000, CRC(78337271) SHA1(bd29de6a5b6db3baddecf82c3b6c8b366c64289e) )

	ROM_REGION( 0x180000, "audiocpu", 0 )       /* 68000 Code */
	ROM_LOAD16_BYTE( "e09-27.ic30", 0x100001, 0x40000, CRC(6d54839c) SHA1(a28c9b0727128b82bb0fa71dc951e3f03ee45e1b) )
	ROM_LOAD16_BYTE( "e09-28.ic31", 0x100000, 0x40000, CRC(566d7d83) SHA1(92661ccb631f843bf704c50d54fae28f6b5b272b) )

	ROM_REGION( 0x4000, "dsp", ROMREGION_ERASE00 ) /* TMS320C51 internal rom */
	ROM_LOAD16_WORD( "e07-11.ic29", 0x0000, 0x4000, NO_DUMP )

	ROM_REGION( 0x010000, "sub", 0 )        /* MC68HC11M0 code */
	ROM_LOAD( "e09-29.ic65",  0x000000, 0x010000, CRC(80ac1428) SHA1(5a2a1e60a11ecdb8743c20ddacfb61f9fd00f01c) )

	ROM_REGION( 0x1800000, "gfx1", 0 )
	ROM_LOAD32_WORD( "e09-05.ic9",   0x0800002, 0x200000, CRC(a948782f) SHA1(2a2b0d2955e036ddf424c54131435a20dbba3dd4) )
	ROM_LOAD32_WORD( "e09-13.ic22",  0x0800000, 0x200000, CRC(985859e2) SHA1(8af9a73eba2151a5ef60799682fe667663a42743) )
	ROM_LOAD32_WORD( "e09-06.ic10",  0x0c00002, 0x200000, CRC(218dcb5b) SHA1(72aedd2890e076540195d738c76ba446769c8e89) )
	ROM_LOAD32_WORD( "e09-14.ic23",  0x0c00000, 0x200000, CRC(6d123616) SHA1(01ac1e920f7c4a03adf365c8a7831b8385f0b78b) )
	ROM_LOAD32_WORD( "e09-07.ic11",  0x1000002, 0x200000, CRC(37fd7efc) SHA1(24a275d302ec8940479d15f1aeb96a288868bd41) )
	ROM_LOAD32_WORD( "e09-15.ic24",  0x1000000, 0x200000, CRC(0d773f3b) SHA1(f867a4d5956c2ebfa9858499d9716b4dc723d76b) )
	ROM_LOAD32_WORD( "e09-08.ic12",  0x1400002, 0x200000, CRC(5c080485) SHA1(c950cd00df5b6d2d0a119ba318fa8b0a3f471b29) )
	ROM_LOAD32_WORD( "e09-16.ic25",  0x1400000, 0x200000, CRC(35cb8346) SHA1(c2ecedd3c2a28213ef83e776f3007c974128189b) )

	ROM_REGION( 0x1000000, "gfx2", 0 )      /* only accessible to the TMS */
	ROM_LOAD( "e09-01.ic5",   0x0000000, 0x200000, CRC(22a6a53d) SHA1(6efa89151cd5ec43ab9bfa9b92694eb0018dd227) )
	ROM_LOAD( "e09-02.ic6",   0x0200000, 0x200000, CRC(405e2969) SHA1(376b9dd548d876af6798553a6da5deed4de00b76) )
	ROM_LOAD( "e09-03.ic7",   0x0400000, 0x200000, CRC(15327754) SHA1(bf08ab80875b400700241a66715e229dae6752d1) )
	ROM_LOAD( "e09-04.ic8",   0x0600000, 0x200000, CRC(fd598d6e) SHA1(679d9d64a0cd031a6c8cb5e170b77fc5811b6d73) )
	ROM_LOAD( "e09-09.ic18",  0x0800000, 0x200000, CRC(a527b387) SHA1(790240b4dfcdf2bf70edb943ec7aeb2f0d8cdfa9) )
	ROM_LOAD( "e09-10.ic19",  0x0a00000, 0x200000, CRC(4de6253c) SHA1(33517c0895b7ee04f4a84074d0b7bf42b53d5816) )
	ROM_LOAD( "e09-11.ic20",  0x0c00000, 0x200000, CRC(18cc0ba7) SHA1(626929a501def6f1b8bd6a468786efb0b0dda9fa) )
	ROM_LOAD( "e09-12.ic21",  0x0e00000, 0x200000, CRC(3273e438) SHA1(e9581d52f5db1c1924a860464579332a2f23e713) )

	ROM_REGION16_BE( 0x1000000, "ensoniq.0", ROMREGION_ERASE00  )
	ROM_LOAD16_BYTE( "e09-17.ic32",  0x000000, 0x200000, CRC(a8a6512e) SHA1(71bd3ccd65b731270b92da334b9fb99c28e267fe) )
	ROM_LOAD16_BYTE( "e09-18.ic33",  0x400000, 0x200000, CRC(bdf1f5eb) SHA1(a568a99a90e0afbcd26ddd320f515ed62cf0db1a) )
	ROM_LOAD16_BYTE( "e09-19.ic34",  0x800000, 0x200000, CRC(3626c7ed) SHA1(7535f0457b3d9fe1d54712a26322d6144b9e7de6) )
	ROM_LOAD16_BYTE( "e09-20.ic35",  0xc00000, 0x200000, CRC(9652a5c4) SHA1(2933e4e8e57ff618ce21721036d96347471c5539) )
ROM_END


GAME( 1995, dangcurv,  0,        taitojc, dangcurv, taitojc_state, dangcurv, ROT0, "Taito", "Dangerous Curves (Ver 2.2 J)",                         GAME_NOT_WORKING )                        // DANGEROUS CURVES       VER 2.2 J   1995.07.20   17:45
GAME( 1995, landgear,  0,        taitojc, landgear, taitojc_state, taitojc,  ROT0, "Taito", "Landing Gear (Ver 4.2 O)",                             GAME_IMPERFECT_GRAPHICS )                 // LANDING GEAR           VER 4.2 O   Feb  8 1996  09:46:22
GAME( 1995, landgearj, landgear, taitojc, landgear, taitojc_state, taitojc,  ROT0, "Taito", "Landing Gear (Ver 4.2 J)",                             GAME_IMPERFECT_GRAPHICS )                 // LANDING GEAR           VER 4.2 J   Feb  8 1996  09:46:22
GAME( 1995, landgeara, landgear, taitojc, landgear, taitojc_state, taitojc,  ROT0, "Taito", "Landing Gear (Ver 3.1 O)",                             GAME_IMPERFECT_GRAPHICS )                 // LANDING GEAR           VER 3.1 O   Feb  8 1996  09:46:22
GAME( 1995, landgearja,landgear, taitojc, landgear, taitojc_state, taitojc,  ROT0, "Taito", "Landing Gear (Ver 3.0 J)",                             GAME_IMPERFECT_GRAPHICS )                 // LANDING GEAR           VER 3.0 J   Feb  8 1996  09:46:22
GAME( 1996, sidebs,    0,        taitojc, sidebs, taitojc_state,   taitojc,  ROT0, "Taito", "Side by Side (Ver 2.5 J)",                             GAME_IMPERFECT_GRAPHICS )                 // SIDE BY SIDE           VER 2.5 J   1996/ 6/20   18:13:14
GAMEL(1996, dendego,   0,        dendego, dendego, taitojc_state,  taitojc,  ROT0, "Taito", "Densha de GO! (Ver 2.2 J)",                            GAME_IMPERFECT_GRAPHICS, layout_dendego ) // DENSYA DE GO           VER 2.2 J   1997/ 2/ 4   12:00:28
GAMEL(1996, dendegox,  dendego,  dendego, dendego, taitojc_state,  taitojc,  ROT0, "Taito", "Densha de GO! EX (Ver 2.4 J)",                         GAME_IMPERFECT_GRAPHICS, layout_dendego ) // DENSYA DE GO           VER 2.4 J   1997/ 4/18   13:38:34
GAME( 1997, sidebs2,   0,        taitojc, sidebs, taitojc_state,   taitojc,  ROT0, "Taito", "Side by Side 2 (Ver 2.6 A)",                           GAME_IMPERFECT_GRAPHICS )                 // SIDE BY SIDE2          VER 2.6 A   1997/ 6/19   09:39:22
GAME( 1997, sidebs2j,  sidebs2,  taitojc, sidebs, taitojc_state,   taitojc,  ROT0, "Taito", "Side by Side 2 (Ver 2.4 J)",                           GAME_IMPERFECT_GRAPHICS )                 // SIDE BY SIDE2          VER 2.4 J   1997/ 5/26   13:06:37
GAMEL(1998, dendego2,  0,        dendego, dendego, taitojc_state,  dendego2, ROT0, "Taito", "Densha de GO! 2 Kousoku-hen (Ver 2.5 J)",              GAME_IMPERFECT_GRAPHICS, layout_dendego ) // DENSYA DE GO2          VER 2.5 J   1998/ 3/ 2   15:30:55
GAMEL(1998, dendego23k,dendego2, dendego, dendego, taitojc_state,  dendego2, ROT0, "Taito", "Densha de GO! 2 Kousoku-hen 3000-bandai (Ver 2.20 J)", GAME_IMPERFECT_GRAPHICS, layout_dendego ) // DENSYA DE GO! 2 3000   VER 2.20 J  1998/ 7/15   17:42:38
