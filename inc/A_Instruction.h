/*!
   \file A_Instruction.h
   \brief Decode A extensions part of the RISC-V
   \author Màrius Montón
   \date December 2018
*/

#ifndef A_INSTRUCTION__H
#define A_INSTRUCTION__H

#include "systemc"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

typedef enum {
  OP_A_LR,
  OP_A_SC,
  OP_A_AMOSWAP,
  OP_A_AMOADD,
  OP_A_AMOXOR,
  OP_A_AMOAND,
  OP_A_AMOOR,
  OP_A_AMOMIN,
  OP_A_AMOMAX,
  OP_A_AMOMINU,
  OP_A_AMOMAXU,

  OP_A_ERROR
} op_A_Codes;


typedef enum {
  A_LR       = 0b00010,
  A_SC       = 0b00011,
  A_AMOSWAP  = 0b00001,
  A_AMOADD   = 0b00000,
  A_AMOXOR   = 0b00100,
  A_AMOAND   = 0b01100,
  A_AMOOR    = 0b01000,
  A_AMOMIN   = 0b10000,
  A_AMOMAX   = 0b10100,
  A_AMOMINU  = 0b11000,
  A_AMOMAXU  = 0b11100,
} A_Codes;

/**
 * @brief Instruction decoding and fields access
 */
class A_Instruction{
public:

  /**
   * @brief Constructor
   * @param instr Instruction to decode
   */
  A_Instruction(sc_uint<32> instr);

  /**
   * @brief Access to opcode field
   * @return return opcode field
   */
  inline int32_t opcode() {
    return a_instr.range(31,27);
  }

  /**
   * @brief Access to rd field
   * @return rd field
   */
  inline int32_t get_rd() {
    return a_instr.range(11, 7);
  }

  inline void set_rd(int32_t value) {
    a_instr.range(11,7) = value;
  }


  /**
   * @brief Access to rs1 field
   * @return rs1 field
   */
  inline int32_t get_rs1() {
    return a_instr.range(19, 15);
  }

  inline void set_rs1(int32_t value) {
    a_instr.range(19,15) = value;
  }


  /**
   * @brief Access to rs2 field
   * @return rs2 field
   */
  inline int32_t get_rs2() {
    return a_instr.range(24, 20);
  }

  inline void set_rs2(int32_t value) {
    a_instr.range(24,20) = value;
  }


  inline int32_t get_funct3() {
    return a_instr.range(14, 12);
  }

  inline void set_funct3(int32_t value) {
    a_instr.range(14,12) = value;
  }

  /**
   * @brief Decodes opcode of instruction
   * @return opcode of instruction
   */
  op_A_Codes decode();

  inline void dump() {
    cout << hex << "0x" << a_instr << dec << endl;
  }
private:
  sc_uint<32> a_instr;
};

#endif
