/*!
 \file Instruction.cpp
 \brief Decode instructions part of the RISC-V
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Instruction.h"

Instruction::Instruction(sc_dt::sc_uint<32> instr) {
	m_instr = instr;
}

extension_t Instruction::check_extension() {
	if ((m_instr.range(6, 0) == 0b0110011)
			&& (m_instr.range(31, 25) == 0b0000001)) {
		return M_EXTENSION;
	} else if (m_instr.range(6, 0) == 0b0101111) {
		return A_EXTENSION;
	} else if (m_instr.range(1, 0) == 0b11) {
		return BASE_EXTENSION;
	} else if (m_instr.range(1, 0) == 0b00) {
		return C_EXTENSION;
	} else if (m_instr.range(1, 0) == 0b01) {
		return C_EXTENSION;
	} else if (m_instr.range(1, 0) == 0b10) {
		return C_EXTENSION;
	} else {
		return UNKNOWN_EXTENSION;
	}
}

