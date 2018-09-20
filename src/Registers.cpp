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
#if 0
  cout << "x0 (zero): " << register_bank[0] << "\t";
  cout << "x1 (ra): " << register_bank[1] << "\t";
  cout << "x2 (sp): " << register_bank[2] << "\t";
  cout << "x3 (gp): " << register_bank[3] << "\t" << endl;

  cout << "x4 (tp): " << register_bank[4] << "\t";
  cout << "x5 (t0): " << register_bank[5] << "\t";
  cout << "x6 (t1): " << register_bank[6] << "\t";
  cout << "x7 (t2): " << register_bank[7] << "\t" << endl;

  cout << "x8 (s0/fp): " << register_bank[8] << "\t";
  cout << "x9 (s1): " << register_bank[9] << "\t";
  cout << "x10 (a0): " << register_bank[10] << "\t";
  cout << "x11 (a1): " << register_bank[11] << "\t" << endl;

  cout << "x12 (a2): " << register_bank[12] << "\t";
  cout << "x13 (a3): " << register_bank[13] << "\t";
  cout << "x14 (a4): " << register_bank[14] << "\t";
  cout << "x15 (a5): " << register_bank[15] << "\t" << endl;

  cout << "x16 (a6): " << register_bank[16] << "\t";
  cout << "x17 (a7): " << register_bank[17] << "\t";
  cout << "x18 (s2): " << register_bank[18] << "\t";
  cout << "x19 (s3): " << register_bank[19] << "\t" << endl;

#else
  for(int i=0;i<32;i++) {
    cout << "R" << dec << i << ": " << register_bank[i] << "\t";
    if (i % 4 == 3) {
      cout << endl;
    }
  }
#endif

  cout << "PC: 0x" << hex << register_PC << endl;
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
