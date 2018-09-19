/*!
   \file Trace.h
   \brief Basic TLM-2 Trace module
   \author Màrius Montón
   \date September 2018
*/

#ifndef __BUSCTRL_H__
#define __BUSCTRL_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;


#define TRACE_MEMORY_ADDRESS 0x40000000

class BusCtrl: sc_module {
public:
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<BusCtrl> cpu_instr_socket;
    tlm_utils::simple_target_socket<BusCtrl> cpu_data_socket;
    tlm_utils::simple_initiator_socket<BusCtrl> data_memory_socket;
    tlm_utils::simple_initiator_socket<BusCtrl> trace_socket;


    // Constructor
    BusCtrl(sc_module_name name);

    // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

private:
      Log *log;
};

#endif
