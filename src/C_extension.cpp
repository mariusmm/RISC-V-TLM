/*!
 \file C_extension.cpp
 \brief Implement C extensions part of the RISC-V
 \author Màrius Montón
 \date August 2018
*/
#include "C_extension.h"

namespace riscv_tlm {

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
                        [[unlikely]]
                    default:
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
        std::uint32_t mem_addr;
        int rs1;
        int new_pc;

        rs1 = get_rs1();
        mem_addr = 0;

        new_pc = static_cast<std::int32_t>(
                static_cast<std::int32_t>((regs->getValue(rs1)) + static_cast<std::int32_t>(mem_addr)) & 0xFFFFFFFE);

        logger->debug("{} ns. PC: 0x{:x}. C.JR: PC <- 0x{:x}", sc_core::sc_time_stamp().value(), regs->getPC(), new_pc);

        regs->setPC(new_pc);

        return true;
    }

    bool C_extension::Exec_C_MV() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rd();
        rs1 = 0;
        rs2 = get_rs2();

        calc = regs->getValue(rs1) + regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.MV: x{:d}(0x{:x}) + x{:d}(0x{:x}) -> x{:d}(0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, regs->getValue(rs1), rs2, regs->getValue(rs2), rd, calc);

        return true;
    }

    bool C_extension::Exec_C_ADD() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rs1();
        rs1 = get_rs1();
        rs2 = get_rs2();

        calc = regs->getValue(rs1) + regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.ADD: x{:d} + x{} -> x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, rs2, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_LWSP() {
        std::uint32_t mem_addr;
        int rd, rs1;
        std::int32_t imm;
        std::uint32_t data;

        // lw rd, offset[7:2](x2)

        rd = get_rd();
        rs1 = 2;
        imm = get_imm_LWSP();

        mem_addr = imm + regs->getValue(rs1);
        data = mem_intf->readDataMem(mem_addr, 4);
        perf->dataMemoryRead();
        regs->setValue(rd, static_cast<std::int32_t>(data));

        regs->setValue(rd, data);

        logger->debug("{} ns. PC: 0x{:x}. C.LWSP: x{:d} + {:d}(@0x{:x}) -> x{:d}({:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, imm, mem_addr, rd, data);

        return true;
    }

    bool C_extension::Exec_C_ADDI4SPN() {
        int rd, rs1;
        std::int32_t imm;
        std::int32_t calc;

        rd = get_rdp();
        rs1 = 2;
        imm = get_imm_ADDI4SPN();

        if (imm == 0) {
            RaiseException(EXCEPTION_CAUSE_ILLEGAL_INSTRUCTION, m_instr);
            return false;
        }

        calc = static_cast<std::int32_t>(regs->getValue(rs1)) + imm;
        regs->setValue(rd, calc);

        logger->debug("{} ns. PC: 0x{:x}. C.ADDI4SN: x{:d} + (0x{:x}) + {:d} -> x{:d}(0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, regs->getValue(rs1), imm, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_ADDI16SP() {
        // addi x2, x2, nzimm[9:4]
        int rd;
        std::int32_t imm;

        if (get_rd() == 2) {
            int rs1;
            std::int32_t calc;

            rd = 2;
            rs1 = 2;
            imm = get_imm_ADDI16SP();

            calc = static_cast<std::int32_t>(regs->getValue(rs1)) + imm;
            regs->setValue(rd, calc);

            logger->debug("{} ns. PC: 0x{:x}. C.ADDI16SP: x{:d} + {:d} -> x{:d} (0x{:x})",
                          sc_core::sc_time_stamp().value(), regs->getPC(),
                          rs1, imm, rd, calc);
        } else {
            /* C.LUI OPCODE */
            rd = get_rd();
            imm = get_imm_LUI();
            regs->setValue(rd, imm);

            logger->debug("{} ns. PC: 0x{:x}. C.LUI: x{:d} <- 0x{:x}", sc_core::sc_time_stamp().value(), regs->getPC(),
                          rd, imm);
        }

        return true;
    }

    bool C_extension::Exec_C_SWSP() {
        // sw rs2, offset(x2)
        std::uint32_t mem_addr;
        int rs1, rs2;
        std::int32_t imm;
        std::uint32_t data;

        rs1 = 2;
        rs2 = get_rs2();
        imm = get_imm_CSS();

        mem_addr = imm + regs->getValue(rs1);
        data = regs->getValue(rs2);

        mem_intf->writeDataMem(mem_addr, data, 4);
        perf->dataMemoryWrite();

        logger->debug("{} ns. PC: 0x{:x}. C.SWSP: x{:d}(0x{:x}) -> x{:d} + {} (@0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs2, data, rs1, imm, mem_addr);

        return true;
    }

    bool C_extension::Exec_C_BEQZ() {
        int rs1;
        int new_pc;
        std::uint32_t val1;

        rs1 = get_rs1p();
        val1 = regs->getValue(rs1);

        if (val1 == 0) {
            new_pc = static_cast<std::int32_t>(regs->getPC()) + get_imm_CB();
            regs->setPC(new_pc);
        } else {
            regs->incPCby2();
            new_pc = static_cast<std::int32_t>(regs->getPC());
        }

        logger->debug("{} ns. PC: 0x{:x}. C.BEQZ: x{:d}(0x{:x}) == 0? -> PC (0xx{:d})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, val1, new_pc);

        return true;
    }

    bool C_extension::Exec_C_BNEZ() {
        int rs1;
        int new_pc;
        std::uint32_t val1;

        rs1 = get_rs1p();
        val1 = regs->getValue(rs1);

        if (val1 != 0) {
            new_pc = static_cast<std::int32_t>(regs->getPC()) + get_imm_CB();
            regs->setPC(new_pc);
        } else {
            regs->incPCby2(); //PC <- PC +2
            new_pc = static_cast<std::int32_t>(regs->getPC());
        }

        logger->debug("{} ns. PC: 0x{:x}. C.BNEZ: x{:d}(0x{:x}) != 0? -> PC (0xx{:d})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, val1, new_pc);

        return true;
    }

    bool C_extension::Exec_C_LI() {
        int rd, rs1;
        std::int32_t imm;
        std::int32_t calc;

        rd = get_rd();
        rs1 = 0;
        imm = get_imm_ADDI();

        calc = static_cast<std::int32_t>(regs->getValue(rs1)) + imm;
        regs->setValue(rd, calc);

        logger->debug("{} ns. PC: 0x{:x}. C.LI: x{:d} ({:d}) + {:d} -> x{:d}(0x{:x}) ",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, regs->getValue(rs1), imm, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_SRLI() {
        int rd, rs1, rs2;
        std::uint32_t shift;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2();

        shift = rs2 & 0x1F;

        calc = static_cast<std::uint32_t>(regs->getValue(rs1)) >> shift;
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.SRLI: x{:d} >> {} -> x{:d}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, shift, rd);

        return true;
    }

    bool C_extension::Exec_C_SRAI() {
        int rd, rs1, rs2;
        std::uint32_t shift;
        std::int32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2();

        shift = rs2 & 0x1F;

        calc = static_cast<std::int32_t>(regs->getValue(rs1)) >> shift;
        regs->setValue(rd, calc);

        logger->debug("{} ns. PC: 0x{:x}. C.SRAI: x{:d} >> {} -> x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, shift, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_SLLI() {
        int rd, rs1, rs2;
        std::uint32_t shift;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_imm_ADDI();

        shift = rs2 & 0x1F;

        calc = static_cast<std::uint32_t>(regs->getValue(rs1)) << shift;
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.SLLI: x{:d} << {} -> x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, shift, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_ANDI() {
        int rd, rs1;
        std::uint32_t imm;
        std::uint32_t aux;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        imm = get_imm_ADDI();

        aux = regs->getValue(rs1);
        calc = aux & imm;
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.ANDI: x{:d}(0x{:x}) AND 0x{:x} -> x{:d}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, aux, imm, rd);

        return true;
    }

    bool C_extension::Exec_C_SUB() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2p();

        calc = regs->getValue(rs1) - regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.SUB: x{:d} - x{:d} -> x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, rs2, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_XOR() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2p();

        calc = regs->getValue(rs1) ^ regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.XOR: x{:d} XOR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, rs2, rd);

        return true;
    }

    bool C_extension::Exec_C_OR() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2p();

        calc = regs->getValue(rs1) | regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.OR: x{:d} OR x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, rs2, rd);

        return true;
    }

    bool C_extension::Exec_C_AND() {
        int rd, rs1, rs2;
        std::uint32_t calc;

        rd = get_rs1p();
        rs1 = get_rs1p();
        rs2 = get_rs2p();

        calc = regs->getValue(rs1) & regs->getValue(rs2);
        regs->setValue(rd, static_cast<std::int32_t>(calc));

        logger->debug("{} ns. PC: 0x{:x}. C.AND: x{:d} AND x{:d} -> x{:d}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rs1, rs2, rd);

        return true;
    }

    bool C_extension::Exec_C_ADDI() const {
        int rd, rs1;
        std::int32_t imm;
        std::int32_t calc;

        rd = get_rd();
        rs1 = rd;
        imm = get_imm_ADDI();

        calc = static_cast<std::int32_t>(regs->getValue(rs1)) + imm;
        regs->setValue(rd, calc);

        logger->debug("{} ns. PC: 0x{:x}. C.ADDI: x{:d} + {} -> x{:d}(0x{:x})", sc_core::sc_time_stamp().value(),
                      regs->getPC(), rs1, imm, rd, calc);

        return true;
    }

    bool C_extension::Exec_C_JALR() {
        std::uint32_t mem_addr = 0;
        int rd, rs1;
        int new_pc, old_pc;

        rd = 1;
        rs1 = get_rs1();

        old_pc = static_cast<std::int32_t>(regs->getPC());
        regs->setValue(rd, old_pc + 2);

        new_pc = static_cast<std::int32_t>((regs->getValue(rs1) + mem_addr) & 0xFFFFFFFE);
        regs->setPC(new_pc);

        logger->debug("{} ns. PC: 0x{:x}. C.JALR: x{:d} <- 0x{:x} PC <- 0xx{:x}", sc_core::sc_time_stamp().value(),
                      regs->getPC(),
                      rd, old_pc + 4, new_pc);

        return true;
    }

    bool C_extension::Exec_C_LW() {
        std::uint32_t mem_addr;
        int rd, rs1;
        std::int32_t imm;
        std::uint32_t data;

        rd = get_rdp();
        rs1 = get_rs1p();
        imm = get_imm_L();

        mem_addr = imm + regs->getValue(rs1);
        data = mem_intf->readDataMem(mem_addr, 4);
        perf->dataMemoryRead();
        regs->setValue(rd, static_cast<std::int32_t>(data));

        logger->debug("{} ns. PC: 0x{:x}. C.LW: x{:d}(0x{:x}) + {:d} (@0x{:x}) -> {:d} (0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs1, regs->getValue(rs1), imm, mem_addr, rd, data);

        return true;
    }

    bool C_extension::Exec_C_SW() {
        std::uint32_t mem_addr;
        int rs1, rs2;
        std::int32_t imm;
        std::uint32_t data;

        rs1 = get_rs1p();
        rs2 = get_rs2p();
        imm = get_imm_L();

        mem_addr = imm + regs->getValue(rs1);
        data = regs->getValue(rs2);

        mem_intf->writeDataMem(mem_addr, data, 4);
        perf->dataMemoryWrite();

        logger->debug("{} ns. PC: 0x{:x}. C.SW: x{:d}(0x{:x}) -> x{:d} + 0x{:x}(@0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rs2, data, rs1, imm, mem_addr);

        return true;
    }

    bool C_extension::Exec_C_JAL(int m_rd) {
        std::int32_t mem_addr;
        int rd;
        int new_pc, old_pc;

        rd = m_rd;
        mem_addr = get_imm_J();
        old_pc = static_cast<std::int32_t>(regs->getPC());

        new_pc = old_pc + mem_addr;
        regs->setPC(new_pc);

        old_pc = old_pc + 2;
        regs->setValue(rd, old_pc);

        logger->debug("{} ns. PC: 0x{:x}. C.JAL: x{:d} <- 0x{:x}. PC + 0x{:x} -> PC (0x{:x})",
                      sc_core::sc_time_stamp().value(), regs->getPC(),
                      rd, old_pc, mem_addr, new_pc);

        return true;
    }

    bool C_extension::Exec_C_EBREAK() {

        logger->debug("C.EBREAK");
        std::cout << "\n" << "C.EBRAK  Instruction called, dumping information"
                  << "\n";
        regs->dump();
        std::cout << "Simulation time " << sc_core::sc_time_stamp() << "\n";
        perf->dump();

        sc_core::sc_stop();

        return true;
    }

    bool C_extension::process_instruction(Instruction &inst, bool *breakpoint) {
        bool PC_not_affected = true;

        *breakpoint = false;

        setInstr(inst.getInstr());

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
                std::cout << "C_EBREAK" << std::endl;
                *breakpoint = true;
                break;
                [[unlikely]] default:
                std::cout << "C instruction not implemented yet" << "\n";
                inst.dump();
                NOP();
                break;
        }

        return PC_not_affected;
    }

}