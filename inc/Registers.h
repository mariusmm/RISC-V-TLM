/*!
 \file Registers.h
 \brief Basic register file
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REGISTERS_H
#define REGISTERS_H

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <iomanip>
#include <unordered_map>

#include "systemc"
#include "tlm.h"

#include "Performance.h"
#include "Memory.h"

namespace riscv_tlm {

#define MISA_A_EXTENSION (1 << 0)
#define MISA_B_EXTENSION (1 << 1)
#define MISA_C_EXTENSION (1 << 2)
#define MISA_I_BASE (1 << 8)
#define MISA_M_EXTENSION (1 << 12)
#define MISA_MXL (1 << 30)

#define CSR_MVENDORID (0xF11)
#define CSR_MARCHID (0xF12)
#define CSR_MIMPID (0xF13)
#define CSR_MHARTID (0xF14)

#define CSR_MSTATUS (0x300)
#define CSR_MISA (0x301)
#define CSR_MEDELEG (0x302)
#define CSR_MIDELEG (0x303)
#define CSR_MIE (0x304)
#define CSR_MTVEC (0x305)
#define CSR_MCOUNTEREN (0x306)
#define CSR_MSTATUSH (0x310)

#define CSR_MSCRATCH (0x340)
#define CSR_MEPC (0x341)
#define CSR_MCAUSE (0x342)
#define CSR_MTVAL (0x343)
#define CSR_MIP (0x344)

#define CSR_SSCRATCH (0x140)
#define CSR_SEPC (0x141)
#define CSR_SCAUSE (0x142)
#define CSR_STVAL (0x143)
#define CSR_SIP (0x144)

#define CSR_MCYCLE (0xB00)
#define CSR_MINSTRET (0xB02)
#define CSR_MCYCLEH (0xB80)
#define CSR_MINSTRETH (0xB82)

#define CSR_CYCLE (0xC00)
#define CSR_TIME (0xC01)
#define CSR_INSTRET (0xC02)

#define CSR_CYCLEH (0xC80)
#define CSR_TIMEH (0xC81)
#define CSR_INSTRETH (0xC02)

#define CSR_STVEC (0x105)

#define MSTATUS_UIE (1 << 0)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_MIE (1 << 3)
#define MSTATUS_UPIE (1 << 4)
#define MSTATUS_SPIE (1 << 5)
#define MSTATUS_MPIE (1 << 7)
#define MSTATUS_SPP (1 << 8)
#define MSTATUS_MPP (1 << 11)
#define MSTATUS_FS  (1 << 13)
#define MSTATUS_XS    (1 << 15)
#define MSTATUS_MPRV (1 << 17)
#define MSTATUS_SUM (1 << 18)
#define MSTATUS_MXR (1 << 19)
#define MSTATUS_TVM (1 << 20)
#define MSTATUS_TW (1 << 21)
#define MSTATUS_TSR (1 << 22)

#define MIP_USIP (1 << 0)
#define MIP_SSIP (1 << 1)
#define MIP_MSIP (1 << 3)
#define MIP_UTIP (1 << 4)
#define MIP_STIP (1 << 5)
#define MIP_MTIP (1 << 7)
#define MIP_UEIP (1 << 8)
#define MIP_SEIP (1 << 9)
#define MIP_MEIP (1 << 11)

#define MIE_USIE (1 << 0)
#define MIE_SSIE (1 << 1)
#define MIE_MSIE (1 << 3)
#define MIE_UTIE (1 << 4)
#define MIE_STIE (1 << 5)
#define MIE_MTIE (1 << 7)
#define MIE_UEIE (1 << 8)
#define MIE_SEIE (1 << 9)
#define MIE_MEIE (1 << 11)

/* 1 ns tick in CYCLE & TIME counters */
#define TICKS_PER_SECOND (1000000)

