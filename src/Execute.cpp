#include "Execute.h"

SC_HAS_PROCESS(Execute);
Execute::Execute(sc_module_name name
  , Registers *register_bank)
  : sc_module(name)
  , data_bus("data_bus")
  , regs(register_bank) {
    perf = Performance::getInstance();
    log = Log::getInstance();
}

bool Execute::LUI(Instruction &inst) {
  int rd;
  uint32_t imm = 0;

  rd = inst.get_rd();
  imm = inst.get_imm_U() << 12;
  regs->setValue(rd, imm);
  log->SC_log(Log::INFO) << "LUI x" << dec
          << rd << " <- 0x" << hex << imm << endl;

  return true;
}

bool Execute::AUIPC(Instruction &inst) {
  int rd;
  uint32_t imm = 0;
  int new_pc;

  rd = inst.get_rd();
  imm = inst.get_imm_U() << 12;
  new_pc = regs->getPC() + imm;

  regs->setValue(rd, new_pc);

  log->SC_log(Log::INFO) << "AUIPC x" << dec
          << rd << " <- 0x" << hex << imm << " + PC (0x"
          << new_pc << ")" << endl;

  return true;
}

bool Execute::JAL(Instruction &inst, bool c_extension, int m_rd) {
  int32_t mem_addr = 0;
  int rd;
  int new_pc, old_pc;

  if (c_extension == false) {
    rd = inst.get_rd();
    mem_addr = inst.get_imm_J();
    old_pc = regs->getPC();
    new_pc = old_pc + mem_addr;

    regs->setPC(new_pc);

    old_pc = old_pc + 4;
    regs->setValue(rd, old_pc);
  } else {
    C_Instruction c_inst(inst.getInstr());

    rd = m_rd;
    mem_addr = c_inst.get_imm_J();
    old_pc = regs->getPC();

    new_pc = old_pc + mem_addr;
    regs->setPC(new_pc);

    old_pc = old_pc + 2;
    regs->setValue(rd, old_pc);
  }

  log->SC_log(Log::INFO) << "JAL: x" << dec
          << rd << " <- 0x" << hex << old_pc << dec
          << ". PC + 0x" << hex << mem_addr << " -> PC (0x"
          << new_pc << ")" << endl;

  return true;
}

bool Execute::JALR(Instruction &inst, bool c_extension) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int new_pc, old_pc;

  if (c_extension == false) {
    rd = inst.get_rd();
    rs1 = inst.get_rs1();
    mem_addr = inst.get_imm_I();

    old_pc = regs->getPC();
    regs->setValue(rd, old_pc + 4);

    new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
    regs->setPC(new_pc);

    log->SC_log(Log::INFO) << "JALR: x" << dec
            << rd << " <- 0x" << hex << old_pc + 4
            << " PC <- 0x" << new_pc << endl;

  } else  {
    C_Instruction c_inst(inst.getInstr());

    rd = 1;
    rs1 = c_inst.get_rs1();

    old_pc = regs->getPC();
    regs->setValue(rd, old_pc + 2);

    new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
    regs->setPC(new_pc);

    log->SC_log(Log::INFO) << "C.JALR: x" << dec
            << rd << " <- 0x" << hex << old_pc + 4
            << " PC <- 0x" << hex << new_pc << endl;
  }

  return true;
}

