//
// Created by marius on 11/11/22.
//
#if 0
#include "F_extension.h"

namespace riscv_tlm {


    template<>
    std::int32_t F_extension<std::uint32_t>::get_imm_S() const {
        std::uint32_t aux = 0;

        aux = this->m_instr.range(31, 25) << 5;
        aux |= this->m_instr.range(11, 7);

        if (this->m_instr[31] == 1) {
            aux |= (0b11111111111111111111) << 12;
        }

        return static_cast<std::int32_t>(aux);
    }
}
#endif