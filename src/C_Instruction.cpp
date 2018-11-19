#include "C_Instruction.h"


C_Instruction::C_Instruction(sc_uint<32> instr) {
  m_instr = instr;
}

op_C_Codes C_Instruction::decode() {

  switch (opcode()) {

    case 0b00:
      switch(get_funct3()) {
        case C_ADDI4SPN:
          return OP_C_ADDI4SPN;
          break;
        case C_FLD:
          return OP_C_FLD;
          break;
        case C_LW:
          return OP_C_LW;
          break;
        case C_FLW:
          return OP_C_FLW;
          break;
        case C_FSD:
          return OP_C_FSD;
          break;
        case C_SW:
          return OP_C_SW;
          break;
        case C_FSW:
          return OP_C_FSW;
          break;
        default:
          return OP_C_ERROR;
          break;
      }
      break;

    case 0b01:
      switch(get_funct3()) {
        case C_ADDI:
          return OP_C_ADDI;
          break;
        case C_JAL:
          return OP_C_JAL;
          break;
        case C_LI:
          return OP_C_LI;
          break;
        case C_ADDI16SP:
          return OP_C_ADDI16SP;
          break;
        case C_SRLI:
          switch(m_instr.range(11,10)) {
            case C_2_SRLI:
              return OP_C_SRLI;
              break;
            case C_2_SRAI:
              return OP_C_SRAI;
              break;
            case C_2_ANDI:
              return OP_C_ANDI;
              break;
            case C_2_SUB:
              switch(m_instr.range(6,5)) {
                case C_3_SUB:
                  return OP_C_SUB;
                  break;
                case C_3_XOR:
                  return OP_C_XOR;
                  break;
                case C_3_OR:
                  return OP_C_OR;
                  break;
                case C_3_AND:
                  return OP_C_AND;
                  break;
              }
          }
          break;
        case C_J:
          return OP_C_J;
          break;
        case C_BEQZ:
          return OP_C_BEQZ;
          break;
        case C_BNEZ:
          return OP_C_BNEZ;
          break;
        default:
          return OP_C_ERROR;
          break;
      }
      break;

    case 0b10:
      switch(get_funct3()) {
        case C_SLLI:
          return OP_C_SLLI;
          break;
        case C_FLDSP:
        case C_LWSP:
          return OP_C_LWSP;
          break;
        case C_FLWSP:
          return OP_C_FLWSP;
          break;
        case C_JR:
          if (m_instr[12] == 0) {
            if (m_instr.range(6,2) == 0) {
              return OP_C_JR;
            } else {
              return OP_C_MV;
            }
          } else {
            if (m_instr.range(11,2) == 0) {
              return OP_C_EBREAK;
            } else if (m_instr.range(6,2) == 0) {
              return OP_C_JALR;
            } else {
              return OP_C_ADD;
            }
          }
          break;
        case C_FDSP:
          break;
        case C_SWSP:
          return OP_C_SWSP;
          break;
        case C_FWWSP:
        default:
          return OP_C_ERROR;
          break;
      }
      break;

    default:
      return OP_C_ERROR;
      break;

  }

  return OP_C_ERROR;
}
