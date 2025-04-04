/***************************************************************************

    dsp16.h

    WE|AT&T DSP16 series emulator.

***************************************************************************/

#include "emu.h"
#include "debugger.h"
#include "dsp16.h"

//
// TODO:
//  * Store the cache in 15 unique memory locations as it is on-chip.
//  * Modify cycle counts when running from within the cache
//

//**************************************************************************
//  DEVICE INTERFACE
//**************************************************************************

// device type definition
const device_type DSP16 = &device_creator<dsp16_device>;


//-------------------------------------------------
//  dsp16_device - constructor
//-------------------------------------------------

dsp16_device::dsp16_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: cpu_device(mconfig, DSP16, "DSP16", tag, owner, clock),
		m_program_config("program", ENDIANNESS_LITTLE, 16, 16, -1),
		m_data_config("data", ENDIANNESS_LITTLE, 16, 16, -1),
		m_i(0),
		m_pc(0),
		m_pt(0),
		m_pr(0),
		m_pi(0),
		m_j(0),
		m_k(0),
		m_rb(0),
		m_re(0),
		m_r0(0),
		m_r1(0),
		m_r2(0),
		m_r3(0),
		m_x(0),
		m_y(0),
		m_p(0),
		m_a0(0),
		m_a1(0),
		m_auc(0),
		m_psw(0),
		m_c0(0),
		m_c1(0),
		m_c2(0),
		m_sioc(0),
		m_pioc(0),
		m_ppc(0),
		m_cacheStart(CACHE_INVALID),
		m_cacheEnd(CACHE_INVALID),
		m_cacheRedoNextPC(CACHE_INVALID),
		m_cacheIterations(0),
		m_program(NULL),
		m_data(NULL),
		m_direct(NULL),
		m_icount(0)
{
	// Allocate & setup
}



//-------------------------------------------------
//  device_start - start up the device
//-------------------------------------------------

void dsp16_device::device_start()
{
	// register state with the debugger
	state_add(STATE_GENPC,    "GENPC",     m_pc).noshow();
	//state_add(STATE_GENPCBASE, "GENPCBASE", m_ppc).noshow();
	state_add(STATE_GENFLAGS, "GENFLAGS",  m_psw).callimport().callexport().formatstr("%10s").noshow();
	state_add(DSP16_PC,       "PC",        m_pc);
	state_add(DSP16_I,        "I",         m_i);
	state_add(DSP16_PT,       "PT",        m_pt);
	state_add(DSP16_PR,       "PR",        m_pr);
	state_add(DSP16_PI,       "PI",        m_pi);
	state_add(DSP16_J,        "J",         m_j);
	state_add(DSP16_K,        "K",         m_k);
	state_add(DSP16_RB,       "RB",        m_rb);
	state_add(DSP16_RE,       "RE",        m_re);
	state_add(DSP16_R0,       "R0",        m_r0);
	state_add(DSP16_R1,       "R1",        m_r1);
	state_add(DSP16_R2,       "R2",        m_r2);
	state_add(DSP16_R3,       "R3",        m_r3);
	state_add(DSP16_X,        "X",         m_x);
	state_add(DSP16_Y,        "Y",         m_y);
	state_add(DSP16_P,        "P",         m_p);
	state_add(DSP16_A0,       "A0",        m_a0).mask(U64(0xfffffffff));
	state_add(DSP16_A1,       "A1",        m_a1).mask(U64(0xfffffffff));
	state_add(DSP16_AUC,      "AUC",       m_auc); //.formatstr("%6s");
	state_add(DSP16_PSW,      "PSW",       m_psw); //.formatstr("%16s");
	state_add(DSP16_C0,       "C0",        m_c0);
	state_add(DSP16_C1,       "C1",        m_c1);
	state_add(DSP16_C2,       "C2",        m_c2);
	state_add(DSP16_SIOC,     "SIOC",      m_sioc).formatstr("%16s");
	state_add(DSP16_PIOC,     "PIOC",      m_pioc); //.formatstr("%16s");

	// register our state for saving
	save_item(NAME(m_i));
	save_item(NAME(m_pc));
	save_item(NAME(m_pt));
	save_item(NAME(m_pr));
	save_item(NAME(m_pi));
	save_item(NAME(m_j));
	save_item(NAME(m_k));
	save_item(NAME(m_rb));
	save_item(NAME(m_re));
	save_item(NAME(m_r0));
	save_item(NAME(m_r1));
	save_item(NAME(m_r2));
	save_item(NAME(m_r3));
	save_item(NAME(m_x));
	save_item(NAME(m_y));
	save_item(NAME(m_p));
	save_item(NAME(m_a0));
	save_item(NAME(m_a1));
	save_item(NAME(m_auc));
	save_item(NAME(m_psw));
	save_item(NAME(m_c0));
	save_item(NAME(m_c1));
	save_item(NAME(m_c2));
	save_item(NAME(m_sioc));
	save_item(NAME(m_pioc));
	save_item(NAME(m_ppc));
	save_item(NAME(m_cacheStart));
	save_item(NAME(m_cacheEnd));
	save_item(NAME(m_cacheRedoNextPC));
	save_item(NAME(m_cacheIterations));

	// get our address spaces
	m_program = &space(AS_PROGRAM);
	m_data = &space(AS_DATA);
	m_direct = &m_program->direct();

	// set our instruction counter
	m_icountptr = &m_icount;
}