bool Execute::BEQ(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if (regs->getValue(rs1) == regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "BEQ x" << dec
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") == x" << dec
          << rs2 << "(0x" << hex << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::BNE(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;
  uint32_t val1, val2;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  val1 = regs->getValue(rs1);
  val2 = regs->getValue(rs2);

  if (val1 != val2) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "BNE: x" << dec
          << rs1 << "(0x" << hex <<  val1 << ") == x" << dec
          << rs2 << "(0x" << hex << val2 << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::BLT(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if ((int32_t)regs->getValue(rs1) < (int32_t)regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BLT x" << dec
          << rs1 << "(0x" << hex << (int32_t)regs->getValue(rs1) << ") < x" << dec
          << rs2 << "(0x" << hex << (int32_t)regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::BGE(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if ((int32_t)regs->getValue(rs1) >= (int32_t)regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BGE x" << dec
          << rs1 << "(0x" << hex << (int32_t)regs->getValue(rs1) << ") > x" << dec
          << rs2 << "(0x" << hex << (int32_t)regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::BLTU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if ((uint32_t) regs->getValue(rs1) < (uint32_t) regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "BLTU x" << dec
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") < x" << dec
          << rs2 << "(0x" << hex << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::BGEU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if ((uint32_t) regs->getValue(rs1) >= (uint32_t) regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BGEU x" << dec
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") > x" << dec
          << rs2 << "(0x" << hex << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::LB(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  int8_t data;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 1);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LB: x"
          << rs1 << " + " << imm << " (@0x"
          << hex << mem_addr << dec << ") -> x" << rd << endl;

  return true;
}

bool Execute::LH(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  int16_t data;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 2);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LH: x"
          << rs1 << " + " << imm << " (@0x"
          << hex << mem_addr << dec << ") -> x" << rd << endl;

  return true;
}

bool Execute::LW(Instruction &inst, bool c_extension) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint32_t data;

  if (c_extension == false) {
    rd = inst.get_rd();
    rs1 = inst.get_rs1();
    imm = inst.get_imm_I();
  } else {
    C_Instruction c_inst(inst.getInstr());

    rd = c_inst.get_rdp();
    rs1 = c_inst.get_rs1p();
    imm = c_inst.get_imm_L();
  }

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << dec << "C.LW: x"
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") + "
          << dec << imm << " (@0x" << hex << mem_addr << dec << ") -> x" << rd << hex
          << " (0x" << data << ")"<< endl;

  return true;
}

bool Execute::LBU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint8_t data;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 1);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LBU: x"
          << rs1 << " + " << imm << " (@0x"
          << hex << mem_addr << dec << ") -> x" << rd << endl;

  return true;
}

bool Execute::LHU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint16_t data;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 2);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LHU: x" << dec
          << rs1 << " + " << imm << " (@0x"
          << hex << mem_addr << dec << ") -> x"
          << rd << "(0x" << hex << data << ")"<< endl;

  return true;
}

bool Execute::SB(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();
  imm = inst.get_imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 1);

  log->SC_log(Log::INFO) << "SB: x" << dec
          << rs2 << " -> x" << rs1 << " + 0x" << hex << imm
          << " (@0x" << hex << mem_addr << dec << ")" << endl;

  return true;
}

bool Execute::SH(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();
  imm = inst.get_imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 2);

  log->SC_log(Log::INFO) << "SH: x" << dec
          << rs2 << " -> x"
          << rs1 << " + 0x" << hex << imm << " (@0x" << hex
          << mem_addr << dec << ")" << endl;

  return true;
}

bool Execute::SW(Instruction &inst, bool c_extension) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  if (c_extension == false) {
    rs1 = inst.get_rs1();
    rs2 = inst.get_rs2();
    imm = inst.get_imm_S();
  } else {
    C_Instruction c_inst(inst.getInstr());
    rs1 = c_inst.get_rs1p();
    rs2 = c_inst.get_rs2p();
    imm = c_inst.get_imm_L();
  }

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << "SW: x" << dec
          << rs2 << "(0x" << hex << data << ") -> x" << dec
          << rs1 << " + 0x" << hex << imm << " (@0x" << hex
          << mem_addr << dec << ")" << endl;

  return true;
}

bool Execute::ADDI(Instruction &inst, bool c_extension) {
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  if (c_extension == false) {
    rd = inst.get_rd();
    rs1 = inst.get_rs1();
    imm = inst.get_imm_I();
  } else {
    C_Instruction c_inst(inst.getInstr());

    rd = c_inst.get_rd();
    rs1 = rd;
    imm = c_inst.get_imm_ADDI();
  }

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ADDI: x" << dec
          << rs1 << " + " << imm << " -> x" << dec
          << rd  << "(0x" << hex << calc << ")"<< endl;

  return true;
}

bool Execute::SLTI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  if (regs->getValue(rs1) < imm) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTI: x"
            << rs1 << " < "
            << imm << " => " << "1 -> x"
            << rd  << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTI: x"
            << rs1 << " < "
            << imm << " => " << "0 -> x"
            << rd  << endl;
  }

  return true;
}

bool Execute::SLTIU(Instruction &inst) {
  int rd, rs1;
  int32_t imm;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  if ((uint32_t) regs->getValue(rs1) < (uint32_t)imm) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTIU: x"
            << rs1 << " < "
            << imm << " => " << "1 -> x"
            << rd  << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTIU: x"
            << rs1 << " < "
            << imm << " => " << "0 -> x"
            << rd  << endl;
  }

  return true;
}

