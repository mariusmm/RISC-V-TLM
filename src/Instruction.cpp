#include "Instruction.h"


Instruction::Instruction(sc_int<32> instr) {
  m_instr = instr;


}

opCodes Instruction::decode() {
  switch (opcode()) {
    case LUI:
      return OP_LUI;
    case AUIPC:
      return OP_AUIPC;
    case JAL:
        return OP_JAL;
    case JALR:
        return OP_JALR;
    case BEQ:
      switch(funct3()) {
        case BEQ_F:
          return OP_BEQ;
        case BNE_F:
          return OP_BNE;
        case BLT_F:
          return OP_BLT;
        case BGE_F:
          return OP_BGE;
        case BLTU_F:
          return OP_BLTU;
        case BGEU_F:
          return OP_BGEU;
      }
      return OP_ERROR;
    case LB:
      switch(funct3()) {
        case LB_F:
          return OP_LB;
        case LH_F:
          return OP_LH;
        case LW_F:
          return OP_LW;
        case LBU_F:
          return OP_LBU;
        case LHU_F:
          return OP_LHU;
      }
      return OP_ERROR;
    case SB:
      switch(funct3()) {
        case SB_F:
          return OP_SB;
        case SH_F:
          return OP_SH;
        case SW_F:
          return OP_SW;
      }
      return OP_ERROR;
    case ADDI:
      switch(funct3()) {
        case ADDI_F:
          return OP_ADDI;
        case SLTI_F:
          return OP_SLTI;
        case SLTIU_F:
          return OP_SLTIU;
        case XORI_F:
          return OP_XORI;
        case ORI_F:
          return OP_ORI;
        case ANDI_F:
          return OP_ANDI;
        case SLLI_F:
          return OP_SLLI;
        case SRLI_F:
          switch(funct7()) {
            case SRLI_F7:
              return OP_SRLI;
            case SRAI_F7:
              return OP_SRAI;
          }
          return OP_ERROR;
      }
      return OP_ERROR;
    case ADD: {
      switch(funct3()) {
        case ADD_F:
          switch (funct7()) {
            case ADD_F7:
              return OP_ADD;
            case SUB_F7:
              return OP_SUB;
          };
          break;
        case SLL_F:
          return OP_SLL;
        case SLT_F:
          return OP_SLT;
        case SLTU_F:
          return OP_SLTU;
        case XOR_F:
          return OP_XOR;
        case SRL_F:
          switch(funct7()) {
            case SRL_F7:
              return OP_SRL;
            case SRA_F7:
              return OP_SRA;
          }
        case OR_F:
          return OP_OR;
        case AND_F:
          return OP_AND;
      }
    } /* ADD */
      return OP_ERROR;
    default:
      return OP_ERROR;
  }
}
