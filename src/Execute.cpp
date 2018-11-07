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

void Execute::LUI(Instruction &inst) {
  int rd;
  uint32_t imm = 0;

  rd = inst.get_rd();
  imm = inst.get_imm_U() << 12;
  regs->setValue(rd, imm);
  log->SC_log(Log::INFO) << dec << "LUI x"
          << rd << " <- 0x" << hex << imm << endl;

}

void Execute::AUIPC(Instruction &inst) {
  int rd;
  uint32_t imm = 0;
  int new_pc;

  rd = inst.get_rd();
  imm = inst.get_imm_U() << 12;
  new_pc = regs->getPC() + imm;

  regs->setValue(rd, new_pc);

  log->SC_log(Log::INFO) << dec << "AUIPC x"
          << rd << " <- " << imm << " + PC (0x" << hex
          << new_pc << ")" << endl;
}

void Execute::JAL(Instruction &inst, bool c_extension, int m_rd) {
  int16_t mem_addr = 0;
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

  log->SC_log(Log::INFO) << dec << "JAL: x"
          << rd << " <- 0x" << hex << old_pc << dec
          << ". PC + " << mem_addr << " -> PC (0x"
          << hex << new_pc << ")" << endl;
}

void Execute::JALR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int new_pc, old_pc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  mem_addr = inst.get_imm_I();

  old_pc = regs->getPC();
  regs->setValue(rd, old_pc + 4);

  new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << dec << "JALR: x"
          << rd << " <- 0x" << hex << old_pc + 4
          << " PC <- 0x" << hex << new_pc << endl;
}

void Execute::BEQ(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if (regs->getValue(rs1) == regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
    std::cout << "HERE new_pc" << new_pc << std::endl;
  } else {
    regs->incPC();
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "BEQ x" << dec
          << rs1 << "(" << regs->getValue(rs1) << ") == x"
          << rs2 << "(" << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::BNE(Instruction &inst) {
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
          << rs1 << "(" <<  val1 << ") == x"
          << rs2  << "(" << val2 << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::BLT(Instruction &inst) {
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
          << rs1 << "(" << (int32_t)regs->getValue(rs1) << ") < x"
          << rs2 << "(" << (int32_t)regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::BGE(Instruction &inst) {
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
          << rs1 << "(" << (int32_t)regs->getValue(rs1) << ") > x"
          << rs2 << "(" << (int32_t)regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::BLTU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if (regs->getValue(rs1) < regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
    new_pc = regs->getPC();
  }

  log->SC_log(Log::INFO) << "BLTU x"
          << rs1 << "(" << regs->getValue(rs1) << ") < x"
          << rs2 << "(" << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::BGEU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  if (regs->getValue(rs1) >= regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.get_imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BGEU x" << dec
          << rs1 << "(" << regs->getValue(rs1) << ") > x"
          << rs2 << "(" << regs->getValue(rs2) << ")? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::LB(Instruction &inst) {
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
}

void Execute::LH(Instruction &inst) {
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
}

void Execute::LW(Instruction &inst, bool c_extension) {
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
          << rs1 << " + " << imm << " (@0x" << hex
          << mem_addr << dec << ") -> x" << rd << endl;
}

void Execute::LBU(Instruction &inst) {
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
          << hex <<mem_addr << dec << ") -> x" << rd << endl;
}

void Execute::LHU(Instruction &inst) {
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

  log->SC_log(Log::INFO) << "LHU: x"
          << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> x" << rd << endl;
}

void Execute::SB(Instruction &inst) {
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

  log->SC_log(Log::INFO) << "SB: x"
          << rs2 << " -> x" << rs1 << " + " << imm
          << " (@0x" << hex <<mem_addr << dec << ")" << endl;
}

void Execute::SH(Instruction &inst) {
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

  log->SC_log(Log::INFO) << "SH: x"
          << rs2 << " -> x"
          << rs1 << " + " << imm << " (@0x" << hex
          << mem_addr << dec << ")" << endl;
}

void Execute::SW(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();
  imm = inst.get_imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << dec << "SW: x"
          << rs2 << "(0x" << hex << data << ") -> x" << dec
          << rs1 << " + " << imm << " (@0x" << hex
          << mem_addr << dec << ")" << endl;
}

void Execute::ADDI(Instruction &inst, bool c_extension) {
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

  log->SC_log(Log::INFO) << dec << "ADDI: x"
          << rs1 << " + " << imm << " -> x"
          << rd  << "(" << calc << ")"<< endl;
}

void Execute::SLTI(Instruction &inst) {
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
}

void Execute::SLTIU(Instruction &inst) {
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
}

void Execute::XORI(Instruction &inst) {
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
}

void Execute::ORI(Instruction &inst) {
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
}

void Execute::ANDI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  imm = inst.get_imm_I();

  calc = regs->getValue(rs1) & imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ANDI: x"
          << rs1 << " AND "
          << imm << " -> x"
          << rd  << endl;
}

void Execute::SLLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SLLI: x"
          << rs1 << " << " << shift << " -> x"
          << rd  << endl;
}

void Execute::SRLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRLI: x"
          << rs1 << " >> " << shift << " -> x"
          << rd  << endl;
}

void Execute::SRAI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  shift = rs2 & 0x1F;

  calc = regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRAI: x"
          << rs1 << " >> " << shift << " -> x"
          << rd  << endl;
}

void Execute::ADD(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;
  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ADD: x"
          << rs1 << " + x"
          << rs2 << " -> x"
          << rd  << endl;
}

void Execute::SUB(Instruction &inst) {
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
          << rd  << endl;
}

void Execute::SLL(Instruction &inst) {
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
}

void Execute::SLT(Instruction &inst) {
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
}

void Execute::SLTU(Instruction &inst) {
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
}

void Execute::XOR(Instruction &inst) {
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
}

void Execute::SRL(Instruction &inst) {
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
}

void Execute::SRA(Instruction &inst) {
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
}

void Execute::OR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) | regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "OR: x" << rs1 << " OR x" << rs2
          << "-> x" << rd << endl;
}