bool Execute::XORI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  calc = regs->getValue(rs1) ^ imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "XORI: x"
          << rs1 << " XOR "
          << imm << "-> x"
          << rd  << endl;

  return true;
}

bool Execute::ORI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  calc = regs->getValue(rs1) | imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ORI: x"
          << rs1 << " OR "
          << imm << "-> x"
          << rd  << endl;

  return true;
}

bool Execute::ANDI(Instruction &inst) {
  int rd, rs1;
  uint32_t imm;
  uint32_t calc;
  uint32_t aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  aux = regs->getValue(rs1);
  calc = aux & imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ANDI: x"
          << rs1 << "(0x" << hex << aux << ") AND 0x"
          << imm << " -> x"
          << dec << rd  << "(0x" << hex << calc << ")" << endl;

  return true;
}

bool Execute::SLLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_shamt();

  if (rs2 >= 0x20) {
    // raise an exception, but how?
    cout << "ILEGAL INSTRUCTION, shamt[5] != 0" << endl;
    RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION);

    return false;
  }

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SLLI: x" << dec
          << rs1 << " << " << shift << " -> x"
          << rd << "(0x" << hex << calc << ")" << endl;

  return true;
}

bool Execute::SRLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRLI: x" << dec
          << rs1 << " >> " << shift << " -> x"
          << rd  << endl;

  return true;
}

bool Execute::SRAI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRAI: x" << dec
          << rs1 << " >> " << shift << " -> x"
          << rd  << endl;

  return true;
}

bool Execute::ADD(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;
  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);

  // log->SC_log(Log::INFO) << "ADD 0x" << hex << regs->getValue(rs1)
  //   << " + 0x" << regs->getValue(rs2) << " = " << calc << endl;

  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ADD: x" << dec
          << rs1 << " + x"
          << rs2 << " -> x"
          << rd  << hex << "(0x" << calc << ")"<< endl;

  return true;
}

bool Execute::SUB(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;
  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) - regs->getValue(rs2);
  regs->setValue(rd, calc);

  /* Can insert some arbitrary execution time */
  //wait(sc_time(10, SC_NS));

  log->SC_log(Log::INFO) << "SUB: x"
          << rs1 << " - x"
          << rs2 << " -> x"
          << rd << "("<< calc << ")" << endl;

  return true;
}

bool Execute::SLL(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SLL: x"
          << rs1 << " << " << shift << " -> x"
          << rd  << endl;

  return true;
}

bool Execute::SLT(Instruction &inst) {
  int rd, rs1, rs2;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if (regs->getValue(rs1) < regs->getValue(rs2)) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLT: x"
            << rs1 << " < x"
            << rs2 << " => " << "1 -> x"
            << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLT: x"
            << rs1 << " < x"
            << rs2 << " => " << "0 -> x"
            << rd  << endl;
  }

  return true;
}

bool Execute::SLTU(Instruction &inst) {
  int rd, rs1, rs2;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if ( (uint32_t)regs->getValue(rs1) < (uint32_t)regs->getValue(rs2)) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTU: x"
            << rs1 << " < x"
            << rs2 << " => " << "1 -> x"
            << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTU: x"
            << rs1 << " < x"
            << rs2 << " => " << "0 -> x"
            << rd  << endl;
  }

  return true;
}

bool Execute::XOR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) ^ regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "XOR: x"
          << rs1 << " XOR x"
          << rs2 << "-> x"
          << rd  << endl;

  return true;
}

bool Execute::SRL(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRL: x"
          << rs1 << " >> " << shift << " -> x" << rd << endl;

  return true;
}

bool Execute::SRA(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRA: x"
          << rs1 << " >> " << shift << " -> x" << rd << endl;

  return true;
}

bool Execute::OR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) | regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "OR: x" << rs1 << " OR x" << rs2
          << "-> x" << rd << endl;

  return true;
}

bool Execute::AND(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) & regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "AND: x" << rs1 << " AND x" << rs2
          << "-> x" << rd << endl;

  return true;
}

bool Execute::FENCE(Instruction &inst) {
  log->SC_log(Log::INFO) << "FENCE" << endl;

  return true;
}

