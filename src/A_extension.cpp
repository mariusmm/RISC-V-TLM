/*!
 \file A_extension.h
 \brief Implement A extensions part of the RISC-V
 \author Màrius Montón
 \date December 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "A_extension.h"

op_A_Codes A_extension::decode() const {

	switch (opcode()) {
	case A_LR:
		return OP_A_LR;
		break;
	case A_SC:
		return OP_A_SC;
		break;
	case A_AMOSWAP:
		return OP_A_AMOSWAP;
		break;
	case A_AMOADD:
		return OP_A_AMOADD;
		break;
	case A_AMOXOR:
		return OP_A_AMOXOR;
		break;
	case A_AMOAND:
		return OP_A_AMOAND;
		break;
	case A_AMOOR:
		return OP_A_AMOOR;
		break;
	case A_AMOMIN:
		return OP_A_AMOMIN;
		break;
	case A_AMOMAX:
		return OP_A_AMOMAX;
		break;
	case A_AMOMINU:
		return OP_A_AMOMINU;
		break;
	case A_AMOMAXU:
		return OP_A_AMOMAXU;
		break;
	[[unlikely]] default:
		return OP_A_ERROR;
		break;

	}

	return OP_A_ERROR;
}

bool A_extension::Exec_A_LR() {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	if (rs2 != 0) {
		std::cout << "ILEGAL INSTRUCTION, LR.W: rs2 != 0" << std::endl;
		RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, m_instr);

		return false;
	}

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);
	regs->setValue(rd, data);

	TLB_reserve(mem_addr);

	log->SC_log(Log::INFO) << std::dec << "LR.W: x" << rs1 << " (@0x"
			<< std::hex << mem_addr << std::dec << ") -> x" << rd << std::endl;

	return true;
}

bool A_extension::Exec_A_SC() {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = regs->getValue(rs2);

	if (TLB_reserved(mem_addr) == true) {
		mem_intf->writeDataMem(mem_addr, data, 4);
		regs->setValue(rd, 0);  // SC writes 0 to rd on success
	} else {
		regs->setValue(rd, 1);  // SC writes nonzero on failure
	}

	log->SC_log(Log::INFO) << std::dec << "SC.W: (@0x" << std::hex << mem_addr
			<< std::dec << ") <- x" << rs2 << std::hex << "(0x" << data << ")"
			<< std::endl;

	return true;
}

bool A_extension::Exec_A_AMOSWAP() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;
	uint32_t aux;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// swap
	aux = regs->getValue(rs2);
	regs->setValue(rs2, data);

	mem_intf->writeDataMem(mem_addr, aux, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOSWAP " << std::endl;
	return true;
}

bool A_extension::Exec_A_AMOADD() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// add
	data = data + regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOADD " << std::endl;

	return true;
}

bool A_extension::Exec_A_AMOXOR() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// add
	data = data ^ regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOXOR " << std::endl;

	return true;
}
bool A_extension::Exec_A_AMOAND() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// add
	data = data & regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOAND " << std::endl;

	return true;
}

bool A_extension::Exec_A_AMOOR() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// add
	data = data | regs->getValue(rs2);

	mem_intf->writeDataMem(mem_addr, data, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOOR " << std::endl;
	return true;
}

bool A_extension::Exec_A_AMOMIN() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;
	uint32_t aux;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// min
	aux = regs->getValue(rs2);
	if ((int32_t) data < (int32_t) aux) {
		aux = data;
	}

	mem_intf->writeDataMem(mem_addr, aux, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOMIN " << std::endl;

	return true;
}
bool A_extension::Exec_A_AMOMAX() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;
	uint32_t aux;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// >
	aux = regs->getValue(rs2);
	if ((int32_t) data > (int32_t) aux) {
		aux = data;
	}

	mem_intf->writeDataMem(mem_addr, aux, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOMAX " << std::endl;

	return true;
}
bool A_extension::Exec_A_AMOMINU() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;
	uint32_t aux;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// min
	aux = regs->getValue(rs2);
	if (data < aux) {
		aux = data;
	}

	mem_intf->writeDataMem(mem_addr, aux, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOMINU " << std::endl;

	return true;
}
bool A_extension::Exec_A_AMOMAXU() const {
	uint32_t mem_addr = 0;
	int rd, rs1, rs2;
	uint32_t data;
	uint32_t aux;

	/* These instructions must be atomic */

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	mem_addr = regs->getValue(rs1);
	data = mem_intf->readDataMem(mem_addr, 4);

	regs->setValue(rd, data);

	// max
	aux = regs->getValue(rs2);
	if (data > aux) {
		aux = data;
	}

	mem_intf->writeDataMem(mem_addr, aux, 4);

	log->SC_log(Log::INFO) << std::dec << "AMOMAXU " << std::endl;

	return true;
}

void A_extension::TLB_reserve(uint32_t address) {
	TLB_A_Entries.insert(address);
	return;
}

bool A_extension::TLB_reserved(uint32_t address) {
	if (TLB_A_Entries.count(address) == 1) {
		TLB_A_Entries.erase(address);
		return true;
	} else {
		return false;
	}
}

bool A_extension::process_instruction(Instruction *inst) {
	bool PC_not_affected = true;

	setInstr(inst->getInstr());

	switch (decode()) {
	case OP_A_LR:
		Exec_A_LR();
		break;
	case OP_A_SC:
		Exec_A_SC();
		break;
	case OP_A_AMOSWAP:
		Exec_A_AMOSWAP();
		break;
	case OP_A_AMOADD:
		Exec_A_AMOADD();
		break;
	case OP_A_AMOXOR:
		Exec_A_AMOXOR();
		break;
	case OP_A_AMOAND:
		Exec_A_AMOAND();
		break;
	case OP_A_AMOOR:
		Exec_A_AMOOR();
		break;
	case OP_A_AMOMIN:
		Exec_A_AMOMIN();
		break;
	case OP_A_AMOMAX:
		Exec_A_AMOMAX();
		break;
	case OP_A_AMOMINU:
		Exec_A_AMOMINU();
		break;
	case OP_A_AMOMAXU:
		Exec_A_AMOMAXU();
		break;
	[[unlikely]] default:
		std::cout << "A instruction not implemented yet" << std::endl;
		inst->dump();
		NOP();
		break;
	}

	return PC_not_affected;
}
