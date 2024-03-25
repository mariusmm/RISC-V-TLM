/*!
 \file Timer.cpp
 \brief Basic TLM-2 Timer module
 \author Màrius Montón
 \date January 2019
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Timer.h"
#include <cstdint>

namespace riscv_tlm::peripherals {

    SC_HAS_PROCESS(Timer);

    Timer::Timer(sc_core::sc_module_name const &name) :
            sc_module(name), socket("timer_socket"), m_mtime(0), m_mtimecmp(0) {

        socket.register_b_transport(this, &Timer::b_transport);

        SC_THREAD(run);
    }

    [[noreturn]] void Timer::run() {

        auto *irq_trans = new tlm::tlm_generic_payload;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        std::uint32_t cause = 1 << 31 | 0x07;     // Machine timer interrupt
        irq_trans->set_command(tlm::TLM_WRITE_COMMAND);
        irq_trans->set_data_ptr(reinterpret_cast<unsigned char *>(&cause));
        irq_trans->set_data_length(4);
        irq_trans->set_streaming_width(4);
        irq_trans->set_byte_enable_ptr(nullptr);
        irq_trans->set_dmi_allowed(false);
        irq_trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        irq_trans->set_address(0);

        while (true) {
            wait(timer_event);
            irq_line->b_transport(*irq_trans, delay);
        }
    }

    void Timer::b_transport(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) {

        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64 addr = trans.get_address();
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        delay = sc_core::SC_ZERO_TIME;

        std::uint32_t aux_value = 0;


        if (cmd == tlm::TLM_WRITE_COMMAND) {
            memcpy(&aux_value, ptr, len);
            switch (addr) {
                case TIMER_MEMORY_ADDRESS_LO:
                    m_mtime.range(31, 0) = aux_value;
                    break;
                case TIMER_MEMORY_ADDRESS_HI:
                    m_mtime.range(63, 32) = aux_value;
                    break;
                case TIMERCMP_MEMORY_ADDRESS_LO:
                    m_mtimecmp.range(31, 0) = aux_value;
                    break;
                case TIMERCMP_MEMORY_ADDRESS_HI:
                    m_mtimecmp.range(63, 32) = aux_value;

                    std::uint64_t notify_time;
                    // notify needs relative time, mtimecmp works in absolute time
                    notify_time = m_mtimecmp - m_mtime;

                    //timer_event.notify(sc_core::sc_time(notify_time, sc_core::SC_NS));
                    timer_event.notify(sc_core::sc_time::from_value(notify_time));
                    break;
                default:
                    trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
                    return;
            }
        } else { // TLM_READ_COMMAND
            switch (addr) {
                case TIMER_MEMORY_ADDRESS_LO:
                    m_mtime = sc_core::sc_time_stamp().value();
                    aux_value = m_mtime.range(31, 0);
                    break;
                case TIMER_MEMORY_ADDRESS_HI:
                    aux_value = m_mtime.range(63, 32);
                    break;
                case TIMERCMP_MEMORY_ADDRESS_LO:
                    aux_value = m_mtimecmp.range(31, 0);
                    break;
                case TIMERCMP_MEMORY_ADDRESS_HI:
                    aux_value = m_mtimecmp.range(63, 32);
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