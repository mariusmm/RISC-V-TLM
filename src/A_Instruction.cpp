#include "A_Instruction.h"


A_Instruction::A_Instruction(sc_uint<32> instr) {
  a_instr = instr;
}

op_A_Codes A_Instruction::decode() {

  switch (opcode()) {
    case A_LR:
      return OP_A_LR;
      break;
    case A_SC:
      return OP_A_SC;
      break;
    case A_AMOSWAP:
      return OP_A_AMOSWAP;
      break;
    case A_AMOADD:
      return OP_A_AMOADD;
      break;
    case A_AMOXOR:
      return OP_A_AMOXOR;
      break;
    case A_AMOAND:
      return OP_A_AMOAND;
      break;
    case A_AMOOR:
      return OP_A_AMOOR;
      break;
    case A_AMOMIN:
      return OP_A_AMOMIN;
      break;
    case A_AMOMAX:
      return OP_A_AMOMAX;
      break;
    case A_AMOMINU:
      return OP_A_AMOMINU;
      break;
    case A_AMOMAXU:
      return OP_A_AMOMAXU;
      break;
    default:
      return OP_A_ERROR;
      break;

  }

  return OP_A_ERROR;
}
