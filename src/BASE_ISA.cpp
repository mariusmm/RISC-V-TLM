/*!
 \file BASE_ISA.cpp
 \brief RISC-V ISA implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BASE_ISA.h"

typedef enum {
	LUI = 0b0110111,
	AUIPC = 0b0010111,
	JAL = 0b1101111,
	JALR = 0b1100111,

	BEQ = 0b1100011,
	BEQ_F = 0b000,
	BNE_F = 0b001,
	BLT_F = 0b100,
	BGE_F = 0b101,
	BLTU_F = 0b110,
	BGEU_F = 0b111,

	LB = 0b0000011,
	LB_F = 0b000,
	LH_F = 0b001,
	LW_F = 0b010,
	LBU_F = 0b100,
	LHU_F = 0b101,

	SB = 0b0100011,
	SB_F = 0b000,
	SH_F = 0b001,
	SW_F = 0b010,

	ADDI = 0b0010011,
	ADDI_F = 0b000,
	SLTI_F = 0b010,
	SLTIU_F = 0b011,
	XORI_F = 0b100,
	ORI_F = 0b110,
	ANDI_F = 0b111,
	SLLI_F = 0b001,
	SRLI_F = 0b101,
	SRLI_F7 = 0b0000000,
	SRAI_F7 = 0b0100000,

	ADD = 0b0110011,
	ADD_F = 0b000,
	SUB_F = 0b000,
	ADD_F7 = 0b0000000,
	SUB_F7 = 0b0100000,

	SLL_F = 0b001,
	SLT_F = 0b010,
	SLTU_F = 0b011,
	XOR_F = 0b100,
	SRL_F = 0b101,
	SRA_F = 0b101,
	SRL_F7 = 0b0000000,
	SRA_F7 = 0b0100000,
	OR_F = 0b110,
	AND_F = 0b111,

	FENCE = 0b0001111,
	ECALL = 0b1110011,
	ECALL_F = 0b000000000000,
	EBREAK_F = 0b000000000001,
	URET_F = 0b000000000010,
	SRET_F = 0b000100000010,
	MRET_F = 0b001100000010,
	WFI_F = 0b000100000101,
	SFENCE_F = 0b0001001,

	ECALL_F3 = 0b000,
	CSRRW = 0b001,
	CSRRS = 0b010,
	CSRRC = 0b011,
	CSRRWI = 0b101,
	CSRRSI = 0b110,
	CSRRCI = 0b111,
} Codes;

bool BASE_ISA::Exec_LUI() {
	int rd;
	uint32_t imm = 0;

	rd = get_rd();
	imm = get_imm_U() << 12;
	regs->setValue(rd, imm);

	FILE_LOG(logINFO) << "LUI x" << std::dec << rd << " <- 0x" << std::hex
			<< imm << std::endl;

	return true;
}

bool BASE_ISA::Exec_AUIPC() {
	int rd;
	uint32_t imm = 0;
	int new_pc;

	rd = get_rd();
	imm = get_imm_U() << 12;
	new_pc = regs->getPC() + imm;

	regs->setValue(rd, new_pc);

		FILE_LOG(logINFO) << "AUIPC x" << std::dec << rd << " <- 0x"
			<< std::hex << imm << " + PC (0x" << new_pc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_JAL() {
	int32_t mem_addr = 0;
	int rd;
	int new_pc, old_pc;

	rd = get_rd();
	mem_addr = get_imm_J();
	old_pc = regs->getPC();
	new_pc = old_pc + mem_addr;

	regs->setPC(new_pc);

	old_pc = old_pc + 4;
	regs->setValue(rd, old_pc);

		FILE_LOG(logINFO) << "JAL: x" << std::dec << rd << " <- 0x" << std::hex
			<< old_pc << std::dec << ". PC + 0x" << std::hex << mem_addr
			<< " -> PC (0x" << new_pc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_JALR() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int new_pc, old_pc;

	rd = get_rd();
	rs1 = get_rs1();
	mem_addr = get_imm_I();

	old_pc = regs->getPC();
	regs->setValue(rd, old_pc + 4);

	new_pc = (regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE;
	regs->setPC(new_pc);

		FILE_LOG(logINFO) << "JALR: x" << std::dec << rd << " <- 0x"
			<< std::hex << old_pc + 4 << " PC <- 0x" << new_pc << std::endl;

		return true;
}

bool BASE_ISA::Exec_BEQ() {
	int rs1, rs2;
	int new_pc = 0;

	rs1 = get_rs1();
	rs2 = get_rs2();

	if (regs->getValue(rs1) == regs->getValue(rs2)) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
		new_pc = regs->getPC();
	}

		FILE_LOG(logINFO) << "BEQ x" << std::dec << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") == x" << std::dec << rs2 << "(0x"
			<< std::hex << regs->getValue(rs2) << ")? -> PC (0x" << std::hex
			<< new_pc << ")" << std::dec << std::endl;


	return true;
}

bool BASE_ISA::Exec_BNE() {
	int rs1, rs2;
	int new_pc = 0;
	uint32_t val1, val2;

	rs1 = get_rs1();
	rs2 = get_rs2();

	val1 = regs->getValue(rs1);
	val2 = regs->getValue(rs2);

	if (val1 != val2) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
		new_pc = regs->getPC();
	}

		FILE_LOG(logINFO) << "BNE: x" << std::dec << rs1 << "(0x" << std::hex
			<< val1 << ") == x" << std::dec << rs2 << "(0x" << std::hex << val2
			<< ")? -> PC (0x" << std::hex << new_pc << ")" << std::dec
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_BLT() {
	int rs1, rs2;
	int new_pc = 0;

	rs1 = get_rs1();
	rs2 = get_rs2();

	if ((int32_t) regs->getValue(rs1) < (int32_t) regs->getValue(rs2)) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
	}

		FILE_LOG(logINFO) << "BLT x" << std::dec << rs1 << "(0x" << std::hex
			<< (int32_t) regs->getValue(rs1) << ") < x" << std::dec << rs2
			<< "(0x" << std::hex << (int32_t) regs->getValue(rs2)
			<< ")? -> PC (0x" << std::hex << new_pc << ")" << std::dec
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_BGE() {
	int rs1, rs2;
	int new_pc = 0;

	rs1 = get_rs1();
	rs2 = get_rs2();

	if ((int32_t) regs->getValue(rs1) >= (int32_t) regs->getValue(rs2)) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
	}

		FILE_LOG(logINFO) << "BGE x" << std::dec << rs1 << "(0x" << std::hex
			<< (int32_t) regs->getValue(rs1) << ") > x" << std::dec << rs2
			<< "(0x" << std::hex << (int32_t) regs->getValue(rs2)
			<< ")? -> PC (0x" << std::hex << new_pc << ")" << std::dec
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_BLTU() {
	int rs1, rs2;
	int new_pc = 0;

	rs1 = get_rs1();
	rs2 = get_rs2();

	if ((uint32_t) regs->getValue(rs1) < (uint32_t) regs->getValue(rs2)) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
		new_pc = regs->getPC();
	}

		FILE_LOG(logINFO) << "BLTU x" << std::dec << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") < x" << std::dec << rs2 << "(0x"
			<< std::hex << regs->getValue(rs2) << ")? -> PC (0x" << std::hex
			<< new_pc << ")" << std::dec << std::endl;

	return true;
}

bool BASE_ISA::Exec_BGEU() {
	int rs1, rs2;
	int new_pc = 0;

	rs1 = get_rs1();
	rs2 = get_rs2();

	if ((uint32_t) regs->getValue(rs1) >= (uint32_t) regs->getValue(rs2)) {
		new_pc = regs->getPC() + get_imm_B();
		regs->setPC(new_pc);
	} else {
		regs->incPC();
	}

		FILE_LOG(logINFO) << "BGEU x" << std::dec << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") > x" << std::dec << rs2 << "(0x"
			<< std::hex << regs->getValue(rs2) << ")? -> PC (0x" << std::hex
			<< new_pc << ")" << std::dec << std::endl;

	return true;
}

bool BASE_ISA::Exec_LB() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	int8_t data;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 1);
	regs->setValue(rd, data);

		FILE_LOG(logINFO) << "LB: x" << rs1 << " + " << imm << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_LH() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	int16_t data;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 2);
	regs->setValue(rd, data);

		FILE_LOG(logINFO) << "LH: x" << rs1 << " + " << imm << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_LW() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	uint32_t data;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);
	regs->setValue(rd, data);

		FILE_LOG(logINFO) << std::dec << "LW: x" << rs1 << "(0x" << std::hex
			<< regs->getValue(rs1) << ") + " << std::dec << imm << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::hex
			<< " (0x" << data << ")" << std::endl;
	return true;
}

bool BASE_ISA::Exec_LBU() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	uint8_t data;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 1);
	regs->setValue(rd, data);

		FILE_LOG(logINFO) << "LBU: x" << rs1 << " + " << imm << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::endl;
	return true;
}

bool BASE_ISA::Exec_LHU() {
	uint32_t mem_addr = 0;
	int rd, rs1;
	int32_t imm = 0;
	uint16_t data;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	mem_addr = imm + regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 2);
	regs->setValue(rd, data);

		FILE_LOG(logINFO) << "LHU: x" << std::dec << rs1 << " + " << imm
			<< " (@0x" << std::hex << mem_addr << std::dec << ") -> x" << rd
			<< "(0x" << std::hex << data << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SB() {
	uint32_t mem_addr = 0;
	int rs1, rs2;
	int32_t imm = 0;
	uint32_t data;

	rs1 = get_rs1();
	rs2 = get_rs2();
	imm = get_imm_S();

	mem_addr = imm + regs->getValue(rs1);
	data = regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 1);

		FILE_LOG(logINFO) << "SB: x" << std::dec << rs2 << " -> x" << rs1
			<< " + 0x" << std::hex << imm << " (@0x" << std::hex << mem_addr
			<< std::dec << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SH() {
	uint32_t mem_addr = 0;
	int rs1, rs2;
	int32_t imm = 0;
	uint32_t data;

	rs1 = get_rs1();
	rs2 = get_rs2();
	imm = get_imm_S();

	mem_addr = imm + regs->getValue(rs1);
	data = regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 2);

		FILE_LOG(logINFO) << "SH: x" << std::dec << rs2 << " -> x" << rs1
			<< " + 0x" << std::hex << imm << " (@0x" << std::hex << mem_addr
			<< std::dec << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SW() {
	uint32_t mem_addr = 0;
	int rs1, rs2;
	int32_t imm = 0;
	uint32_t data;

	rs1 = get_rs1();
	rs2 = get_rs2();
	imm = get_imm_S();

	mem_addr = imm + regs->getValue(rs1);
	data = regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

		FILE_LOG(logINFO) << "SW: x" << std::dec << rs2 << "(0x" << std::hex
			<< data << ") -> x" << std::dec << rs1 << " + 0x" << std::hex << imm
			<< " (@0x" << std::hex << mem_addr << std::dec << ")" << std::endl;
	return true;
}

bool BASE_ISA::Exec_ADDI() {
	int rd, rs1;
	int32_t imm = 0;
	int32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	calc = regs->getValue(rs1) + imm;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "ADDI: x" << std::dec << rs1 << " + " << imm
			<< " -> x" << std::dec << rd << "(0x" << std::hex << calc << ")"
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_SLTI() {
	int rd, rs1;
	int32_t imm;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	if (regs->getValue(rs1) < imm) {
		regs->setValue(rd, 1);
		FILE_LOG(logINFO) << "SLTI: x" << rs1 << " < " << imm << " => "
				<< "1 -> x" << rd << std::endl;
	} else {
		regs->setValue(rd, 0);
		FILE_LOG(logINFO) << "SLTI: x" << rs1 << " < " << imm << " => "
				<< "0 -> x" << rd << std::endl;
	}

	return true;
}

bool BASE_ISA::Exec_SLTIU() {
	int rd, rs1;
	int32_t imm;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	if ((uint32_t) regs->getValue(rs1) < (uint32_t) imm) {
		regs->setValue(rd, 1);
		FILE_LOG(logINFO) << "SLTIU: x" << rs1 << " < " << imm << " => "
				<< "1 -> x" << rd << std::endl;
	} else {
		regs->setValue(rd, 0);
		FILE_LOG(logINFO) << "SLTIU: x" << rs1 << " < " << imm << " => "
				<< "0 -> x" << rd << std::endl;
	}

	return true;
}

bool BASE_ISA::Exec_XORI() {
	int rd, rs1;
	int32_t imm;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	calc = regs->getValue(rs1) ^ imm;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "XORI: x" << rs1 << " XOR " << imm << "-> x" << rd
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_ORI() {
	int rd, rs1;
	int32_t imm;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	calc = regs->getValue(rs1) | imm;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "ORI: x" << rs1 << " OR " << imm << "-> x" << rd
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_ANDI() {
	int rd, rs1;
	uint32_t imm;
	uint32_t calc;
	uint32_t aux;

	rd = get_rd();
	rs1 = get_rs1();
	imm = get_imm_I();

	aux = regs->getValue(rs1);
	calc = aux & imm;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "ANDI: x" << rs1 << "(0x" << std::hex << aux
			<< ") AND 0x" << imm << " -> x" << std::dec << rd << "(0x"
			<< std::hex << calc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SLLI() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_shamt();

	if (rs2 >= 0x20) {
		std::cout << "ILEGAL INSTRUCTION, shamt[5] != 0" << std::endl;
		RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, m_instr);

		return false;
	}

	shift = rs2 & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) << shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SLLI: x" << std::dec << rs1 << " << " << shift
			<< " -> x" << rd << "(0x" << std::hex << calc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SRLI() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	shift = rs2 & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) >> shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SRLI: x" << std::dec << rs1 << " >> " << shift
			<< " -> x" << rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_SRAI() {
	int rd, rs1, rs2;
	uint32_t shift;
	int32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	shift = rs2 & 0x1F;

	calc = regs->getValue(rs1) >> shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SRAI: x" << std::dec << rs1 << " >> " << shift
			<< " -> x" << rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_ADD() {
	int rd, rs1, rs2;
	uint32_t calc;
	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) + regs->getValue(rs2);

	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "ADD: x" << std::dec << rs1 << " + x" << rs2
			<< " -> x" << rd << std::hex << "(0x" << calc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SUB() {
	int rd, rs1, rs2;
	uint32_t calc;
	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) - regs->getValue(rs2);
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SUB: x" << rs1 << " - x" << rs2 << " -> x" << rd
			<< "(" << calc << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SLL() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	shift = regs->getValue(rs2) & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) << shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SLL: x" << rs1 << " << " << shift << " -> x"
			<< rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_SLT() {
	int rd, rs1, rs2;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	if (regs->getValue(rs1) < regs->getValue(rs2)) {
		regs->setValue(rd, 1);
		FILE_LOG(logINFO) << "SLT: x" << rs1 << " < x" << rs2 << " => "
				<< "1 -> x" << rd << std::endl;
	} else {
		regs->setValue(rd, 0);
		FILE_LOG(logINFO) << "SLT: x" << rs1 << " < x" << rs2 << " => "
				<< "0 -> x" << rd << std::endl;
	}

	return true;
}

bool BASE_ISA::Exec_SLTU() {
	int rd, rs1, rs2;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	if ((uint32_t) regs->getValue(rs1) < (uint32_t) regs->getValue(rs2)) {
		regs->setValue(rd, 1);
		FILE_LOG(logINFO) << "SLTU: x" << rs1 << " < x" << rs2 << " => "
				<< "1 -> x" << rd << std::endl;
	} else {
		regs->setValue(rd, 0);
		FILE_LOG(logINFO) << "SLTU: x" << rs1 << " < x" << rs2 << " => "
				<< "0 -> x" << rd << std::endl;
	}

	return true;
}

bool BASE_ISA::Exec_XOR() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) ^ regs->getValue(rs2);
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "XOR: x" << rs1 << " XOR x" << rs2 << "-> x" << rd
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_SRL() {
	int rd, rs1, rs2;
	uint32_t shift;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	shift = regs->getValue(rs2) & 0x1F;

	calc = ((uint32_t) regs->getValue(rs1)) >> shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SRL: x" << rs1 << " >> " << shift << " -> x"
			<< rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_SRA() {
	int rd, rs1, rs2;
	uint32_t shift;
	int32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	shift = regs->getValue(rs2) & 0x1F;

	calc = regs->getValue(rs1) >> shift;
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "SRA: x" << rs1 << " >> " << shift << " -> x"
			<< rd << std::endl;

	return true;
}

bool BASE_ISA::Exec_OR() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) | regs->getValue(rs2);
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "OR: x" << rs1 << " OR x" << rs2 << "-> x" << rd
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_AND() {
	int rd, rs1, rs2;
	uint32_t calc;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	calc = regs->getValue(rs1) & regs->getValue(rs2);
	regs->setValue(rd, calc);

		FILE_LOG(logINFO) << "AND: x" << rs1 << " AND x" << rs2 << "-> x" << rd
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_FENCE() {
	FILE_LOG(logINFO) << "FENCE" << std::endl;

	return true;
}

bool BASE_ISA::Exec_ECALL() {

	FILE_LOG(logINFO) << "ECALL" << std::endl;
	std::cout << std::endl << "ECALL Instruction called, stopping simulation"
			<< std::endl;
	regs->dump();
	std::cout << "Simulation time " << sc_core::sc_time_stamp() << std::endl;
	perf->dump();

	uint32_t gp_value = regs->getValue(Registers::gp);
	if (gp_value == 1) {
		std::cout << "GP value is 1, test result is OK" << std::endl;
	} else {
		std::cout << "GP value is " << gp_value << std::endl;
	}
	//SC_REPORT_ERROR("Execute", "ECALL");
	sc_core::sc_stop();
	return true;
}

bool BASE_ISA::Exec_EBREAK() {

	FILE_LOG(logINFO) << "EBREAK" << std::endl;
	std::cout << std::endl << "EBRAK  Instruction called, dumping information"
			<< std::endl;
	regs->dump();
	std::cout << "Simulation time " << sc_core::sc_time_stamp() << std::endl;
	perf->dump();

	RaiseException(EXCEPTION_CAUSE_BREAKPOINT, m_instr);

	return true;
}

bool BASE_ISA::Exec_CSRRW() {
	int rd, rs1;
	int csr;
	uint32_t aux;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	/* These operations must be atomical */
	if (rd != 0) {
		aux = regs->getCSR(csr);
		regs->setValue(rd, aux);
	}

	aux = regs->getValue(rs1);
	regs->setCSR(csr, aux);

	FILE_LOG(logINFO) << std::hex << "CSRRW: CSR #" << csr << " -> x"
			<< std::dec << rd << ". x" << rs1 << "-> CSR #" << std::hex << csr
			<< " (0x" << aux << ")" << std::endl;

	return true;
}

