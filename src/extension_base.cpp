/*!
 \file extension_base.h
 \brief Base class for ISA extensions
 \author Màrius Montón
 \date May 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "extension_base.h"

extension_base::extension_base(const sc_dt::sc_uint<32>  & instr,
		Registers *register_bank, MemoryInterface *mem_interface) :
		m_instr(instr), regs(register_bank), mem_intf(mem_interface) {

	perf = Performance::getInstance();
	log = Log::getInstance();
}

extension_base::~extension_base() =default;

void extension_base::setInstr(uint32_t p_instr) {
	m_instr = sc_dt::sc_uint<32>(p_instr);
}

void extension_base::dump() const {
	std::cout << std::hex << "0x" << m_instr << std::dec << std::endl;
}

void extension_base::RaiseException(uint32_t cause, uint32_t inst) {
	uint32_t new_pc, current_pc, m_cause;

	current_pc = regs->getPC();
	m_cause = regs->getCSR(CSR_MSTATUS);
	m_cause |= cause;

	new_pc = regs->getCSR(CSR_MTVEC);

	regs->setCSR(CSR_MEPC, current_pc);

	if (cause == EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION) {
		regs->setCSR(CSR_MTVAL, inst);
	} else {
		regs->setCSR(CSR_MTVAL, current_pc);
	}

	regs->setCSR(CSR_MCAUSE, cause);
	regs->setCSR(CSR_MSTATUS, m_cause);

	regs->setPC(new_pc);

	log->SC_log(Log::ERROR) << "Exception! new PC 0x" << std::hex << new_pc
			<< std::endl << std::flush;
}

bool extension_base::NOP() {

	log->SC_log(Log::INFO) << "NOP" << "\n";
    sc_core::sc_stop();
	return true;
}
