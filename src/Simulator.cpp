/*!
   \file Simulator.cpp
   \brief Top level simulation entity
   \author Màrius Montón
   \date September 2018
*/

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include <signal.h>

#include "CPU.h"
#include "Memory.h"
#include "BusCtrl.h"
#include "Trace.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

string filename;

/**
 * @class Simulator
 * This class instiates all necessary modules, connects its ports and starts
 * the simulation.
 *
 * @brief Top simulation entity
 */
SC_MODULE(Simulator)
{
  CPU    *cpu;
  Memory *MainMemory;
  BusCtrl* Bus;
  Trace *trace;

  uint32_t start_PC;
  sc_signal<bool> IRQ;

  SC_CTOR(Simulator)
  {
    MainMemory = new Memory("Main_Memory", filename);
    start_PC = MainMemory->getPCfromHEX();

    cpu    = new CPU("cpu", start_PC);

    Bus = new BusCtrl("BusCtrl");
    trace = new Trace("Trace");

    cpu->instr_bus.bind(Bus->cpu_instr_socket);
    cpu->exec->data_bus.bind(Bus->cpu_data_socket);

    Bus->memory_socket.bind(MainMemory->socket);
    Bus->trace_socket.bind(trace->socket);
    //cpu->interrupt.bind(IRQ);
  }

  ~Simulator() {
    cout << "Simulator destructor" << endl;
    delete cpu;
    delete MainMemory;
    delete Bus;
    delete trace;
  }
};

Simulator *top;

void intHandler(int dummy) {
  delete top;
  //sc_stop();
  exit(-1);
}

int sc_main(int argc, char* argv[])
{

  signal(SIGINT, intHandler);

  if (argv[1] == nullptr) {
    cerr << "Filename needed for hex memory" << endl;
    return -1;
  }
  filename = argv[1];

  top = new Simulator("top");
  sc_start();
  return 0;
}
