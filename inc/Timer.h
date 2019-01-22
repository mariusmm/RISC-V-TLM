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

    tlm_utils::simple_initiator_socket<Timer> irq_line;
    //sc_out<bool> timer_irq;

    /**
     *
     * @brief Constructor
     * @param name module name
     */
    Timer(sc_module_name name);

    /**
     * @brief Waits for event timer_event and triggers an IRQ
     *
     * Waits for event timer_event and triggers an IRQ (if it is not already
     * triggered).
     * After that, it posts the timer_event to 20 ns in the future to clear the IRQ
     * line.
     *
     */
    void run();

    /**
     *
     * @brief TLM-2.0 socket implementaiton
     * @param trans TLM-2.0 transaction
     * @param delay transactino delay time
     */
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

private:
  sc_uint<64> m_mtime;        /**< mtime register */
  sc_uint<64> m_mtimecmp;     /**< mtimecmp register */
  sc_event timer_event;       /**< event */
};

#endif
