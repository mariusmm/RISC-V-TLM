/*!
 \file M_extension.h
 \brief Implement M extensions part of the RISC-V
 \author Màrius Montón
 \date November 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef M_EXTENSION__H
#define M_EXTENSION__H

#include "systemc"

#include "extension_base.h"
#include "Log.h"
#include "Registers.h"

typedef enum {
	OP_M_MUL,
	OP_M_MULH,
	OP_M_MULHSU,
	OP_M_MULHU,
	OP_M_DIV,
	OP_M_DIVU,
	OP_M_REM,
	OP_M_REMU,

	OP_M_ERROR
} op_M_Codes;

typedef enum {
	M_MUL = 0b000,
	M_MULH = 0b001,
	M_MULHSU = 0b010,
	M_MULHU = 0b011,
	M_DIV = 0b100,
	M_DIVU = 0b101,
	M_REM = 0b110,
	M_REMU = 0b111,
} M_Codes;

/**
 * @brief Instruction decoding and fields access
 */
class M_extension: public extension_base {
public:

	/**
	 * @brief Constructor, same as base clase
	 */
	using extension_base::extension_base;

	/**
	 * @brief Decodes opcode of instruction
	 * @return opcode of instruction
	 */
	op_M_Codes decode();

	inline void dump() {
		std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
	}

	bool Exec_M_MUL();
	bool Exec_M_MULH();
	bool Exec_M_MULHSU();
	bool Exec_M_MULHU();
	bool Exec_M_DIV();
	bool Exec_M_DIVU();
	bool Exec_M_REM();
	bool Exec_M_REMU();

	bool process_instruction(Instruction &inst);

private:

	/**
	 * @brief Access to opcode field
	 * @return return opcode field
	 */
	inline int32_t opcode() {
		return m_instr.range(14, 12);
	}

	/**
	 * @brief Access to rd field
	 * @return rd field
	 */
	inline int32_t get_rd() {
		return m_instr.range(11, 7);
	}

	inline void set_rd(int32_t value) {
		m_instr.range(11, 7) = value;
	}

	/**
	 * @brief Access to rs1 field
	 * @return rs1 field
	 */
	inline int32_t get_rs1() {
		return m_instr.range(19, 15);
	}

	inline void set_rs1(int32_t value) {
		m_instr.range(19, 15) = value;
	}

	/**
	 * @brief Access to rs2 field
	 * @return rs2 field
	 */
	inline int32_t get_rs2() {
		return m_instr.range(24, 20);
	}

	inline void set_rs2(int32_t value) {
		m_instr.range(24, 20) = value;
	}

	inline int32_t get_funct3() {
		return m_instr.range(14, 12);
	}

	inline void set_funct3(int32_t value) {
		m_instr.range(14, 12) = value;
	}
};

#endif