bool Execute::ECALL(Instruction &inst) {

  log->SC_log(Log::INFO) << "ECALL" << endl;
  std::cout << endl << "ECALL Instruction called, stopping simulation" << endl;
  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();

  uint32_t gp_value = regs->getValue(Registers::gp);
  if ( gp_value == 1) {
      cout << "GP value is 1, test result is OK" << endl;
  } else {
      cout << "GP value is " << gp_value << endl;
  }
  //SC_REPORT_ERROR("Execute", "ECALL");
  sc_stop();
  return true;
}

bool Execute::EBREAK(Instruction &inst) {

  log->SC_log(Log::INFO) << "EBREAK" << endl;
  std::cout << endl << "EBRAK  Instruction called, dumping information" << endl;
  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();

  RaiseException(EXCEPTION_CAUSE_BREAKPOINT);

  return true;
}

bool Execute::CSRRW(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  /* These operations must be atomical */
  if (rd != 0) {
    aux = regs->getCSR(csr);
    regs->setValue(rd, aux);
  }

  aux = regs->getValue(rs1);
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << hex << "CSRRW: CSR #"
          << csr << " -> x" << dec << rd
          << ". x" << rs1 << "-> CSR #" << hex << csr << " (0x"
		  << aux << ")" << endl;

  return true;
}

bool Execute::CSRRS(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux, aux2;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rd == 0) {
    log->SC_log(Log::INFO) << "CSRRS with rd1 == 0, doing nothing." << endl;
    return false;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  bitmask = regs->getValue(rs1);

  regs->setValue(rd, aux);

  aux2 = aux | bitmask;
  regs->setCSR(csr, aux2);

  log->SC_log(Log::INFO) << "CSRRS: CSR #"
          << csr << "(0x" << hex << aux << ") -> x" << dec << rd
          << ". x" << rs1 << " & CSR #" << csr
          << " <- 0x" << hex << aux2 << endl;

  return true;
}

bool Execute::CSRRC(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux, aux2;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rd == 0) {
    log->SC_log(Log::INFO) << "CSRRC with rd1 == 0, doing nothing." << endl;
    return true;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  bitmask = regs->getValue(rs1);

  regs->setValue(rd, aux);

  aux2 = aux & ~bitmask;
  regs->setCSR(csr, aux2);

  log->SC_log(Log::INFO) << "CSRRC: CSR #"
          << csr << "(0x" << hex << aux << ") -> x" << dec << rd
          << ". x" << rs1 << " & CSR #" << csr
          << " <- 0x" << hex << aux2 << endl;

  return true;
}

bool Execute::CSRRWI(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();


  /* These operations must be atomical */
  if (rd != 0) {
    aux = regs->getCSR(csr);
    regs->setValue(rd, aux);
  }
  aux = rs1;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRWI: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << "-> CSR #" << csr << endl;

  return true;
}

bool Execute::CSRRSI(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rs1 == 0) {
    return true;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = rs1;
  aux = aux | bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRSI: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr
          << "(0x" << hex << aux << ")"<< endl;

  return true;
}

bool Execute::CSRRCI(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rs1 == 0) {
    return true;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = rs1;
  aux = aux & ~bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRCI: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr
          << "(0x" << hex << aux << ")"<< endl;

  return true;
}

/*********************** Privileged Instructions ******************************/

bool Execute::MRET(Instruction &inst) {
  uint32_t new_pc = 0;

  new_pc = regs->getCSR(CSR_MEPC);
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "MRET: PC <- 0x" << hex << new_pc << endl;

  // update mstatus
  uint32_t csr_temp;
  csr_temp = regs->getCSR(CSR_MSTATUS);
  if (csr_temp & MSTATUS_MPIE) {
	  csr_temp |= MSTATUS_MIE;
  }
  csr_temp |= MSTATUS_MPIE;
  regs->setCSR(CSR_MSTATUS, csr_temp);

  return true;
}

bool Execute::SRET(Instruction &inst) {
  uint32_t new_pc = 0;

  new_pc = regs->getCSR(CSR_SEPC);
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "SRET: PC <- 0x" << hex << new_pc << endl;

  return true;
}

bool Execute::WFI(Instruction &inst) {
  log->SC_log(Log::INFO) << "WFI" << endl;

  return true;
}

bool Execute::SFENCE(Instruction &inst) {
  log->SC_log(Log::INFO) << "SFENCE" << endl;

  return true;
}

