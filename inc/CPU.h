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
#include "tlm_utils/simple_target_socket.h"

#include "BASE_ISA.h"
#include "C_extension.h"
#include "M_extension.h"
#include "A_extension.h"
#include "MemoryInterface.h"
#include "Performance.h"
#include "Registers.h"

namespace riscv_tlm {

    class CPU : sc_core::sc_module  {
    public:

        /* Constructors */
        explicit CPU(sc_core::sc_module_name const &name, bool debug);

        CPU() noexcept = delete;
        CPU(const CPU& other) noexcept = delete;
        CPU(CPU && other) noexcept = delete;
        CPU& operator=(const CPU& other) noexcept = delete;
        CPU& operator=(CPU&& other) noexcept = delete;

        /* Destructors */
        ~CPU() override = default;

        /**
         * @brief Perform one instruction step
         * @return Breackpoint found (TBD, always true)
         */
        virtual bool CPU_step() = 0;

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
        * @brief DMI pointer is not longer valid
        * @param start memory address region start
        * @param end memory address region end
        */
        void invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end);

        /**
        * @brief CPU main thread
        */
        [[noreturn]] void CPU_thread();

        /**
         * @brief Process and triggers IRQ if all conditions met
         * @return true if IRQ is triggered, false otherwise
         */
        virtual bool cpu_process_IRQ() = 0;

        /**
         * @brief callback for IRQ simple socket
         * @param trans transaction to perform (empty)
         * @param delay time to annotate
         *
         * it triggers an IRQ when called
         */
        virtual void call_interrupt(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) = 0;

        virtual std::uint64_t getStartDumpAddress() = 0;
        virtual std::uint64_t getEndDumpAddress() = 0;

    public:
        MemoryInterface *mem_intf;
    protected:
        Performance *perf;
        std::shared_ptr<spdlog::logger> logger;
        tlm_utils::tlm_quantumkeeper *m_qk;
        Instruction inst;
        bool interrupt;
        bool irq_already_down;
        sc_core::sc_time default_time;
        bool dmi_ptr_valid;
        tlm::tlm_generic_payload trans;
        unsigned char *dmi_ptr = nullptr;
    };

    /**
     * @brief RISC_V CPU 32 bits model
     * @param name name of the module
     */
    class RV32 : public CPU {
    public:
        using BaseType = std::uint32_t;

        /**
         * @brief Constructor
         * @param name Module name
         * @param PC   Program Counter initialize value
         * @param debug To start debugging
         */
        RV32(sc_core::sc_module_name const &name, BaseType PC, bool debug);

        /**
         * @brief Destructor
         */
        ~RV32() override;

        bool CPU_step() override;
        Registers<BaseType> *getRegisterBank() { return register_bank; }

    private:
        Registers<BaseType> *register_bank;
        C_extension<BaseType> *c_inst;
        M_extension<BaseType> *m_inst;
        A_extension<BaseType> *a_inst;
        BASE_ISA<BaseType> *exec;
        BaseType int_cause;
        BaseType INSTR;

        /**
         *
         * @brief Process and triggers IRQ if all conditions met
         * @return true if IRQ is triggered, false otherwise
         */
        bool cpu_process_IRQ() override;

        /**
         * @brief callback for IRQ simple socket
         * @param trans transaction to perform (empty)
         * @param delay time to annotate
         *
         * it triggers an IRQ when called
         */
        void call_interrupt(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) override;

        std::uint64_t getStartDumpAddress() override;
        std::uint64_t getEndDumpAddress() override;
    }; // RV32 class

    /**
     * @brief RISC_V CPU 64 bits model
     * @param name name of the module
     */
    class RV64 : public CPU {
    public:
        using BaseType = std::uint64_t;

        /**
         * @brief Constructor
         * @param name Module name
         * @param PC   Program Counter initialize value
         * @param debug To start debugging
         */
        RV64(sc_core::sc_module_name const &name, BaseType PC, bool debug);

        /**
         * @brief Destructor
         */
        ~RV64() override;

        bool CPU_step() override;
        Registers<BaseType> *getRegisterBank() { return register_bank; }

    private:
        Registers<BaseType> *register_bank;
        C_extension<BaseType> *c_inst;
        M_extension<BaseType> *m_inst;
        A_extension<BaseType> *a_inst;
        BASE_ISA<BaseType> *exec;
        BaseType int_cause;
        BaseType INSTR;

        /**
         *
         * @brief Process and triggers IRQ if all conditions met
         * @return true if IRQ is triggered, false otherwise
         */
        bool cpu_process_IRQ() override;

        /**
         * @brief callback for IRQ simple socket
         * @param trans transaction to perform (empty)
         * @param delay time to annotate
         *
         * it triggers an IRQ when called
         */
        void call_interrupt(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) override;

        std::uint64_t getStartDumpAddress() override;
        std::uint64_t getEndDumpAddress() override;
    }; // RV64 class

}
#endif
