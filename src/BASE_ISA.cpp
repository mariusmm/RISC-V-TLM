/*!
 \file BASE_ISA.cpp
 \brief RISC-V ISA implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BASE_ISA.h"

namespace riscv_tlm {

    ///// RV32 Specialization
    template<>
    int32_t BASE_ISA<std::uint32_t>::get_imm_I() const {
        std::uint32_t aux = 0;

        aux = this->m_instr.range(31, 20);

        /* sign extension (optimize) */
        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
        }

        return static_cast<std::int32_t>(aux);
    }

    template<>
    std::int32_t BASE_ISA<std::uint32_t>::get_imm_S() const {
        std::uint32_t aux = 0;

        aux = this->m_instr.range(31, 25) << 5;
        aux |= this->m_instr.range(11, 7);

        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
        }

        return static_cast<std::int32_t>(aux);
    }

    template<>
    std::int32_t BASE_ISA<std::uint32_t>::get_imm_B() const {
        std::uint32_t aux = 0;

        aux |= this->m_instr[7] << 11;
        aux |= this->m_instr.range(30, 25) << 5;
        aux |= this->m_instr[31] << 12;
        aux |= this->m_instr.range(11, 8) << 1;

        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
        }

        return static_cast<std::int32_t>(aux);
    }


    template<>
    std::int32_t BASE_ISA<std::uint32_t>::get_imm_J() const {
        std::uint32_t aux = 0;

        aux = this->m_instr[31] << 20;
        aux |= this->m_instr.range(19, 12) << 12;
        aux |= this->m_instr[20] << 11;
        aux |= this->m_instr.range(30, 21) << 1;

        /* bit extension (better way to do that?) */
        if (this->m_instr[31] == 1) {
            aux |= (0b111111111111) << 20;
        }

        return static_cast<std::int32_t>(aux);
    }

    ///// RV64 Specialization
    template<>
    int64_t BASE_ISA<std::uint64_t>::get_imm_I() const {
        std::uint64_t aux = 0;

        aux = this->m_instr.range(31, 20);

        /* sign extension (optimize) */
        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
            aux |= 0xFFFFFFFFULL << 32;
        }

        return static_cast<std::int64_t>(aux);
    }

    template<>
    std::int64_t BASE_ISA<std::uint64_t>::get_imm_S() const {
        std::uint64_t aux = 0;

        aux = this->m_instr.range(31, 25) << 5;
        aux |= this->m_instr.range(11, 7);

        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
            aux |= 0xFFFFFFFFULL << 32;
        }

        return static_cast<std::int64_t>(aux);
    }

    template<>
    std::int64_t BASE_ISA<std::uint64_t>::get_imm_B() const {
        std::uint64_t aux = 0;

        aux |= this->m_instr[7] << 11;
        aux |= this->m_instr.range(30, 25) << 5;
        aux |= this->m_instr[31] << 12;
        aux |= this->m_instr.range(11, 8) << 1;

        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
            aux |= 0xFFFFFFFFULL << 32;
        }

        return static_cast<std::int64_t>(aux);
    }

    template<>
    std::int64_t BASE_ISA<std::uint64_t>::get_imm_J() const {
        std::uint64_t aux = 0;

        aux = this->m_instr[31] << 20;
        aux |= this->m_instr.range(19, 12) << 12;
        aux |= this->m_instr[20] << 11;
        aux |= this->m_instr.range(30, 21) << 1;

        /* bit extension (better way to do that?) */
        if (this->m_instr[31] == 1) {
            aux |= (0b111111111111) << 20;
        }

        return static_cast<std::int64_t>(aux);
    }
}