/*!
 \file C_extension.cpp
 \brief Implement C extensions part of the RISC-V
 \author Màrius Montón
 \date August 2018
*/
#include "C_extension.h"

op_C_Codes C_extension::decode() const {

	switch (opcode()) {

	case 0b00:
		switch (get_funct3()) {
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
		[[unlikely]] default:
			return OP_C_ERROR;
			break;
		}
		break;

	case 0b01:
		switch (get_funct3()) {
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
			switch (m_instr.range(11, 10)) {
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
				switch (m_instr.range(6, 5)) {
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
		[[unlikely]] default:
			return OP_C_ERROR;
			break;
		}
		break;

	case 0b10:
		switch (get_funct3()) {
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
				if (m_instr.range(6, 2) == 0) {
					return OP_C_JR;
				} else {
					return OP_C_MV;
				}
			} else {
				if (m_instr.range(11, 2) == 0) {
					return OP_C_EBREAK;
				} else if (m_instr.range(6, 2) == 0) {
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
		[[unlikely]] default:
			return OP_C_ERROR;
			break;
		}
		break;

	[[unlikely]] default:

		return OP_C_ERROR;
		break;

	}
	return OP_C_ERROR;
}

bool C_extension::Exec_C_JR() {
	uint32_t mem_addr = 0;
	int rs1;
	int new_pc;

	rs1 = get_rs1();
	mem_addr = 0;

	new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
	regs->setPC(new_pc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "JR: PC <- 0x" << std::hex << new_pc << "\n";
	}

	return true;
}

bool C_extension::Exec_C_MV() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rd();
	rs1 = 0;
	rs2 = get_rs2();

	calc = regs->getValue(rs1) + regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.MV: x" << std::dec << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") + x" << std::dec << rs2 << "(0x"
			<< std::hex << regs->getValue(rs2) << ") -> x" << std::dec << rd
			<< "(0x" << std::hex << calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_ADD() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rs1();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) + regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.ADD: x" << std::dec << rs1 << " + x" << rs2
			<< " -> x" << rd << "(0x" << std::hex << calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_LWSP() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	uint32_t data;

	// lw rd, offset[7:2](x2)

	rd = get_rd();
	rs1 = 2;
	imm = get_imm_LWSP();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.LWSP: x" << std::dec << rs1 << " + " << imm
			<< " (@0x" << std::hex << mem_addr << std::dec << ") -> x" << rd
			<< "(" << std::hex << data << ")" << std::dec << "\n";
	}

	return true;
}

