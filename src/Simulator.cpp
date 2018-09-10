#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include <signal.h>

#include "CPU.h"
#include "Memory.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

string filename;

SC_MODULE(Top)
{
  //Initiator *initiator;
  CPU    *cpu;
  Memory    *memory;

  sc_signal<bool> IRQ;

  SC_CTOR(Top)
  {
    cpu    = new CPU("cpu");
    memory = new Memory("memory", filename);

    cpu->instr_bus.bind(memory->socket);
    //cpu->interrupt.bind(IRQ);
  }

  ~Top() {
    cout << "Top destructor" << endl;
    delete cpu;
    delete memory;
  }
};

Top *top;

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

  top = new Top("top");
  sc_start();
  return 0;
}
