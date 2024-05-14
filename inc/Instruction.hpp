/*!
 \file Instruction.hpp
 \brief Decode instructions part of the RISC-V
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INSTRUCTION__H
#define INSTRUCTION__H

#include <systemc>

#include "extension_base.hpp"

#include <cstdint>

namespace riscv_tlm {

    typedef enum {
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
    } extension_t;

/**
 * @brief Instruction decoding and fields access
 */
    class Instruction {
    public:

        explicit Instruction(std::uint32_t instr);

        /**
         * @brief returns what instruction extension
         * @return extension
         */
        extension_t check_extension() const;

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
