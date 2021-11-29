/*!
 \file A_extension.h
 \brief Implement A extensions part of the RISC-V
 \author Màrius Montón
 \date December 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef A_EXTENSION__H
#define A_EXTENSION__H

#include "systemc"

#include <unordered_set>

#include "Registers.h"
#include "MemoryInterface.h"
#include "extension_base.h"

namespace riscv_tlm {

    typedef enum {
        OP_A_LR,
        OP_A_SC,
        OP_A_AMOSWAP,
        OP_A_AMOADD,
        OP_A_AMOXOR,
        OP_A_AMOAND,
        OP_A_AMOOR,
        OP_A_AMOMIN,
        OP_A_AMOMAX,
        OP_A_AMOMINU,
        OP_A_AMOMAXU,

        OP_A_ERROR
    } op_A_Codes;

    typedef enum {
        A_LR = 0b00010,
        A_SC = 0b00011,
        A_AMOSWAP = 0b00001,
        A_AMOADD = 0b00000,
        A_AMOXOR = 0b00100,
        A_AMOAND = 0b01100,
        A_AMOOR = 0b01000,
        A_AMOMIN = 0b10000,
        A_AMOMAX = 0b10100,
        A_AMOMINU = 0b11000,
        A_AMOMAXU = 0b11100,
    } A_Codes;

/**
 * @brief Instruction decoding and fields access
 */
    class A_extension : public extension_base {
    public:

        /**
         * @brief Constructor, same as base class
         */
        using extension_base::extension_base;

        /**
         * @brief Access to opcode field
         * @return return opcode field
         */
        inline int32_t opcode() const override {
            return static_cast<int32_t>(m_instr.range(31, 27));
        }

        /**
         * @brief Decodes opcode of instruction
         * @return opcode of instruction
         */
        op_A_Codes decode() const;

        inline void dump() const override {
            std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
        }

        bool Exec_A_LR();

        bool Exec_A_SC();

        bool Exec_A_AMOSWAP() const;

        bool Exec_A_AMOADD() const;

        bool Exec_A_AMOXOR() const;

        bool Exec_A_AMOAND() const;

        bool Exec_A_AMOOR() const;

        bool Exec_A_AMOMIN() const;

        bool Exec_A_AMOMAX() const;

        bool Exec_A_AMOMINU() const;

        bool Exec_A_AMOMAXU() const;

        bool process_instruction(Instruction &inst);

        void TLB_reserve(std::uint32_t address);

        bool TLB_reserved(std::uint32_t address);

    private:
        std::unordered_set<std::uint32_t> TLB_A_Entries;
    };
}

#endif
