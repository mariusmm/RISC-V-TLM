/*!
 \file C_extension.hpp
 \brief Implement C extensions part of the RISC-V
 \author Màrius Montón
 \date August 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef C_EXTENSION__H
#define C_EXTENSION__H

#include <systemc>

#include "extension_base.hpp"

namespace riscv_tlm::extensions {

    typedef enum {
        OP_C_ADDI4SPN,
        OP_C_FLD,
        OP_C_LW,
        OP_C_FLW,
        OP_C_LD,
        OP_C_FSD,
        OP_C_SW,
        OP_C_FSW,
        OP_C_SD,

        OP_C_NOP,
        OP_C_ADDI,
        OP_C_JAL,
        OP_C_ADDIW,
        OP_C_LI,
        OP_C_ADDI16SP,
        OP_C_LUI,
        OP_C_SRLI,
        OP_C_SRAI,
        OP_C_ANDI,
        OP_C_SUB,
        OP_C_SUBW,
        OP_C_XOR,
        OP_C_ADDW,
        OP_C_OR,
        OP_C_AND,
        OP_C_J,
        OP_C_BEQZ,
        OP_C_BNEZ,

        OP_C_SLLI,
        OP_C_FLDSP,
        OP_C_LWSP,
        OP_C_FLWSP,
        OP_C_LDSP,
        OP_C_JR,
        OP_C_MV,
        OP_C_EBREAK,
        OP_C_JALR,
        OP_C_ADD,
        OP_C_FSDSP,
        OP_C_SWSP,
        OP_C_FSWSP,
        OP_C_SDSP,
        OP_C_ERROR
    } op_C_Codes;

    typedef enum {
        C_ADDI4SPN = 0b000,
        C_FLD = 0b001,
        C_LW = 0b010,
        C_FLW = 0b011,
        C_FSD = 0b101,
        C_SW = 0b110,
        C_FSW = 0b111,

        C_ADDI = 0b000,
        C_JAL = 0b001,
        C_LI = 0b010,
        C_ADDI16SP = 0b011,
        C_SRLI = 0b100,
        C_2_SRLI = 0b00,
        C_2_SRAI = 0b01,
        C_2_ANDI = 0b10,
        C_2_SUB = 0b11,
        C_3_SUB = 0b00,
        C_3_XOR = 0b01,
        C_3_OR = 0b10,
        C_3_AND = 0b11,
        C_J = 0b101,
        C_BEQZ = 0b110,
        C_BNEZ = 0b111,

        C_SLLI = 0b000,
        C_FLDSP = 0b001,
        C_LWSP = 0b010,
        C_FLWSP = 0b011,
        C_JR = 0b100,
        C_FDSP = 0b101,
        C_SWSP = 0b110,
        C_FWWSP = 0b111,
    } C_Codes;

/**
 * @brief Instruction decoding and fields access
 */
    template<typename T>
    class C_extension : public extension_base<T> {
    public:

        /**
         * @brief Constructor, same as base class
         */
        using extension_base<T>::extension_base;

        using signed_T = typename std::make_signed<T>::type;
        using unsigned_T = typename std::make_unsigned<T>::type;

        /**
         * @brief Access to opcode field
         * @return return opcode field
         */
        [[nodiscard]] inline unsigned_T opcode() const override {
            return static_cast<unsigned_T>(this->m_instr.range(1, 0));
        }

        [[nodiscard]] inline std::uint32_t get_rdp() const {
            return static_cast<std::uint32_t>(this->m_instr.range(4, 2) + 8);
        }

        /**
         * @brief Access to rs1 field
         * @return rs1 field
         */
        [[nodiscard]] inline std::uint32_t get_rs1() const override {
            return static_cast<std::uint32_t>(this->m_instr.range(11, 7));
        }

        inline void set_rs1(std::uint32_t value) override {
            this->m_instr.range(11, 7) = value;
        }

        [[nodiscard]] inline std::uint32_t get_rs1p() const {
            return static_cast<std::uint32_t>(this->m_instr.range(9, 7) + 8);
        }

        /**
         * @brief Access to rs2 field
         * @return rs2 field
         */
        [[nodiscard]] inline std::uint32_t get_rs2() const override {
            return static_cast<std::uint32_t>(this->m_instr.range(6, 2));
        }

        inline void set_rs2(std::uint32_t value) override {
            this->m_instr.range(6, 2) = value;
        }

        [[nodiscard]] inline std::uint32_t get_rs2p() const {
            return static_cast<std::uint32_t>(this->m_instr.range(4, 2) + 8);
        }

        [[nodiscard]] inline std::uint32_t get_funct3() const override {
            return static_cast<std::uint32_t>(this->m_instr.range(15, 13));
        }

        inline void set_funct3(std::uint32_t value) override {
            this->m_instr.range(15, 13) = value;
        }

        /**
         * @brief Access to immediate field for I-type
         * @return immediate_I field
         */
        [[nodiscard]] inline std::int32_t get_imm_I() const {
            std::int32_t aux = 0;

            aux = static_cast<std::int32_t>(this->m_instr.range(31, 20));

            /* sign extension (optimize) */
            if (this->m_instr[31] == 1) {
                aux |= (0b11111111111111111111) << 12;
            }

            return aux;
        }

        inline void set_imm_I(std::uint32_t value) const {
            this->m_instr.range(31, 20) = value;
        }

        /**
         * @brief Access to immediate field for S-type
         * @return immediate_S field
         */
        [[nodiscard]] inline std::int32_t get_imm_S() const {
            std::int32_t aux = 0;

            aux = static_cast<std::int32_t>(this->m_instr.range(31, 25) << 5);
            aux |= static_cast<std::int32_t>(this->m_instr.range(11, 7));

            if (this->m_instr[31] == 1) {
                aux |= (0b11111111111111111111) << 12;
            }

            return aux;
        }

        inline void set_imm_S(std::uint32_t value) const {
            sc_dt::sc_uint<32> aux = value;

            this->m_instr.range(31, 25) = aux.range(11, 5);
            this->m_instr.range(11, 7) = aux.range(4, 0);
        }

        /**
         * @brief Access to immediate field for U-type
         * @return immediate_U field
         */
        [[nodiscard]] inline std::uint32_t get_imm_U() const {
            return static_cast<std::uint32_t>(this->m_instr.range(31, 12));
        }

        inline void set_imm_U(std::uint32_t value) const {
            this->m_instr.range(31, 12) = (value << 12);
        }

        /**
         * @brief Access to immediate field for B-type
         * @return immediate_B field
         */
        [[nodiscard]] inline std::int32_t get_imm_B() const {
            std::int32_t aux = 0;

            aux |= static_cast<std::int32_t>(this->m_instr[7] << 11);
            aux |= static_cast<std::int32_t>(this->m_instr.range(30, 25) << 5);
            aux |= static_cast<std::int32_t>(this->m_instr[31] << 12);
            aux |= static_cast<std::int32_t>(this->m_instr.range(11, 8) << 1);

            if (this->m_instr[31] == 1) {
                aux |= (0b11111111111111111111) << 12;
            }

            return aux;
        }

        inline void set_imm_B(std::uint32_t value) const {
            sc_dt::sc_uint<32> aux = value;

            this->m_instr[31] = aux[12];
            this->m_instr.range(30, 25) = aux.range(10, 5);
            this->m_instr.range(11, 7) = aux.range(4, 1);
            this->m_instr[6] = aux[11];
        }

        /**
         * @brief Access to immediate field for J-type
         * @return immediate_J field
         */
        [[nodiscard]] inline std::int32_t get_imm_J() const {
            std::int32_t aux = 0;

            aux = static_cast<std::int32_t>(this->m_instr[12] << 11);
            aux |= static_cast<std::int32_t>(this->m_instr[11] << 4);
            aux |= static_cast<std::int32_t>(this->m_instr[10] << 9);
            aux |= static_cast<std::int32_t>(this->m_instr[9] << 8);
            aux |= static_cast<std::int32_t>(this->m_instr[8] << 10);
            aux |= static_cast<std::int32_t>(this->m_instr[7] << 6);
            aux |= static_cast<std::int32_t>(this->m_instr[6] << 7);
            aux |= static_cast<std::int32_t>(this->m_instr.range(5, 3) << 1);
            aux |= static_cast<std::int32_t>(this->m_instr[2] << 5);

            if (this->m_instr[12] == 1) {
                aux |= 0b11111111111111111111 << 12;
            }

            return aux;
        }

        inline void set_imm_J(std::uint32_t value) const {
            sc_dt::sc_uint<32> aux = (value << 20);

            this->m_instr[31] = aux[20];
            this->m_instr.range(30, 21) = aux.range(10, 1);
            this->m_instr[20] = aux[11];
            this->m_instr.range(19, 12) = aux.range(19, 12);
        }

        [[nodiscard]] inline std::uint32_t get_imm_L() const {
            std::uint32_t aux = 0;

            aux = this->m_instr.range(12, 10) << 3;
            aux |= this->m_instr[6] << 2;
            aux |= this->m_instr[5] << 6;

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_LWSP() const {
            std::uint32_t aux = 0;

            aux = this->m_instr[12] << 5;
            aux |= this->m_instr.range(6, 4) << 2;
            aux |= this->m_instr.range(3, 2) << 6;

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_LDSP() const {
            std::uint32_t aux = 0;

            aux = static_cast<std::uint32_t>(this->m_instr[12] << 5);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(6, 5) << 3);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(4, 2) << 6);

            return aux;
        }

        [[nodiscard]] inline std::int32_t get_imm_ADDI() const {
            std::uint32_t aux = 0;

            aux = static_cast<std::uint32_t>(this->m_instr[12] << 5);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(6, 2));

            if (this->m_instr[12] == 1) {
                aux |= 0b11111111111111111111111111 << 6;
            }
            return static_cast<std::int32_t>(aux);
        }

        [[nodiscard]] inline std::uint32_t get_imm_ADDI4SPN() const {
            std::uint32_t aux = 0;

            aux = static_cast<std::uint32_t>(this->m_instr.range(12, 11) << 4);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(10, 7) << 6);
            aux |= static_cast<std::uint32_t>(this->m_instr[6] << 2);
            aux |= static_cast<std::uint32_t>(this->m_instr[5] << 3);

            return aux;
        }

        [[nodiscard]] inline std::int32_t get_imm_ADDI16SP() const {
            std::uint32_t aux = 0;

            aux = static_cast<std::uint32_t>(this->m_instr[12] << 9);
            aux |= static_cast<std::uint32_t>(this->m_instr[6] << 4);
            aux |= static_cast<std::uint32_t>(this->m_instr[5] << 6);
            aux |= static_cast<std::uint32_t>(this->m_instr[4] << 8);
            aux |= static_cast<std::uint32_t>(this->m_instr[3] << 7);
            aux |= static_cast<std::uint32_t>(this->m_instr[2] << 5);

            if (this->m_instr[12] == 1) {
                aux |= 0b1111111111111111111111 << 10;
            }
            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_CSS() const {
            std::uint32_t aux = 0;
            aux = static_cast<std::uint32_t>(this->m_instr.range(12, 9) << 2);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(8, 7) << 6);

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_CSDSP() const {
            std::uint32_t aux = 0;
            aux = static_cast<std::uint32_t>(this->m_instr.range(12, 10) << 3);
            aux |= static_cast<std::uint32_t>(this->m_instr.range(9, 7) << 6);

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_CB() const {
            std::uint32_t aux = 0;

            aux = static_cast<std::uint32_t>(this->m_instr[12] << 8);
            aux |= static_cast<std::uint32_t>(this->m_instr[11] << 4);
            aux |= static_cast<std::uint32_t>(this->m_instr[10] << 3);
            aux |= static_cast<std::uint32_t>(this->m_instr[6] << 7);
            aux |= static_cast<std::uint32_t>(this->m_instr[5] << 6);
            aux |= static_cast<std::uint32_t>(this->m_instr[4] << 2);
            aux |= static_cast<std::uint32_t>(this->m_instr[3] << 1);
            aux |= static_cast<std::uint32_t>(this->m_instr[2] << 5);

            if (this->m_instr[12] == 1) {
                aux |= 0b11111111111111111111111 << 9;
            }

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_imm_CL() const {
            std::uint32_t aux = 0;

            aux = this->m_instr.range(12, 10) << 3;
            aux |= this->m_instr.range(6, 5) << 6;

            return aux;
        }

        [[nodiscard]] inline std::int32_t get_imm_LUI() const {
            std::int32_t aux = 0;

            aux = this->m_instr[12] << 17;
            aux |= this->m_instr.range(6, 2) << 12;

            if (this->m_instr[12] == 1) {
                aux |= 0b111111111111111 << 17;
            }

            return aux;
        }

        [[nodiscard]] inline std::uint32_t get_csr() const {
            return get_imm_I();
        }

        /**
         * @brief Decodes opcode of instruction
         * @return opcode of instruction
         */
        [[nodiscard]] op_C_Codes decode() const {

            switch (opcode()) {

                case 0b00:
                    switch (get_funct3()) {
                        case C_ADDI4SPN:
                            return OP_C_ADDI4SPN;
                            break;
                        case C_FLD:
                            return OP_C_FLD;
                            break;
                        case C_LW:
                            return OP_C_LW;
                            break;
                        case C_FLW:
                            if (sizeof(signed_T) == 4) {
                                // RV32
                                return OP_C_FLW;
                            } else {
                                // RV64
                                return OP_C_LD;
                            }
                            break;
                        case C_FSD:
                            return OP_C_FSD;
                            break;
                        case C_SW:
                            return OP_C_SW;
                            break;
                        case C_FSW:
                            if (sizeof(signed_T) == 4) {
                                return OP_C_FSW;
                            } else {
                                return OP_C_SD;
                            }
                            break;
                            [[unlikely]] default:
                            return OP_C_ERROR;
                            break;
                    }
                    break;

                case 0b01:
                    switch (get_funct3()) {
                        case C_ADDI:
                            return OP_C_ADDI;
                            break;
                        case C_JAL:
                            if (sizeof(signed_T) == 4) {
                                return OP_C_JAL;
                            } else {
                                return OP_C_ADDIW;
                            }
                            break;
                        case C_LI:
                            return OP_C_LI;
                            break;
                        case C_ADDI16SP:
                            return OP_C_ADDI16SP;
                            break;
                        case C_SRLI:
                            switch (this->m_instr.range(11, 10)) {
                                case C_2_SRLI:
                                    return OP_C_SRLI;
                                    break;
                                case C_2_SRAI:
                                    return OP_C_SRAI;
                                    break;
                                case C_2_ANDI:
                                    return OP_C_ANDI;
                                    break;
                                case C_2_SUB:
                                    switch (this->m_instr.range(6, 5)) {
                                        case C_3_SUB:
                                            if (this->m_instr[12] == 0) {
                                                return OP_C_SUB;
                                            } else {
                                                return OP_C_SUBW;
                                            }
                                            break;
                                        case C_3_XOR:
                                            if (this->m_instr[12] == 0) {
                                                return OP_C_XOR;
                                            } else {
                                                return OP_C_ADDW;
                                            }
                                            break;
                                        case C_3_OR:
                                            return OP_C_OR;
                                            break;
                                        case C_3_AND:
                                            return OP_C_AND;
                                            break;
                                    }
                            }
                            break;
                        case C_J:
                            return OP_C_J;
                            break;
                        case C_BEQZ:
                            return OP_C_BEQZ;
                            break;
                        case C_BNEZ:
                            return OP_C_BNEZ;
                            break;
                            [[unlikely]] default:
                            return OP_C_ERROR;
                            break;
                    }
                    break;

                case 0b10:
                    switch (get_funct3()) {
                        case C_SLLI:
                            return OP_C_SLLI;
                            break;
                        case C_FLDSP:
                        case C_LWSP:
                            return OP_C_LWSP;
                            break;
                        case C_FLWSP:
                            if (sizeof(signed_T) == 4) {
                                return OP_C_FLWSP;
                            } else {
                                return OP_C_LDSP;
                            }
                            break;
                        case C_JR:
                            if (this->m_instr[12] == 0) {
                                if (this->m_instr.range(6, 2) == 0) {
                                    return OP_C_JR;
                                } else {
                                    return OP_C_MV;
                                }
                            } else {
                                if (this->m_instr.range(11, 2) == 0) {
                                    return OP_C_EBREAK;
                                } else if (this->m_instr.range(6, 2) == 0) {
                                    return OP_C_JALR;
                                } else {
                                    return OP_C_ADD;
                                }
                            }
                            break;
                        case C_FDSP:
                            break;
                        case C_SWSP:
                            return OP_C_SWSP;
                            break;
                        case C_FWWSP:
                            if (sizeof(signed_T) == 4) {
                                return OP_C_FSWSP;
                            } else {
                                return OP_C_SDSP;
                            }
                            break;
                        default:
                            return OP_C_ERROR;
                            break;
                    }
                    break;

                    [[unlikely]] default:

                    return OP_C_ERROR;
                    break;
            }
            return OP_C_ERROR;
        }

        // PASS
        bool Exec_C_JR() const {
            std::uint32_t mem_addr;
            unsigned int rs1;
            unsigned_T new_pc;

            rs1 = get_rs1();
            mem_addr = 0;

            new_pc = static_cast<unsigned_T>(
                    static_cast<unsigned_T>((this->regs->getValue(rs1)) + static_cast<unsigned_T>(mem_addr)) &
                    0xFFFFFFFE);

            this->logger->debug("{} ns. PC: 0x{:x}. C.JR: PC <- 0x{:x}. x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(), new_pc, rs1, this->regs->getValue(rs1));

            this->regs->setPC(new_pc);

            return true;
        }

        bool Exec_C_MV() const {
            unsigned int rd, rs2;
            unsigned_T calc;

            rd = this->get_rd();
            rs2 = get_rs2();

            calc = this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.MV: x{:d}(0x{:x}) -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs2, this->regs->getValue(rs2), rd, calc);

            return true;
        }

        bool Exec_C_ADD() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1();
            rs1 = get_rs1();
            rs2 = get_rs2();

            calc = this->regs->getValue(rs1) + this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.ADD: x{:d} + x{} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_C_LWSP() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            std::uint32_t imm;
            unsigned_T data;

            // lw rd, offset[7:2](x2)

            rd = this->get_rd();
            rs1 = 2;
            imm = static_cast<std::uint32_t>(get_imm_LWSP());

            mem_addr = imm + this->regs->getValue(rs1);
            data = static_cast<std::int32_t>(this->mem_intf->readDataMem(mem_addr, 4));

            this->perf->dataMemoryRead();
            this->regs->setValue(rd, static_cast<std::int32_t>(data));

            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. C.LWSP: x{:d} + {:d}(@0x{:x}) -> x{:d}({:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, imm, mem_addr, rd, data);

            return true;
        }

        bool Exec_C_ADDI4SPN() {
            unsigned int rd, rs1;
            signed_T imm;
            signed_T calc;

            rd = get_rdp();
            rs1 = 2;
            imm = static_cast<signed_T>(get_imm_ADDI4SPN());

            if (imm == 0) {
                this->RaiseException(Exception_cause::ILLEGAL_INSTRUCTION, this->m_instr);
                return false;
            }

            calc = static_cast<signed_T>(this->regs->getValue(rs1)) + imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.ADDI4SN: x{:d} + (0x{:x}) + {:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), imm, rd, calc);

            return true;
        }

        bool Exec_C_ADDI16SP() const {
            unsigned int rd;
            signed_T imm;

            if (this->get_rd() == 2) {
                int rs1;
                unsigned_T calc;

                rd = 2;
                rs1 = 2;
                imm = get_imm_ADDI16SP();
                calc = this->regs->getValue(rs1) + imm;
                this->regs->setValue(rd, calc);

                this->logger->debug("{} ns. PC: 0x{:x}. C.ADDI16SP: x{:d} + {:d} -> x{:d} (0x{:x})",
                                    sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                    rs1, imm, rd, calc);
            } else {
                /* C.LUI OPCODE */
                rd = this->get_rd();
                imm = get_imm_LUI();
                this->regs->setValue(rd, imm);

                this->logger->debug("{} ns. PC: 0x{:x}. C.LUI: x{:d} <- 0x{:x}", sc_core::sc_time_stamp().value(),
                                    this->regs->getPC(),
                                    rd, imm);
            }

            return true;
        }

        bool Exec_C_SWSP() const {
            std::uint32_t mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint32_t data;

            rs1 = 2;
            rs2 = get_rs2();
            imm = static_cast<std::int32_t>(get_imm_CSS());

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. C.SWSP: x{:d}(0x{:x}) -> x{:d} + {} (@0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs2, data, rs1, imm, mem_addr);

            return true;
        }

        bool Exec_C_BEQZ() const {
            unsigned int rs1;
            unsigned_T new_pc, old_pc;
            unsigned_T val1;

            rs1 = get_rs1p();
            val1 = this->regs->getValue(rs1);
            old_pc = this->regs->getPC();

            if (val1 == 0) {
                new_pc = static_cast<unsigned_T>(this->regs->getPC()) + static_cast<std::int32_t>(get_imm_CB());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPCby2();
                new_pc = static_cast<unsigned_T>(this->regs->getPC());
            }

            this->logger->debug("{} ns. PC: 0x{:x}. C.BEQZ: x{:d}(0x{:x}) == 0? -> PC (0xx{:d})",
                                sc_core::sc_time_stamp().value(), old_pc,
                                rs1, val1, new_pc);

            return true;
        }

        bool Exec_C_BNEZ() const {
            unsigned int rs1;
            unsigned_T new_pc, old_pc;
            unsigned_T val1;

            rs1 = get_rs1p();
            val1 = this->regs->getValue(rs1);
            old_pc = this->regs->getPC();

            if (val1 != 0) {
                new_pc = static_cast<unsigned_T>(this->regs->getPC()) + static_cast<std::int32_t>(get_imm_CB());
                this->regs->setPC(new_pc);
            } else {
                this->regs->incPCby2(); //PC <- PC +2
                new_pc = static_cast<unsigned_T>(this->regs->getPC());
            }

            this->logger->debug("{} ns. PC: 0x{:x}. C.BNEZ: x{:d}(0x{:x}) != 0? -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), old_pc,
                                rs1, val1, new_pc);

            return true;
        }

        bool Exec_C_LI() const {
            unsigned int rd, rs1;
            std::int32_t imm;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = 0;
            imm = static_cast<std::int32_t>(get_imm_ADDI());
            calc = this->regs->getValue(rs1) + imm;

            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.LI: x{:d} ({:d}) + {:d} -> x{:d}(0x{:x}) ",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), imm, rd, calc);

            return true;
        }

        bool Exec_C_SRLI() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t shift;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_imm_ADDI();

            shift = rs2;

            calc = static_cast<unsigned_T>(this->regs->getValue(rs1)) >> shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SRLI: x{:d} >> {} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd);

            return true;
        }

        bool Exec_C_SRAI() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t shift;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_imm_ADDI();

            shift = rs2;

            calc = static_cast<signed_T>(this->regs->getValue(rs1)) >> shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SRAI: x{:d} >> {} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_C_SLLI() const {
            unsigned int rd, rs1;
            unsigned_T shift;
            unsigned_T calc;

            rd = get_rs1();
            rs1 = get_rs1();
            shift = get_imm_ADDI();

            calc = this->regs->getValue(rs1) << shift;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SLLI: x{:d} << {} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, shift, rd, calc);

            return true;
        }

        bool Exec_C_ANDI() const {
            unsigned int rd, rs1;
            signed_T imm;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            imm = get_imm_ADDI();

            calc = this->regs->getValue(rs1) & imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. ANDI: x{:d} C.AND 0x{:x} -> x{:d}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, rd);

            return true;
        }

        bool Exec_C_SUB() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc = this->regs->getValue(rs1) - this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SUB: x{:d} - x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_C_SUBW() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc =  static_cast<std::int32_t>((this->regs->getValue(rs1) - this->regs->getValue(rs2)) & 0xFFFFFFFF);
            this->regs->setValue(rd, static_cast<std::int32_t>(calc));

            this->logger->debug("{} ns. PC: 0x{:x}. C.SUBW: x{:d} - x{:d} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_C_ADDW() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc = static_cast<std::int32_t>((this->regs->getValue(rs1) + this->regs->getValue(rs2)) & 0xFFFFFFFF);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.ADDW: x{:d} + x{} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, calc);

            return true;
        }

        bool Exec_C_SDSP() const {
            unsigned_T mem_addr;
            unsigned int rs1, rs2;
            signed_T imm;
            std::uint64_t data;

            rs1 = 2;
            rs2 = get_rs2();
            imm = get_imm_CSDSP();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SDSP: 0x{:x} -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs2, rs1, imm, mem_addr);

            this->mem_intf->writeDataMem(mem_addr, data & 0xFFFFFFFF, 4);
            this->mem_intf->writeDataMem(mem_addr + 4, data >> 32, 4);
            this->perf->dataMemoryWrite();

            return true;
        }

        bool Exec_C_XOR() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc = this->regs->getValue(rs1) ^ this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.XOR: x{:d} XOR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd);

            return true;
        }

        bool Exec_C_OR() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc = this->regs->getValue(rs1) | this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.OR: x{:d} OR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd);

            return true;
        }

        bool Exec_C_AND() const {
            unsigned int rd, rs1, rs2;
            unsigned_T calc;

            rd = get_rs1p();
            rs1 = get_rs1p();
            rs2 = get_rs2p();

            calc = this->regs->getValue(rs1) & this->regs->getValue(rs2);
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.AND: x{:d} AND x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd);

            return true;
        }

        bool Exec_C_ADDI() const {
            unsigned int rd, rs1;
            signed_T imm;
            unsigned_T calc;

            rd = this->get_rd();
            rs1 = rd;
            imm = static_cast<signed_T>(get_imm_ADDI());

            calc = this->regs->getValue(rs1) + imm;
            this->regs->setValue(rd, calc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.ADDI: x{:d} + {} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(), rs1, imm, rd, calc);

            return true;
        }

        bool Exec_C_ADDIW() const {
            unsigned int rd, rs1;
            std::int32_t imm;
            std::int32_t aux;
            std::int64_t calc;

            rd = this->get_rd();
            rs1 = rd;
            imm = get_imm_ADDI();

            aux = static_cast<std::int32_t>(this->regs->getValue(rs1) & 0xFFFFFFFF);
            aux = static_cast<std::int32_t>(aux + imm);
            calc = static_cast<std::int32_t>(aux);

            this->regs->setValue(rd, calc);
            this->logger->debug("{} ns. PC: 0x{:x}. C.ADDIW: x{:d} + {} -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(), rs1, imm, rd, calc);

            return true;
        }

        bool Exec_C_JALR() const {
            std::uint32_t mem_addr = 0;
            unsigned int rd, rs1;
            std::uint32_t new_pc, old_pc;

            rd = 1;
            rs1 = get_rs1();

            old_pc = static_cast<std::int32_t>(this->regs->getPC());
            new_pc = static_cast<std::int32_t>((this->regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE);

            this->regs->setValue(rd, old_pc + 2);
            this->regs->setPC(new_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.JALR: x{:d} <- 0x{:x} PC <- 0xx{:x}",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rd, old_pc + 4, new_pc);

            return true;
        }

        bool Exec_C_LW() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            unsigned_T imm;
            signed_T data;

            rd = get_rdp();
            rs1 = get_rs1p();
            imm = get_imm_L();

            mem_addr = imm + this->regs->getValue(rs1);
            data = static_cast<std::int32_t>(this->mem_intf->readDataMem(mem_addr, 4));

            this->perf->dataMemoryRead();
            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. C.LW: x{:d}(0x{:x}) + {:d} (@0x{:x}) -> {:d} (0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs1, this->regs->getValue(rs1), imm, mem_addr, rd, data);

            return true;
        }

        bool Exec_C_LD() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            unsigned_T imm;
            std::uint64_t data;

            rd = get_rdp();
            rs1 = get_rs1p();
            imm = get_imm_CL();

            mem_addr = imm + this->regs->getValue(rs1);
            data = static_cast<std::uint32_t>(this->mem_intf->readDataMem(mem_addr, 4));
            std::uint64_t aux = static_cast<std::uint32_t>(this->mem_intf->readDataMem(mem_addr + 4, 4));
            data |= aux << 32;

            this->perf->dataMemoryRead();
            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. C.LD: 0x{:x} + x{:d} (0x{:x}) -> x{:d}(0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, imm, mem_addr, rd, data);

            return true;
        }

        bool Exec_C_SD() const {
            unsigned_T mem_addr;
            unsigned int rs1, rs2;
            signed_T imm;
            std::uint64_t data;

            rs1 = get_rs1p();
            rs2 = get_rs2p();
            imm = get_imm_CL();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->logger->debug("{} ns. PC: 0x{:x}. C.SD: 0x{:x} -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs2, rs1, imm, mem_addr);

            this->mem_intf->writeDataMem(mem_addr, data & 0xFFFFFFFF, 4);
            this->mem_intf->writeDataMem(mem_addr + 4, data >> 32, 4);
            this->perf->dataMemoryWrite();

            return true;
        }

        bool Exec_C_SW() const {
            std::uint32_t mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint32_t data;

            rs1 = get_rs1p();
            rs2 = get_rs2p();
            imm = get_imm_L();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. C.SW: x{:d}(0x{:x}) -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs2, data, rs1, imm, mem_addr);

            return true;
        }

        bool Exec_C_FSW_SD() const {
            std::uint32_t mem_addr;
            unsigned int rs1, rs2;
            std::int32_t imm;
            std::uint64_t data;
            std::uint32_t aux;

            rs1 = get_rs1p();
            rs2 = get_rs2p();
            imm = get_imm_L();

            mem_addr = imm + this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            aux = data >> 32;
            this->mem_intf->writeDataMem(mem_addr, aux, 4);

            aux = data & 0x00000000FFFFFFFF;
            this->mem_intf->writeDataMem(mem_addr+4, aux, 4);

            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. C.SD: x{:d}(0x{:x}) -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                rs2, data, rs1, imm, mem_addr);
            return true;
        }

        // Implemented RV64 C.LDSP only (FLWSP is for RV32 with F extension)
        bool Exec_C_LDSP() const {
            unsigned_T mem_addr;
            unsigned int rd, rs1;
            unsigned_T imm;
            std::uint64_t data;
            std::uint64_t aux;

            rd = this->get_rd();
            rs1 = 2;
            imm = get_imm_LDSP();

            if (rd == 0) {
                // Error
                std::cout << "C.LDSP Error!\n";
            }

            mem_addr = imm + this->regs->getValue(rs1);
            data  = static_cast<std::uint32_t>(this->mem_intf->readDataMem(mem_addr, 4));
            aux = static_cast<std::uint32_t>(this->mem_intf->readDataMem(mem_addr + 4, 4));
            data |= aux << 32;

            this->perf->dataMemoryRead();
            this->regs->setValue(rd, data);

            this->logger->debug("{} ns. PC: 0x{:x}. C.LDSP: x{:d}(0x{:x}) -> x{:d} + 0x{:x}(@0x{:x})",
                                sc_core::sc_time_stamp().value(), this->regs->getPC(),
                                2, data, rs1, imm, mem_addr);
            return true;
        }


        bool Exec_C_JAL(int m_rd) const {
            std::int32_t mem_addr;
            unsigned int rd;
            std::uint32_t new_pc, old_pc;

            rd = m_rd;
            mem_addr = get_imm_J();
            old_pc = static_cast<std::int32_t>(this->regs->getPC());

            new_pc = old_pc + mem_addr;
            this->regs->setPC(new_pc);

            old_pc = old_pc + 2;
            this->regs->setValue(rd, old_pc);

            this->logger->debug("{} ns. PC: 0x{:x}. C.JAL: x{:d} <- 0x{:x}. PC + 0x{:x} -> PC (0x{:x})",
                                sc_core::sc_time_stamp().value(), old_pc - 2,
                                rd, old_pc, mem_addr, new_pc);

            return true;
        }

        bool Exec_C_EBREAK() {

            this->logger->debug("{} ns. PC: 0x{:x}. C.EBREAK", sc_core::sc_time_stamp().value(), this->regs->getPC());
            std::cout << std::endl << "C.EBRAK  Instruction called, dumping information"
                      << std::endl;
            this->regs->dump();
            std::cout << "Simulation time " << sc_core::sc_time_stamp() << "\n";
            this->perf->dump();

            this->RaiseException(Exception_cause::BREAK, this->m_instr);

            return false;
        }

        bool exec_instruction(CPU::Instruction &inst, bool *breakpoint, op_C_Codes code) {
            bool PC_not_affected = true;

            *breakpoint = false;

            this->setInstr(inst.getInstr());

            switch (code) {
                case OP_C_ADDI4SPN:
                    PC_not_affected = Exec_C_ADDI4SPN();
                    break;
                case OP_C_LW:
                    Exec_C_LW();
                    break;
                case OP_C_SW:
                    Exec_C_SW();
                    break;
                case OP_C_ADDI:
                    Exec_C_ADDI();
                    break;
                case OP_C_JAL:
                    Exec_C_JAL(1);
                    PC_not_affected = false;
                    break;
                case OP_C_J:
                    Exec_C_JAL(0);
                    PC_not_affected = false;
                    break;
                case OP_C_LI:
                    Exec_C_LI();
                    break;
                case OP_C_SLLI:
                    Exec_C_SLLI();
                    break;
                case OP_C_LWSP:
                    Exec_C_LWSP();
                    break;
                case OP_C_JR:
                    Exec_C_JR();
                    PC_not_affected = false;
                    break;
                case OP_C_MV:
                    Exec_C_MV();
                    break;
                case OP_C_JALR:
                    Exec_C_JALR();
                    PC_not_affected = false;
                    break;
                case OP_C_ADD:
                    Exec_C_ADD();
                    break;
                case OP_C_SWSP:
                    Exec_C_SWSP();
                    break;
                case OP_C_ADDI16SP:
                    Exec_C_ADDI16SP();
                    break;
                case OP_C_BEQZ:
                    Exec_C_BEQZ();
                    PC_not_affected = false;
                    break;
                case OP_C_BNEZ:
                    Exec_C_BNEZ();
                    PC_not_affected = false;
                    break;
                case OP_C_SRLI:
                    Exec_C_SRLI();
                    break;
                case OP_C_SRAI:
                    Exec_C_SRAI();
                    break;
                case OP_C_ANDI:
                    Exec_C_ANDI();
                    break;
                case OP_C_SUB:
                    Exec_C_SUB();
                    break;
                case OP_C_XOR:
                    Exec_C_XOR();
                    break;
                case OP_C_OR:
                    Exec_C_OR();
                    break;
                case OP_C_AND:
                    Exec_C_AND();
                    break;
                case OP_C_FSW:
                    Exec_C_FSW_SD();
                    break;
                case OP_C_FLWSP:
                    //Exec_C_FLWSP_LDSP();
                    break;
                case OP_C_EBREAK:
                    Exec_C_EBREAK();
                    std::cout << "C_EBREAK" << std::endl;
                    *breakpoint = true;
                    PC_not_affected = false;
                    break;
                case OP_C_LD:
                    Exec_C_LD();
                    break;
                case OP_C_SD:
                    Exec_C_SD();
                    break;
                case OP_C_ADDIW:
                    Exec_C_ADDIW();
                    break;
                case OP_C_ADDW:
                    Exec_C_ADDW();
                    break;
                case OP_C_SUBW:
                    Exec_C_SUBW();
                    break;
                case OP_C_SDSP:
                    Exec_C_SDSP();
                    break;
                case OP_C_LDSP:
                    Exec_C_LDSP();
                    break;
                 [[unlikely]] default:
                    std::cout << "C instruction not implemented yet" << "\n";
                    inst.dump();
                    this->NOP();
                    break;
            }

            return PC_not_affected;
        }

    };
}

#endif