/**
 * @brief Register file implementation
 */
    template<typename T>
    class Registers {
    public:

        enum {
            x0 = 0,
            x1 = 1,
            x2,
            x3,
            x4,
            x5,
            x6,
            x7,
            x8,
            x9,
            x10,
            x11,
            x12,
            x13,
            x14,
            x15,
            x16,
            x17,
            x18,
            x19,
            x20,
            x21,
            x22,
            x23,
            x24,
            x25,
            x26,
            x27,
            x28,
            x29,
            x30,
            x31,
            zero = x0,
            ra = x1,
            sp = x2,
            gp = x3,
            tp = x4,
            t0 = x5,
            t1 = x6,
            t2 = x7,
            s0 = x8,
            fp = x8,
            s1 = x9,
            a0 = x10,
            a1 = x11,
            a2 = x12,
            a3 = x13,
            a4 = x14,
            a5 = x15,
            a6 = x16,
            a7 = x17,
            s2 = x18,
            s3 = x19,
            s4 = x20,
            s5 = x21,
            s6 = x22,
            s7 = x23,
            s8 = x24,
            s9 = x25,
            s10 = x26,
            s11 = x27,
            t3 = x28,
            t4 = x29,
            t5 = x30,
            t6 = x31
        };

        /**
         * Default constructor
         */
        Registers() {
          perf = Performance::getInstance();

          initCSR();
          register_bank[sp] = Memory::SIZE - 4; // default stack at the end of the memory
          register_PC = 0x80000000;       // default _start address
        };

        /**
         * Set value for a register
         * @param reg_num register number
         * @param value   register value
         */
        void setValue(int reg_num, T value) {
          if ((reg_num != 0) && (reg_num < 32)) {
              register_bank[reg_num] = value;
              perf->registerWrite();
            }
        }

        /**
         * Returns register value
         * @param  reg_num register number
         * @return         register value
         */
        T getValue(int reg_num) const {
          if ((reg_num >= 0) && (reg_num < 32)) {
              perf->registerRead();
              return register_bank[reg_num];
            } else {
              /* TODO Exten sign for any possible T type */
              return static_cast<T>(0xFFFFFFFF);
            }
        }

        /**
         * Returns PC value
         * @return PC value
         */
        T getPC() const {
          return register_PC;
        }

        /**
         * Sets arbitraty value to PC
         * @param new_pc new address to PC
         */
        void setPC(T new_pc) {
          register_PC = new_pc;
        }

        /**
         * Increments PC couunter to next address
         */
        inline void incPC() {
            register_PC += 4;
        }

        inline void incPCby2() {
            register_PC += 2;
        }

        /**
         * @brief Get CSR value
         * @param csr CSR number to access
         * @return CSR value
         */
        T getCSR(int csr) {
          T ret_value;

          switch (csr) {
              case CSR_CYCLE:
              case CSR_MCYCLE:
                ret_value = static_cast<std::uint64_t>(sc_core::sc_time(
                    sc_core::sc_time_stamp()
                    - sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
                            & 0x00000000FFFFFFFF;
              break;
              case CSR_CYCLEH:
              case CSR_MCYCLEH:
                ret_value = static_cast<std::uint32_t>((std::uint64_t) (sc_core::sc_time(
                    sc_core::sc_time_stamp()
                    - sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
                                                           >> 32 & 0x00000000FFFFFFFF);
              break;
              case CSR_TIME:
                ret_value = static_cast<std::uint64_t>(sc_core::sc_time(
                    sc_core::sc_time_stamp()
                    - sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
                            & 0x00000000FFFFFFFF;
              break;
              case CSR_TIMEH:
                ret_value = static_cast<std::uint32_t>((std::uint64_t) (sc_core::sc_time(
                    sc_core::sc_time_stamp()
                    - sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
                                                           >> 32 & 0x00000000FFFFFFFF);
              break;
              [[likely]] default:
            ret_value = CSR[csr];
              break;
            }
          return ret_value;
        }

        /**
         * @brief Set CSR value
         * @param csr   CSR number to access
         * @param value new value to register
         */
        void setCSR(int csr, T value) {
         /* @FIXME: rv32mi-p-ma_fetch tests doesn't allow MISA to be writable,
          * but Volume II: Privileged Architecture v1.10 says MISA is writable (?)
          */
          if (csr != CSR_MISA) {
              CSR[csr] = value;
            }
        }

        /**
         * Dump register data to console
         */
        void dump() {
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

    private:
        /**
         * bank of registers (32 regs of 32bits each)
         */
        std::array<T, 32> register_bank = {{0}};

        /**
         * Program counter (32 bits width)
         */
        T register_PC;

        /**
         * CSR registers (4096 maximum)
         */
        std::unordered_map<T, unsigned int> CSR;

        Performance *perf;

        void initCSR() {
          CSR[CSR_MISA] = MISA_MXL | MISA_M_EXTENSION | MISA_C_EXTENSION
                          | MISA_A_EXTENSION | MISA_I_BASE;
          CSR[CSR_MSTATUS] = MISA_MXL;
        }
    };
}
#endif
