#include "Performance.h"

Performance* Performance::getInstance()
{
    if (instance == 0)
    {
        instance = new Performance();
    }

    return instance;
}

Performance::Performance()
{}

void Performance::dump() {
  cout << dec << "# data memory reads: "  << data_memory_read << endl;
  cout << "# data memory writes: " << data_memory_write << endl;
  cout << "# code memory reads: "  << code_memory_read << endl;
  cout << "# code memory writes: " << code_memory_write << endl;
  cout << "# registers read: "     << register_read << endl;
  cout << "# registers write: "    << register_write << endl;
  cout << "# instructions executed: " << instructions_executed <<  endl;
}

Performance* Performance::instance = 0;
