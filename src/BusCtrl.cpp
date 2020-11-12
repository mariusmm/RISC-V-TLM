/**
 @file BusCtrl.cpp
 @brief Basic TLM-2 Bus controller
 @author Màrius Montón
 @date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later
#include "BusCtrl.h"

SC_HAS_PROCESS(BusCtrl);
BusCtrl::BusCtrl(sc_core::sc_module_name name) :
		sc_module(name), memory_socket("memory_socket"), trace_socket(
				"trace_socket") {

#if 1
	for (std::size_t i { 0 }; i < cpu_instr_socket.size(); i++) {
		cpu_instr_socket[i].register_b_transport(this, &BusCtrl::b_transport,
				i);
		cpu_data_socket[i].register_b_transport(this, &BusCtrl::b_transport, i);
	}
#else
for (auto iter: cpu_instr_socket) {
               iter.register_b_transport(this, &BusCtrl::b_transport, 0);
       }
#endif

	cpu_instr_socket[0].register_get_direct_mem_ptr(this,
			&BusCtrl::instr_direct_mem_ptr, 0);

	log = Log::getInstance();
	memory_socket.register_invalidate_direct_mem_ptr(this,
			&BusCtrl::invalidate_direct_mem_ptr);
}

void BusCtrl::b_transport(int id, tlm::tlm_generic_payload &trans,
		sc_core::sc_time &delay) {
	//tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 adr = trans.get_address() / 4;

	switch (adr) {
	case TIMER_MEMORY_ADDRESS_HI / 4:
	case TIMER_MEMORY_ADDRESS_LO / 4:
	case TIMERCMP_MEMORY_ADDRESS_HI / 4:
	case TIMERCMP_MEMORY_ADDRESS_LO / 4:
		timer_socket->b_transport(trans, delay);
		break;
	case TRACE_MEMORY_ADDRESS / 4:
		trace_socket->b_transport(trans, delay);
		break;
	[[likely]] default:
		memory_socket->b_transport(trans, delay);
		break;
	}

#if 0
    if (cmd == tlm::TLM_READ_COMMAND) {
      log->SC_log(Log::DEBUG) << "RD Address: @0x" << hex << adr << dec << endl;
    } else {
      log->SC_log(Log::DEBUG) << "WR Address: @0x" << hex << adr << dec << endl;
    }
#endif

	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

bool BusCtrl::instr_direct_mem_ptr(int id, tlm::tlm_generic_payload &gp,
		tlm::tlm_dmi &dmi_data) {
	return memory_socket->get_direct_mem_ptr(gp, dmi_data);
}

void BusCtrl::invalidate_direct_mem_ptr(sc_dt::uint64 start,
		sc_dt::uint64 end) {
	for (int i = 0; i < NUM_PORTS; i++) {
		cpu_instr_socket[i]->invalidate_direct_mem_ptr(start, end);
	}

}

