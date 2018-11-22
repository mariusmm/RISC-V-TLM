/*!
   \file Execute.h
   \brief RISC-V ISA implementation
   \author Màrius Montón
   \date August 2018
*/
#ifndef Execute_H
#define Execute_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Instruction.h"
#include "C_Instruction.h"
#include "M_Instruction.h"
#include "Registers.h"
#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Risc_V execute module
 */
class Execute : sc_module {
public:

  /**
   * @brief Constructor
   * @param name          module name
   * @param register_bank pointer to register bank to use
   */
  Execute(sc_module_name name,
    Registers *register_bank);

  /* Quick & dirty way to publish a socket though modules */
  tlm_utils::simple_initiator_socket<Execute> data_bus;

  void LUI(Instruction &inst);
  void AUIPC(Instruction &inst);

  void JAL(Instruction &inst, bool c_extension = false, int m_rd = 1);
  void JALR(Instruction &inst, bool c_extension = false);

  void BEQ(Instruction &inst);
  void BNE(Instruction &inst);
  void BLT(Instruction &inst);
  void BGE(Instruction &inst);
  void BLTU(Instruction &inst);
  void BGEU(Instruction &inst);

  void LB(Instruction &inst);
  void LH(Instruction &inst);
  void LW(Instruction &inst, bool c_extension = false);
  void LBU(Instruction &inst);
  void LHU(Instruction &inst);

  void SB(Instruction &inst);
  void SH(Instruction &inst);
  void SW(Instruction &inst, bool c_extension = false);
  void SBU(Instruction &inst);
  void SHU(Instruction &inst);

  void ADDI(Instruction &inst, bool c_extension = false);
  void SLTI(Instruction &inst);
  void SLTIU(Instruction &inst);
  void XORI(Instruction &inst);
  void ORI(Instruction &inst);
  void ANDI(Instruction &inst);
  bool SLLI(Instruction &inst);
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

  void FENCE(Instruction &inst);
  void ECALL(Instruction &inst);

  void CSRRW(Instruction &inst);
  void CSRRS(Instruction &inst);
  void CSRRC(Instruction &inst);
  void CSRRWI(Instruction &inst);
  void CSRRSI(Instruction &inst);
  void CSRRCI(Instruction &inst);

/*********************** Privileged Instructions ******************************/
  void MRET(Instruction &inst);
  void WFI(Instruction &inst);

  /* C Extensions */
  void C_JR(Instruction &inst);
  void C_MV(Instruction &inst);
  void C_LWSP(Instruction &inst);
  void C_ADDI4SPN(Instruction &inst);
  void C_SLLI(Instruction &inst);
  void C_ADDI16SP(Instruction &inst);
  void C_SWSP(Instruction &inst);
  void C_BEQZ(Instruction &inst);
  void C_BNEZ(Instruction &inst);
  void C_LI(Instruction &inst);
  void C_SRLI(Instruction &inst);
  void C_SRAI(Instruction &inst);
  void C_ANDI(Instruction &inst);
  void C_ADD(Instruction &inst);
  void C_SUB(Instruction &inst);
  void C_XOR(Instruction &inst);
  void C_OR(Instruction &inst);
  void C_AND(Instruction &inst);

  /* M Extensinos */
  void M_MUL(Instruction &inst);
  void M_MULH(Instruction &inst);
  void M_MULHSU(Instruction &inst);
  void M_MULHU(Instruction &inst);
  void M_DIV(Instruction &inst);
  void M_DIVU(Instruction &inst);
  void M_REM(Instruction &inst);
  void M_REMU(Instruction &inst);

  void NOP(Instruction &inst);

private:
  uint32_t readDataMem(uint32_t addr, int size);
  void writeDataMem(uint32_t addr, uint32_t data, int size);

  void RaiseException(uint32_t cause);

  Registers *regs;
  Performance *perf;
  Log *log;
};


#endif
