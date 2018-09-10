/*!
   \file Registers.h
   \brief Basic register file implementation
   \author Màrius Montón
   \date August 2018
*/
#ifndef REGISTERS_H
#define REGISTERS_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include "tlm.h"

#include "Performance.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Register file implementation
 */
class Registers {
public:

  /**
   * Default constructor
   */
  Registers();

  /**
   * Set value for a register
   * @param reg_num register number
   * @param value   register value
   */
  void setValue(int reg_num, int32_t value);

  /**
   * Returns register value
   * @param  reg_num register number
   * @return         register value
   */
  int32_t getValue(int reg_num);

  /**
   * Returns PC value
   * @return PC value
   */
  uint32_t getPC();

  /**
   * Sets arbitraty value to PC
   * @param new_pc new address to PC
   */
  void setPC(uint32_t new_pc);

  /**
   * Increments PC couunter to next address
   */
  inline void incPC() {
    register_PC += 4;
  }

  /**
   * @brief Get CSR value
   * @param csr CSR number to access
   * @return CSR value
   */
  inline uint32_t getCSR(int csr) {
    return CSR[csr];
  }

  /**
   * @brief Set CSR value
   * @param csr   CSR number to access
   * @param value new value to register
   */
  inline void setCSR(int csr, uint32_t value) {
    CSR[csr] = value;
  }

  /**
   * Dump register data to console
   */
  void dump();
private:
  /**
   * bank of registers (32 regs of 32bits each)
   */
  int32_t register_bank[32];

  /**
   * Program counter (32 bits width)
   */
  uint32_t register_PC;

  /**
   * CSR registers (4096 maximum)
   */
  uint32_t CSR[4096];
  Performance *perf;
};

#endif
