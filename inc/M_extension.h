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
	op_M_Codes decode() const;

	inline virtual void dump() const override {
		std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
	}

	bool Exec_M_MUL() const;
	bool Exec_M_MULH() const;
	bool Exec_M_MULHSU() const;
	bool Exec_M_MULHU() const;
	bool Exec_M_DIV() const;
	bool Exec_M_DIVU() const;
	bool Exec_M_REM() const;
	bool Exec_M_REMU() const;

	bool process_instruction(Instruction *inst);

private:

	/**
	 * @brief Access to opcode field
	 * @return return opcode field
	 */
	inline int32_t opcode() const {
		return m_instr.range(14, 12);
	}

};

#endif
