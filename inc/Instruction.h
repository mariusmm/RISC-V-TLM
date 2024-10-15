/*!
 \file Instruction.h
 \brief Decode instructions part of the RISC-V
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INSTRUCTION__H
#define INSTRUCTION__H

#include "systemc"
#include "extension_base.h"
#include <cstdint>

namespace riscv_tlm {

    using extension_t = enum {
        BASE_EXTENSION,
        M_EXTENSION,
        A_EXTENSION,
        F_EXTENSION,
        D_EXTENSION,
        Q_EXTENSION,
        L_EXTENSION,
        C_EXTENSION,
        R_EXTENSION,
        J_EXTENSION,
        P_EXTENSION,
        V_EXTENSION,
        N_EXTENSION,
        UNKNOWN_EXTENSION
    };

/**
 * @brief Instruction decoding and fields access
 */
    class Instruction {
    public:

        Instruction(std::uint32_t instr);

        /**
         * @brief returns what instruction extension
         * @return extension
         */
        extension_t check_extension() const {
            if ((((m_instr & 0x0000007F) == 0b0110011) || ((m_instr & 0x0000007F) == 0b0111011))
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

        void setInstr(std::uint32_t p_instr) {
            m_instr = p_instr;
        }

        /**
         * @brief return instruction
         * @return all instruction bits (31:0)
         */
        std::uint32_t getInstr() const {
            return m_instr;
        }

        inline void dump() const {
            std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
        }



    private:
        std::uint32_t m_instr;
    };
}

#endif
