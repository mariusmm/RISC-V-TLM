/*!
 \file Registers.h
 \brief Basic register file
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REGISTERS_H
#define REGISTERS_H

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <iomanip>
#include <unordered_map>

#include "systemc"
#include "tlm.h"

#include "Performance.h"
#include "Memory.h"

#define MISA_A_EXTENSION (1 << 0)
#define MISA_B_EXTENSION (1 << 1)
#define MISA_C_EXTENSION (1 << 2)
#define MISA_I_BASE (1 << 8)
#define MISA_M_EXTENSION (1 << 12)
#define MISA_MXL (1 << 30)

#define CSR_MVENDORID (0xF11)
#define CSR_MARCHID (0xF12)
#define CSR_MIMPID (0xF13)
#define CSR_MHARTID (0xF14)

#define CSR_MSTATUS (0x300)
#define CSR_MISA (0x301)
#define CSR_MEDELEG (0x302)
#define CSR_MIDELEG (0x303)
#define CSR_MIE (0x304)
#define CSR_MTVEC (0x305)
#define CSR_MCOUNTEREN (0x306)
#define CSR_MSTATUSH (0x310)

#define CSR_MSCRATCH (0x340)
#define CSR_MEPC (0x341)
#define CSR_MCAUSE (0x342)
#define CSR_MTVAL (0x343)
#define CSR_MIP (0x344)

#define CSR_SSCRATCH (0x140)
#define CSR_SEPC (0x141)
#define CSR_SCAUSE (0x142)
#define CSR_STVAL (0x143)
#define CSR_SIP (0x144)

#define CSR_MCYCLE (0xB00)
#define CSR_MINSTRET (0xB02)
#define CSR_MCYCLEH (0xB80)
#define CSR_MINSTRETH (0xB82)

#define CSR_CYCLE (0xC00)
#define CSR_TIME (0xC01)
#define CSR_INSTRET (0xC02)

#define CSR_CYCLEH (0xC80)
#define CSR_TIMEH (0xC81)
#define CSR_INSTRETH (0xC02)

#define CSR_STVEC (0x105)

#define MSTATUS_UIE (1 << 0)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_MIE (1 << 3)
#define MSTATUS_UPIE (1 << 4)
#define MSTATUS_SPIE (1 << 5)
#define MSTATUS_MPIE (1 << 7)
#define MSTATUS_SPP (1 << 8)
#define MSTATUS_MPP (1 << 11)
#define MSTATUS_FS  (1 << 13)
#define MSTATUS_XS	(1 << 15)
#define MSTATUS_MPRV (1 << 17)
#define MSTATUS_SUM (1 << 18)
#define MSTATUS_MXR (1 << 19)
#define MSTATUS_TVM (1 << 20)
#define MSTATUS_TW (1 << 21)
#define MSTATUS_TSR (1 << 22)

#define MIP_USIP (1 << 0)
#define MIP_SSIP (1 << 1)
#define MIP_MSIP (1 << 3)
#define MIP_UTIP (1 << 4)
#define MIP_STIP (1 << 5)
#define MIP_MTIP (1 << 7)
#define MIP_UEIP (1 << 8)
#define MIP_SEIP (1 << 9)
#define MIP_MEIP (1 << 11)

#define MIE_USIE (1 << 0)
#define MIE_SSIE (1 << 1)
#define MIE_MSIE (1 << 3)
#define MIE_UTIE (1 << 4)
#define MIE_STIE (1 << 5)
#define MIE_MTIE (1 << 7)
#define MIE_UEIE (1 << 8)
#define MIE_SEIE (1 << 9)
#define MIE_MEIE (1 << 11)

/* 1 ns tick in CYCLE & TIME counters */
#define TICKS_PER_SECOND (1000000)

/**
 * @brief Register file implementation
 */
class Registers {
public:

	enum {
		x0 = 0,
		x1 = 1,
		x2,
		x3,
		x4,
		x5,
		x6,
		x7,
		x8,
		x9,
		x10,
		x11,
		x12,
		x13,
		x14,
		x15,
		x16,
		x17,
		x18,
		x19,
		x20,
		x21,
		x22,
		x23,
		x24,
		x25,
		x26,
		x27,
		x28,
		x29,
		x30,
		x31,
		zero = x0,
		ra = x1,
		sp = x2,
		gp = x3,
		tp = x4,
		t0 = x5,
		t1 = x6,
		t2 = x7,
		s0 = x8,
		fp = x8,
		s1 = x9,
		a0 = x10,
		a1 = x11,
		a2 = x12,
		a3 = x13,
		a4 = x14,
		a5 = x15,
		a6 = x16,
		a7 = x17,
		s2 = x18,
		s3 = x19,
		s4 = x20,
		s5 = x21,
		s6 = x22,
		s7 = x23,
		s8 = x24,
		s9 = x25,
		s10 = x26,
		s11 = x27,
		t3 = x28,
		t4 = x29,
		t5 = x30,
		t6 = x31
	};
	/**
	 * Default constructor
	 */
	Registers();

	/**
	 * Set value for a register
	 * @param reg_num register number
	 * @param value   register value
	 */
	void setValue(int reg_num, int32_t value);

	/**
	 * Returns register value
	 * @param  reg_num register number
	 * @return         register value
	 */
	uint32_t getValue(int reg_num) const;

	/**
	 * Returns PC value
	 * @return PC value
	 */
	uint32_t getPC() const;

	/**
	 * Sets arbitraty value to PC
	 * @param new_pc new address to PC
	 */
	void setPC(uint32_t new_pc);

	/**
	 * Increments PC couunter to next address
	 */
	inline void incPC(bool C_ext = false) {
		if (C_ext) {
			register_PC += 2;
		} else {
			register_PC += 4;
		}
	}

    inline void incPCby2() {
        register_PC += 2;
    }

	/**
	 * @brief Get CSR value
	 * @param csr CSR number to access
	 * @return CSR value
	 */
	uint32_t getCSR(int csr);

	/**
	 * @brief Set CSR value
	 * @param csr   CSR number to access
	 * @param value new value to register
	 */
	void setCSR(int csr, uint32_t value);

	/**
	 * Dump register data to console
	 */
	void dump();
private:
	/**
	 * bank of registers (32 regs of 32bits each)
	 */
	std::array<uint32_t, 32> register_bank = { {0} };

	/**
	 * Program counter (32 bits width)
	 */
	uint32_t register_PC;

	/**
	 * CSR registers (4096 maximum)
	 */
	std::unordered_map<unsigned int, uint32_t> CSR{0};

	Performance *perf;

	void initCSR();
};

#endif
