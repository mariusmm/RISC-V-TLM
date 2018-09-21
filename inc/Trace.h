/*!
   \file Trace.h
   \brief Basic TLM-2 Trace module
   \author Màrius Montón
   \date September 2018
*/

#ifndef __TRACE_H__
#define __TRACE_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;
/**
 * @brief Simple trace peripheral
 *
 * This peripheral outputs to cout any character written to its unique register
 */
class Trace: sc_module {
public:
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Trace> socket;

    // Constructor
    Trace(sc_module_name name);

    // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );
};

#endif
