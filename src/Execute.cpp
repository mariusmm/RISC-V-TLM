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

  rd = inst.rd();
  imm = inst.imm_U() << 12;
  regs->setValue(rd, imm);
  log->SC_log(Log::INFO) << "LUI R" << rd << " <- 0x" << hex << imm << endl;

}

void Execute::AUIPC(Instruction &inst) {
  int rd;
  uint32_t imm = 0;
  int new_pc;

  rd = inst.rd();
  imm = inst.imm_U() << 12;
  new_pc = regs->getPC() + imm;

  regs->setPC(new_pc);
  regs->setValue(rd, new_pc);

  log->SC_log(Log::INFO) << "AUIPC R" << rd << " + PC -> PC (" << new_pc << ")" << endl;
}

void Execute::JAL(Instruction &inst) {
  int32_t mem_addr = 0;
  int rd;
  int new_pc, old_pc;

  rd = inst.rd();
  mem_addr = inst.imm_J();

  old_pc = regs->getPC();


  new_pc = old_pc + mem_addr;
  regs->setPC(new_pc);

  old_pc = old_pc + 4;
  regs->setValue(rd, old_pc);

  log->SC_log(Log::INFO) << dec << "JAL: R" << rd << " <- 0x" << hex << old_pc
          << dec << " PC + " << mem_addr << " -> PC (0x"
          << hex << new_pc << ")" << endl;
}

void Execute::JALR(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int new_pc, old_pc;

  rd = inst.rd();
  rs1 = inst.rs1();
  mem_addr = inst.imm_I();

  old_pc = regs->getPC();
  regs->setValue(rd, old_pc + 4);


  new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
  regs->setPC(new_pc);

  log->SC_log(Log::INFO) << "JALR: R" << dec << rd << " <- 0x" << hex << old_pc + 4
          << " PC <- 0x" << hex << new_pc << endl;
}

void Execute::BEQ(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if (regs->getValue(rs1) == regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BEQ R" << rs1 << " == R" << rs2  << "? -> PC (" << new_pc << ")" << endl;

}

void Execute::BNE(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;
  uint32_t val1, val2;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  val1 = regs->getValue(rs1);
  val2 = regs->getValue(rs2);

  if (val1 != val2) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BNE: R" << rs1 << "(" <<  val1
          << ") == R"  << rs2  << "(" << val2 << ")? -> PC ("
          << new_pc << ")" << endl;
}

void Execute::BLT(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if ((int32_t)regs->getValue(rs1) < (int32_t)regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BLT R" << rs1 << " < R" << rs2  << "? -> PC (" << new_pc << ")" << endl;
}

void Execute::BGE(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if ((int32_t)regs->getValue(rs1) >= (int32_t)regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BGE R" << rs1 << "(" <<
          (int32_t)regs->getValue(rs1) << ") > R" <<
          rs2  << "(" << (int32_t)regs->getValue(rs2)
          << ")? -> PC (" << new_pc << ")" << endl;
}

void Execute::BLTU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if (regs->getValue(rs1) < regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BLTU R" << rs1 << " < R" << rs2  << "? -> PC (" << new_pc << ")" << endl;
}

void Execute::BGEU(Instruction &inst) {
  int rs1, rs2;
  int new_pc = 0;

  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if (regs->getValue(rs1) >= regs->getValue(rs2)) {
    new_pc = regs->getPC() + inst.imm_B();
    regs->setPC(new_pc);
  } else {
    regs->incPC();
  }

  log->SC_log(Log::INFO) << "BGEU R" << rs1 << " > R" << rs2  << "? -> PC (" << new_pc << ")" << endl;
}

void Execute::LB(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  int8_t data;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 1);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LB: R" << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> R" << rd << endl;
}

void Execute::LH(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  int16_t data;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 2);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LH: R" << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> R" << rd << endl;
}

void Execute::LW(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint32_t data;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 4);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LW: R" << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> R" << rd << endl;
}

void Execute::LBU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint8_t data;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 1);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LBU: R" << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> R" << rd << endl;
}

