/*!
 \file Registers.cpp
 \brief Basic register file implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Registers.h"

namespace riscv_tlm {
    /* Specialization for each XLEN (RV32, RV64)*/
    template<>
    void Registers<std::uint32_t>::initCSR() {
        CSR[CSR_MISA] = MISA_MXL | MISA_M_EXTENSION | MISA_C_EXTENSION
                        | MISA_A_EXTENSION | MISA_I_BASE;
        CSR[CSR_MSTATUS] = MISA_MXL;
    }

    template<>
    void Registers<std::uint64_t>::initCSR() {
        CSR[CSR_MISA] = (((std::uint64_t) 0x02) << 30) | MISA_M_EXTENSION | MISA_C_EXTENSION
                        | MISA_A_EXTENSION | MISA_I_BASE;
        CSR[CSR_MSTATUS] = MISA_MXL;
    }
}