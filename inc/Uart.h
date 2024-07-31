/*!
 \file Uart.h
 \brief Basic TLM-2 UART module
 \author Màrius Montón
 \date May 2024
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __UART_H__
#define __UART_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#include "BusCtrl.h"

namespace riscv_tlm::peripherals {
/**
 * @brief Simple timer peripheral
 *
 * It runs a 1 ns (nanoseconds) pace
 *
 */
    class Uart : sc_core::sc_module {
    public:
        // TLM-2 socket, defaults to 32-bits wide, base protocol
        tlm_utils::simple_target_socket<Uart> socket;

        //tlm_utils::simple_initiator_socket<Uart> irq_line;

        /**
         *
         * @brief Constructor
         * @param name module name
         */
        explicit Uart(sc_core::sc_module_name const &name);

        /**
         * @brief Waits for event timer_event and triggers an IRQ
         *
         * Waits for event timer_event and triggers an IRQ (if it is not already
         * triggered).
         * After that, it posts the timer_event to 20 ns in the future to clear the IRQ
         * line.
         *
         */
       // [[noreturn]] void run();

        /**
         *
         * @brief TLM-2.0 socket implementation
         * @param trans TLM-2.0 transaction
         * @param delay transaction delay time
         */
        virtual void b_transport(tlm::tlm_generic_payload &trans,
                                 sc_core::sc_time &delay);

    private:
        sc_dt::sc_uint<32> UartRxReg; /**< mtime register */
        sc_dt::sc_uint<32> UartTxReg; /**< mtimecmp register */
        sc_dt::sc_uint<32> UartStatusReg;

        sc_core::sc_event uart_event; /**< event */
    };
}
#endif
