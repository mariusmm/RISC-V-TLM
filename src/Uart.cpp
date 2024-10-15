/*!
 \file Uart.cpp
 \brief Basic TLM-2 UART module
 \author Màrius Montón
 \date May 2024

Based on https://github.com/lowRISC/ibex-demo-system/blob/main/rtl/system/uart.sv model

 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Uart.h"
#include <cstdint>

namespace riscv_tlm::peripherals {

    Uart::Uart(sc_core::sc_module_name const &name) :
            sc_module(name), socket("uart_socket"), UartRxReg(0), UartTxReg(0), UartStatusReg(0) {

        socket.register_b_transport(this, &Uart::b_transport);

        //SC_THREAD(run);
    }
void Uart::b_transport(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) {

        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64 addr = trans.get_address() - IBEX_UART_MEMORY_ADDRESS;
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        delay = sc_core::SC_ZERO_TIME;

        std::uint32_t aux_value = 0;
        std::uint64_t notify_time = 0;

        if (cmd == tlm::TLM_WRITE_COMMAND) {
            memcpy(&aux_value, ptr, len);
            switch (addr) {
                case IBEX_UART_RX_REG:
                    break;
                case IBEX_UART_TX_REG:
                    std::cout << aux_value << std::flush;
                    break;
                case IBEX_UART_STATUS_REG:
                    uart_event.notify(sc_core::sc_time(notify_time, sc_core::SC_NS));
                    break;                            
                
                default:
                    trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
                    return;
            }
        } else { // TLM_READ_COMMAND
            switch (addr) {
                case IBEX_UART_RX_REG:
                    aux_value = 0;
                    break;
                case IBEX_UART_TX_REG:
                    aux_value = 0;
                    break;
                case IBEX_UART_STATUS_REG:
                    aux_value = UartStatusReg;
                    break;
                default:
                    trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
                    return;
            }
            memcpy(ptr, &aux_value, len);
        }

        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }
}
