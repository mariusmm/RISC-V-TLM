/*!
 \file BASE_ISA.h
 \brief RISC-V ISA implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef Execute_H
#define Execute_H

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <type_traits>
#include <limits>

#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "MemoryInterface.h"
#include "Instruction.h"
#include "C_extension.h"
#include "M_extension.h"
#include "A_extension.h"
#include "Registers.h"

namespace riscv_tlm {

    typedef enum {
        OP_LUI,
        OP_AUIPC,
        OP_JAL,
        OP_JALR,

        OP_BEQ,
        OP_BNE,
        OP_BLT,
        OP_BGE,
        OP_BLTU,
        OP_BGEU,

        OP_LB,
        OP_LH,
        OP_LW,
        OP_LBU,
        OP_LHU,

        OP_SB,
        OP_SH,
        OP_SW,

        OP_ADDI,
        OP_SLTI,
        OP_SLTIU,
        OP_XORI,
        OP_ORI,
        OP_ANDI,
        OP_SLLI,
        OP_SRLI,
        OP_SRAI,

        OP_ADD,
        OP_SUB,
        OP_SLL,
        OP_SLT,
        OP_SLTU,
        OP_XOR,
        OP_SRL,
        OP_SRA,
        OP_OR,
        OP_AND,

        OP_FENCE,
        OP_ECALL,
        OP_EBREAK,

        OP_CSRRW,
        OP_CSRRS,
        OP_CSRRC,
        OP_CSRRWI,
        OP_CSRRSI,
        OP_CSRRCI,

        OP_URET,
        OP_SRET,
        OP_MRET,
        OP_WFI,
        OP_SFENCE,

        OP_ERROR
    } opCodes;

    enum Codes {
        LUI = 0b0110111,
        AUIPC = 0b0010111,
        JAL = 0b1101111,
        JALR = 0b1100111,

        BEQ = 0b1100011,
        BEQ_F = 0b000,
        BNE_F = 0b001,
        BLT_F = 0b100,
        BGE_F = 0b101,
        BLTU_F = 0b110,
        BGEU_F = 0b111,

        LB = 0b0000011,
        LB_F = 0b000,
        LH_F = 0b001,
        LW_F = 0b010,
        LBU_F = 0b100,
        LHU_F = 0b101,

        SB = 0b0100011,
        SB_F = 0b000,
        SH_F = 0b001,
        SW_F = 0b010,

        ADDI = 0b0010011,
        ADDI_F = 0b000,
        SLTI_F = 0b010,
        SLTIU_F = 0b011,
        XORI_F = 0b100,
        ORI_F = 0b110,
        ANDI_F = 0b111,
        SLLI_F = 0b001,
        SRLI_F = 0b101,
        SRLI_F7 = 0b0000000,
        SRAI_F7 = 0b0100000,

        ADD = 0b0110011,
        ADD_F = 0b000,
        SUB_F = 0b000,
        ADD_F7 = 0b0000000,
        SUB_F7 = 0b0100000,

        SLL_F = 0b001,
        SLT_F = 0b010,
        SLTU_F = 0b011,
        XOR_F = 0b100,
        SRL_F = 0b101,
        SRA_F = 0b101,
        SRL_F7 = 0b0000000,
        SRA_F7 = 0b0100000,
        OR_F = 0b110,
        AND_F = 0b111,

        FENCE = 0b0001111,
        ECALL = 0b1110011,
        ECALL_F = 0b000000000000,
        EBREAK_F = 0b000000000001,
        URET_F = 0b000000000010,
        SRET_F = 0b000100000010,
        MRET_F = 0b001100000010,
        WFI_F = 0b000100000101,
        SFENCE_F = 0b0001001,

        ECALL_F3 = 0b000,
        CSRRW = 0b001,
        CSRRS = 0b010,
        CSRRC = 0b011,
        CSRRWI = 0b101,
        CSRRSI = 0b110,
        CSRRCI = 0b111,
    };

/**
 * @brief Risc_V execute module
 */
    template<typename T>
    class BASE_ISA : public extension_base<T> {
    public:

        /**
         * @brief Constructor, same as base class
         */
        using extension_base<T>::extension_base;

        /**
         * @brief Deduce signed type for T type
         */
        using signed_T = typename std::make_signed<T>::type;

        /**
         * @brief Deduce unsigned type for T type
         */
        using unsigned_T = typename std::make_unsigned<T>::type;

        /**
         * @brief Access to funct7 field
         * @return funct7 field
         */
        inline std::int32_t get_funct7() const {
            return this->m_instr.range(31, 25);
        }

        /**
         * @brief Gets immediate field value for I-type
         * @return immediate_I field
         * @note Specialized
         */
        signed_T get_imm_I() const;

        /**
         * @brief Gets immediate field value for S-type
         * @return immediate_S field
         * @note Specialized
         */
         signed_T get_imm_S() const;

        /**
         * @brief Gets immediate field value for U-type
         * @return immediate_U field
         */
        inline unsigned_T get_imm_U() const {
            return this->m_instr.range(31, 12);
        }

        /**
         * @brief Gets immediate field value for B-type
         * @return immediate_B field
         * @note Specialized
         */
        signed_T get_imm_B() const;

        /**
         * @brief Gets immediate field value for J-type
         * @return immediate_J field
         * @note Specialized
         */
        signed_T get_imm_J() const;

        /**
         * @brief Returns shamt field for Shifts instructions
         * @return value corresponding to inst(25:20)
         */
        inline unsigned_T get_shamt() const {
            return this->m_instr.range(25, 20);
        }

        /**
         * @brief Returns CSR field for CSR instructions
         * @return value corresponding to instr(31:20)
         */
        inline unsigned_T get_csr() const {
            return this->m_instr.range(31, 20);
        }

        /**
         * @brief Access to opcode field
         * @return return opcode field
         */
        inline unsigned_T opcode() const override {
            return this->m_instr.range(6, 0);
        }

        bool Exec_LUI() const {
            unsigned int rd;
            unsigned_T imm;

            rd = this->get_rd();
            imm = get_imm_U() << 12;
            this->regs->setValue(rd, imm);

            this->logger->debug("{} ns. PC: 0x{:x}. LUI: x{:d} <- 0x{:x}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rd, imm);

            return true;
        }

        bool Exec_AUIPC() const {
            unsigned int rd;
            unsigned_T imm;
            unsigned_T new_pc;

            rd = this->get_rd();
            imm = get_imm_U() << 12;
            new_pc = this->regs->getPC() + imm;

            this->regs->setValue(rd, new_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. AUIPC: x{:d} <- 0x{:x} + PC (0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rd, imm, new_pc);

            return true;
        }

        bool Exec_JAL() const {
            std::int32_t mem_addr;
            unsigned int rd;
            unsigned_T new_pc, old_pc;

            rd = this->get_rd();
            mem_addr = get_imm_J();
            old_pc = this->regs->getPC();
            new_pc = old_pc + mem_addr;

            this->regs->setPC(new_pc);

            old_pc = old_pc + 4;
            this->regs->setValue(rd, old_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. JAL: x{:d} <- 0x{:x}. PC + 0x{:x} -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rd, old_pc, mem_addr, new_pc);

            return true;
        }

        bool Exec_JALR() {
            signed_T mem_addr;
            unsigned int rd, rs1;
            unsigned_T new_pc, old_pc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            mem_addr = get_imm_I();

            old_pc = this->regs->getPC();
            this->regs->setValue(rd, old_pc + 4);

            new_pc = static_cast<unsigned_T>((this->regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE);

            if ((new_pc & 0x00000003) != 0) {
                // not aligned
                this->logger->debug("{} ns. PC: 0x{:x}. JALR: x{:d} <- 0x{:x} PC <- 0x{:x}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rd, old_pc + 4, new_pc);

                this->logger->debug("{} ns. PC: 0x{:x}. JALR : Exception");
                this->RaiseException(EXCEPTION_CAUSE_LOAD_ADDR_MISALIGN, this->m_instr);
            } else {
                this->regs->setPC(new_pc);
            }

            this->logger->debug("{} ns. PC: 0x{:x}. JALR: x{:d} <- 0x{:x}. PC <- 0x{:x}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rd, old_pc + 4, new_pc);

            return true;
        }

        bool Exec_BEQ() const {
            unsigned int rs1, rs2;
            unsigned_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = this->regs->getValue(rs1);
            val2 = this->regs->getValue(rs2);

            if (val1 == val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BEQ: x{:d}(0x{:x}) == x{:d}(0x{:x})? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), rs2, this->regs->getValue(rs2), this->regs->getPC());

            return true;
        }

        bool Exec_BNE() const {
            unsigned int rs1, rs2;
            signed_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = static_cast<signed_T>(this->regs->getValue(rs1));
            val2 = static_cast<signed_T>(this->regs->getValue(rs2));

            if (val1 != val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BNE: x{:d}(0x{:x}) != x{:d}(0x{:x})? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, val1, rs2, val2, this->regs->getPC());

            return true;
        }

        bool Exec_BLT() const {
            unsigned int rs1, rs2;
            signed_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = static_cast<signed_T>(this->regs->getValue(rs1));
            val2 = static_cast<signed_T>(this->regs->getValue(rs2));

            if (val1 < val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BLT: x{:d}(0x{:x}) < x{:d}(0x{:x})? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), rs2, this->regs->getValue(rs2), this->regs->getPC());

            return true;
        }

        bool Exec_BGE() const {
            unsigned int rs1, rs2;
            signed_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = static_cast<signed_T>(this->regs->getValue(rs1));
            val2 = static_cast<signed_T>(this->regs->getValue(rs2));

            if (val1 >= val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BGE: x{:d}(0x{:x}) > x{:d}(0x{:x})? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), rs2, this->regs->getValue(rs2), this->regs->getPC());

            return true;
        }

        bool Exec_BLTU() const {
            unsigned int rs1, rs2;
            unsigned_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = this->regs->getValue(rs1);
            val2 = this->regs->getValue(rs2);

            if (val1 < val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BLTU: x{:d}(0x{:x}) < x{:d}(0x{:x})? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), rs2, this->regs->getValue(rs2), this->regs->getPC());

            return true;
        }

        bool Exec_BGEU() const {
            unsigned int rs1, rs2;
            unsigned_T val1, val2;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = this->regs->getValue(rs1);
            val2 = this->regs->getValue(rs2);

            if (val1 >= val2) {
                unsigned_T new_pc;
                new_pc = static_cast<unsigned_T>(this->regs->getPC() + get_imm_B());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPC();
            }

            this->logger->debug("{} ns. PC: 0x{:x}. BGEU: x{:d}(0x{:x}) > x{:d}(0x{:x}) -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), rs2, this->regs->getValue(rs2), this->regs->getPC());

            return true;
        }

        bool Exec_LB() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::int32_t imm;
            std::int8_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            mem_addr = imm + this->regs->getValue(rs1);
            data = static_cast<std::int8_t>(this->mem_intf->readDataMem(mem_addr, 1));
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. LB: x{:d} + x{:d}(0x{:x}) -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd);

            return true;
        }

        bool Exec_LH() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::int32_t imm;
            int16_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            mem_addr = imm + this->regs->getValue(rs1);
            data = static_cast<int16_t>(this->mem_intf->readDataMem(mem_addr, 2));
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. LH: x{:d} + x{:d}(0x{:x}) -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd);

            return true;
        }

        bool Exec_LW() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::int32_t imm;
            std::uint32_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, static_cast<std::int32_t>(data));

            this->logger->debug("{} ns. PC: 0x{:x}. LW: x{:d} + x{:d}(0x{:x}) -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd);

            return true;
        }

        bool Exec_LBU() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::int32_t imm;
            std::uint8_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 1);
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, static_cast<std::int32_t>(data));

            this->logger->debug("{} ns. PC: 0x{:x}. LBU: x{:d} + x{:d}(0x{:x}) -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd);

            return true;
        }

        bool Exec_LHU() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::int32_t imm;
            uint16_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 2);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. LHU: x{:d} + x{:d}(0x{:x}) -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd);

            return true;
        }

        bool Exec_SB() const {
            unsigned_T mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint32_t data;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();
            imm = get_imm_S();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 1);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. SB: x{:d} -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs2, rs1, imm, mem_addr);

            return true;
        }

        bool Exec_SH() const {
            unsigned_T mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint32_t data;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();
            imm = get_imm_S();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 2);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. SH: x{:d} -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs2, rs1, imm, mem_addr);

            return true;
        }

        bool Exec_SW() const {
            unsigned_T mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint32_t data;

            rs1 = this->get_rs1();
            rs2 = this->get_rs2();
            imm = get_imm_S();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. SW: x{:d} -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs2, rs1, imm, mem_addr);

            return true;
        }

        bool Exec_ADDI() const {
            unsigned int rd, rs1;
            std::int32_t imm;
            signed_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            calc = static_cast<unsigned_T>(this->regs->getValue(rs1)) + imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. ADDI: x{:d} + x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, rd, calc);

            return true;
        }

        bool Exec_SLTI() const {
            unsigned int rd, rs1;
            std::int32_t imm;
            signed_T val1;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            val1 = static_cast<signed_T>(this->regs->getValue(rs1));

            if (val1 < imm) {
                this->regs->setValue(rd, 1);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTI: x{:d} < x{:d} => 1 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, imm, rd);
            } else {
                this->regs->setValue(rd, 0);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTI: x{:d} < x{:d} => 0 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, imm, rd);
            }

            return true;
        }

        bool Exec_SLTIU() const {
            unsigned int rd, rs1;
            unsigned_T imm;
            unsigned_T val1;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = static_cast<unsigned_T>(get_imm_I());
            val1 = static_cast<unsigned_T>(this->regs->getValue(rs1));

            if (val1 < imm) {
                this->regs->setValue(rd, 1);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTIU: x{:d} < x{:d} => 1 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, imm, rd);
            } else {
                this->regs->setValue(rd, 0);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTIU: x{:d} < x{:d} => 0 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, imm, rd);
            }

            return true;
        }

        bool Exec_XORI() const {
            unsigned int rd, rs1;
            unsigned_T imm;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            calc = this->regs->getValue(rs1) ^ imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. XORI: x{:d} XOR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, rd);

            return true;
        }

        bool Exec_ORI() const {
            unsigned int rd, rs1;
            unsigned_T imm;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            calc = this->regs->getValue(rs1) | imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. ORI: x{:d} OR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, rd);

            return true;
        }

        bool Exec_ANDI() const {
            unsigned int rd, rs1;
            unsigned_T imm;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            imm = get_imm_I();

            calc = this->regs->getValue(rs1) & imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. ANDI: x{:d} AND 0x{:x} -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, rd);

            return true;
        }

        bool Exec_SLLI() {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = get_shamt();

            if (rs2 >= 0x20) {
                std::cout << "ILEGAL INSTRUCTION, shamt[5] != 0" << "\n";
                this->RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, this->m_instr);

                return false;
            }

            shift = rs2 & 0x1F;

            calc = static_cast<unsigned_T>(this->regs->getValue(rs1)) << shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SLLI: x{:d} << {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_SRLI() const {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            shift = rs2 & 0x1F;

            calc = static_cast<unsigned_T>(this->regs->getValue(rs1)) >> shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SRLI: x{:d} >> {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_SRAI() const {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            signed_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            shift = rs2 & 0x1F;

            calc = static_cast<signed_T>(this->regs->getValue(rs1)) >> shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SRAI: x{:d} >> {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_ADD() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;
            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            calc = this->regs->getValue(rs1) + this->regs->getValue(rs2);

            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. ADD: x{:d} + x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_SUB() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;
            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            calc = this->regs->getValue(rs1) - this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SUB: x{:d} - x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_SLL() const {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            shift = this->regs->getValue(rs2) & 0x1F;

            calc = this->regs->getValue(rs1) << shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SLL: x{:d} << x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_SLT() const {
            unsigned int rd, rs1, rs2;
            signed_T val1, val2;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = static_cast<signed_T>(this->regs->getValue(rs1));
            val2 = static_cast<signed_T>(this->regs->getValue(rs2));

            if (val1 < val2) {
                this->regs->setValue(rd, 1);
                this->logger->debug("{} ns. PC: 0x{:x}. SLT: x{:d} < x{:d} => 1 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, rs2, rd);
            } else {
                this->regs->setValue(rd, 0);
                this->logger->debug("{} ns. PC: 0x{:x}. SLT: x{:d} < x{:d} => 0 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, rs2, rd);
            }

            return true;
        }

        bool Exec_SLTU() const {
            unsigned int rd, rs1, rs2;
            unsigned_T val1, val2;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            val1 = this->regs->getValue(rs1);
            val2 = this->regs->getValue(rs2);

            if (val1 < val2) {
                this->regs->setValue(rd, 1);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTU: x{:d} < x{:d} => 1 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, rs2, rd);
            } else {
                this->regs->setValue(rd, 0);
                this->logger->debug("{} ns. PC: 0x{:x}. SLTU: x{:d} < x{:d} => 0 -> x{:d}",
                                    sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rs1, rs2, rd);
            }

            return true;
        }

        bool Exec_XOR() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            calc = this->regs->getValue(rs1) ^ this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. XOR: x{:d} XOR x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_SRL() const {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            shift = this->regs->getValue(rs2) & 0x1F;
            calc = this->regs->getValue(rs1) >> shift;

            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SRL: x{:d} >> {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_SRA() const {
            unsigned int rd, rs1, rs2;
            unsigned_T shift;
            signed_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            shift = this->regs->getValue(rs2) & 0x1F;
            calc = static_cast<signed_T>(this->regs->getValue(rs1)) >> shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. SRA: x{:d} >> {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_OR() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            calc = this->regs->getValue(rs1) | this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. OR: x{:d} OR x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_AND() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            calc = this->regs->getValue(rs1) & this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. AND: x{:d} AND x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_FENCE() const {
            this->logger->debug("{} ns. PC: 0x{:x}. FENCE");

            return true;
        }

        bool Exec_ECALL() {

            this->logger->debug("{} ns. PC: 0x{:x}. ECALL");

            std::cout << std::endl << "ECALL Instruction called, stopping simulation"
                      << std::endl;
            this->regs->dump();
            std::cout << "Simulation time " << sc_core::sc_time_stamp() << "\n";
            this->perf->dump();

#if 0
            std::uint32_t gp_value = this->regs->getValue(Registers::gp);
            if (gp_value == 1) {
                std::cout << "GP value is 1, test result is OK" << "\n";
            } else {
                std::cout << "GP value is " << gp_value << "\n";
            }

            sc_core::sc_stop();
#else
            this->RaiseException(11, this->m_instr);
#endif
            return true;
        }

        bool Exec_EBREAK() {

            this->logger->debug("{} ns. PC: 0x{:x}. EBREAK");
            std::cout << std::endl << "EBRAK  Instruction called, dumping information"
                      << std::endl;
            this->regs->dump();
            std::cout << "Simulation time " << sc_core::sc_time_stamp() << "\n";
            this->perf->dump();

            this->RaiseException(11, this->m_instr);

            return false;
        }

        bool Exec_CSRRW() const {
            unsigned int rd, rs1;
            int csr;
            std::uint32_t aux;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            /* These operations must be atomical */
            if (rd != 0) {
                aux = this->regs->getCSR(csr);
                this->regs->setValue(rd, static_cast<std::int32_t>(aux));
            }

            aux = this->regs->getValue(rs1);
            this->regs->setCSR(csr, aux);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRW: CSR #{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                csr, rd, aux);

            return true;
        }

        bool Exec_CSRRS() const {
            unsigned int rd, rs1;
            int csr;
            std::uint32_t bitmask, aux, aux2;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            if (rd == 0) {
                this->logger->debug("{} ns. PC: 0x{:x}. CSRRS with rd1 == 0, doing nothing.");
                return false;
            }

            /* These operations must be atomical */
            aux = this->regs->getCSR(csr);
            bitmask = this->regs->getValue(rs1);

            this->regs->setValue(rd, static_cast<std::int32_t>(aux));

            aux2 = aux | bitmask;
            this->regs->setCSR(csr, aux2);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRS: CSR #{:d}(0x{:x}) -> x{:d}(0x{:x}) & CSR #{:d} <- 0x{:x}",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                csr, aux, rd, rs1, csr, aux2);

            return true;
        }

        bool Exec_CSRRC() const {
            unsigned int rd, rs1;
            int csr;
            std::uint32_t bitmask, aux, aux2;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            if (rd == 0) {
                this->logger->debug("{} ns. PC: 0x{:x}. CSRRC with rd1 == 0, doing nothing.");
                return true;
            }

            /* These operations must be atomical */
            aux = this->regs->getCSR(csr);
            bitmask = this->regs->getValue(rs1);

            this->regs->setValue(rd, static_cast<std::int32_t>(aux));

            aux2 = aux & ~bitmask;
            this->regs->setCSR(csr, aux2);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRC: CSR #{:d}(0x{:x}) -> x{:d}(0x{:x}) & CSR #{:d} <- 0x{:x}",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                csr, aux, rd, rs1, csr, aux2);

            return true;
        }

        bool Exec_CSRRWI() const {
            unsigned int rd, rs1;
            int csr;
            std::uint32_t aux;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            /* These operations must be atomical */
            if (rd != 0) {
                aux = this->regs->getCSR(csr);
                this->regs->setValue(rd, static_cast<std::int32_t>(aux));
            }
            aux = rs1;
            this->regs->setCSR(csr, aux);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRWI: CSR #{:d} -> x{:d}. x{:d} -> CSR #{:d}",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                csr, rd, rs1, csr);

            return true;
        }

        bool Exec_CSRRSI() const {
            unsigned int rd, rs1;
            int csr;
            std::uint32_t bitmask, aux;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            if (rs1 == 0) {
                return true;
            }

            /* These operations must be atomical */
            aux = this->regs->getCSR(csr);
            this->regs->setValue(rd, static_cast<std::int32_t>(aux));

            bitmask = rs1;
            aux = aux | bitmask;
            this->regs->setCSR(csr, aux);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRSI: CSR #{:d} -> x{:d}. x{:d} & CSR #{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                csr, rd, rs1, csr, aux);

            return true;
        }

        bool Exec_CSRRCI() const {
            unsigned rd, rs1;
            int csr;
            std::uint32_t bitmask, aux;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            csr = get_csr();

            if (rs1 == 0) {
                return true;
            }

            /* These operations must be atomical */
            aux = this->regs->getCSR(csr);
            this->regs->setValue(rd, static_cast<std::int32_t>(aux));

            bitmask = rs1;
            aux = aux & ~bitmask;
            this->regs->setCSR(csr, aux);

            this->logger->debug("{} ns. PC: 0x{:x}. CSRRCI: CSR #{:d} -> x{:d}. x{:d} & CSR #{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                csr, rd, rs1, csr, aux);

            return true;
        }

/*********************** Privileged Instructions ******************************/

        bool Exec_MRET() const {
            std::uint32_t new_pc = 0;

            new_pc = this->regs->getCSR(CSR_MEPC);
            this->regs->setPC(new_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. MRET: PC <- 0x{:x}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(), new_pc);

            // update mstatus
            std::uint32_t csr_temp;
            csr_temp = this->regs->getCSR(CSR_MSTATUS);
            if (csr_temp & MSTATUS_MPIE) {
                csr_temp |= MSTATUS_MIE;
            }
            csr_temp |= MSTATUS_MPIE;
            this->regs->setCSR(CSR_MSTATUS, csr_temp);

            return true;
        }

        bool Exec_SRET() const {
            std::uint32_t new_pc = 0;

            new_pc = this->regs->getCSR(CSR_SEPC);
            this->regs->setPC(new_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. SRET: PC <- 0x{:x}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC());
            return true;
        }

        bool Exec_WFI() const {
            this->logger->debug("{} ns. PC: 0x{:x}. WFI");
            return true;
        }

        bool Exec_SFENCE() const {
            this->logger->debug("{} ns. PC: 0x{:x}. SFENCE");
            return true;
        }

        /**
         * @brief Executes default ISA instruction
         * @param  inst instruction to execute
         * @return  true if PC is affected by instruction
         */
        bool process_instruction(Instruction &inst, bool *breakpoint = nullptr) {
            bool PC_not_affected = true;

            *breakpoint = false;
            this->setInstr(inst.getInstr());

            switch (decode()) {
                case OP_LUI:
                    Exec_LUI();
                    break;
                case OP_AUIPC:
                    Exec_AUIPC();
                    break;
                case OP_JAL:
                    Exec_JAL();
                    PC_not_affected = false;
                    break;
                case OP_JALR:
                    Exec_JALR();
                    PC_not_affected = false;
                    break;
                case OP_BEQ:
                    Exec_BEQ();
                    PC_not_affected = false;
                    break;
                case OP_BNE:
                    Exec_BNE();
                    PC_not_affected = false;
                    break;
                case OP_BLT:
                    Exec_BLT();
                    PC_not_affected = false;
                    break;
                case OP_BGE:
                    Exec_BGE();
                    PC_not_affected = false;
                    break;
                case OP_BLTU:
                    Exec_BLTU();
                    PC_not_affected = false;
                    break;
                case OP_BGEU:
                    Exec_BGEU();
                    PC_not_affected = false;
                    break;
                case OP_LB:
                    Exec_LB();
                    break;
                case OP_LH:
                    Exec_LH();
                    break;
                case OP_LW:
                    Exec_LW();
                    break;
                case OP_LBU:
                    Exec_LBU();
                    break;
                case OP_LHU:
                    Exec_LHU();
                    break;
                case OP_SB:
                    Exec_SB();
                    break;
                case OP_SH:
                    Exec_SH();
                    break;
                case OP_SW:
                    Exec_SW();
                    break;
                case OP_ADDI:
                    Exec_ADDI();
                    break;
                case OP_SLTI:
                    Exec_SLTI();
                    break;
                case OP_SLTIU:
                    Exec_SLTIU();
                    break;
                case OP_XORI:
                    Exec_XORI();
                    break;
                case OP_ORI:
                    Exec_ORI();
                    break;
                case OP_ANDI:
                    Exec_ANDI();
                    break;
                case OP_SLLI:
                    PC_not_affected = Exec_SLLI();
                    break;
                case OP_SRLI:
                    Exec_SRLI();
                    break;
                case OP_SRAI:
                    Exec_SRAI();
                    break;
                case OP_ADD:
                    Exec_ADD();
                    break;
                case OP_SUB:
                    Exec_SUB();
                    break;
                case OP_SLL:
                    Exec_SLL();
                    break;
                case OP_SLT:
                    Exec_SLT();
                    break;
                case OP_SLTU:
                    Exec_SLTU();
                    break;
                case OP_XOR:
                    Exec_XOR();
                    break;
                case OP_SRL:
                    Exec_SRL();
                    break;
                case OP_SRA:
                    Exec_SRA();
                    break;
                case OP_OR:
                    Exec_OR();
                    break;
                case OP_AND:
                    Exec_AND();
                    break;
                case OP_FENCE:
                    Exec_FENCE();
                    break;
                case OP_ECALL:
                    Exec_ECALL();
                    *breakpoint = true;
                    std::cout << "ECALL" << std::endl;
                    break;
                case OP_EBREAK:
                    PC_not_affected = Exec_EBREAK();
//		*breakpoint = true;
                    break;
                case OP_CSRRW:
                    Exec_CSRRW();
                    break;
                case OP_CSRRS:
                    Exec_CSRRS();
                    break;
                case OP_CSRRC:
                    Exec_CSRRC();
                    break;
                case OP_CSRRWI:
                    Exec_CSRRWI();
                    break;
                case OP_CSRRSI:
                    Exec_CSRRSI();
                    break;
                case OP_CSRRCI:
                    Exec_CSRRCI();
                    break;
                case OP_MRET:
                    Exec_MRET();
                    PC_not_affected = false;
                    break;
                case OP_SRET:
                    Exec_SRET();
                    PC_not_affected = false;
                    break;
                case OP_WFI:
                    Exec_WFI();
                    break;
                case OP_SFENCE:
                    Exec_SFENCE();
                    break;
                    [[unlikely]] default:
                    std::cout << "Wrong instruction" << "\n";
                    inst.dump();
                    this->NOP();
                    //sc_stop();
                    break;
            }

            return PC_not_affected;
        }

        /**
         * @brief Decodes opcode of instruction
         * @return opcode of instruction
         */
        opCodes decode() {
            switch (opcode()) {
                case LUI:
                    return OP_LUI;
                case AUIPC:
                    return OP_AUIPC;
                case JAL:
                    return OP_JAL;
                case JALR:
                    return OP_JALR;
                case BEQ:
                    switch (this->get_funct3()) {
                        case BEQ_F:
                            return OP_BEQ;
                        case BNE_F:
                            return OP_BNE;
                        case BLT_F:
                            return OP_BLT;
                        case BGE_F:
                            return OP_BGE;
                        case BLTU_F:
                            return OP_BLTU;
                        case BGEU_F:
                            return OP_BGEU;
                    }
                    return OP_ERROR;
                case LB:
                    switch (this->get_funct3()) {
                        case LB_F:
                            return OP_LB;
                        case LH_F:
                            return OP_LH;
                        case LW_F:
                            return OP_LW;
                        case LBU_F:
                            return OP_LBU;
                        case LHU_F:
                            return OP_LHU;
                    }
                    return OP_ERROR;
                case SB:
                    switch (this->get_funct3()) {
                        case SB_F:
                            return OP_SB;
                        case SH_F:
                            return OP_SH;
                        case SW_F:
                            return OP_SW;
                    }
                    return OP_ERROR;
                case ADDI:
                    switch (this->get_funct3()) {
                        case ADDI_F:
                            return OP_ADDI;
                        case SLTI_F:
                            return OP_SLTI;
                        case SLTIU_F:
                            return OP_SLTIU;
                        case XORI_F:
                            return OP_XORI;
                        case ORI_F:
                            return OP_ORI;
                        case ANDI_F:
                            return OP_ANDI;
                        case SLLI_F:
                            return OP_SLLI;
                        case SRLI_F:
                            switch (this->get_funct7()) {
                                case SRLI_F7:
                                    return OP_SRLI;
                                case SRAI_F7:
                                    return OP_SRAI;
                            }
                            return OP_ERROR;
                    }
                    return OP_ERROR;
                case ADD: {
                    switch (this->get_funct3()) {
                        case ADD_F:
                            switch (this->get_funct7()) {
                                case ADD_F7:
                                    return OP_ADD;
                                case SUB_F7:
                                    return OP_SUB;
                                default:
                                    return OP_ADD;
                            }
                            break;
                        case SLL_F:
                            return OP_SLL;
                        case SLT_F:
                            return OP_SLT;
                        case SLTU_F:
                            return OP_SLTU;
                        case XOR_F:
                            return OP_XOR;
                        case SRL_F:
                            switch (this->get_funct7()) {
                                case SRL_F7:
                                    return OP_SRL;
                                case SRA_F7:
                                    return OP_SRA;
                                default:
                                    return OP_ERROR;
                            }
                        case OR_F:
                            return OP_OR;
                        case AND_F:
                            return OP_AND;
                        default:
                            return OP_ERROR;
                    }
                } /* ADD */
                case FENCE:
                    return OP_FENCE;
                case ECALL: {
                    switch (this->get_funct3()) {
                        case ECALL_F3:
                            switch (this->get_csr()) {
                                case ECALL_F:
                                    return OP_ECALL;
                                case EBREAK_F:
                                    return OP_EBREAK;
                                case URET_F:
                                    return OP_URET;
                                case SRET_F:
                                    return OP_SRET;
                                case MRET_F:
                                    return OP_MRET;
                                case WFI_F:
                                    return OP_WFI;
                                case SFENCE_F:
                                    return OP_SFENCE;
                            }
                            if (this->m_instr.range(31, 25) == 0b0001001) {
                                return OP_SFENCE;
                            }
                            break;
                        case CSRRW:
                            return OP_CSRRW;
                            break;
                        case CSRRS:
                            return OP_CSRRS;
                            break;
                        case CSRRC:
                            return OP_CSRRC;
                            break;
                        case CSRRWI:
                            return OP_CSRRWI;
                            break;
                        case CSRRSI:
                            return OP_CSRRSI;
                            break;
                        case CSRRCI:
                            return OP_CSRRCI;
                            break;
                    }
                }
                    break;
                default:
                    return OP_ERROR;
            }

            return OP_ERROR;
        }
    };
}
#endif
