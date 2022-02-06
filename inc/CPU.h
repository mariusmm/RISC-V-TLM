/*!
 \file CPU.h
 \brief Main CPU class
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CPU_BASE_H
#define CPU_BASE_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

#include "memory.h"
#include "MemoryInterface.h"
#include "BASE_ISA.h"
#include "Registers.h"
#include "Instruction.h"
#include "C_extension.h"
#include "M_extension.h"
#include "A_extension.h"


namespace riscv_tlm {
/**
 * @brief ISC_V CPU model
 * @param name name of the module
 */
    class CPU : sc_core::sc_module {
    public:

        /**
         * @brief Instruction Memory bus socket
         * @param trans transction to perfoem
         * @param delay time to annotate
         */
        tlm_utils::simple_initiator_socket<CPU> instr_bus;

        /**
         * @brief IRQ line socket
         * @param trans transction to perform (empty)
         * @param delay time to annotate
         */
        tlm_utils::simple_target_socket<CPU> irq_line_socket;

        /**
         * @brief Constructor
         * @param name Module name
         * @param PC   Program Counter initialize value
         * @param debug To start debugging
         */
        CPU(sc_core::sc_module_name const &name, std::uint32_t PC, bool debug);

        /**
         * @brief Destructor
         */
        ~CPU() override;

        MemoryInterface *mem_intf;

        bool CPU_step();


        Registers<std::uint32_t> *getRegisterBank() { return register_bank; }

    private:
        Registers<std::uint32_t> *register_bank;
        Performance *perf;
        std::shared_ptr<spdlog::logger> logger;
        C_extension *c_inst;
        M_extension *m_inst;
        A_extension *a_inst;
        BASE_ISA *exec;
        tlm_utils::tlm_quantumkeeper *m_qk;

        Instruction inst;
        bool interrupt;
        std::uint32_t int_cause;
        bool irq_already_down;
        sc_core::sc_time default_time;
        bool dmi_ptr_valid;

        tlm::tlm_generic_payload trans;
        std::uint32_t INSTR;
        unsigned char *dmi_ptr = nullptr;


        /**
         *
         * @brief Process and triggers IRQ if all conditions met
         * @return true if IRQ is triggered, false otherwise
         */
        bool cpu_process_IRQ();

        /**
         * main thread for CPU simulation
         * @brief CPU mai thread
         */
        [[noreturn]] void CPU_thread();

        /**
         * @brief callback for IRQ simple socket
         * @param trans transaction to perform (empty)
         * @param delay time to annotate
         *
         * it triggers an IRQ when called
         */
        void call_interrupt(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay);

        /**
         * DMI pointer is not longer valid
         * @param start memory address region start
         * @param end memory address region end
         */
        void invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end);
    };

}
#endif
