/*!
   \file RISC_V_execute.h
   \brief RISC-V ISA implementation
   \author Màrius Montón
   \date August 2018
*/
#ifndef RISC_V_EXECUTE_H
#define RISC_V_EXECUTE_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Instruction.h"
#include "Registers.h"
#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Risc_V execute module
 */
class RISC_V_execute : sc_module {
public:

  /**
   * @brief Constructor
   * @param name          module name
   * @param register_bank pointer to register bank to use
   */
  RISC_V_execute(sc_module_name name,
    Registers *register_bank);

  /* Quick & dirty way to publish a socket though modules */
  tlm_utils::simple_initiator_socket<RISC_V_execute> data_bus;

  void LUI(Instruction &inst);
  void AUIPC(Instruction &inst);

  void JAL(Instruction &inst);
  void JALR(Instruction &inst);

  void BEQ(Instruction &inst);
  void BNE(Instruction &inst);
  void BLT(Instruction &inst);
  void BGE(Instruction &inst);
  void BLTU(Instruction &inst);
  void BGEU(Instruction &inst);

  void LB(Instruction &inst);
  void LH(Instruction &inst);
  void LW(Instruction &inst);
  void LBU(Instruction &inst);
  void LHU(Instruction &inst);

  void SB(Instruction &inst);
  void SH(Instruction &inst);
  void SW(Instruction &inst);
  void SBU(Instruction &inst);
  void SHU(Instruction &inst);

  void ADDI(Instruction &inst);
  void SLTI(Instruction &inst);
  void SLTIU(Instruction &inst);
  void XORI(Instruction &inst);
  void ORI(Instruction &inst);
  void ANDI(Instruction &inst);
  void SLLI(Instruction &inst);
  void SRLI(Instruction &inst);
  void SRAI(Instruction &inst);

  void ADD(Instruction &inst);
  void SUB(Instruction &inst);
  void SLL(Instruction &inst);
  void SLT(Instruction &inst);
  void SLTU(Instruction &inst);

  void XOR(Instruction &inst);
  void SRL(Instruction &inst);
  void SRA(Instruction &inst);
  void OR(Instruction &inst);
  void AND(Instruction &inst);

  void CSRRW(Instruction &inst);
  void CSRRS(Instruction &inst);
  void CSRRC(Instruction &inst);
  void CSRRWI(Instruction &inst);
  void CSRRSI(Instruction &inst);
  void CSRRCI(Instruction &inst);

  void NOP(Instruction &inst);

private:
  uint32_t readDataMem(uint32_t addr, int size);
  void writeDataMem(uint32_t addr, uint32_t data, int size);
  Registers *regs;
  Performance *perf;
  Log *log;
};


#endif
