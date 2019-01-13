#include "BusCtrl.h"

SC_HAS_PROCESS(BusCtrl);
BusCtrl::BusCtrl(sc_module_name name): sc_module(name)
  ,cpu_instr_socket("cpu_instr_socket")
  ,cpu_data_socket("cpu_data_socket")
  ,memory_socket("memory_socket")
  ,trace_socket("trace_socket")
   {
    cpu_instr_socket.register_b_transport(this, &BusCtrl::b_transport);
    cpu_data_socket.register_b_transport(this, &BusCtrl::b_transport);
    log = Log::getInstance();
  }


void BusCtrl::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {
    //tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address() / 4;

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
      default:
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

    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}
