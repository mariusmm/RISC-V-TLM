#include "CPU.h"

SC_HAS_PROCESS(CPU);
CPU::CPU(sc_module_name name, uint32_t PC) :
		sc_module(name), instr_bus("instr_bus") {
	register_bank = new Registers();
	exec = new Execute("Execute", register_bank);
	perf = Performance::getInstance();
	log = Log::getInstance();

	register_bank->setPC(PC);

	//register_bank->setValue(Registers::sp, (0xD0000 / 4) - 1);
	register_bank->setValue(Registers::sp, (0x10000000 / 4) - 1);

	irq_line_socket.register_b_transport(this, &CPU::call_interrupt);
	interrupt = false;

	int_cause = 0;
	irq_already_down = false;

	dmi_ptr_valid = false;
	instr_bus.register_invalidate_direct_mem_ptr( this, &CPU::invalidate_direct_mem_ptr);

	SC_THREAD(CPU_thread);
}

CPU::~CPU() {
	cout << "*********************************************" << endl;
	register_bank->dump();
	cout << "end time: " << sc_time_stamp() << endl;
	perf->dump();
	cout << "*********************************************" << endl;
}

bool CPU::cpu_process_IRQ() {
	uint32_t csr_temp;
	uint32_t new_pc, old_pc;
	bool ret_value = false;

	if (interrupt == true) {
		csr_temp = register_bank->getCSR(CSR_MSTATUS);
		if (csr_temp & MSTATUS_MIE) {
		} else {
			log->SC_log(Log::DEBUG) << "interrupt delayed" << endl;
			return ret_value;
		}

		csr_temp = register_bank->getCSR(CSR_MIP);

		if ((csr_temp & MIP_MEIP) == 0) {
			csr_temp |= MIP_MEIP;  // MEIP bit in MIP register (11th bit)
			register_bank->setCSR(CSR_MIP, csr_temp);
			log->SC_log(Log::DEBUG) << "Interrupt!" << endl;

			/* updated MEPC register */
			old_pc = register_bank->getPC();
			register_bank->setCSR(CSR_MEPC, old_pc);
			log->SC_log(Log::INFO) << "Old PC Value 0x" << hex << old_pc
					<< endl;

			/* update MCAUSE register */
			register_bank->setCSR(CSR_MCAUSE, 0x80000000);

			/* set new PC address */
			new_pc = register_bank->getCSR(CSR_MTVEC);
			//new_pc = new_pc & 0xFFFFFFFC; // last two bits always to 0
			log->SC_log(Log::DEBUG) << "NEW PC Value 0x" << hex << new_pc
					<< endl;
			register_bank->setPC(new_pc);

			ret_value = true;
			interrupt = false;
			irq_already_down = false;
		}
	} else {
		if (irq_already_down == false) {
			csr_temp = register_bank->getCSR(CSR_MIP);
			csr_temp &= ~MIP_MEIP;
			register_bank->setCSR(CSR_MIP, csr_temp);
			irq_already_down = true;
		}
	}

	return ret_value;
}

bool CPU::process_c_instruction(Instruction &inst) {
	bool PC_not_affected = true;

	C_Instruction c_inst(inst.getInstr());

	switch (c_inst.decode()) {
	case OP_C_ADDI4SPN:
		PC_not_affected = exec->C_ADDI4SPN(inst);
		break;
	case OP_C_LW:
		exec->LW(inst, true);
		break;
	case OP_C_SW:
		exec->SW(inst, true);
		break;
	case OP_C_ADDI:
		exec->ADDI(inst, true);
		break;
	case OP_C_JAL:
		exec->JAL(inst, true, 1);
		PC_not_affected = false;
		break;
	case OP_C_J:
		exec->JAL(inst, true, 0);
		PC_not_affected = false;
		break;
	case OP_C_LI:
		exec->C_LI(inst);
		break;
	case OP_C_SLLI:
		exec->C_SLLI(inst);
		break;
	case OP_C_LWSP:
		exec->C_LWSP(inst);
		break;
	case OP_C_JR:
		exec->C_JR(inst);
		PC_not_affected = false;
		break;
	case OP_C_MV:
		exec->C_MV(inst);
		break;
	case OP_C_JALR:
		exec->JALR(inst, true);
		PC_not_affected = false;
		break;
	case OP_C_ADD:
		exec->C_ADD(inst);
		break;
	case OP_C_SWSP:
		exec->C_SWSP(inst);
		break;
	case OP_C_ADDI16SP:
		exec->C_ADDI16SP(inst);
		break;
	case OP_C_BEQZ:
		exec->C_BEQZ(inst);
		PC_not_affected = false;
		break;
	case OP_C_BNEZ:
		exec->C_BNEZ(inst);
		PC_not_affected = false;
		break;
	case OP_C_SRLI:
		exec->C_SRLI(inst);
		break;
	case OP_C_SRAI:
		exec->C_SRAI(inst);
		break;
	case OP_C_ANDI:
		exec->C_ANDI(inst);
		break;
	case OP_C_SUB:
		exec->C_SUB(inst);
		break;
	case OP_C_XOR:
		exec->C_XOR(inst);
		break;
	case OP_C_OR:
		exec->C_OR(inst);
		break;
	case OP_C_AND:
		exec->C_AND(inst);
		break;
	default:
		std::cout << "C instruction not implemented yet" << endl;
		inst.dump();
		exec->NOP(inst);
		//sc_stop();
		break;

	}

	return PC_not_affected;
}

