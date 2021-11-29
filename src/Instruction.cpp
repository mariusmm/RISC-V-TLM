/*!
 \file Instruction.cpp
 \brief Decode instructions part of the RISC-V
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Instruction.h"

namespace riscv_tlm {

    Instruction::Instruction(std::uint32_t instr) {
        m_instr = instr;
    }

    extension_t Instruction::check_extension() const {
        if (((m_instr & 0x0000007F) == 0b0110011)
            && (((m_instr & 0x7F000000) >> 25) == 0b0000001)) {
            return M_EXTENSION;
        } else if ((m_instr & 0x0000007F) == 0b0101111) {
            return A_EXTENSION;
        } else if ((m_instr & 0x00000003) == 0b00) {
            return C_EXTENSION;
        } else if ((m_instr & 0x00000003) == 0b01) {
            return C_EXTENSION;
        } else if ((m_instr & 0x00000003) == 0b10) {
            return C_EXTENSION;
        } else {
            return BASE_EXTENSION;
        }
    }

}

