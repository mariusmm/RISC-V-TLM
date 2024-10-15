/**
 @file BusCtrl.cpp
 @brief Basic TLM-2 Bus controller
 @author Màrius Montón
 @date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BusCtrl.h"

namespace riscv_tlm {

    BusCtrl::BusCtrl(sc_core::sc_module_name const &name) :
            sc_module(name), cpu_instr_socket("cpu_instr_socket"), cpu_data_socket(
            "cpu_data_socket") {
        cpu_instr_socket.register_b_transport(this, &BusCtrl::b_transport);
        cpu_data_socket.register_b_transport(this, &BusCtrl::b_transport);

        cpu_instr_socket.register_get_direct_mem_ptr(this,
                                                     &BusCtrl::instr_direct_mem_ptr);
    }

    void BusCtrl::b_transport(tlm::tlm_generic_payload &trans,
                              sc_core::sc_time &delay) {

        sc_dt::uint64 addr = trans.get_address();

        if (addr >= TO_HOST_ADDRESS) {
            std::cout << "To host\n" << std::flush;
            sc_core::sc_stop();
            return;
        }

        auto portId = decode(addr);
        peripherals_sockets[portId]->b_transport(trans, delay);

        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    bool BusCtrl::instr_direct_mem_ptr(tlm::tlm_generic_payload &gp,
                                       tlm::tlm_dmi &dmi_data) {
        sc_dt::uint64 addr = gp.get_address();
        auto portId = decode(addr);
        auto decodeSocket = &peripherals_sockets[portId];

        return (*decodeSocket)->get_direct_mem_ptr(gp, dmi_data);

    }

    void BusCtrl::invalidate_direct_mem_ptr(sc_dt::uint64 start,
                                            sc_dt::uint64 end) {
        cpu_instr_socket->invalidate_direct_mem_ptr(start, end);
    }


    unsigned int BusCtrl::decode(const sc_dt::uint64 address) {

        // Change to peripherals_map.contains() when C++20 available
        auto it = peripherals_map.find(address_range(address, address));
        if (it != peripherals_map.end()) {
            return it->second;
        }

        return 0;
    }

    unsigned int BusCtrl::encode(const std::string name, const sc_dt::uint64 st_address, const sc_dt::uint64 end_address) {

        auto to_insert = address_range(st_address, end_address);
        // Change to peripherals_map.contains() when C++20 available
        auto it = peripherals_map.find(to_insert);
        if (it == peripherals_map.end()) {
            peripherals_count++;
            peripherals_map.insert({to_insert, peripherals_count});
            std::cout << "New peripheral (" << name <<"): Address 0x" << std::hex << to_insert.s << ", 0x" << to_insert.e << " to port #" << std::dec << peripherals_count << std::endl;
            return peripherals_count;
        }

        return 0;
    }

    unsigned int BusCtrl::register_peripheral(const std::string name, sc_dt::uint64 start, sc_dt::uint64 end) {
        return encode(name, start, end);
    }
}