bool CPU::process_m_instruction(Instruction &inst) {
	bool PC_not_affected = true;

	M_Instruction m_inst(inst.getInstr());

	switch (m_inst.decode()) {
	case OP_M_MUL:
		exec->M_MUL(inst);
		break;
	case OP_M_MULH:
		exec->M_MULH(inst);
		break;
	case OP_M_MULHSU:
		exec->M_MULHSU(inst);
		break;
	case OP_M_MULHU:
		exec->M_MULHU(inst);
		break;
	case OP_M_DIV:
		exec->M_DIV(inst);
		break;
	case OP_M_DIVU:
		exec->M_DIVU(inst);
		break;
	case OP_M_REM:
		exec->M_REM(inst);
		break;
	case OP_M_REMU:
		exec->M_REMU(inst);
		break;
	default:
		std::cout << "M instruction not implemented yet" << endl;
		inst.dump();
		exec->NOP(inst);
		break;
	}

	return PC_not_affected;
}

bool CPU::process_a_instruction(Instruction inst) {
	bool PC_not_affected = true;

	A_Instruction a_inst(inst.getInstr());

	switch (a_inst.decode()) {
	case OP_A_LR:
		exec->A_LR(inst);
		break;
	case OP_A_SC:
		exec->A_SC(inst);
		break;
	case OP_A_AMOSWAP:
		exec->A_AMOSWAP(inst);
		break;
	case OP_A_AMOADD:
		exec->A_AMOADD(inst);
		break;
	case OP_A_AMOXOR:
		exec->A_AMOXOR(inst);
		break;
	case OP_A_AMOAND:
		exec->A_AMOAND(inst);
		break;
	case OP_A_AMOOR:
		exec->A_AMOOR(inst);
		break;
	case OP_A_AMOMIN:
		exec->A_AMOMIN(inst);
		break;
	case OP_A_AMOMAX:
		exec->A_AMOMAX(inst);
		break;
	case OP_A_AMOMINU:
		exec->A_AMOMINU(inst);
		break;
	case OP_A_AMOMAXU:
		exec->A_AMOMAXU(inst);
		break;
	default:
		std::cout << "A instruction not implemented yet" << endl;
		inst.dump();
		exec->NOP(inst);
		break;
	}

	return PC_not_affected;
}

bool CPU::process_base_instruction(Instruction &inst) {
	bool PC_not_affected = true;

	switch (inst.decode()) {
	case OP_LUI:
		exec->LUI(inst);
		break;
	case OP_AUIPC:
		exec->AUIPC(inst);
		break;
	case OP_JAL:
		exec->JAL(inst);
		PC_not_affected = false;
		break;
	case OP_JALR:
		exec->JALR(inst);
		PC_not_affected = false;
		break;
	case OP_BEQ:
		exec->BEQ(inst);
		PC_not_affected = false;
		break;
	case OP_BNE:
		exec->BNE(inst);
		PC_not_affected = false;
		break;
	case OP_BLT:
		exec->BLT(inst);
		PC_not_affected = false;
		break;
	case OP_BGE:
		exec->BGE(inst);
		PC_not_affected = false;
		break;
	case OP_BLTU:
		exec->BLTU(inst);
		PC_not_affected = false;
		break;
	case OP_BGEU:
		exec->BGEU(inst);
		PC_not_affected = false;
		break;
	case OP_LB:
		exec->LB(inst);
		break;
	case OP_LH:
		exec->LH(inst);
		break;
	case OP_LW:
		exec->LW(inst);
		break;
	case OP_LBU:
		exec->LBU(inst);
		break;
	case OP_LHU:
		exec->LHU(inst);
		break;
	case OP_SB:
		exec->SB(inst);
		break;
	case OP_SH:
		exec->SH(inst);
		break;
	case OP_SW:
		exec->SW(inst);
		break;
	case OP_ADDI:
		exec->ADDI(inst);
		break;
	case OP_SLTI:
		exec->SLTI(inst);
		break;
	case OP_SLTIU:
		exec->SLTIU(inst);
		break;
	case OP_XORI:
		exec->XORI(inst);
		break;
	case OP_ORI:
		exec->ORI(inst);
		break;
	case OP_ANDI:
		exec->ANDI(inst);
		break;
	case OP_SLLI:
		PC_not_affected = exec->SLLI(inst);
		break;
	case OP_SRLI:
		exec->SRLI(inst);
		break;
	case OP_SRAI:
		exec->SRAI(inst);
		break;
	case OP_ADD:
		exec->ADD(inst);
		break;
	case OP_SUB:
		exec->SUB(inst);
		break;
	case OP_SLL:
		exec->SLL(inst);
		break;
	case OP_SLT:
		exec->SLT(inst);
		break;
	case OP_SLTU:
		exec->SLTU(inst);
		break;
	case OP_XOR:
		exec->XOR(inst);
		break;
	case OP_SRL:
		exec->SRL(inst);
		break;
	case OP_SRA:
		exec->SRA(inst);
		break;
	case OP_OR:
		exec->OR(inst);
		break;
	case OP_AND:
		exec->AND(inst);
		break;
	case OP_FENCE:
		exec->FENCE(inst);
		break;
	case OP_ECALL:
		exec->ECALL(inst);
		break;
	case OP_EBREAK:
		exec->EBREAK(inst);
		break;
	case OP_CSRRW:
		exec->CSRRW(inst);
		break;
	case OP_CSRRS:
		exec->CSRRS(inst);
		break;
	case OP_CSRRC:
		exec->CSRRC(inst);
		break;
	case OP_CSRRWI:
		exec->CSRRWI(inst);
		break;
	case OP_CSRRSI:
		exec->CSRRSI(inst);
		break;
	case OP_CSRRCI:
		exec->CSRRCI(inst);
		break;

	case OP_MRET:
		exec->MRET(inst);
		PC_not_affected = false;
		break;
	case OP_SRET:
		exec->SRET(inst);
		PC_not_affected = false;
		break;
	case OP_WFI:
		exec->WFI(inst);
		break;
	case OP_SFENCE:
		exec->SFENCE(inst);
		break;
	default:
		std::cout << "Wrong instruction" << endl;
		inst.dump();
		exec->NOP(inst);
		//sc_stop();
		break;
	}
     
	return PC_not_affected;
}

