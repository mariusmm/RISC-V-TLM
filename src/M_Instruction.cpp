#include "M_Instruction.h"


M_Instruction::M_Instruction(sc_uint<32> instr) {
  m_instr = instr;
}

op_M_Codes M_Instruction::decode() {

  switch (opcode()) {
    case M_MUL:
      return OP_M_MUL;
      break;
    case M_MULH:
      return OP_M_MULH;
      break;
    case M_MULHSU:
      return OP_M_MULHSU;
      break;
    case M_MULHU:
      return OP_M_MULHU;
      break;
    case M_DIV:
      return OP_M_DIV;
      break;
    case M_DIVU:
      return OP_M_DIVU;
      break;
    case M_REM:
      return OP_M_REM;
      break;
    case M_REMU:
      return OP_M_REMU;
      break;
    default:
      return OP_M_ERROR;
      break;

  }

  return OP_M_ERROR;
}