/**************************** C Instructions **********************************/

bool Execute::C_JR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1;
  int new_pc;

  C_Instruction c_inst(inst.getInstr());

  rs1 = c_inst.get_rs1();
  mem_addr = 0;

  new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "JR: PC <- 0x" << hex << new_pc << endl;

  return true;
}

bool Execute::C_MV(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rd();
  rs1 = 0;
  rs2 = c_inst.get_rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.MV: x" << dec
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") + x" << dec
          << rs2 << "(0x" << hex << regs->getValue(rs2) << ") -> x" << dec
          << rd  << "(0x" << hex << calc << ")" << endl;

  return true;
}

bool Execute::C_ADD(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1();
  rs1 = c_inst.get_rs1();
  rs2 = c_inst.get_rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.ADD: x"
          << dec << rs1 << " + x"
          << rs2 << " -> x"
          << rd  << "(0x" << hex << calc << ")" << endl;

  return true;
}

bool Execute::C_LWSP(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint32_t data;

  // lw rd, offset[7:2](x2)
  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rd();
  rs1 = 2;
  imm = c_inst.get_imm_LWSP();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "C.LWSP: x" << dec
          << rs1 << " + " << imm
          << " (@0x" << hex << mem_addr << dec << ") -> x"
          << rd << "(" << hex << data << ")"<< dec << endl;

  return true;
}

bool Execute::C_ADDI4SPN(Instruction  &inst) {
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rdp();
  rs1 = 2;
  imm = c_inst.get_imm_ADDI4SPN();

  if (imm == 0) {
    RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, inst.getInstr() );
    return false;
  }

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << dec << "ADDI4SPN: x"
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") + "
          << dec << imm << " -> x"
          << rd << "(0x" << hex << calc << ")" << endl;

  return true;
}

bool Execute::C_ADDI16SP(Instruction &inst) {
  // addi x2, x2, nzimm[9:4]
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  if (c_inst.get_rd() == 2) {
    rd = 2;
    rs1 = 2;
    imm = c_inst.get_imm_ADDI16SP();

    calc = regs->getValue(rs1) + imm;
    regs->setValue(rd, calc);

    log->SC_log(Log::INFO) << dec << "ADDI16SP: x"
            << rs1 << " + "
            << dec << imm << " -> x"
            << rd  << "(0x" << hex << calc << ")" << endl;
  } else {
    /* C.LUI OPCODE */
    rd = c_inst.get_rd();
    imm = c_inst.get_imm_LUI();
    regs->setValue(rd, imm);
    log->SC_log(Log::INFO) << dec << "C.LUI x"
            << rd << " <- 0x" << hex << imm << endl;
  }

  return true;
}

bool Execute::C_SWSP(Instruction &inst) {
  // sw rs2, offset(x2)
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  C_Instruction c_inst(inst.getInstr());

  rs1 = 2;
  rs2 = c_inst.get_rs2();
  imm = c_inst.get_imm_CSS();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "SWSP: x"
          << rs2 << "(0x" << hex << data << ") -> x" << dec
          << rs1 << " + " << imm << " (@0x" << hex
          << mem_addr << dec << ")" << endl;

  return true;
}

bool Execute::C_BEQZ(Instruction &inst) {
  int rs1;
  int new_pc = 0;
  uint32_t val1;
  C_Instruction c_inst(inst.getInstr());

  rs1 = c_inst.get_rs1p();
  val1 = regs->getValue(rs1);

  if (val1 == 0) {
    new_pc = regs->getPC() + c_inst.get_imm_CB();
    regs->setPC(new_pc);
  } else {
    regs->incPC(true); //PC <- PC + 2
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "C.BEQZ: x" << dec
          << rs1 << "(" <<  val1 << ") == 0? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::C_BNEZ(Instruction &inst) {
  int rs1;
  int new_pc = 0;
  uint32_t val1;
  C_Instruction c_inst(inst.getInstr());

  rs1 = c_inst.get_rs1p();
  val1 = regs->getValue(rs1);

  if (val1 != 0) {
    new_pc = regs->getPC() + c_inst.get_imm_CB();
    regs->setPC(new_pc);
  } else {
    regs->incPC(true); //PC <- PC +2
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "C.BNEZ: x" << dec
          << rs1 << "(0x" << hex << val1 << ") != 0? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;

  return true;
}

bool Execute::C_LI(Instruction &inst) {
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rd();
  rs1 = 0;
  imm = c_inst.get_imm_ADDI();

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << dec << "LI: x"
          << rs1 << "(" << regs->getValue(rs1) << ") + "
          << imm << " -> x" << rd  << "(" << calc << ")" << endl;

  return true;
}

bool Execute::C_SRLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.SRLI: x"
          << rs1 << " >> " << shift << " -> x"
          << rd  << endl;

  return true;
}

bool Execute::C_SRAI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = (int32_t)regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.SRAI: x"
          << rs1 << " >> " << dec << shift << " -> x"
          << rd << "("<< calc << ")" << endl;

  return true;
}

