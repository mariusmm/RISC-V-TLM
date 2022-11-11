/*!
 \file CPU.cpp
 \brief Main CPU class
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later
#include "CPU.h"

namespace riscv_tlm {

    SC_HAS_PROCESS(CPURV32);

    CPURV32::CPURV32(sc_core::sc_module_name const &name, BaseType PC, bool debug) :
            CPU(name, debug), INSTR(0) {

        register_bank = new Registers<BaseType>();
        mem_intf = new MemoryInterface();
        register_bank->setPC(PC);
        register_bank->setValue(Registers<BaseType>::sp, (Memory::SIZE / 4) - 1);

        int_cause = 0;

        instr_bus.register_invalidate_direct_mem_ptr(this,
                                                     &CPURV32::invalidate_direct_mem_ptr);

        base_inst = new BASE_ISA<BaseType>(0, register_bank, mem_intf);
        c_inst = new C_extension<BaseType>(0, register_bank, mem_intf);
        m_inst = new M_extension<BaseType>(0, register_bank, mem_intf);
        a_inst = new A_extension<BaseType>(0, register_bank, mem_intf);

        trans.set_data_ptr(reinterpret_cast<unsigned char *>(&INSTR));

        logger->info("Created CPURV32 CPU");
        std::cout << "Created CPURV32 CPU" << std::endl;
    }

    CPURV32::~CPURV32() {
        delete register_bank;
        delete mem_intf;
        delete base_inst;
        delete c_inst;
        delete m_inst;
        delete a_inst;
        delete m_qk;
    }

    bool CPURV32::cpu_process_IRQ() {
        BaseType csr_temp;
        bool ret_value = false;

        if (interrupt) {
            csr_temp = register_bank->getCSR(CSR_MSTATUS);
            if ((csr_temp & MSTATUS_MIE) == 0) {
                logger->debug("{} ns. PC: 0x{:x}. Interrupt delayed", sc_core::sc_time_stamp().value(),
                              register_bank->getPC());

                return ret_value;
            }

            csr_temp = register_bank->getCSR(CSR_MIP);

            if ((csr_temp & MIP_MEIP) == 0) {
                csr_temp |= MIP_MEIP;  // MEIP bit in MIP register (11th bit)
                register_bank->setCSR(CSR_MIP, csr_temp);

                logger->debug("{} ns. PC: 0x{:x}. Interrupt!", sc_core::sc_time_stamp().value(),
                              register_bank->getPC());

                /* updated MEPC register */
                BaseType old_pc = register_bank->getPC();
                register_bank->setCSR(CSR_MEPC, old_pc);

                logger->debug("{} ns. PC: 0x{:x}. Old PC Value 0x{:x}", sc_core::sc_time_stamp().value(),
                              register_bank->getPC(),
                              old_pc);

                /* update MCAUSE register */
                register_bank->setCSR(CSR_MCAUSE, 0x80000000);

                /* set new PC address */
                BaseType new_pc = register_bank->getCSR(CSR_MTVEC);
                //new_pc = new_pc & 0xFFFFFFFC; // last two bits always to 0
                logger->debug("{} ns. PC: 0x{:x}. NEW PC Value 0x{:x}", sc_core::sc_time_stamp().value(),
                              register_bank->getPC(),
                              new_pc);
                register_bank->setPC(new_pc);

                ret_value = true;
                interrupt = false;
                irq_already_down = false;
            }
        } else {
            if (!irq_already_down) {
                csr_temp = register_bank->getCSR(CSR_MIP);
                csr_temp &= ~MIP_MEIP;
                register_bank->setCSR(CSR_MIP, csr_temp);
                irq_already_down = true;
            }
        }

        return ret_value;
    }

    bool CPURV32::CPU_step() {
        bool PC_not_affected = false;

        /* Get new PC value */
        if (dmi_ptr_valid) {
            /* if memory_offset at Memory module is set, this won't work */
            std::memcpy(&INSTR, dmi_ptr + register_bank->getPC(), 4);
        } else {
            sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
            tlm::tlm_dmi dmi_data;
            trans.set_address(register_bank->getPC());
            instr_bus->b_transport(trans, delay);

            if (trans.is_response_error()) {
                SC_REPORT_ERROR("CPU base", "Read memory");
            }

            if (trans.is_dmi_allowed()) {
                dmi_ptr_valid = instr_bus->get_direct_mem_ptr(trans, dmi_data);
                if (dmi_ptr_valid) {
                    std::cout << "Get DMI_PTR " << std::endl;
                    dmi_ptr = dmi_data.get_dmi_ptr();
                }
            }
        }

        perf->codeMemoryRead();
        inst.setInstr(INSTR);
        bool breakpoint = false;

        base_inst->setInstr(INSTR);
        auto deco = base_inst->decode();

        if (deco != OP_ERROR) {
            PC_not_affected = base_inst->process_instruction(inst, &breakpoint, deco);
            if (PC_not_affected) {
                register_bank->incPC();
            }

        } else {
            c_inst->setInstr(INSTR);
            auto c_deco = c_inst->decode();
            if (c_deco != OP_C_ERROR ) {
                PC_not_affected = c_inst->process_instruction(inst, &breakpoint, c_deco);
                if (PC_not_affected) {
                    register_bank->incPCby2();
                }
            } else {
                m_inst->setInstr(INSTR);
                auto m_deco = m_inst->decode();
                if (m_deco != OP_M_ERROR) {
                    PC_not_affected = m_inst->process_instruction(inst, m_deco);
                    if (PC_not_affected) {
                        register_bank->incPC();
                    }
                } else {
                    a_inst->setInstr(INSTR);
                    auto a_deco = a_inst->decode();
                    if (a_deco != OP_A_ERROR) {
                        PC_not_affected = a_inst->process_instruction(inst, a_deco);
                        if (PC_not_affected) {
                            register_bank->incPC();
                        }
                    } else {
                        std::cout << "Extension not implemented yet" << std::endl;
                        inst.dump();
                        base_inst->NOP();
                    }
                }
            }
        }

        if (breakpoint) {
            std::cout << "Breakpoint set to true\n";
        }

        perf->instructionsInc();

        return breakpoint;
    }



    void CPURV32::call_interrupt(tlm::tlm_generic_payload &m_trans,
                              sc_core::sc_time &delay) {
        interrupt = true;
        /* Socket caller send a cause (its id) */
        memcpy(&int_cause, m_trans.get_data_ptr(), sizeof(BaseType));
        delay = sc_core::SC_ZERO_TIME;
    }

    std::uint64_t CPURV32::getStartDumpAddress() {
        return register_bank->getValue(Registers<std::uint32_t>::t0);
    }

    std::uint64_t CPURV32::getEndDumpAddress() {
        return register_bank->getValue(Registers<std::uint32_t>::t1);
    }
}