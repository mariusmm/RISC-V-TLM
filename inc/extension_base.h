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

#define EXCEPTION_CAUSE_INSTRUCTION_MISALIGN  0
#define EXCEPTION_CAUSE_INSTRUCTION_ACCESS    1
#define EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION   2
#define EXCEPTION_CAUSE_BREAKPOINT            3
#define EXCEPTION_CAUSE_BREAK                 3
#define EXCEPTION_CAUSE_LOAD_ADDR_MISALIGN    4
#define EXCEPTION_CAUSE_LOAD_ACCESS_FAULT     5

namespace riscv_tlm {

    template<typename T>
    class extension_base {

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

        void RaiseException(std::uint32_t cause, std::uint32_t inst) {
            std::uint32_t new_pc, current_pc, m_cause;

            current_pc = regs->getPC();
            m_cause = regs->getCSR(CSR_MSTATUS);
            m_cause |= cause;

            new_pc = regs->getCSR(CSR_MTVEC);

            regs->setCSR(CSR_MEPC, current_pc);

            if (cause == EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION) {
                regs->setCSR(CSR_MTVAL, inst);
            } else {
                regs->setCSR(CSR_MTVAL, current_pc);
            }

            regs->setCSR(CSR_MCAUSE, cause);
            regs->setCSR(CSR_MSTATUS, m_cause);

            regs->setPC(new_pc);

            logger->debug("{} ns. PC: 0x{:x}. Exception! new PC 0x{:x} ", sc_core::sc_time_stamp().value(),
                          regs->getPC(),
                          new_pc);
        }

        bool NOP() {
            logger->debug("{} ns. PC: 0x{:x}. NOP! new PC 0x{:x} ", sc_core::sc_time_stamp().value(), regs->getPC());
            sc_core::sc_stop();
            return true;
        }

        /* pure virtual functions */
        virtual std::uint32_t opcode() const = 0;

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
