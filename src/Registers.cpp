/*!
   \file Registers.cpp
   \brief Basic register file implementation
   \author Màrius Montón
   \date August 2018
*/

#include "Registers.h"

Registers::Registers() {

  memset(register_bank, 0, sizeof(uint32_t)*32); // 32 registers of 32 bits each
  memset(CSR, 0, sizeof(uint32_t)*4096);
  perf = Performance::getInstance();

  initCSR();

  //cout << "Memory size: 0x" << hex << Memory::SIZE << endl;
  //cout << "SP address: 0x" << hex << (0x10000000 / 4) - 1 << endl;

  register_bank[sp] = Memory::SIZE-4; // default stack at the end of the memory
  register_PC = 0x80000000;       // default _start address
}

void Registers::dump(void) {
  cout << "************************************" << endl;
  cout << "Registers dump" << dec << endl;
  cout << "x0 (zero):  " << right << setw(11) << register_bank[0];
  cout << " x1 (ra):    " << right << setw(11) << register_bank[1];
  cout << " x2 (sp):    " << right << setw(11) << register_bank[2];
  cout << " x3 (gp):    " << right << setw(11) << register_bank[3] << endl;

  cout << "x4 (tp):    " << right << setw(11) << register_bank[4];
  cout << " x5 (t0):    " << right << setw(11) << register_bank[5];
  cout << " x6 (t1):    " << right << setw(11) << register_bank[6];
  cout << " x7 (t2):    " << right << setw(11) << register_bank[7] << endl;

  cout << "x8 (s0/fp): " << right << setw(11) << register_bank[8];
  cout << " x9 (s1):    " << right << setw(11) << register_bank[9];
  cout << " x10 (a0):   " << right << setw(11) << register_bank[10];
  cout << " x11 (a1):   " << right << setw(11) << register_bank[11] << endl;

  cout << "x12 (a2):   " << right << setw(11) << register_bank[12];
  cout << " x13 (a3):   " << right << setw(11) << register_bank[13];
  cout << " x14 (a4):   " << right << setw(11) << register_bank[14];
  cout << " x15 (a5):   " << right << setw(11) << register_bank[15] << endl;

  cout << "x16 (a6):   " << right << setw(11) << register_bank[16];
  cout << " x17 (a7):   " << right << setw(11) << register_bank[17];
  cout << " x18 (s2):   " << right << setw(11) << register_bank[18];
  cout << " x19 (s3):   " << right << setw(11) << register_bank[19] << endl;

  cout << "x20 (s4):   " << right << setw(11) << register_bank[20];
  cout << " x21 (s5):   " << right << setw(11) << register_bank[21];
  cout << " x22 (s6):   " << right << setw(11) << register_bank[22];
  cout << " x23 (s7):   " << right << setw(11) << register_bank[23] << endl;

  cout << "x24 (s8):   " << right << setw(11) << register_bank[24];
  cout << " x25 (s9):   " << right << setw(11) << register_bank[25];
  cout << " x26 (s10):  " << right << setw(11) << register_bank[26];
  cout << " x27 (s11):  " << right << setw(11) << register_bank[27] << endl;

  cout << "x28 (t3):   " << right << setw(11) << register_bank[28];
  cout << " x29 (t4):   " << right << setw(11) << register_bank[29];
  cout << " x30 (t5):   " << right << setw(11) << register_bank[30];
  cout << " x31 (t6):   " << right << setw(11) << register_bank[31] << endl;

  cout << "PC: 0x" << hex << register_PC << dec << endl;
  cout << "************************************" << endl;
}


void Registers::setValue(int reg_num, int32_t value) {
  if ((reg_num != 0) && (reg_num < 32)) {
   register_bank[reg_num] = value;
   perf->registerWrite();
  }
}

int32_t Registers::getValue(int reg_num) {
  if ((reg_num >= 0) && (reg_num < 32)){
    perf->registerRead();
    return register_bank[reg_num];
  } else {
    return 0xFFFFFFFF;
  }
}

uint32_t Registers::getPC() {
  return register_PC;
}

void Registers::setPC(uint32_t new_pc) {
  register_PC = new_pc;
}

uint32_t Registers::getCSR(int csr) {
  uint32_t ret_value = 0;

  switch (csr) {
    case CSR_CYCLE:
    case CSR_MCYCLE:
      ret_value = (uint64_t)(sc_time(sc_time_stamp()
            - sc_time(SC_ZERO_TIME)).to_double()) & 0x00000000FFFFFFFF;
      break;
    case CSR_CYCLEH:
    case CSR_MCYCLEH:
      ret_value = (uint32_t)((uint64_t)(sc_time(sc_time_stamp()
          - sc_time(SC_ZERO_TIME)).to_double()) >> 32 & 0x00000000FFFFFFFF);
      break;
    case CSR_TIME:
      ret_value = (uint64_t)(sc_time(sc_time_stamp()
          - sc_time(SC_ZERO_TIME)).to_double()) & 0x00000000FFFFFFFF;
      break;
    case CSR_TIMEH:
      ret_value = (uint32_t)((uint64_t)(sc_time(sc_time_stamp()
          - sc_time(SC_ZERO_TIME)).to_double()) >> 32 & 0x00000000FFFFFFFF);
      break;
    default:
      ret_value = CSR[csr];
      break;
  }
  return ret_value;
}


void Registers::setCSR(int csr, uint32_t value) {
  /* @FIXME: rv32mi-p-ma_fetch tests doesn't allow MISA to writable,
   * but Volume II: Privileged Architectura v1.10 says MISRA is writable (?)
   */
  if (csr != CSR_MISA) {
    CSR[csr] = value;
  }
}

void Registers::initCSR() {
  CSR[CSR_MISA] = MISA_MXL | MISA_M_EXTENSION | MISA_C_EXTENSION
        | MISA_A_EXTENSION | MISA_I_BASE;
  CSR[CSR_MSTATUS] = MISA_MXL ;

}
