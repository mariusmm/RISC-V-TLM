/**
 @file BusCtrl.hpp
 @brief Basic TLM-2 Bus controller
 @author Màrius Montón
 @date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __BUSCTRL_H__
#define __BUSCTRL_H__

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

namespace riscv_tlm {

/**
 * Memory mapped Trace peripheral address
 */
#define TRACE_MEMORY_ADDRESS 0x40000000

#define TIMER_MEMORY_ADDRESS_LO 0x40004000
#define TIMER_MEMORY_ADDRESS_HI 0x40004004
#define TIMERCMP_MEMORY_ADDRESS_LO 0x40004008
#define TIMERCMP_MEMORY_ADDRESS_HI 0x4000400C

#define TO_HOST_ADDRESS 0x90000000

/**
 * @brief Simple bus controller
 *
 * This module manages instructon & data bus. It has 2 target ports,
 * cpu_instr_socket and cpu_data_socket that receives accesses from CPU and
 * has 2 initiator ports to access main Memory and Trace module.
 * It will be expanded with more ports when required (for DMA,
 * other peripherals, etc.)
 */
    class BusCtrl : sc_core::sc_module {
    public:
        /**
         * @brief TLM target socket CPU instruction memory bus
         */
        tlm_utils::simple_target_socket<BusCtrl> cpu_instr_socket;

        /**
         * @brief TLM target socket CPU data memory bus
         */
        tlm_utils::simple_target_socket<BusCtrl> cpu_data_socket;

        /**
         * @brief TLM initiator socket Main memory bus
         */
        tlm_utils::simple_initiator_socket<BusCtrl> memory_socket;

        /**
         * @brief TLM initiator socket Trace module
         */
        tlm_utils::simple_initiator_socket<BusCtrl> trace_socket;

        /**
         * @brief TLM initiator socket Trace module
         */
        tlm_utils::simple_initiator_socket<BusCtrl> timer_socket;

        /**
         * @brief constructor
         * @param name module's name
         */
        explicit BusCtrl(sc_core::sc_module_name const &name);

        /**
         * @brief TLM-2 blocking mechanism
         * @param trans transtractino to perform
         * @param delay delay associated to this transaction
         */
        virtual void b_transport(tlm::tlm_generic_payload &trans,
                                 sc_core::sc_time &delay);

    private:
        bool instr_direct_mem_ptr(tlm::tlm_generic_payload &,
                                  tlm::tlm_dmi &dmi_data);

        void invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end);
    };
}
#endif
