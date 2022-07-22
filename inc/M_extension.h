/*!
 \file M_extension.h
 \brief Implement M extensions part of the RISC-V
 \author Màrius Montón
 \date November 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef M_EXTENSION__H
#define M_EXTENSION__H

#include "systemc"

#include "extension_base.h"
#include "Registers.h"

namespace riscv_tlm {

    typedef enum {
        OP_M_MUL,
        OP_M_MULH,
        OP_M_MULHSU,
        OP_M_MULHU,
        OP_M_DIV,
        OP_M_DIVU,
        OP_M_REM,
        OP_M_REMU,

        OP_M_ERROR
    } op_M_Codes;

    typedef enum {
        M_MUL = 0b000,
        M_MULH = 0b001,
        M_MULHSU = 0b010,
        M_MULHU = 0b011,
        M_DIV = 0b100,
        M_DIVU = 0b101,
        M_REM = 0b110,
        M_REMU = 0b111,
    } M_Codes;

/**
 * @brief Instruction decoding and fields access
 */
    template<typename T>
    class M_extension : public extension_base<T> {
    public:

        /**
         * @brief Constructor, same as base class
         */
        using extension_base<T>::extension_base;

        using signed_T = typename std::make_signed<T>::type;
        using unsigned_T = typename std::make_unsigned<T>::type;

        /**
         * @brief Decodes opcode of instruction
         * @return opcode of instruction
         */
        [[nodiscard]] op_M_Codes decode() const {

            switch (opcode()) {
                case M_MUL:
                    return OP_M_MUL;
                    break;
                case M_MULH:
                    return OP_M_MULH;
                    break;
                case M_MULHSU:
                    return OP_M_MULHSU;
                    break;
                case M_MULHU:
                    return OP_M_MULHU;
                    break;
                case M_DIV:
                    return OP_M_DIV;
                    break;
                case M_DIVU:
                    return OP_M_DIVU;
                    break;
                case M_REM:
                    return OP_M_REM;
                    break;
                case M_REMU:
                    return OP_M_REMU;
                    break;
                    [[unlikely]] default:
                    return OP_M_ERROR;
                    break;
            }

            return OP_M_ERROR;
        }

        inline void dump() const override {
            std::cout << std::hex << "0x" << this->m_instr << std::dec << std::endl;
        }

        void Exec_M_MUL() const {
            unsigned int rd, rs1, rs2;
            std::int32_t multiplier, multiplicand;
            std::int64_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            multiplier = static_cast<std::int32_t>(extension_base<T>::regs->getValue(rs1));
            multiplicand = static_cast<std::int32_t>(extension_base<T>::regs->getValue(rs2));

            result = static_cast<std::int64_t>(multiplier * multiplicand);
            result = result & 0x00000000FFFFFFFF;
            this->regs->setValue(rd, static_cast<std::int32_t>(result));

            this->logger->debug("{} ns. PC: 0x{:x}. M.MUL: x{:d} * x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                extension_base<T>::regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_MULH() const {
            unsigned int rd, rs1, rs2;
            std::int32_t multiplier, multiplicand;
            std::int64_t result;
            std::int32_t ret_value;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            multiplier = static_cast<std::int32_t>(this->regs->getValue(rs1));
            multiplicand = static_cast<std::int32_t>(this->regs->getValue(rs2));

            result = static_cast<std::int64_t>(multiplier) * static_cast<std::int64_t>(multiplicand);

            ret_value = static_cast<std::int32_t>((result >> 32) & 0x00000000FFFFFFFF);
            this->regs->setValue(rd, ret_value);

            this->logger->debug("{} ns. PC: 0x{:x}. M.MULH: x{:d} * x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_MULHSU() const {
            unsigned int rd, rs1, rs2;
            std::int32_t multiplier;
            std::uint32_t multiplicand;
            std::int64_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            multiplier = static_cast<std::int32_t>(this->regs->getValue(rs1));
            multiplicand = this->regs->getValue(rs2);

            result = static_cast<std::int64_t>(multiplier * static_cast<std::uint64_t>(multiplicand));
            result = (result >> 32) & 0x00000000FFFFFFFF;
            this->regs->setValue(rd, static_cast<std::int32_t>(result));

            this->logger->debug("{} ns. PC: 0x{:x}. M.MULHSU: x{:d} * x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_MULHU() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t multiplier, multiplicand;
            std::uint64_t result;
            std::int32_t ret_value;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            multiplier = static_cast<std::int32_t>(this->regs->getValue(rs1));
            multiplicand = static_cast<std::int32_t>(this->regs->getValue(rs2));

            result = static_cast<std::uint64_t>(multiplier) * static_cast<std::uint64_t>(multiplicand);
            ret_value = static_cast<std::int32_t>((result >> 32) & 0x00000000FFFFFFFF);
            this->regs->setValue(rd, ret_value);

            this->logger->debug("{} ns. PC: 0x{:x}. M.MULHU: x{:d} * x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_DIV() const {
            unsigned int rd, rs1, rs2;
            std::int32_t divisor, dividend;
            std::int64_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            dividend = static_cast<std::int32_t>(this->regs->getValue(rs1));
            divisor = static_cast<std::int32_t>(this->regs->getValue(rs2));

            if (divisor == 0) {
                result = -1;
            } else if ((divisor == -1) && (dividend == static_cast<std::int32_t>(0x80000000))) {
                result = 0x0000000080000000;
            } else {
                result = dividend / divisor;
                result = result & 0x00000000FFFFFFFF;
            }

            this->regs->setValue(rd, static_cast<std::int32_t>(result));

            this->logger->debug("{} ns. PC: 0x{:x}. M.DIV: x{:d} / x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_DIVU() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t divisor, dividend;
            std::uint64_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            dividend = this->regs->getValue(rs1);
            divisor = this->regs->getValue(rs2);

            if (divisor == 0) {
                result = -1;
            } else {
                result = dividend / divisor;
                result = result & 0x00000000FFFFFFFF;
            }

            this->regs->setValue(rd, static_cast<std::int32_t>(result));

            this->logger->debug("{} ns. PC: 0x{:x}. M.DIVU: x{:d} / x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_REM() const {
            unsigned int rd, rs1, rs2;
            std::int32_t divisor, dividend;
            std::int32_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            dividend = static_cast<std::int32_t>(this->regs->getValue(rs1));
            divisor = static_cast<std::int32_t>(this->regs->getValue(rs2));

            if (divisor == 0) {
                result = dividend;
            } else if ((divisor == -1) && (dividend == static_cast<std::int32_t>(0x80000000))) {
                result = 0;
            } else {
                result = dividend % divisor;
            }

            this->regs->setValue(rd, result);

            this->logger->debug("{} ns. PC: 0x{:x}. M.REM: x{:d} / x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        void Exec_M_REMU() const {
            unsigned int rd, rs1, rs2;
            std::uint32_t divisor, dividend;
            std::uint32_t result;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            dividend = static_cast<std::int32_t>(this->regs->getValue(rs1));
            divisor = static_cast<std::int32_t>(this->regs->getValue(rs2));

            if (divisor == 0) {
                result = dividend;
            } else {
                result = dividend % divisor;
            }

            this->regs->setValue(rd, static_cast<std::int32_t>(result));

            this->logger->debug("{} ns. PC: 0x{:x}. M.REMU: x{:d} / x{:d} -> x{:d}({:d})",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, rs2, rd, result);
        }

        bool process_instruction(Instruction &inst) {
            this->setInstr(inst.getInstr());

            switch (decode()) {
                case OP_M_MUL:
                    Exec_M_MUL();
                    break;
                case OP_M_MULH:
                    Exec_M_MULH();
                    break;
                case OP_M_MULHSU:
                    Exec_M_MULHSU();
                    break;
                case OP_M_MULHU:
                    Exec_M_MULHU();
                    break;
                case OP_M_DIV:
                    Exec_M_DIV();
                    break;
                case OP_M_DIVU:
                    Exec_M_DIVU();
                    break;
                case OP_M_REM:
                    Exec_M_REM();
                    break;
                case OP_M_REMU:
                    Exec_M_REMU();
                    break;
                    [[unlikely]] default:
                    std::cout << "M instruction not implemented yet" << "\n";
                    inst.dump();
                    //NOP(inst);
                    sc_core::sc_stop();
                    break;
            }

            return true;
        }

    private:

        /**
         * @brief Access to opcode field
         * @return return opcode field
         */
        [[nodiscard]] inline unsigned_T opcode() const override {
            return static_cast<unsigned_T>(this->m_instr.range(14, 12));
        }

    };
}

#endif
