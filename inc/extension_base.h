/*!
 \file extension_base.h
 \brief Base class for ISA extensions
 \author Màrius Montón
 \date May 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_EXTENSION_BASE_H_
#define INC_EXTENSION_BASE_H_

#include "systemc"

#include "Instruction.h"
#include "Registers.h"
#include "Log.h"
#include "MemoryInterface.h"

#define EXCEPTION_CAUSE_INSTRUCTION_MISALIGN  0
#define EXCEPTION_CAUSE_INSTRUCTION_ACCESS    1
#define EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION   2
#define EXCEPTION_CAUSE_BREAKPOINT            3
#define EXCEPTION_CAUSE_LOAD_ADDR_MISALIGN    4
#define EXCEPTION_CAUSE_LOAD_ACCESS_FAULT     5

class extension_base {

public:
	extension_base(sc_dt::sc_uint<32> instr, Registers *register_bank,
			MemoryInterface *mem_interface);
	virtual ~extension_base() = 0;

	void setInstr(uint32_t p_instr);
	void RaiseException(uint32_t cause, uint32_t inst);
	bool NOP();

	/* pure virtual functions */
	virtual int32_t opcode() = 0;
	virtual int32_t get_rd() = 0;
	virtual void set_rd(int32_t value) = 0;
	virtual int32_t get_rs1() = 0;
	virtual void set_rs1(int32_t value) = 0;
	virtual int32_t get_rs2() = 0;
	virtual void set_rs2(int32_t value) = 0;
	virtual int32_t get_funct3() = 0;
	virtual void set_funct3(int32_t value) = 0;

	void dump();

protected:
	sc_dt::sc_uint<32> m_instr;
	Registers *regs;
	Performance *perf;
	Log *log;
	MemoryInterface *mem_intf;
};

#endif /* INC_EXTENSION_BASE_H_ */
