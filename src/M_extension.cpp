/*!
 \file M_extension.h
 \brief Implement M extensions part of the RISC-V
 \author Màrius Montón
 \date November 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#include "M_extension.h"

op_M_Codes M_extension::decode() const {

	switch (opcode()) {
	case M_MUL:
		return OP_M_MUL;
		break;
	case M_MULH:
		return OP_M_MULH;
		break;
	case M_MULHSU:
		return OP_M_MULHSU;
		break;
	case M_MULHU:
		return OP_M_MULHU;
		break;
	case M_DIV:
		return OP_M_DIV;
		break;
	case M_DIVU:
		return OP_M_DIVU;
		break;
	case M_REM:
		return OP_M_REM;
		break;
	case M_REMU:
		return OP_M_REMU;
		break;
	[[unlikely]] default:
		return OP_M_ERROR;
		break;
	}

	return OP_M_ERROR;
}

bool M_extension::Exec_M_MUL() const {
	int rd, rs1, rs2;
	int32_t multiplier, multiplicand;
	int64_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	multiplier = regs->getValue(rs1);
	multiplicand = regs->getValue(rs2);

	result = (int64_t) multiplier * multiplicand;
	result = result & 0x00000000FFFFFFFF;
	regs->setValue(rd, static_cast<int32_t>(result));

	log->SC_log(Log::INFO) << std::dec << "MUL: x" << rs1 << " * x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_MULH() const {
	int rd, rs1, rs2;
	int32_t multiplier, multiplicand;
	int64_t result;
	int32_t ret_value;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	multiplier = regs->getValue(rs1);
	multiplicand = regs->getValue(rs2);

	result = (int64_t) multiplier * (int64_t) multiplicand;

	ret_value = (int32_t) ((result >> 32) & 0x00000000FFFFFFFF);
	regs->setValue(rd, ret_value);

	log->SC_log(Log::INFO) << std::dec << "MULH: x" << rs1 << " * x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_MULHSU() const {
	int rd, rs1, rs2;
	int32_t multiplier;
	uint32_t multiplicand;
	int64_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	multiplier = regs->getValue(rs1);
	multiplicand = regs->getValue(rs2);

	result = static_cast<int64_t>(multiplier * (uint64_t) multiplicand);
	result = (result >> 32) & 0x00000000FFFFFFFF;
	regs->setValue(rd, static_cast<int32_t>(result));

	log->SC_log(Log::INFO) << std::dec << "MULHSU: x" << rs1 << " * x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_MULHU() const {
	int rd, rs1, rs2;
	uint32_t multiplier, multiplicand;
	uint64_t result;
	int32_t ret_value;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	multiplier = (uint32_t) regs->getValue(rs1);
	multiplicand = (uint32_t) regs->getValue(rs2);

	result = (uint64_t) multiplier * (uint64_t) multiplicand;
	ret_value = static_cast<int32_t>((result >> 32) & 0x00000000FFFFFFFF);
	regs->setValue(rd, ret_value);

	log->SC_log(Log::INFO) << std::dec << "MULHU: x" << rs1 << " * x" << rs2
			<< " -> x" << rd << "(" << ret_value << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_DIV() const {
	int rd, rs1, rs2;
	int32_t divisor, dividend;
	int64_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	dividend = regs->getValue(rs1);
	divisor = regs->getValue(rs2);

	if (divisor == 0) {
		result = -1;
	} else if ((divisor == -1) && (dividend == (int32_t) 0x80000000)) {
		result = 0x0000000080000000;
	} else {
		result = dividend / divisor;
		result = result & 0x00000000FFFFFFFF;
	}

	regs->setValue(rd, static_cast<int32_t>(result));

	log->SC_log(Log::INFO) << std::dec << "DIV: x" << rs1 << " / x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_DIVU() const {
	int rd, rs1, rs2;
	uint32_t divisor, dividend;
	uint64_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	dividend = regs->getValue(rs1);
	divisor = regs->getValue(rs2);

	if (divisor == 0) {
		result = -1;
	} else {
		result = dividend / divisor;
		result = result & 0x00000000FFFFFFFF;
	}

	regs->setValue(rd, static_cast<int32_t>(result));

	log->SC_log(Log::INFO) << std::dec << "DIVU: x" << rs1 << " / x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_REM() const {
	int rd, rs1, rs2;
	int32_t divisor, dividend;
	int32_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	dividend = regs->getValue(rs1);
	divisor = regs->getValue(rs2);

	if (divisor == 0) {
		result = dividend;
	} else if ((divisor == -1) && (dividend == (int32_t) 0x80000000)) {
		result = 0;
	} else {
		result = dividend % divisor;
	}

	regs->setValue(rd, result);

	log->SC_log(Log::INFO) << std::dec << "REM: x" << rs1 << " / x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::Exec_M_REMU() const {
	int rd, rs1, rs2;
	uint32_t divisor, dividend;
	uint32_t result;

	rd = get_rd();
	rs1 = get_rs1();
	rs2 = get_rs2();

	dividend = regs->getValue(rs1);
	divisor = regs->getValue(rs2);

	if (divisor == 0) {
		result = dividend;
	} else {
		result = dividend % divisor;
	}

	regs->setValue(rd, static_cast<int32_t>(result));

	log->SC_log(Log::INFO) << std::dec << "REMU: x" << rs1 << " / x" << rs2
			<< " -> x" << rd << "(" << result << ")" << "\n";

	return true;
}

bool M_extension::process_instruction(Instruction *inst) {
	bool PC_not_affected = true;

	setInstr(inst->getInstr());

	switch (decode()) {
	case OP_M_MUL:
		Exec_M_MUL();
		break;
	case OP_M_MULH:
		Exec_M_MULH();
		break;
	case OP_M_MULHSU:
		Exec_M_MULHSU();
		break;
	case OP_M_MULHU:
		Exec_M_MULHU();
		break;
	case OP_M_DIV:
		Exec_M_DIV();
		break;
	case OP_M_DIVU:
		Exec_M_DIVU();
		break;
	case OP_M_REM:
		Exec_M_REM();
		break;
	case OP_M_REMU:
		Exec_M_REMU();
		break;
	[[unlikely]] default:
		std::cout << "M instruction not implemented yet" << "\n";
		inst->dump();
		//NOP(inst);
		sc_core::sc_stop();
		break;
	}

	return PC_not_affected;
}
