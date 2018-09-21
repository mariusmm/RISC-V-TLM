/*!
   \file CPU.h
   \brief Main CPU class
   \author Màrius Montón
   \date August 2018
*/
#ifndef CPU_BASE_H
#define CPU_BASE_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Instruction.h"
#include "Execute.h"
#include "Registers.h"
#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief ISC_V CPU model
 * @param name name of the module
 */
class CPU: sc_module {
public:

  tlm_utils::simple_initiator_socket<CPU> instr_bus;

  //tlm_utils::simple_initiator_socket<cpu_base> data_bus;

  //sc_in<sc_signal<bool> > interrupt;

  CPU(sc_module_name name, uint32_t PC);
  ~CPU();

  Execute *exec;

private:
  Registers *register_bank;
  Performance *perf;
  Log *log;

  void CPU_thread(void);
};

#endif