void Execute::LHU(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rd, rs1;
  int32_t imm = 0;
  uint16_t data;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  mem_addr = imm + regs->getValue(rs1);
  data = readDataMem(mem_addr, 2);
  regs->setValue(rd, data);

  log->SC_log(Log::INFO) << "LHU: R" << rs1 << " + " << imm << " (@0x"
          << hex <<mem_addr << dec << ") -> R" << rd << endl;
}

void Execute::SB(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.rs1();
  rs2 = inst.rs2();
  imm = inst.imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 1);

  log->SC_log(Log::INFO) << "SB: R" << rs2 << " -> R" << rs1 << " + "
          << imm << " (@0x" << hex <<mem_addr << dec << ")" <<  endl;
}

void Execute::SH(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.rs1();
  rs2 = inst.rs2();
  imm = inst.imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 2);

  log->SC_log(Log::INFO) << "SH: R" << rs2 << " -> R" << rs1 << " + "
          << imm << " (@0x" << hex <<mem_addr << dec << ")" <<  endl;
}

void Execute::SW(Instruction &inst) {
  uint32_t mem_addr = 0;
  int rs1, rs2;
  int32_t imm = 0;
  uint32_t data;

  rs1 = inst.rs1();
  rs2 = inst.rs2();
  imm = inst.imm_S();

  mem_addr = imm + regs->getValue(rs1);
  data = regs->getValue(rs2);

  writeDataMem(mem_addr, data, 4);

  log->SC_log(Log::INFO) << "SW: R" << dec << rs2 << "(0x" << hex << data
          << ") -> R" << dec << rs1 << " + " << imm
          << " (@0x" << hex << mem_addr << dec << ")" <<  endl;
}

void Execute::ADDI(Instruction &inst) {
  int rd, rs1;
  int32_t imm = 0;
  int32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  calc = regs->getValue(rs1) + imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << dec << "ADDI: R" << rs1 << " + " << imm << " -> R" << rd << endl;
}

void Execute::SLTI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  if (regs->getValue(rs1) < imm) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTI: R" << rs1 << " < " << imm
          << " => " << "1 -> R" << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTI: R" << rs1 << " < " << imm
          << " => " << "0 -> R" << rd << endl;
  }
}

void Execute::SLTIU(Instruction &inst) {
  int rd, rs1;
  int32_t imm;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  if ((uint32_t) regs->getValue(rs1) < (uint32_t)imm) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTIU: R" << rs1 << " < " << imm
          << " => " << "1 -> R" << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTIU: R" << rs1 << " < " << imm
          << " => " << "0 -> R" << rd << endl;
  }
}

void Execute::XORI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  calc = regs->getValue(rs1) ^ imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "XORI: R" << rs1 << " XOR " << imm
          << "-> R" << rd << endl;
}

void Execute::ORI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  calc = regs->getValue(rs1) | imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ORI: R" << rs1 << " OR " << imm
          << "-> R" << rd << endl;
}

void Execute::ANDI(Instruction &inst) {
  int rd, rs1;
  int32_t imm;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  imm = inst.imm_I();

  calc = regs->getValue(rs1) & imm;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ANDI: R" << rs1 << " AND " << imm
          << " -> R" << rd << endl;
}

void Execute::SLLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SLLI: R" << rs1 << " << " << shift << " -> R" << rd << endl;
}

void Execute::SRLI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = rs2 & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRLI: R" << rs1 << " >> " << shift << " -> R" << rd << endl;
}

void Execute::SRAI(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = rs2 & 0x1F;

  calc = regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRAI: R" << rs1 << " >> " << shift << " -> R" << rd << endl;
}

void Execute::ADD(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;
  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  calc = regs->getValue(rs1) + regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "ADD: R" << rs1 << " + R" << rs2 << " -> R" << rd << endl;
}

