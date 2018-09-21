/**
   @file BusCtrl.h
   @brief Basic TLM-2 Bus controller
   @author Màrius Montón
   @date September 2018
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

/**
 * Memory mapped Trace peripheral address
 */
#define TRACE_MEMORY_ADDRESS 0x40000000
/**
 * @brief Simple bus controller
 *
 * This module manages instructon & data bus. It has 2 target ports,
 * cpu_instr_socket and cpu_data_socket that receives accesses from CPU and
 * has 2 initiator ports to access main Memory and Trace module.
 * It will be expanded with more ports when required (for DMA,
 * other peripherals, etc.)
 */
class BusCtrl: sc_module {
public:
    /**
     * @brief TLM target socket CPU instruction memory bus
     */
    tlm_utils::simple_target_socket<BusCtrl> cpu_instr_socket;

    /**
     * @brief TLM target socket CPU data memory bus
     */
    tlm_utils::simple_target_socket<BusCtrl> cpu_data_socket;

    /**
     * @brief TLM initiator socket Main memory bus
     */
    tlm_utils::simple_initiator_socket<BusCtrl> memory_socket;

    /**
     * @brief TLM initiator socket Trace module
     */
    tlm_utils::simple_initiator_socket<BusCtrl> trace_socket;


    /**
     * @brief constructor
     * @param name module's name
     */

    BusCtrl(sc_module_name name);

    /**
     * @brief TLM-2 blocking mechanism
     * @param trans transtractino to perform
     * @param delay delay associated to this transaction
     */
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

private:
      Log *log;
};

#endif