//-------------------------------------------------
//  device_reset - reset the device
//-------------------------------------------------

void dsp16_device::device_reset()
{
	// Page 7-5
	m_pc = 0x0000;
	m_sioc = 0x0000;
	m_pioc = 0x0008;
	m_rb = 0x0000;
	m_re = 0x0000;
	// AUC is not affected by reset
	m_ppc = m_pc;

	// Hacky cache emulation.
	m_cacheStart = CACHE_INVALID;
	m_cacheEnd = CACHE_INVALID;
	m_cacheRedoNextPC = CACHE_INVALID;
	m_cacheIterations = 0;
}


//-------------------------------------------------
//  memory_space_config - return the configuration
//  of the specified address space, or NULL if
//  the space doesn't exist
//-------------------------------------------------

const address_space_config *dsp16_device::memory_space_config(address_spacenum spacenum) const
{
	return (spacenum == AS_PROGRAM) ? &m_program_config :
			(spacenum == AS_DATA) ? &m_data_config :
			NULL;
}


//-------------------------------------------------
//  state_string_export - export state as a string
//  for the debugger
//-------------------------------------------------

void dsp16_device::state_string_export(const device_state_entry &entry, astring &string)
{
	switch (entry.index())
	{
		case STATE_GENFLAGS:
		string.printf("(below)");
			break;

		// Placeholder for a better view later (TODO)
		case DSP16_SIOC:
			string.printf("%04x", *(UINT16*)entry.dataptr());
			break;
	}
}


//-------------------------------------------------
//  disasm_min_opcode_bytes - return the length
//  of the shortest instruction, in bytes
//-------------------------------------------------

UINT32 dsp16_device::disasm_min_opcode_bytes() const
{
	return 2;
}


//-------------------------------------------------
//  disasm_max_opcode_bytes - return the length
//  of the longest instruction, in bytes
//-------------------------------------------------

UINT32 dsp16_device::disasm_max_opcode_bytes() const
{
	return 4;
}


//-------------------------------------------------
//  disasm_disassemble - call the disassembly
//  helper function
//-------------------------------------------------

offs_t dsp16_device::disasm_disassemble(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, UINT32 options)
{
	extern CPU_DISASSEMBLE( dsp16a );
	return CPU_DISASSEMBLE_NAME(dsp16a)(NULL, buffer, pc, oprom, opram, 0);
}



/***************************************************************************
    MEMORY ACCESSORS
***************************************************************************/

inline UINT32 dsp16_device::data_read(const UINT16& addr)
{
	return m_data->read_word(addr << 1);
}

inline void dsp16_device::data_write(const UINT16& addr, const UINT16& data)
{
	m_data->write_word(addr << 1, data & 0xffff);
}

inline UINT32 dsp16_device::opcode_read(const UINT8 pcOffset)
{
	const UINT16 readPC = m_pc + pcOffset;
	return m_direct->read_decrypted_dword(readPC << 1);
}


/***************************************************************************
    CORE EXECUTION LOOP
***************************************************************************/

//-------------------------------------------------
//  execute_min_cycles - return minimum number of
//  cycles it takes for one instruction to execute
//-------------------------------------------------

UINT32 dsp16_device::execute_min_cycles() const
{
	return 1;
}


//-------------------------------------------------
//  execute_max_cycles - return maximum number of
//  cycles it takes for one instruction to execute
//-------------------------------------------------

UINT32 dsp16_device::execute_max_cycles() const
{
	return 1;
}


//-------------------------------------------------
//  execute_input_lines - return the number of
//  input/interrupt lines
//-------------------------------------------------

UINT32 dsp16_device::execute_input_lines() const
{
	return 1;
}


void dsp16_device::execute_set_input(int inputnum, int state)
{
	// Only has one external IRQ line
}


void dsp16_device::execute_run()
{
	do
	{
		// debugging
		m_ppc = m_pc;   // copy PC to previous PC
		debugger_instruction_hook(this, m_pc);

		// instruction fetch & execute
		UINT8 cycles;
		UINT8 pcAdvance;
		const UINT16 op = opcode_read();
		execute_one(op, cycles, pcAdvance);

		// step
		m_pc += pcAdvance;
		m_icount -= cycles;

	} while (m_icount > 0);
}

#include "dsp16ops.c"
