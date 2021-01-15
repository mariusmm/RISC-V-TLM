/*!
 \file CPU.h
 \brief Main CPU class
 \author Màrius Montón
 \date July 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CPU_BASE_H
#define CPU_BASE_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

#include "memory.h"
#include "MemoryInterface.h"
#include "BASE_ISA.h"
#include "Registers.h"
#include "Log.h"
#include "Instruction.h"
#include "C_extension.h"
#include "M_extension.h"
#include "A_extension.h"

/**
 * @brief ISC_V CPU 64 bitsmodel
 * @param name name of the module
 */
class CPU64: sc_core::sc_module {
public:

	/**
	 * @brief Instruction Memory bus socket
	 * @param trans transction to perfoem
	 * @param delay time to annotate
	 */
	tlm_utils::simple_initiator_socket<CPU64> instr_bus;

	/**
	 * @brief IRQ line socket
	 * @param trans transction to perform (empty)
	 * @param delay time to annotate
	 */
	tlm_utils::simple_target_socket<CPU64> irq_line_socket;

	/**
	 * @brief Constructor
	 * @param name Module name
	 * @param PC   Program Counter initialize value
	 */
	CPU64(sc_core::sc_module_name name, uint32_t PC);

	/**
	 * @brief Destructor
	 */
	~CPU64();

	MemoryInterface *mem_intf;

private:
	Registers *register_bank;
	Performance *perf;
	Log *log;
	Instruction *inst;
	C_extension *c_inst;
	M_extension *m_inst;
	A_extension *a_inst;
	BASE_ISA *exec;

	tlm_utils::tlm_quantumkeeper *m_qk;

	bool interrupt;
	uint32_t int_cause;
	bool irq_already_down;
	sc_core::sc_time default_time;
	bool dmi_ptr_valid;

	/**
	 *
	 * @brief Process and triggers IRQ if all conditions met
	 * @return true if IRQ is triggered, false otherwise
	 */
	bool cpu_process_IRQ();

	/**
	 * main thread for CPU simulation
	 * @brief CPU mai thread
	 */
	void CPU_thread(void);

	/**
	 * @brief callback for IRQ simple socket
	 * @param trans transaction to perform (empty)
	 * @param delay time to annotate
	 *
	 * it triggers an IRQ when called
	 */
	void call_interrupt(tlm::tlm_generic_payload &trans,
			sc_core::sc_time &delay);

	/**
	 * DMI pointer is not longer valid
	 * @param start memory address region start
	 * @param end memory address region end
	 */
	void invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end);
};

#endif