void Execute::AND(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  rs2 = inst.get_rs2();

  calc = regs->getValue(rs1) & regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "AND: x" << rs1 << " AND x" << rs2
          << "-> x" << rd << endl;
}

void Execute::FENCE(Instruction &inst) {
  log->SC_log(Log::INFO) << "FENCE" << endl;
}

void Execute::ECALL(Instruction &inst) {

  log->SC_log(Log::INFO) << "ECALL" << endl;
  std::cout << "ECALL Instruction called, stopping simulation" << endl;
  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();

  SC_REPORT_ERROR("Execute", "ECALL");
}

void Execute::CSRRW(Instruction &inst) {
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

  log->SC_log(Log::INFO) << "CSRRW: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << "-> CSR #" << csr << endl;
}

void Execute::CSRRS(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rd == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = regs->getValue(rs1);
  aux = aux | bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRS: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr << endl;
}

void Execute::CSRRC(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rd == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = regs->getValue(rs1);
  aux = aux & ~bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRC: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr << endl;
}

void Execute::CSRRWI(Instruction &inst) {
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
}

void Execute::CSRRSI(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rs1 == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = rs1;
  aux = aux | bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRSI: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr << endl;
}

void Execute::CSRRCI(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.get_rd();
  rs1 = inst.get_rs1();
  csr = inst.get_csr();

  if (rs1 == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = rs1;
  aux = aux & ~bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRCI: CSR #"
          << csr << " -> x" << rd
          << ". x" << rs1 << " & CSR #" << csr << endl;
}

void Execute::MRET(Instruction &inst) {
  uint32_t new_pc = 0;

  new_pc = regs->getCSR(0x341);
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "MRET: PC <- 0x" << hex << new_pc << endl;
}

void Execute::C_JR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1;
  int new_pc;

  C_Instruction c_inst(inst.getInstr());

  rs1 = c_inst.get_rs1();
  mem_addr = 0;

  std::cout << "rs1 :" << rs1 << std::endl;

  new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "JR: PC <- 0x" << hex << new_pc << endl;
}

void Execute::C_MV(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rd();
  rs1 = 0;
  rs2 = c_inst.get_rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "MV: x" << dec
          << rs1 << "(" << regs->getValue(rs1) << ") + x"
          << rs2 << "(" << regs->getValue(rs2) << ") -> x"
          << rd  << "(" << calc << ")" << endl;
}

void Execute::C_LWSP(Instruction &inst) {
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
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") + "
          << dec << imm << " (@0x" << hex << mem_addr << dec << ") -> x"
          << rd << "(" << hex << data << ")"<< dec << endl;
}

void Execute::C_ADDI4SPN(Instruction  &inst) {
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = c_inst.get_rdp();
  rs1 = 2;
  imm = c_inst.get_imm_ADDI4SPN();

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << dec << "ADDI4SPN: x"
          << rs1 << "(0x" << hex << regs->getValue(rs1) << ") + "
          << dec << imm << " -> x"
          << rd << "(0x" << hex << calc << ")" << endl;
}

void Execute::C_ADDI16SP(Instruction &inst) {
  // addi x2, x2, nzimm[9:4]
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  C_Instruction c_inst(inst.getInstr());

  rd = 2;
  rs1 = 2;
  imm = c_inst.get_imm_ADDI16SP();

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << dec << "ADDI16SP: x"
          << rs1 << " + "
          << dec << imm << " -> x"
          << rd  << "(0x" << hex << calc << ")" << endl;
}

void Execute::C_SWSP(Instruction &inst) {
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
}

void Execute::C_BEQZ(Instruction &inst) {
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
}

void Execute::C_BNEZ(Instruction &inst) {
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
          << rs1 << "(" <<  val1 << ") != 0? -> PC (0x"
          << hex << new_pc << ")" << dec << endl;
}

void Execute::C_LI(Instruction &inst) {

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
}



void Execute::NOP(Instruction &inst) {
  cout << endl;
  regs->dump();
  cout << "Simulation time " << sc_time_stamp() << endl;
  perf->dump();

  SC_REPORT_ERROR("Execute", "NOP");
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