bool Execute::C_SLLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_imm_ADDI();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.SLLI: x"
          << dec << rs1 << " << "
          << shift << " -> x"
          << rd  << "(0x" << calc << ")"<< endl;

  return true;
}


bool Execute::C_ANDI(Instruction &inst) {
  int rd, rs1;
  uint32_t imm;
  uint32_t aux;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  imm = c_inst.get_imm_ADDI();

  aux = regs->getValue(rs1);
  calc =  aux & imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.ANDI: x"
          << rs1 << "(" << aux << ") AND "
          << imm << " -> x"
          << rd  << endl;

  return true;
}

bool Execute::C_SUB(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2p();

  calc = regs->getValue(rs1) - regs->getValue(rs2);
  regs->setValue(rd, calc);

  /* Can insert some arbitrary execution time */
  //wait(sc_time(10, SC_NS));

  log->SC_log(Log::INFO) << "C.SUB: x"
          << dec << rs1 << " - x"
          << rs2 << " -> x"
          << rd  << endl;

  return true;
}


bool Execute::C_XOR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2p();

  calc = regs->getValue(rs1) ^ regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.XOR: x"
          << dec << rs1 << " XOR x"
          << rs2 << "-> x"
          << rd  << endl;

  return true;
}

bool Execute::C_OR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2p();

  calc = regs->getValue(rs1) | regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C_OR: x"
          << dec << rs1 << " OR x"
          << rs2 << "-> x"
          << rd << endl;

  return true;
}

bool Execute::C_AND(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rs1p();
  rs1 = c_inst.get_rs1p();
  rs2 = c_inst.get_rs2p();

  calc = regs->getValue(rs1) & regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "C.AND: x"
          << dec << rs1 << " AND x"
          << rs2 << "-> x"
          << rd << endl;

  return true;
}

/******************************************************************************/
/* M  Extensions */
/******************************************************************************/
bool Execute::M_MUL(Instruction &inst) {
  int rd, rs1, rs2;
  int32_t multiplier, multiplicand;
  int64_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  multiplier = regs->getValue(rs1);
  multiplicand = regs->getValue(rs2);

  result = multiplier * multiplicand;
  result = result & 0x00000000FFFFFFFF;
  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "MUL: x" << rs1 << " * x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_MULH(Instruction &inst) {
  int rd, rs1, rs2;
  int32_t multiplier, multiplicand;
  int64_t result;
  int32_t ret_value;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  multiplier = regs->getValue(rs1);
  multiplicand = regs->getValue(rs2);

  result = (int64_t) multiplier * (int64_t) multiplicand;

  ret_value = (int32_t)((result >> 32) & 0x00000000FFFFFFFF);
  regs->setValue(rd, ret_value);

  log->SC_log(Log::INFO) << dec << "MULH: x" << rs1 << " * x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_MULHSU(Instruction &inst) {
  int rd, rs1, rs2;
  int32_t multiplier;
  uint32_t multiplicand;
  int64_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  multiplier = regs->getValue(rs1);
  multiplicand = regs->getValue(rs2);

  result = (int64_t) multiplier * (uint64_t) multiplicand;
  result = (result >> 32) & 0x00000000FFFFFFFF;
  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "MULHSU: x" << rs1 << " * x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_MULHU(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t multiplier, multiplicand;
  uint64_t result;
  int32_t ret_value;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  multiplier = (uint32_t) regs->getValue(rs1);
  multiplicand = (uint32_t) regs->getValue(rs2);

  result = (uint64_t) multiplier * (uint64_t) multiplicand;
  ret_value = (uint32_t)(result >> 32) & 0x00000000FFFFFFFF;
  regs->setValue(rd, ret_value);

  log->SC_log(Log::INFO) << dec << "MULHU: x" << rs1 << " * x" << rs2
    << " -> x" << rd << "(" << ret_value << ")" << endl;

  return true;
}

