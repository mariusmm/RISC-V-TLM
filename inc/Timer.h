/*!
   \file Timer.h
   \brief Basic TLM-2 Timer module
   \author Màrius Montón
   \date January 2019
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#include "BusCtrl.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Simple timer peripheral
 *
 * It runs a 1 ns (nanoseconds) pace
 *
 */
class Timer: sc_module {
public:
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Timer> socket;
    sc_out<bool> timer_irq;

    // Constructor
    Timer(sc_module_name name);

    void run();

    // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

private:
  sc_uint<64> m_mtime;
  sc_uint<64> m_mtimecmp;
  sc_event timer_event;
  bool irq_value;
};

#endif
