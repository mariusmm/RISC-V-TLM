/*!
 \file Registers.cpp
 \brief Basic register file implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Registers.h"

Registers::Registers() {

	perf = Performance::getInstance();

	initCSR();

	//std::cout << "Memory size: 0x" << std::hex << Memory::SIZE << std::endl;
	//std::cout << "SP address: 0x" << std::hex << (0x10000000 / 4) - 1 << std::endl;

	register_bank[sp] = Memory::SIZE - 4; // default stack at the end of the memory
	register_PC = 0x80000000;       // default _start address
}

void Registers::dump(void) {
	std::cout << "************************************" << std::endl;
	std::cout << "Registers dump" << std::dec << std::endl;
	std::cout << "x0 (zero):  " << std::right << std::setw(11)
			<< register_bank[0];
	std::cout << " x1 (ra):    " << std::right << std::setw(11)
			<< register_bank[1];
	std::cout << " x2 (sp):    " << std::right << std::setw(11)
			<< register_bank[2];
	std::cout << " x3 (gp):    " << std::right << std::setw(11)
			<< register_bank[3] << std::endl;

	std::cout << "x4 (tp):    " << std::right << std::setw(11)
			<< register_bank[4];
	std::cout << " x5 (t0):    " << std::right << std::setw(11)
			<< register_bank[5];
	std::cout << " x6 (t1):    " << std::right << std::setw(11)
			<< register_bank[6];
	std::cout << " x7 (t2):    " << std::right << std::setw(11)
			<< register_bank[7] << std::endl;

	std::cout << "x8 (s0/fp): " << std::right << std::setw(11)
			<< register_bank[8];
	std::cout << " x9 (s1):    " << std::right << std::setw(11)
			<< register_bank[9];
	std::cout << " x10 (a0):   " << std::right << std::setw(11)
			<< register_bank[10];
	std::cout << " x11 (a1):   " << std::right << std::setw(11)
			<< register_bank[11] << std::endl;

	std::cout << "x12 (a2):   " << std::right << std::setw(11)
			<< register_bank[12];
	std::cout << " x13 (a3):   " << std::right << std::setw(11)
			<< register_bank[13];
	std::cout << " x14 (a4):   " << std::right << std::setw(11)
			<< register_bank[14];
	std::cout << " x15 (a5):   " << std::right << std::setw(11)
			<< register_bank[15] << std::endl;

	std::cout << "x16 (a6):   " << std::right << std::setw(11)
			<< register_bank[16];
	std::cout << " x17 (a7):   " << std::right << std::setw(11)
			<< register_bank[17];
	std::cout << " x18 (s2):   " << std::right << std::setw(11)
			<< register_bank[18];
	std::cout << " x19 (s3):   " << std::right << std::setw(11)
			<< register_bank[19] << std::endl;

	std::cout << "x20 (s4):   " << std::right << std::setw(11)
			<< register_bank[20];
	std::cout << " x21 (s5):   " << std::right << std::setw(11)
			<< register_bank[21];
	std::cout << " x22 (s6):   " << std::right << std::setw(11)
			<< register_bank[22];
	std::cout << " x23 (s7):   " << std::right << std::setw(11)
			<< register_bank[23] << std::endl;

	std::cout << "x24 (s8):   " << std::right << std::setw(11)
			<< register_bank[24];
	std::cout << " x25 (s9):   " << std::right << std::setw(11)
			<< register_bank[25];
	std::cout << " x26 (s10):  " << std::right << std::setw(11)
			<< register_bank[26];
	std::cout << " x27 (s11):  " << std::right << std::setw(11)
			<< register_bank[27] << std::endl;

	std::cout << "x28 (t3):   " << std::right << std::setw(11)
			<< register_bank[28];
	std::cout << " x29 (t4):   " << std::right << std::setw(11)
			<< register_bank[29];
	std::cout << " x30 (t5):   " << std::right << std::setw(11)
			<< register_bank[30];
	std::cout << " x31 (t6):   " << std::right << std::setw(11)
			<< register_bank[31] << std::endl;

	std::cout << "PC: 0x" << std::hex << register_PC << std::dec << std::endl;
	std::cout << "************************************" << std::endl;
}

void Registers::setValue(int reg_num, int32_t value) {
	if ((reg_num != 0) && (reg_num < 32)) {
		register_bank[reg_num] = value;
		perf->registerWrite();
	}
}

int32_t Registers::getValue(int reg_num) {
	if ((reg_num >= 0) && (reg_num < 32)) {
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
		ret_value = (uint64_t) (sc_core::sc_time(
				sc_core::sc_time_stamp()
						- sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
				& 0x00000000FFFFFFFF;
		break;
	case CSR_CYCLEH:
	case CSR_MCYCLEH:
		ret_value = (uint32_t) ((uint64_t) (sc_core::sc_time(
				sc_core::sc_time_stamp()
						- sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
				>> 32 & 0x00000000FFFFFFFF);
		break;
	case CSR_TIME:
		ret_value = (uint64_t) (sc_core::sc_time(
				sc_core::sc_time_stamp()
						- sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
				& 0x00000000FFFFFFFF;
		break;
	case CSR_TIMEH:
		ret_value = (uint32_t) ((uint64_t) (sc_core::sc_time(
				sc_core::sc_time_stamp()
						- sc_core::sc_time(sc_core::SC_ZERO_TIME)).to_double())
				>> 32 & 0x00000000FFFFFFFF);
		break;
	[[likely]] default:
		ret_value = CSR[csr];
		break;
	}
	return ret_value;
}

void Registers::setCSR(int csr, uint32_t value) {
	/* @FIXME: rv32mi-p-ma_fetch tests doesn't allow MISA to writable,
	 * but Volume II: Privileged Architecture v1.10 says MISA is writable (?)
	 */
	if (csr != CSR_MISA) {
		CSR[csr] = value;
	}
}

void Registers::initCSR() {
	CSR[CSR_MISA] = MISA_MXL | MISA_M_EXTENSION | MISA_C_EXTENSION
			| MISA_A_EXTENSION | MISA_I_BASE;
	CSR[CSR_MSTATUS] = MISA_MXL;
}