bool BASE_ISA::Exec_CSRRS() {
	int rd, rs1;
	int csr;
	uint32_t bitmask, aux, aux2;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	if (rd == 0) {
		FILE_LOG(logINFO) << "CSRRS with rd1 == 0, doing nothing."
				<< std::endl;
		return false;
	}

	/* These operations must be atomical */
	aux = regs->getCSR(csr);
	bitmask = regs->getValue(rs1);

	regs->setValue(rd, aux);

	aux2 = aux | bitmask;
	regs->setCSR(csr, aux2);

	FILE_LOG(logINFO) << "CSRRS: CSR #" << csr << "(0x" << std::hex << aux
			<< ") -> x" << std::dec << rd << ". x" << rs1 << " & CSR #" << csr
			<< " <- 0x" << std::hex << aux2 << std::endl;

	return true;
}

bool BASE_ISA::Exec_CSRRC() {
	int rd, rs1;
	int csr;
	uint32_t bitmask, aux, aux2;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	if (rd == 0) {
		FILE_LOG(logINFO) << "CSRRC with rd1 == 0, doing nothing."
				<< std::endl;
		return true;
	}

	/* These operations must be atomical */
	aux = regs->getCSR(csr);
	bitmask = regs->getValue(rs1);

	regs->setValue(rd, aux);

	aux2 = aux & ~bitmask;
	regs->setCSR(csr, aux2);

	FILE_LOG(logINFO) << "CSRRC: CSR #" << csr << "(0x" << std::hex << aux
			<< ") -> x" << std::dec << rd << ". x" << rs1 << " & CSR #" << csr
			<< " <- 0x" << std::hex << aux2 << std::endl;

	return true;
}