bool Execute::M_DIV(Instruction &inst) {
  int rd, rs1, rs2;
  int32_t divisor, dividend;
  int64_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  dividend = regs->getValue(rs1);
  divisor = regs->getValue(rs2);

  if (divisor == 0) {
    result = - 1;
  } else if ( (divisor == -1) && (dividend == (int32_t)0x80000000) ) {
    result = 0x0000000080000000;
  } else {
    result = dividend / divisor;
    result = result & 0x00000000FFFFFFFF;
  }

  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "DIV: x" << rs1 << " / x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_DIVU(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t divisor, dividend;
  uint64_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  dividend = regs->getValue(rs1);
  divisor = regs->getValue(rs2);

  if (divisor == 0) {
    result = -1;
  } else {
    result = dividend / divisor;
    result = result & 0x00000000FFFFFFFF;
  }

  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "DIVU: x" << rs1 << " / x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_REM(Instruction &inst) {
  int rd, rs1, rs2;
  int32_t divisor, dividend;
  int32_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  dividend = regs->getValue(rs1);
  divisor = regs->getValue(rs2);

  if (divisor == 0) {
    result = dividend;
  } else if ( (divisor == -1) && (dividend == (int32_t)0x80000000) ) {
    result = 0;
  } else {
    result = dividend % divisor;
  }

  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "REM: x" << rs1 << " / x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}

bool Execute::M_REMU(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t divisor, dividend;
  uint32_t result;

  M_Instruction m_inst(inst.getInstr());

  rd = m_inst.get_rd();
  rs1 = m_inst.get_rs1();
  rs2 = m_inst.get_rs2();

  dividend = regs->getValue(rs1);
  divisor = regs->getValue(rs2);

  if (divisor == 0) {
    result = dividend;
  } else  {
    result = dividend % divisor;
  }

  regs->setValue(rd, result);

  log->SC_log(Log::INFO) << dec << "REMU: x" << rs1 << " / x" << rs2
    << " -> x" << rd << "(" << result << ")" << endl;

  return true;
}


bool Execute::A_LR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  if (rs2 != 0) {
    cout << "ILEGAL INSTRUCTION, LR.W: rs2 != 0" << endl;
    RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION);

    return false;
  }

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);
  regs->setValue(rd, data);

  TLB_reserve(mem_addr);

  log->SC_log(Log::INFO) << dec << "LR.W: x"
          << rs1 << " (@0x" << hex << mem_addr
          << dec << ") -> x" << rd << endl;

  return true;
}

bool Execute::A_SC(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = regs->getValue(rs2);

  if (TLB_reserved(mem_addr) == true) {
    writeDataMem(mem_addr, data, 4);
    regs->setValue(rd, 0);  // SC writes 0 to rd on success
  } else {
    regs->setValue(rd, 1);  // SC writes nonzero on failure
  }

  log->SC_log(Log::INFO) << dec << "SC.W: (@0x" <<
          hex << mem_addr << dec << ") <- x" << rs2 <<
          hex << "(0x" << data << ")" << endl;

  return true;
}

bool Execute::A_AMOSWAP(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;
  uint32_t aux;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // swap
  aux = regs->getValue(rs2);
  regs->setValue(rs2, data);

  writeDataMem(mem_addr, aux, 4);

  log->SC_log(Log::INFO) << dec << "AMOSWAP " << endl;
  return true;
}

bool Execute::A_AMOADD(Instruction &inst)  {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // add
  data = data + regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "AMOADD " << endl;

  return true;
}

