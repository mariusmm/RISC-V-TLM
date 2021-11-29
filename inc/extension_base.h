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

    class extension_base {

    public:
        extension_base(const sc_dt::sc_uint<32> &instr, Registers *register_bank,
                       MemoryInterface *mem_interface);

        virtual ~extension_base() = 0;

        void setInstr(std::uint32_t p_instr);

        void RaiseException(std::uint32_t cause, std::uint32_t inst);

        bool NOP();

        /* pure virtual functions */
        virtual std::int32_t opcode() const = 0;

        virtual std::int32_t get_rd() const {
            return m_instr.range(11, 7);
        }

        virtual void set_rd(std::int32_t value) {
            m_instr.range(11, 7) = value;
        }

        virtual std::int32_t get_rs1() const {
            return m_instr.range(19, 15);
        }

        virtual void set_rs1(std::int32_t value) {
            m_instr.range(19, 15) = value;
        }

        virtual std::int32_t get_rs2() const {
            return m_instr.range(24, 20);
        }

        virtual void set_rs2(std::int32_t value) {
            m_instr.range(24, 20) = value;
        }

        virtual std::int32_t get_funct3() const {
            return m_instr.range(14, 12);
        }

        virtual void set_funct3(std::int32_t value) {
            m_instr.range(14, 12) = value;
        }

        virtual void dump() const;

    protected:
        sc_dt::sc_uint<32> m_instr;
        Registers *regs;
        Performance *perf;
        MemoryInterface *mem_intf;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif /* INC_EXTENSION_BASE_H_ */