bool BASE_ISA::Exec_CSRRWI() {
	int rd, rs1;
	int csr;
	uint32_t aux;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	/* These operations must be atomical */
	if (rd != 0) {
		aux = regs->getCSR(csr);
		regs->setValue(rd, aux);
	}
	aux = rs1;
	regs->setCSR(csr, aux);

	FILE_LOG(logINFO) << "CSRRWI: CSR #" << csr << " -> x" << rd << ". x"
			<< rs1 << "-> CSR #" << csr << std::endl;

	return true;
}

bool BASE_ISA::Exec_CSRRSI() {
	int rd, rs1;
	int csr;
	uint32_t bitmask, aux;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	if (rs1 == 0) {
		return true;
	}

	/* These operations must be atomical */
	aux = regs->getCSR(csr);
	regs->setValue(rd, aux);

	bitmask = rs1;
	aux = aux | bitmask;
	regs->setCSR(csr, aux);

	FILE_LOG(logINFO) << "CSRRSI: CSR #" << csr << " -> x" << rd << ". x"
			<< rs1 << " & CSR #" << csr << "(0x" << std::hex << aux << ")"
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_CSRRCI() {
	int rd, rs1;
	int csr;
	uint32_t bitmask, aux;

	rd = get_rd();
	rs1 = get_rs1();
	csr = get_csr();

	if (rs1 == 0) {
		return true;
	}

	/* These operations must be atomical */
	aux = regs->getCSR(csr);
	regs->setValue(rd, aux);

	bitmask = rs1;
	aux = aux & ~bitmask;
	regs->setCSR(csr, aux);

	FILE_LOG(logINFO) << "CSRRCI: CSR #" << csr << " -> x" << rd << ". x"
			<< rs1 << " & CSR #" << csr << "(0x" << std::hex << aux << ")"
			<< std::endl;

	return true;
}

/*********************** Privileged Instructions ******************************/

bool BASE_ISA::Exec_MRET() {
	uint32_t new_pc = 0;

	new_pc = regs->getCSR(CSR_MEPC);
	regs->setPC(new_pc);

	FILE_LOG(logINFO) << "MRET: PC <- 0x" << std::hex << new_pc
			<< std::endl;

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

bool BASE_ISA::Exec_SRET() {
	uint32_t new_pc = 0;

	new_pc = regs->getCSR(CSR_SEPC);
	regs->setPC(new_pc);

	FILE_LOG(logINFO) << "SRET: PC <- 0x" << std::hex << new_pc
			<< std::endl;

	return true;
}

bool BASE_ISA::Exec_WFI() {
	FILE_LOG(logINFO) << "WFI" << std::endl;

	return true;
}

bool BASE_ISA::Exec_SFENCE() {
	FILE_LOG(logINFO) << "SFENCE" << std::endl;

	return true;
}

bool BASE_ISA::process_instruction(Instruction &inst) {
	bool PC_not_affected = true;

	setInstr(inst.getInstr());

	switch (decode()) {
	case OP_LUI:
		Exec_LUI();
		break;
	case OP_AUIPC:
		Exec_AUIPC();
		break;
	case OP_JAL:
		Exec_JAL();
		PC_not_affected = false;
		break;
	case OP_JALR:
		Exec_JALR();
		PC_not_affected = false;
		break;
	case OP_BEQ:
		Exec_BEQ();
		PC_not_affected = false;
		break;
	case OP_BNE:
		Exec_BNE();
		PC_not_affected = false;
		break;
	case OP_BLT:
		Exec_BLT();
		PC_not_affected = false;
		break;
	case OP_BGE:
		Exec_BGE();
		PC_not_affected = false;
		break;
	case OP_BLTU:
		Exec_BLTU();
		PC_not_affected = false;
		break;
	case OP_BGEU:
		Exec_BGEU();
		PC_not_affected = false;
		break;
	case OP_LB:
		Exec_LB();
		break;
	case OP_LH:
		Exec_LH();
		break;
	case OP_LW:
		Exec_LW();
		break;
	case OP_LBU:
		Exec_LBU();
		break;
	case OP_LHU:
		Exec_LHU();
		break;
	case OP_SB:
		Exec_SB();
		break;
	case OP_SH:
		Exec_SH();
		break;
	case OP_SW:
		Exec_SW();
		break;
	case OP_ADDI:
		Exec_ADDI();
		break;
	case OP_SLTI:
		Exec_SLTI();
		break;
	case OP_SLTIU:
		Exec_SLTIU();
		break;
	case OP_XORI:
		Exec_XORI();
		break;
	case OP_ORI:
		Exec_ORI();
		break;
	case OP_ANDI:
		Exec_ANDI();
		break;
	case OP_SLLI:
		PC_not_affected = Exec_SLLI();
		break;
	case OP_SRLI:
		Exec_SRLI();
		break;
	case OP_SRAI:
		Exec_SRAI();
		break;
	case OP_ADD:
		Exec_ADD();
		break;
	case OP_SUB:
		Exec_SUB();
		break;
	case OP_SLL:
		Exec_SLL();
		break;
	case OP_SLT:
		Exec_SLT();
		break;
	case OP_SLTU:
		Exec_SLTU();
		break;
	case OP_XOR:
		Exec_XOR();
		break;
	case OP_SRL:
		Exec_SRL();
		break;
	case OP_SRA:
		Exec_SRA();
		break;
	case OP_OR:
		Exec_OR();
		break;
	case OP_AND:
		Exec_AND();
		break;
	case OP_FENCE:
		Exec_FENCE();
		break;
	case OP_ECALL:
		Exec_ECALL();
		break;
	case OP_EBREAK:
		Exec_EBREAK();
		break;
	case OP_CSRRW:
		Exec_CSRRW();
		break;
	case OP_CSRRS:
		Exec_CSRRS();
		break;
	case OP_CSRRC:
		Exec_CSRRC();
		break;
	case OP_CSRRWI:
		Exec_CSRRWI();
		break;
	case OP_CSRRSI:
		Exec_CSRRSI();
		break;
	case OP_CSRRCI:
		Exec_CSRRCI();
		break;
	case OP_MRET:
		Exec_MRET();
		PC_not_affected = false;
		break;
	case OP_SRET:
		Exec_SRET();
		PC_not_affected = false;
		break;
	case OP_WFI:
		Exec_WFI();
		break;
	case OP_SFENCE:
		Exec_SFENCE();
		break;
	default:
		std::cout << "Wrong instruction" << std::endl;
		inst.dump();
		NOP();
		//sc_stop();
		break;
	}

	return PC_not_affected;
}

opCodes BASE_ISA::decode() {
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
		switch (get_funct3()) {
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
		switch (get_funct3()) {
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
		switch (get_funct3()) {
		case SB_F:
			return OP_SB;
		case SH_F:
			return OP_SH;
		case SW_F:
			return OP_SW;
		}
		return OP_ERROR;
	case ADDI:
		switch (get_funct3()) {
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
			switch (get_funct7()) {
			case SRLI_F7:
				return OP_SRLI;
			case SRAI_F7:
				return OP_SRAI;
			}
			return OP_ERROR;
		}
		return OP_ERROR;
	case ADD: {
		switch (get_funct3()) {
		case ADD_F:
			switch (get_funct7()) {
			case ADD_F7:
				return OP_ADD;
			case SUB_F7:
				return OP_SUB;
			}
			;
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
			switch (get_funct7()) {
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
	case FENCE:
		return OP_FENCE;
	case ECALL: {
		switch (get_funct3()) {
		case ECALL_F3:
			switch (get_csr()) {
			case ECALL_F:
				return OP_ECALL;
			case EBREAK_F:
				return OP_EBREAK;
			case URET_F:
				return OP_URET;
			case SRET_F:
				return OP_SRET;
			case MRET_F:
				return OP_MRET;
			case WFI_F:
				return OP_WFI;
			case SFENCE_F:
				return OP_SFENCE;
			}
			if (m_instr.range(31, 25) == 0b0001001) {
				return OP_SFENCE;
			}
			break;
		case CSRRW:
			return OP_CSRRW;
			break;
		case CSRRS:
			return OP_CSRRS;
			break;
		case CSRRC:
			return OP_CSRRC;
			break;
		case CSRRWI:
			return OP_CSRRWI;
			break;
		case CSRRSI:
			return OP_CSRRSI;
			break;
		case CSRRCI:
			return OP_CSRRCI;
			break;
		}
	}
		break;
	default:
		return OP_ERROR;
	}

	return OP_ERROR;
}
