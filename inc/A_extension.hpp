/*!
 \file A_extension.hpp
 \brief Implement A extensions part of the RISC-V
 \author Màrius Montón
 \date December 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef A_EXTENSION__H
#define A_EXTENSION__H

#include <systemc>

#include "Registers.hpp"
#include "MemoryInterface.hpp"
#include "extension_base.hpp"

#include <unordered_set>

namespace riscv_tlm::extensions {

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
    template<typename T>
    class A_extension : public extension_base<T> {
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
        inline unsigned_T opcode() const override {
            return static_cast<unsigned_T>(this->m_instr.range(31, 27));
        }

        /**
         * @brief Decodes opcode of instruction
         * @return opcode of instruction
         */
        op_A_Codes decode() const {

            switch (opcode()) {
                case A_LR:
                    return OP_A_LR;
                    break;
                case A_SC:
                    return OP_A_SC;
                    break;
                case A_AMOSWAP:
                    return OP_A_AMOSWAP;
                    break;
                case A_AMOADD:
                    return OP_A_AMOADD;
                    break;
                case A_AMOXOR:
                    return OP_A_AMOXOR;
                    break;
                case A_AMOAND:
                    return OP_A_AMOAND;
                    break;
                case A_AMOOR:
                    return OP_A_AMOOR;
                    break;
                case A_AMOMIN:
                    return OP_A_AMOMIN;
                    break;
                case A_AMOMAX:
                    return OP_A_AMOMAX;
                    break;
                case A_AMOMINU:
                    return OP_A_AMOMINU;
                    break;
                case A_AMOMAXU:
                    return OP_A_AMOMAXU;
                    break;
                    [[unlikely]] default:
                    return OP_A_ERROR;
                    break;

            }

            return OP_A_ERROR;
        }

        inline void dump() const override {
            std::cout << std::hex << "0x" << this->m_instr << std::dec << std::endl;
        }

        bool Exec_A_LR() {
            std::uint32_t mem_addr = 0;
            int rd, rs1, rs2;
            std::uint32_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            if (rs2 != 0) {
                std::cout << "ILEGAL INSTRUCTION, LR.W: rs2 != 0" << std::endl;
                this->RaiseException(Exception_cause::ILLEGAL_INSTRUCTION, this->m_instr);

                return false;
            }

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, static_cast<int32_t>(data));

            TLB_reserve(mem_addr);

            this->logger->debug("{} ns. PC: 0x{:x}. A.LR.W: x{:d}(0x{:x}) -> x{:d}(0x{:x}) ",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                rs1, mem_addr, rd, data);

            return true;
        }

        bool Exec_A_SC() {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->regs->getValue(rs2);

            if (TLB_reserved(mem_addr)) {
                this->mem_intf->writeDataMem(mem_addr, data, 4);
                this->perf->dataMemoryWrite();
                this->regs->setValue(rd, 0);  // SC writes 0 to rd on success
            } else {
                this->regs->setValue(rd, 1);  // SC writes nonzero on failure
            }

            this->logger->debug("{} ns. PC: 0x{:x}. A.SC.W: (0x{:x}) <- x{:d}(0x{:x}) ",
                                sc_core::sc_time_stamp().value(),
                                this->regs->getPC(),
                                mem_addr, rs2, data);

            return true;
        }

        bool Exec_A_AMOSWAP() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;
            std::uint32_t aux;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();
            this->regs->setValue(rd, static_cast<int32_t>(data));

            // swap
            aux = this->regs->getValue(rs2);
            this->regs->setValue(rs2, static_cast<int32_t>(data));

            this->mem_intf->writeDataMem(mem_addr, aux, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOSWAP");

            return true;
        }

        bool Exec_A_AMOADD() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // add
            data = data + this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOADD");

            return true;
        }

        bool Exec_A_AMOXOR() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // add
            data = data ^ this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOXOR");

            return true;
        }

        bool Exec_A_AMOAND() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // add
            data = data & this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOAND");

            return true;
        }

        bool Exec_A_AMOOR() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // add
            data = data | this->regs->getValue(rs2);

            this->mem_intf->writeDataMem(mem_addr, data, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOOR");

            return true;
        }

        bool Exec_A_AMOMIN() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;
            std::uint32_t aux;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // min
            aux = this->regs->getValue(rs2);
            if ((int32_t) data < (int32_t) aux) {
                aux = data;
            }

            this->mem_intf->writeDataMem(mem_addr, aux, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOMIN");

            return true;
        }

        bool Exec_A_AMOMAX() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;
            std::uint32_t aux;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // >
            aux = this->regs->getValue(rs2);
            if ((int32_t) data > (int32_t) aux) {
                aux = data;
            }

            this->mem_intf->writeDataMem(mem_addr, aux, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOMAX");

            return true;
        }

        bool Exec_A_AMOMINU() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;
            std::uint32_t aux;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // min
            aux = this->regs->getValue(rs2);
            if (data < aux) {
                aux = data;
            }

            this->mem_intf->writeDataMem(mem_addr, aux, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOMINU");

            return true;
        }

        bool Exec_A_AMOMAXU() const {
            std::uint32_t mem_addr;
            int rd, rs1, rs2;
            std::uint32_t data;
            std::uint32_t aux;

            /* These instructions must be atomic */

            rd = this->get_rd();
            rs1 = this->get_rs1();
            rs2 = this->get_rs2();

            mem_addr = this->regs->getValue(rs1);
            data = this->mem_intf->readDataMem(mem_addr, 4);
            this->perf->dataMemoryRead();

            this->regs->setValue(rd, static_cast<int32_t>(data));

            // max
            aux = this->regs->getValue(rs2);
            if (data > aux) {
                aux = data;
            }

            this->mem_intf->writeDataMem(mem_addr, aux, 4);
            this->perf->dataMemoryWrite();

            this->logger->debug("{} ns. PC: 0x{:x}. A.AMOMAXU");

            return true;
        }

        void TLB_reserve(std::uint32_t address) {
            TLB_A_Entries.insert(address);
        }

        bool TLB_reserved(std::uint32_t address) {
            if (TLB_A_Entries.count(address) == 1) {
                TLB_A_Entries.erase(address);
                return true;
            } else {
                return false;
            }
        }

        bool exec_instruction(Instruction &inst, op_A_Codes code) {
            bool PC_not_affected = true;

            this->setInstr(inst.getInstr());

            switch (code) {
                case OP_A_LR:
                    Exec_A_LR();
                    break;
                case OP_A_SC:
                    Exec_A_SC();
                    break;
                case OP_A_AMOSWAP:
                    Exec_A_AMOSWAP();
                    break;
                case OP_A_AMOADD:
                    Exec_A_AMOADD();
                    break;
                case OP_A_AMOXOR:
                    Exec_A_AMOXOR();
                    break;
                case OP_A_AMOAND:
                    Exec_A_AMOAND();
                    break;
                case OP_A_AMOOR:
                    Exec_A_AMOOR();
                    break;
                case OP_A_AMOMIN:
                    Exec_A_AMOMIN();
                    break;
                case OP_A_AMOMAX:
                    Exec_A_AMOMAX();
                    break;
                case OP_A_AMOMINU:
                    Exec_A_AMOMINU();
                    break;
                case OP_A_AMOMAXU:
                    Exec_A_AMOMAXU();
                    break;
                    [[unlikely]] default:
                    std::cout << "A instruction not implemented yet" << std::endl;
                    inst.dump();
                    this->NOP();
                    break;
            }

            return PC_not_affected;
        }

    private:
        std::unordered_set<std::uint32_t> TLB_A_Entries;
    };
}

#endif
