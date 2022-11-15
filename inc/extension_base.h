/*!
 \file extension_base.h
 \brief Base class for ISA extensions
 \author Màrius Montón
 \date May 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_EXTENSION_BASE_H_
#define INC_EXTENSION_BASE_H_

#include "systemc"

#include "Instruction.h"
#include "Registers.h"
#include "MemoryInterface.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace riscv_tlm {

    typedef enum {
        INSTRUCTION_MISALIGN = 0,
        INSTRUCTION_ACCESS = 1,
        ILLEGAL_INSTRUCTION = 2,
        BREAKPOINT = 3,
        BREAK = 3,
        LOAD_ADDR_MISALIGN = 4,
        LOAD_ACCESS_FAULT = 5,
        CALL_FROM_M_MODE = 11,
    } Exception_cause;

    template<typename T>
    class extension_base {

        using signed_T = typename std::make_signed<T>::type;
        using unsigned_T = typename std::make_unsigned<T>::type;

    public:
        extension_base(const T &instr, Registers<T> *register_bank,
                       MemoryInterface *mem_interface) :
                m_instr(instr), regs(register_bank), mem_intf(mem_interface) {

            perf = Performance::getInstance();
            logger = spdlog::get("my_logger");
        }

        virtual ~extension_base() = default;

        void setInstr(std::uint32_t p_instr) {
            m_instr = sc_dt::sc_uint<32>(p_instr);
        }

        void RaiseException(Exception_cause cause, std::uint32_t inst) {
            std::uint32_t new_pc, current_pc, m_cause;

            current_pc = regs->getPC();
            m_cause = regs->getCSR(CSR_MSTATUS);
            m_cause |= static_cast<uint32_t>(cause);

            new_pc = regs->getCSR(CSR_MTVEC);

            regs->setCSR(CSR_MEPC, current_pc);

            if (cause == Exception_cause::ILLEGAL_INSTRUCTION) {
                regs->setCSR(CSR_MTVAL, inst);
            } else if (cause == Exception_cause::BREAK) {
                regs->setCSR(CSR_MTVAL, current_pc);
            } else {
                regs->setCSR(CSR_MTVAL, 0);
            }

            regs->setCSR(CSR_MCAUSE, static_cast<uint32_t>(cause));
            regs->setCSR(CSR_MSTATUS, m_cause);

            regs->setPC(new_pc);

            logger->debug("{} ns. PC: 0x{:x}. Exception! new PC 0x{:x} ", sc_core::sc_time_stamp().value(),
                          regs->getPC(),
                          new_pc);
        }

        bool NOP() {
            logger->debug("{} ns. PC: 0x{:x}. NOP! new PC 0x{:x} ", sc_core::sc_time_stamp().value(), regs->getPC());
            logger->flush();
            sc_core::sc_stop();
            return true;
        }

        /* pure virtual functions */
        virtual unsigned_T opcode() const = 0;

        virtual unsigned int get_rd() const {
            return m_instr.range(11, 7);
        }

        virtual void set_rd(unsigned int value) {
            m_instr.range(11, 7) = value;
        }

        virtual unsigned int get_rs1() const {
            return m_instr.range(19, 15);
        }

        virtual void set_rs1(unsigned int value) {
            m_instr.range(19, 15) = value;
        }

        virtual unsigned int get_rs2() const {
            return m_instr.range(24, 20);
        }

        virtual void set_rs2(unsigned int value) {
            m_instr.range(24, 20) = value;
        }

        virtual unsigned int get_funct3() const {
            return m_instr.range(14, 12);
        }

        virtual void set_funct3(unsigned int value) {
            m_instr.range(14, 12) = value;
        }

        // For RV64 SLLI, SRLI & SRAI
        virtual unsigned int get_shamt_slli() const {
            return m_instr.range(25, 20);
        }
        virtual void dump() const {
            std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
        }

    protected:
        sc_dt::sc_uint<32> m_instr;
        Registers<T> *regs;
        Performance *perf;
        MemoryInterface *mem_intf;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif /* INC_EXTENSION_BASE_H_ */
