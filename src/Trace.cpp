#include "Trace.h"

SC_HAS_PROCESS(Trace);
Trace::Trace(sc_module_name name): sc_module(name)
  ,socket("socket") {
    socket.register_b_transport(this, &Trace::b_transport);
  }


void Trace::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {
    //tlm::tlm_command cmd = trans.get_command();
    //sc_dt::uint64    adr = trans.get_address() / 4;
    unsigned char*   ptr = trans.get_data_ptr();
    //unsigned int     len = trans.get_data_length();
    //unsigned char*   byt = trans.get_byte_enable_ptr();
    //unsigned int     wid = trans.get_streaming_width();

    cout << (char) *ptr << flush;

    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}
