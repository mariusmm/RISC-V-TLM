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

}