bool C_extension::Exec_C_ADDI4SPN() {
	int rd, rs1;
	int32_t imm = 0;
	int32_t calc;

	rd = get_rdp();
	rs1 = 2;
	imm = get_imm_ADDI4SPN();

	if (imm == 0) {
		RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, m_instr);
		return false;
	}

	calc = regs->getValue(rs1) + imm;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << std::dec << "C.ADDI4SPN: x" << rs1 << "(0x"
			<< std::hex << regs->getValue(rs1) << ") + " << std::dec << imm
			<< " -> x" << rd << "(0x" << std::hex << calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_ADDI16SP() {
	// addi x2, x2, nzimm[9:4]
	int rd;
	int32_t imm = 0;

	if (get_rd() == 2) {
	  int rs1;
	  int32_t calc;

		rd = 2;
		rs1 = 2;
		imm = get_imm_ADDI16SP();

		calc = regs->getValue(rs1) + imm;
		regs->setValue(rd, calc);

		log->SC_log(Log::INFO) << std::dec << "C.ADDI16SP: x" << rs1 << " + "
				<< std::dec << imm << " -> x" << rd << "(0x" << std::hex << calc
				<< ")" << "\n";
	} else {
		/* C.LUI OPCODE */
		rd = get_rd();
		imm = get_imm_LUI();
		regs->setValue(rd, imm);
		log->SC_log(Log::INFO) << std::dec << "C.LUI x" << rd << " <- 0x"
				<< std::hex << imm << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SWSP() {
	// sw rs2, offset(x2)
	uint32_t mem_addr = 0;
	int rs1, rs2;
	int32_t imm = 0;
	uint32_t data;

	rs1 = 2;
	rs2 = get_rs2();
	imm = get_imm_CSS();

	mem_addr = imm + regs->getValue(rs1);
	data = regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << std::dec << "C.SWSP: x" << rs2 << "(0x"
			<< std::hex << data << ") -> x" << std::dec << rs1 << " + " << imm
			<< " (@0x" << std::hex << mem_addr << std::dec << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_BEQZ() {
	int rs1;
	int new_pc = 0;
	uint32_t val1;

	rs1 = get_rs1p();
	val1 = regs->getValue(rs1);

	if (val1 == 0) {
		new_pc = regs->getPC() + get_imm_CB();
		regs->setPC(new_pc);
	} else {
		regs->incPC(true); //PC <- PC + 2
		new_pc = regs->getPC();
	}

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.BEQZ: x" << std::dec << rs1 << "(" << val1
			<< ") == 0? -> PC (0x" << std::hex << new_pc << ")" << std::dec
			<< "\n";
	}

	return true;
}

bool C_extension::Exec_C_BNEZ() {
	int rs1;
	int new_pc = 0;
	uint32_t val1;

	rs1 = get_rs1p();
	val1 = regs->getValue(rs1);

	if (val1 != 0) {
		new_pc = regs->getPC() + get_imm_CB();
		regs->setPC(new_pc);
	} else {
		regs->incPC(true); //PC <- PC +2
		new_pc = regs->getPC();
	}

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.BNEZ: x" << std::dec << rs1 << "(0x"
			<< std::hex << val1 << ") != 0? -> PC (0x" << std::hex << new_pc
			<< ")" << std::dec << "\n";
	}

	return true;
}

bool C_extension::Exec_C_LI() {
	int rd, rs1;
	int32_t imm = 0;
	int32_t calc;

	rd = get_rd();
	rs1 = 0;
	imm = get_imm_ADDI();

	calc = regs->getValue(rs1) + imm;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << std::dec << "C.LI: x" << rs1 << "("
			<< regs->getValue(rs1) << ") + " << imm << " -> x" << rd << "("
			<< calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SRLI() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2();

	shift = rs2 & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) >> shift;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.SRLI: x" << rs1 << " >> " << shift << " -> x"
			<< rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SRAI() {
	int rd, rs1, rs2;
	uint32_t shift;
	int32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2();

	shift = rs2 & 0x1F;

	calc = (int32_t) regs->getValue(rs1) >> shift;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.SRAI: x" << rs1 << " >> " << std::dec << shift
			<< " -> x" << rd << "(" << calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SLLI() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_imm_ADDI();

	shift = rs2 & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) << shift;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.SLLI: x" << std::dec << rs1 << " << " << shift
			<< " -> x" << rd << "(0x" << calc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_ANDI() {
	int rd, rs1;
	uint32_t imm;
	uint32_t aux;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	imm = get_imm_ADDI();

	aux = regs->getValue(rs1);
	calc = aux & imm;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.ANDI: x" << rs1 << "(" << aux << ") AND "
			<< imm << " -> x" << rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SUB() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2p();

	calc = regs->getValue(rs1) - regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.SUB: x" << std::dec << rs1 << " - x" << rs2
			<< " -> x" << rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_XOR() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2p();

	calc = regs->getValue(rs1) ^ regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.XOR: x" << std::dec << rs1 << " XOR x" << rs2
			<< "-> x" << rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_OR() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2p();

	calc = regs->getValue(rs1) | regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C_OR: x" << std::dec << rs1 << " OR x" << rs2
			<< "-> x" << rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_AND() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rs1p();
	rs1 = get_rs1p();
	rs2 = get_rs2p();

	calc = regs->getValue(rs1) & regs->getValue(rs2);
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.AND: x" << std::dec << rs1 << " AND x" << rs2
			<< "-> x" << rd << "\n";
	}

	return true;
}

bool C_extension::Exec_C_ADDI() const {
	int rd, rs1;
	int32_t imm = 0;
	int32_t calc;

	rd = get_rd();
	rs1 = rd;
	imm = get_imm_ADDI();

	calc = regs->getValue(rs1) + imm;
	regs->setValue(rd, calc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.ADDI: x" << std::dec << rs1 << " + " << imm
			<< " -> x" << std::dec << rd << "(0x" << std::hex << calc << ")"
			<< "\n";
	}

	return true;
}

bool C_extension::Exec_C_JALR() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int new_pc, old_pc;

	rd = 1;
	rs1 = get_rs1();

	old_pc = regs->getPC();
	regs->setValue(rd, old_pc + 2);

	new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
	regs->setPC(new_pc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.JALR: x" << std::dec << rd << " <- 0x"
			<< std::hex << old_pc + 4 << " PC <- 0x" << std::hex << new_pc
			<< "\n";
	}

	return true;
}

bool C_extension::Exec_C_LW() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	uint32_t data;

	rd = get_rdp();
	rs1 = get_rs1p();
	imm = get_imm_L();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);
	regs->setValue(rd, data);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << std::dec << "C.LW: x" << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") + " << std::dec << imm << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::hex
			<< " (0x" << data << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_SW() {
	uint32_t mem_addr = 0;
	int rs1, rs2;
	int32_t imm = 0;
	uint32_t data;

	rs1 = get_rs1p();
	rs2 = get_rs2p();
	imm = get_imm_L();

	mem_addr = imm + regs->getValue(rs1);
	data = regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.SW: x" << std::dec << rs2 << "(0x" << std::hex
			<< data << ") -> x" << std::dec << rs1 << " + 0x" << std::hex << imm
			<< " (@0x" << std::hex << mem_addr << std::dec << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_JAL(int m_rd) {
	int32_t mem_addr = 0;
	int rd;
	int new_pc, old_pc;

	rd = m_rd;
	mem_addr = get_imm_J();
	old_pc = regs->getPC();

	new_pc = old_pc + mem_addr;
	regs->setPC(new_pc);

	old_pc = old_pc + 2;
	regs->setValue(rd, old_pc);

	if (log->getLogLevel() >= Log::INFO) {
		log->SC_log(Log::INFO) << "C.JAL: x" << std::dec << rd << " <- 0x"
			<< std::hex << old_pc << std::dec << ". PC + 0x" << std::hex
			<< mem_addr << " -> PC (0x" << new_pc << ")" << "\n";
	}

	return true;
}

bool C_extension::Exec_C_EBREAK() {

	log->SC_log(Log::INFO) << "C.EBREAK" << "\n";
	std::cout << "\n" << "C.EBRAK  Instruction called, dumping information"
			<< "\n";
	regs->dump();
	std::cout << "Simulation time " << sc_core::sc_time_stamp() << "\n";
	perf->dump();

	RaiseException(EXCEPTION_CAUSE_BREAKPOINT, m_instr);
	NOP();
	return true;
}

bool C_extension::process_instruction(Instruction *inst) {
	bool PC_not_affected = true;

	setInstr(inst->getInstr());

	switch (decode()) {
	case OP_C_ADDI4SPN:
		PC_not_affected = Exec_C_ADDI4SPN();
		break;
	case OP_C_LW:
		Exec_C_LW();
		break;
	case OP_C_SW:
		Exec_C_SW();
		break;
	case OP_C_ADDI:
		Exec_C_ADDI();
		break;
	case OP_C_JAL:
		Exec_C_JAL(1);
		PC_not_affected = false;
		break;
	case OP_C_J:
		Exec_C_JAL(0);
		PC_not_affected = false;
		break;
	case OP_C_LI:
		Exec_C_LI();
		break;
	case OP_C_SLLI:
		Exec_C_SLLI();
		break;
	case OP_C_LWSP:
		Exec_C_LWSP();
		break;
	case OP_C_JR:
		Exec_C_JR();
		PC_not_affected = false;
		break;
	case OP_C_MV:
		Exec_C_MV();
		break;
	case OP_C_JALR:
		Exec_C_JALR();
		PC_not_affected = false;
		break;
	case OP_C_ADD:
		Exec_C_ADD();
		break;
	case OP_C_SWSP:
		Exec_C_SWSP();
		break;
	case OP_C_ADDI16SP:
		Exec_C_ADDI16SP();
		break;
	case OP_C_BEQZ:
		Exec_C_BEQZ();
		PC_not_affected = false;
		break;
	case OP_C_BNEZ:
		Exec_C_BNEZ();
		PC_not_affected = false;
		break;
	case OP_C_SRLI:
		Exec_C_SRLI();
		break;
	case OP_C_SRAI:
		Exec_C_SRAI();
		break;
	case OP_C_ANDI:
		Exec_C_ANDI();
		break;
	case OP_C_SUB:
		Exec_C_SUB();
		break;
	case OP_C_XOR:
		Exec_C_XOR();
		break;
	case OP_C_OR:
		Exec_C_OR();
		break;
	case OP_C_AND:
		Exec_C_AND();
		break;
	case OP_C_EBREAK:
		Exec_C_EBREAK();
		break;
	[[unlikely]] default:
		std::cout << "C instruction not implemented yet" << "\n";
		inst->dump();
		NOP();
		break;
	}

	return PC_not_affected;
}