bool Execute::A_AMOXOR(Instruction &inst)  {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // add
  data = data ^ regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "AMOXOR " << endl;

  return true;
}
bool Execute::A_AMOAND(Instruction &inst)  {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // add
  data = data & regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "AMOAND " << endl;

  return true;
}
bool Execute::A_AMOOR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // add
  data = data | regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "AMOOR " << endl;
  return true;
}
bool Execute::A_AMOMIN(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;
  uint32_t aux;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // min
  aux = regs->getValue(rs2);
  if ((int32_t)data < (int32_t)aux) {
    aux = data;
  }

  writeDataMem(mem_addr, aux, 4);

  log->SC_log(Log::INFO) << dec << "AMOMIN " << endl;

  return true;
}
bool Execute::A_AMOMAX(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;
  uint32_t aux;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // >
  aux = regs->getValue(rs2);
  if ((int32_t)data > (int32_t)aux) {
    aux = data;
  }

  writeDataMem(mem_addr, aux, 4);

  log->SC_log(Log::INFO) << dec << "AMOMAX " << endl;

  return true;
}
bool Execute::A_AMOMINU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;
  uint32_t aux;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // min
  aux = regs->getValue(rs2);
  if (data < aux) {
    aux = data;
  }

  writeDataMem(mem_addr, aux, 4);

  log->SC_log(Log::INFO) << dec << "AMOMINU " << endl;

  return true;
}
bool Execute::A_AMOMAXU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1, rs2;
  uint32_t data;
  uint32_t aux;

  A_Instruction a_inst(inst.getInstr());

  /* These instructions must be atomic */

  rd = a_inst.get_rd();
  rs1 = a_inst.get_rs1();
  rs2 = a_inst.get_rs2();

  mem_addr = regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);

  regs->setValue(rd, data);

  // max
  aux = regs->getValue(rs2);
  if (data > aux) {
    aux = data;
  }

  writeDataMem(mem_addr, aux, 4);

  log->SC_log(Log::INFO) << dec << "AMOMAXU " << endl;

  return true;
}


bool Execute::NOP(Instruction &inst) {
  cout << endl;
  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();

  SC_REPORT_ERROR("Execute", "NOP");

  return true;
}

/**
 * Access data memory to get data for LOAD OPs
 * @param  addr address to access to
 * @param size size of the data to read in bytes
 * @return data value read
 */
uint32_t Execute::readDataMem(uint32_t addr, int size) {
  uint32_t data;
  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  trans.set_command( tlm::TLM_READ_COMMAND );
  trans.set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
  trans.set_data_length( size );
  trans.set_streaming_width( 4 ); // = data_length to indicate no streaming
  trans.set_byte_enable_ptr( 0 ); // 0 indicates unused
  trans.set_dmi_allowed( false ); // Mandatory initial value
  trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
  trans.set_address( addr );

  data_bus->b_transport( trans, delay);

  if ( trans.is_response_error() ) {
    SC_REPORT_ERROR("Memory", "Read memory");
  }
  return data;
}

/**
 * Acces data memory to write data for STORE ops
 * @brief
 * @param addr addr address to access to
 * @param data data to write
 * @param size size of the data to write in bytes
 */
void Execute::writeDataMem(uint32_t addr, uint32_t data, int size) {
  tlm::tlm_generic_payload trans;
  sc_time delay = SC_ZERO_TIME;

  trans.set_command( tlm::TLM_WRITE_COMMAND );
  trans.set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
  trans.set_data_length( size );
  trans.set_streaming_width( 4 ); // = data_length to indicate no streaming
  trans.set_byte_enable_ptr( 0 ); // 0 indicates unused
  trans.set_dmi_allowed( false ); // Mandatory initial value
  trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
  trans.set_address( addr );

  data_bus->b_transport( trans, delay);
}

void Execute::RaiseException(uint32_t cause, uint32_t inst) {
  uint32_t new_pc, current_pc, m_cause;

  current_pc = regs->getPC();
  m_cause = regs->getCSR(CSR_MSTATUS);
  m_cause |= cause;

  new_pc = regs->getCSR(CSR_MTVEC);

  regs->setCSR(CSR_MEPC, current_pc );

  if (cause == EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION) {
    regs->setCSR(CSR_MTVAL, inst);
  } else {
    regs->setCSR(CSR_MTVAL, current_pc );
  }

  regs->setCSR(CSR_MCAUSE, cause);
  regs->setCSR(CSR_MSTATUS, m_cause);

  regs->setPC( new_pc);

  log->SC_log(Log::ERROR) << "Exception! new PC 0x" << hex << new_pc << endl;

  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();
  SC_REPORT_ERROR("Exception" , "Exception");
}


void Execute::TLB_reserve(uint32_t address) {
  TLB_A_Entries.insert(address);
  return;
}

bool Execute::TLB_reserved(uint32_t address) {
  if (TLB_A_Entries.count(address) == 1) {
    TLB_A_Entries.erase(address);
    return true;
  } else {
    return false;
  }
}