void Execute::SUB(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;
  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  calc = regs->getValue(rs1) - regs->getValue(rs2);
  regs->setValue(rd, calc);

  /* Can insert some arbitrary execution time */
  wait(sc_time(10, SC_NS));
  log->SC_log(Log::INFO) << "SUB: R" << rs1 << " - R" << rs2 << " -> R" << rd << endl;
}

void Execute::SLL(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) << shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SLL: R" << rs1 << " << " << shift << " -> R" << rd << endl;
}


/** */
void Execute::SLT(Instruction &inst) {
  int rd, rs1, rs2;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if (regs->getValue(rs1) < regs->getValue(rs2)) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLT: R" << rs1 << " < R" << rs2
          << " => " << "1 -> R" << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLT: R" << rs1 << " < R" << rs2
          << " => " << "0 -> R" << rd << endl;
  }
}


void Execute::SLTU(Instruction &inst) {
  int rd, rs1, rs2;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  if ( (uint32_t)regs->getValue(rs1) < (uint32_t)regs->getValue(rs2)) {
    regs->setValue(rd, 1);
    log->SC_log(Log::INFO) << "SLTU: R" << rs1 << " < R" << rs2
          << " => " << "1 -> R" << rd << endl;
  } else {
    regs->setValue(rd, 0);
    log->SC_log(Log::INFO) << "SLTU: R" << rs1 << " < R" << rs2
          << " => " << "0 -> R" << rd << endl;
  }
}


void Execute::XOR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  calc = regs->getValue(rs1) ^ regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "XOR: R" << rs1 << " XOR R" << rs2
          << "-> R" << rd << endl;
}



void Execute::SRL(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = ((uint32_t)regs->getValue(rs1)) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRL: R" << rs1 << " >> " << shift << " -> R" << rd << endl;
}

void Execute::SRA(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t shift;
  int32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  shift = regs->getValue(rs2) & 0x1F;

  calc = regs->getValue(rs1) >> shift;
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "SRA: R" << rs1 << " >> " << shift << " -> R" << rd << endl;
}


void Execute::OR(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  calc = regs->getValue(rs1) | regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "OR: R" << rs1 << " OR R" << rs2
          << "-> R" << rd << endl;
}


void Execute::AND(Instruction &inst) {
  int rd, rs1, rs2;
  uint32_t calc;

  rd = inst.rd();
  rs1 = inst.rs1();
  rs2 = inst.rs2();

  calc = regs->getValue(rs1) & regs->getValue(rs2);
  regs->setValue(rd, calc);

  log->SC_log(Log::INFO) << "AND: R" << rs1 << " AND R" << rs2
          << "-> R" << rd << endl;
}

void Execute::CSRRW(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t aux;

  rd = inst.rd();
  rs1 = inst.rs1();
  csr = inst.csr();

  if (rd == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);
  aux = regs->getValue(rs1);
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRW: CSR #" << csr << " -> R" << rd
          << ". R" << rs1 << "-> CSR #" << csr << endl;
}

void Execute::CSRRS(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.rd();
  rs1 = inst.rs1();
  csr = inst.csr();

  if (rd == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = regs->getValue(rs1);
  aux = aux | bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRS: CSR #" << csr << " -> R" << rd
          << ". R" << rs1 << " & CSR #" << csr << endl;
}

void Execute::CSRRC(Instruction &inst) {
  int rd, rs1;
  int csr;
  uint32_t bitmask, aux;

  rd = inst.rd();
  rs1 = inst.rs1();
  csr = inst.csr();

  if (rd == 0) {
    return;
  }

  /* These operations must be atomical */
  aux = regs->getCSR(csr);
  regs->setValue(rd, aux);

  bitmask = regs->getValue(rs1);
  aux = aux & ~bitmask;
  regs->setCSR(csr, aux);

  log->SC_log(Log::INFO) << "CSRRC: CSR #" << csr << " -> R" << rd
          << ". R" << rs1 << " & CSR #" << csr << endl;
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
