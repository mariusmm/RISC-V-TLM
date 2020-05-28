/*!
   \file Memory.h
   \brief Basic TLM-2 memory model
   \author Màrius Montón
   \date August 2018
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Basic TLM-2 memory
 */
class Memory: sc_module {
public:
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<Memory> socket;

  //enum { SIZE = 0x90000000  };
  enum { SIZE = 0x10000000  };
  const sc_time LATENCY;

  Memory(sc_module_name name, string filename);
  Memory(sc_module_name name, bool use_file);

  ~Memory(void);
  
  /**
   * @brief Returns Program Counter read from hexfile
   * @return Initial PC
   */
  virtual uint32_t getPCfromHEX();

  // TLM-2 blocking transport method
  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

  // *********************************************
  // TLM-2 forward DMI method
  // *********************************************
  virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                  tlm::tlm_dmi& dmi_data);

  // *********************************************
  // TLM-2 debug transport method
  // *********************************************
  virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);

private:

  /**
   * @brief Memory array in bytes
   */
  //uint8_t mem[SIZE];
  uint8_t *mem;

  /**
   * @brief Log classe
   */
  Log *log;
  /**
  * @brief Program counter (PC) read from hex file
  */
  uint32_t program_counter;

  uint32_t memory_offset;
  /**
   * @brief Read Intel hex file
   * @param filename file name to read
   */
  virtual void readHexFile(string filename);
};
 #endif /* __MEMORY_H__ */