/**
 * main thread for CPU simulation
 * @brief CPU mai thread
 */
void CPU::CPU_thread(void) {

	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
	uint32_t INSTR;
	sc_time delay = SC_ZERO_TIME;
	bool PC_not_affected = false;
	bool incPCby2 = false;
	tlm::tlm_dmi dmi_data;
	unsigned char* dmi_ptr = NULL;

	trans->set_command(tlm::TLM_READ_COMMAND);
	trans->set_data_ptr(reinterpret_cast<unsigned char*>(&INSTR));
	trans->set_data_length(4);
	trans->set_streaming_width(4); // = data_length to indicate no streaming
	trans->set_byte_enable_ptr(0); // 0 indicates unused
	trans->set_dmi_allowed(false); // Mandatory initial value
	trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

	//if (trans->is_dmi_allowed() ) {
//	if (true) {
//		dmi_ptr_valid = instr_bus->get_direct_mem_ptr(*trans, dmi_data);
//		dmi_ptr = dmi_data.get_dmi_ptr();
//	}
	//register_bank->dump();

	while (1) {
		/* Get new PC value */
		//cout << "CPU: PC 0x" << hex << (uint32_t) register_bank->getPC() << endl;
		//dmi_ptr_valid = true;
		if (dmi_ptr_valid == true) {
			/* if memory_offset at Memory module is set, this won't work */
			memcpy(&INSTR, dmi_ptr + register_bank->getPC(), 4);
		} else {
			trans->set_address(register_bank->getPC());
			instr_bus->b_transport(*trans, delay);

			if (trans->is_response_error()) {
				SC_REPORT_ERROR("CPU base", "Read memory");
			}

			if (trans->is_dmi_allowed()) {
                dmi_ptr_valid = instr_bus->get_direct_mem_ptr(*trans, dmi_data);
                if (dmi_ptr_valid) {
                    std::cout << "Get DMI_PTR " << std::endl;
                    dmi_ptr = dmi_data.get_dmi_ptr();
                }
            }
		}

		perf->codeMemoryRead();

		log->SC_log(Log::INFO) << "PC: 0x" << hex << register_bank->getPC()
				<< ". ";

		Instruction inst(INSTR);

		/* check what type of instruction is and execute it */
		switch (inst.check_extension()) {
		case BASE_EXTENSION:
			PC_not_affected = process_base_instruction(inst);
			incPCby2 = false;
			break;
		case C_EXTENSION:
			PC_not_affected = process_c_instruction(inst);
			incPCby2 = true;
			break;
		case M_EXTENSION:
			PC_not_affected = process_m_instruction(inst);
			incPCby2 = false;
			break;
		case A_EXTENSION:
			PC_not_affected = process_a_instruction(inst);
			incPCby2 = false;
			break;
		default:
			std::cout << "Extension not implemented yet" << std::endl;
			inst.dump();
			exec->NOP(inst);
		} // switch (inst.check_extension())

		perf->instructionsInc();

		if (PC_not_affected == true) {
			register_bank->incPC(incPCby2);
		}

		/* Process IRQ (if any) */
		cpu_process_IRQ();

		/* Fixed instruction time to 10 ns (i.e. 100 MHz)*/
		sc_core::wait(10, SC_NS);
	} // while(1)
} // CPU_thread

void CPU::call_interrupt(tlm::tlm_generic_payload &trans, sc_time &delay) {
	interrupt = true;
	/* Socket caller send a cause (its id) */
	memcpy(&int_cause, trans.get_data_ptr(), sizeof(int));
}

void CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end)
{
	dmi_ptr_valid = false;
}
