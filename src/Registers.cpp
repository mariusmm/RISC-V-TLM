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
    void Registers<std::uint32_t>::dump() const {
        std::cout << "************************************" << std::endl;
        std::cout << "Registers dump" << std::dec << std::endl;
        std::cout << std::setfill('0') << std::uppercase;
        std::cout << "x0 (zero):  0x" << std::right << std::setw(8)
                  << std::hex << register_bank[0];
        std::cout << " x1 (ra):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[1];
        std::cout << " x2 (sp):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[2];
        std::cout << " x3 (gp):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[3] << std::endl;

        std::cout << "x4 (tp):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[4];
        std::cout << " x5 (t0):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[5];
        std::cout << " x6 (t1):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[6];
        std::cout << " x7 (t2):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[7] << std::endl;

        std::cout << "x8 (s0/fp): 0x" << std::right << std::setw(8)
                  << std::hex << register_bank[8];
        std::cout << " x9 (s1):    0x" << std::right << std::setw(8)
                  << std::hex << register_bank[9];
        std::cout << " x10 (a0):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[10];
        std::cout << " x11 (a1):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[11] << std::endl;

        std::cout << "x12 (a2):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[12];
        std::cout << " x13 (a3):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[13];
        std::cout << " x14 (a4):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[14];
        std::cout << " x15 (a5):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[15] << std::endl;

        std::cout << "x16 (a6):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[16];
        std::cout << " x17 (a7):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[17];
        std::cout << " x18 (s2):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[18];
        std::cout << " x19 (s3):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[19] << std::endl;

        std::cout << "x20 (s4):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[20];
        std::cout << " x21 (s5):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[21];
        std::cout << " x22 (s6):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[22];
        std::cout << " x23 (s7):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[23] << std::endl;

        std::cout << "x24 (s8):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[24];
        std::cout << " x25 (s9):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[25];
        std::cout << " x26 (s10):  0x" << std::right << std::setw(8)
                  << std::hex << register_bank[26];
        std::cout << " x27 (s11):  0x" << std::right << std::setw(8)
                  << std::hex << register_bank[27] << std::endl;

        std::cout << "x28 (t3):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[28];
        std::cout << " x29 (t4):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[29];
        std::cout << " x30 (t5):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[30];
        std::cout << " x31 (t6):   0x" << std::right << std::setw(8)
                  << std::hex << register_bank[31] << std::endl;

        std::cout << "PC: 0x" << std::setw(8) << std::hex << register_PC << std::dec << std::endl;
        std::cout << "************************************" << std::endl;
    }

    template<>
    void Registers<std::uint64_t>::initCSR() {
        CSR[CSR_MISA] = (((std::uint64_t) 0x02) << 30) | MISA_M_EXTENSION | MISA_C_EXTENSION
                        | MISA_A_EXTENSION | MISA_I_BASE;
        CSR[CSR_MSTATUS] = MISA_MXL;
    }

    template<>
    void Registers<std::uint64_t>::dump() const {
        std::cout << "************************************" << std::endl;
        std::cout << "Registers dump" << std::dec << std::endl;
        std::cout << std::setfill('0') << std::uppercase;
        std::cout << "x0 (zero):  0x" << std::right << std::setw(16)
                  << std::hex << register_bank[0];
        std::cout << " x1 (ra):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[1];
        std::cout << " x2 (sp):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[2];
        std::cout << " x3 (gp):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[3] << std::endl;

        std::cout << "x4 (tp):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[4];
        std::cout << " x5 (t0):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[5];
        std::cout << " x6 (t1):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[6];
        std::cout << " x7 (t2):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[7] << std::endl;

        std::cout << "x8 (s0/fp): 0x" << std::right << std::setw(16)
                  << std::hex << register_bank[8];
        std::cout << " x9 (s1):    0x" << std::right << std::setw(16)
                  << std::hex << register_bank[9];
        std::cout << " x10 (a0):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[10];
        std::cout << " x11 (a1):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[11] << std::endl;

        std::cout << "x12 (a2):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[12];
        std::cout << " x13 (a3):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[13];
        std::cout << " x14 (a4):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[14];
        std::cout << " x15 (a5):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[15] << std::endl;

        std::cout << "x16 (a6):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[16];
        std::cout << " x17 (a7):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[17];
        std::cout << " x18 (s2):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[18];
        std::cout << " x19 (s3):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[19] << std::endl;

        std::cout << "x20 (s4):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[20];
        std::cout << " x21 (s5):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[21];
        std::cout << " x22 (s6):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[22];
        std::cout << " x23 (s7):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[23] << std::endl;

        std::cout << "x24 (s8):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[24];
        std::cout << " x25 (s9):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[25];
        std::cout << " x26 (s10):  0x" << std::right << std::setw(16)
                  << std::hex << register_bank[26];
        std::cout << " x27 (s11):  0x" << std::right << std::setw(16)
                  << std::hex << register_bank[27] << std::endl;

        std::cout << "x28 (t3):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[28];
        std::cout << " x29 (t4):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[29];
        std::cout << " x30 (t5):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[30];
        std::cout << " x31 (t6):   0x" << std::right << std::setw(16)
                  << std::hex << register_bank[31] << std::endl;

        std::cout << "PC: 0x" << std::setw(16) << std::hex << register_PC << std::dec << std::endl;
        std::cout << "************************************" << std::endl;
    }
}