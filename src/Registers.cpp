#include "Registers.h"

Registers::Registers() {

  memset(register_bank, 0, sizeof(int32_t)*32); // 32 registers of 32 bits each
  perf = Performance::getInstance();

  register_bank[sp] = 1024-1;  // SP points to end of memory
  register_PC = 0x10000;       // default _start address
}

void Registers::dump(void) {
  cout << "************************************" << endl;
  cout << "Registers dump" << dec << endl;
  cout << "x0 (zero):  " << left << setw(8) << register_bank[0];
  cout << "x1 (ra):    " << left << setw(8) << register_bank[1];
  cout << "x2 (sp):    " << left << setw(8) << register_bank[2];
  cout << "x3 (gp):    " << left << setw(8) << register_bank[3] << endl;

  cout << "x4 (tp):    " << left << setw(8) << register_bank[4];
  cout << "x5 (t0):    " << left << setw(8) << register_bank[5];
  cout << "x6 (t1):    " << left << setw(8) << register_bank[6];
  cout << "x7 (t2):    " << left << setw(8) << register_bank[7] << endl;

  cout << "x8 (s0/fp): " << left << setw(8) << register_bank[8];
  cout << "x9 (s1):    " << left << setw(8) << register_bank[9];
  cout << "x10 (a0):   " << left << setw(8) << register_bank[10];
  cout << "x11 (a1):   " << left << setw(8) << register_bank[11] << endl;

  cout << "x12 (a2):   " << left << setw(8) << register_bank[12];
  cout << "x13 (a3):   " << left << setw(8) << register_bank[13];
  cout << "x14 (a4):   " << left << setw(8) << register_bank[14];
  cout << "x15 (a5):   " << left << setw(8) << register_bank[15] << endl;

  cout << "x16 (a6):   " << left << setw(8) << register_bank[16];
  cout << "x17 (a7):   " << left << setw(8) << register_bank[17];
  cout << "x18 (s2):   " << left << setw(8) << register_bank[18];
  cout << "x19 (s3):   " << left << setw(8) << register_bank[19] << endl;

  cout << "x20 (s4):   " << left << setw(8) << register_bank[16];
  cout << "x21 (s5):   " << left << setw(8) << register_bank[17];
  cout << "x22 (s6):   " << left << setw(8) << register_bank[18];
  cout << "x23 (s7):   " << left << setw(8) << register_bank[19] << endl;

  cout << "x24 (s8):   " << left << setw(8) << register_bank[16];
  cout << "x25 (s9):   " << left << setw(8) << register_bank[17];
  cout << "x26 (s10):  " << left << setw(8) << register_bank[18];
  cout << "x27 (s11):  " << left << setw(8) << register_bank[19] << endl;

  cout << "x28 (t3):   " << left << setw(8) << register_bank[16];
  cout << "x29 (t4):   " << left << setw(8) << register_bank[17];
  cout << "x30 (t5):   " << left << setw(8) << register_bank[18];
  cout << "x31 (t6):   " << left << setw(8) << register_bank[19] << endl;

  cout << "PC: 0x" << hex << left << setw(8) << register_PC << endl;
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
