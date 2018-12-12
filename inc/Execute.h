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
#include <set>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Instruction.h"
#include "C_Instruction.h"
#include "M_Instruction.h"
#include "A_Instruction.h"
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

  bool LUI(Instruction &inst);
  bool AUIPC(Instruction &inst);

  bool JAL(Instruction &inst, bool c_extension = false, int m_rd = 1);
  bool JALR(Instruction &inst, bool c_extension = false);

  bool BEQ(Instruction &inst);
  bool BNE(Instruction &inst);
  bool BLT(Instruction &inst);
  bool BGE(Instruction &inst);
  bool BLTU(Instruction &inst);
  bool BGEU(Instruction &inst);

  bool LB(Instruction &inst);
  bool LH(Instruction &inst);
  bool LW(Instruction &inst, bool c_extension = false);
  bool LBU(Instruction &inst);
  bool LHU(Instruction &inst);

  bool SB(Instruction &inst);
  bool SH(Instruction &inst);
  bool SW(Instruction &inst, bool c_extension = false);
  bool SBU(Instruction &inst);
  bool SHU(Instruction &inst);

  bool ADDI(Instruction &inst, bool c_extension = false);
  bool SLTI(Instruction &inst);
  bool SLTIU(Instruction &inst);
  bool XORI(Instruction &inst);
  bool ORI(Instruction &inst);
  bool ANDI(Instruction &inst);
  bool SLLI(Instruction &inst);
  bool SRLI(Instruction &inst);
  bool SRAI(Instruction &inst);

  bool ADD(Instruction &inst);
  bool SUB(Instruction &inst);
  bool SLL(Instruction &inst);
  bool SLT(Instruction &inst);
  bool SLTU(Instruction &inst);

  bool XOR(Instruction &inst);
  bool SRL(Instruction &inst);
  bool SRA(Instruction &inst);
  bool OR(Instruction &inst);
  bool AND(Instruction &inst);

  bool FENCE(Instruction &inst);
  bool ECALL(Instruction &inst);
  bool EBREAK(Instruction &inst);

  bool CSRRW(Instruction &inst);
  bool CSRRS(Instruction &inst);
  bool CSRRC(Instruction &inst);
  bool CSRRWI(Instruction &inst);
  bool CSRRSI(Instruction &inst);
  bool CSRRCI(Instruction &inst);

/*********************** Privileged Instructions ******************************/
  bool MRET(Instruction &inst);
  bool SRET(Instruction &inst);
  bool WFI(Instruction &inst);
  bool SFENCE(Instruction &inst);
  
  /* C Extensions */
  bool C_JR(Instruction &inst);
  bool C_MV(Instruction &inst);
  bool C_LWSP(Instruction &inst);
  bool C_ADDI4SPN(Instruction &inst);
  bool C_SLLI(Instruction &inst);
  bool C_ADDI16SP(Instruction &inst);
  bool C_SWSP(Instruction &inst);
  bool C_BEQZ(Instruction &inst);
  bool C_BNEZ(Instruction &inst);
  bool C_LI(Instruction &inst);
  bool C_SRLI(Instruction &inst);
  bool C_SRAI(Instruction &inst);
  bool C_ANDI(Instruction &inst);
  bool C_ADD(Instruction &inst);
  bool C_SUB(Instruction &inst);
  bool C_XOR(Instruction &inst);
  bool C_OR(Instruction &inst);
  bool C_AND(Instruction &inst);

  /* M Extensinos */
  bool M_MUL(Instruction &inst);
  bool M_MULH(Instruction &inst);
  bool M_MULHSU(Instruction &inst);
  bool M_MULHU(Instruction &inst);
  bool M_DIV(Instruction &inst);
  bool M_DIVU(Instruction &inst);
  bool M_REM(Instruction &inst);
  bool M_REMU(Instruction &inst);

  /* A Extensinos */
  bool A_LR(Instruction &inst);
  bool A_SC(Instruction &inst);
  bool A_AMOSWAP(Instruction &inst);
  bool A_AMOADD(Instruction &inst);
  bool A_AMOXOR(Instruction &inst);
  bool A_AMOAND(Instruction &inst);
  bool A_AMOOR(Instruction &inst);
  bool A_AMOMIN(Instruction &inst);
  bool A_AMOMAX(Instruction &inst);
  bool A_AMOMINU(Instruction &inst);
  bool A_AMOMAXU(Instruction &inst);

  bool NOP(Instruction &inst);

private:
  uint32_t readDataMem(uint32_t addr, int size);
  void writeDataMem(uint32_t addr, uint32_t data, int size);

  void RaiseException(uint32_t cause, uint32_t inst = 0);

  std::set<uint32_t> TLB_A_Entries;

  void TLB_reserve(uint32_t address);
  bool TLB_reserved(uint32_t address);

  Registers *regs;
  Performance *perf;
  Log *log;
};


#endif
