/*!
 \file BASE_ISA.h
 \brief RISC-V ISA implementation
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef Execute_H
#define Execute_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "MemoryInterface.h"
#include "Instruction.h"
#include "C_extension.h"
#include "M_extension.h"
#include "A_extension.h"
#include "Registers.h"

typedef enum {
	OP_LUI,
	OP_AUIPC,
	OP_JAL,
	OP_JALR,

	OP_BEQ,
	OP_BNE,
	OP_BLT,
	OP_BGE,
	OP_BLTU,
	OP_BGEU,

	OP_LB,
	OP_LH,
	OP_LW,
	OP_LBU,
	OP_LHU,

	OP_SB,
	OP_SH,
	OP_SW,

	OP_ADDI,
	OP_SLTI,
	OP_SLTIU,
	OP_XORI,
	OP_ORI,
	OP_ANDI,
	OP_SLLI,
	OP_SRLI,
	OP_SRAI,

	OP_ADD,
	OP_SUB,
	OP_SLL,
	OP_SLT,
	OP_SLTU,
	OP_XOR,
	OP_SRL,
	OP_SRA,
	OP_OR,
	OP_AND,

	OP_FENCE,
	OP_ECALL,
	OP_EBREAK,

	OP_CSRRW,
	OP_CSRRS,
	OP_CSRRC,
	OP_CSRRWI,
	OP_CSRRSI,
	OP_CSRRCI,

	OP_URET,
	OP_SRET,
	OP_MRET,
	OP_WFI,
	OP_SFENCE,

	OP_ERROR
} opCodes;

/**
 * @brief Risc_V execute module
 */
class BASE_ISA: public extension_base {
public:

	/**
	 * @brief Constructor, same as base class
	 */
	using extension_base::extension_base;

	/**
	 * @brief Access to rd field
	 * @return rd field
	 */
	inline int32_t get_rd() {
		return m_instr.range(11, 7);
	}

	/**
	 * @brief Sets rd field
	 * @param value desired rd value
	 */
	inline void set_rd(int32_t value) {
		m_instr.range(11, 7) = value;
	}

	/**
	 * @brief Access to rs1 field
	 * @return rs1 field
	 */
	inline int32_t get_rs1() {
		return m_instr.range(19, 15);
	}

	/**
	 * @brief Sets rs1 field
	 * @param value desired rs1 value
	 */
	inline void set_rs1(int32_t value) {
		m_instr.range(19, 15) = value;
	}

	/**
	 * @brief Access to rs2 field
	 * @return rs2 field
	 */
	inline int32_t get_rs2() {
		return m_instr.range(24, 20);
	}

	/**
	 * @brief Sets rs2 field
	 * @param value desired rs2 value
	 */
	inline void set_rs2(int32_t value) {
		m_instr.range(24, 10) = value;
	}

	/**
	 * @brief Access to funct3 field
	 * @return funct3 field
	 */
	inline int32_t get_funct3() {
		return m_instr.range(14, 12);
	}

	/**
	 * @brief Sets func3 field
	 * @param value desired func3 value
	 */
	inline void set_funct3(int32_t value) {
		m_instr.range(14, 12) = value;
	}

	/**
	 * @brief Access to funct7 field
	 * @return funct7 field
	 */
	inline int32_t get_funct7() {
		return m_instr.range(31, 25);
	}

	/**
	 * @brief Sets func7 field
	 * @param value desired func7 value
	 */
	inline void set_func7(int32_t value) {
		m_instr.range(31, 25) = value;
	}

	/**
	 * @brief Gets immediate field value for I-type
	 * @return immediate_I field
	 */
	inline int32_t get_imm_I() {
		int32_t aux = 0;

		aux = m_instr.range(31, 20);

		/* sign extension (optimize) */
		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	/**
	 * @brief Sets immediate field for I-type
	 * @param value desired I value
	 */
	inline void set_imm_I(int32_t value) {
		m_instr.range(31, 20) = value;
	}

	/**
	 * @brief Gets immediate field value for S-type
	 * @return immediate_S field
	 */
	inline int32_t get_imm_S() {
		int32_t aux = 0;

		aux = m_instr.range(31, 25) << 5;
		aux |= m_instr.range(11, 7);

		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	/**
	 * @brief Sets immediate field for S-type
	 * @param value desired S value
	 */
	inline void set_imm_S(int32_t value) {
		sc_dt::sc_uint<32> aux = value;

		m_instr.range(31, 25) = aux.range(11, 5);
		m_instr.range(11, 7) = aux.range(4, 0);
	}

	/**
	 * @brief Gets immediate field value for U-type
	 * @return immediate_U field
	 */
	inline int32_t get_imm_U() {
		return m_instr.range(31, 12);
	}

	/**
	 * @brief Sets immediate field for U-type
	 * @param value desired U value
	 */
	inline void set_imm_U(int32_t value) {
		m_instr.range(31, 12) = (value << 12);
	}

	/**
	 * @brief Gets immediate field value for B-type
	 * @return immediate_B field
	 */
	inline int32_t get_imm_B() {
		int32_t aux = 0;

		aux |= m_instr[7] << 11;
		aux |= m_instr.range(30, 25) << 5;
		aux |= m_instr[31] << 12;
		aux |= m_instr.range(11, 8) << 1;

		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	/**
	 * @brief Sets immediate field for B-type
	 * @param value desired B value
	 */
	inline void set_imm_B(int32_t value) {
		sc_dt::sc_uint<32> aux = value;

		m_instr[31] = aux[12];
		m_instr.range(30, 25) = aux.range(10, 5);
		m_instr.range(11, 7) = aux.range(4, 1);
		m_instr[6] = aux[11];
	}

	/**
	 * @brief Gets immediate field value for J-type
	 * @return immediate_J field
	 */
	inline int32_t get_imm_J() {
		int32_t aux = 0;

		aux = m_instr[31] << 20;
		aux |= m_instr.range(19, 12) << 12;
		aux |= m_instr[20] << 11;
		aux |= m_instr.range(30, 21) << 1;

		/* bit extension (better way to do that?) */
		if (m_instr[31] == 1) {
			aux |= (0b111111111111) << 20;
		}

		return aux;
	}

	/**
	 * @brief Sets immediate field for J-type
	 * @param value desired J value
	 */
	inline void set_imm_J(int32_t value) {
		sc_dt::sc_uint<32> aux = (value << 20);

		m_instr[31] = aux[20];
		m_instr.range(30, 21) = aux.range(10, 1);
		m_instr[20] = aux[11];
		m_instr.range(19, 12) = aux.range(19, 12);
	}

	/**
	 * @brief Returns shamt field for Shifts instructions
	 * @return value corresponding to inst(25:20)
	 */
	inline int32_t get_shamt() {
		return m_instr.range(25, 20);
	}

	/**
	 * @brief Returns CSR field for CSR instructions
	 * @return value corresponding to instr(31:20)
	 */
	inline int32_t get_csr() {
		int32_t aux = 0;

		aux = m_instr.range(31, 20);

		return aux;
	}

	/**
	 * @brief Access to opcode field
	 * @return return opcode field
	 */
	inline int32_t opcode() {
		return m_instr.range(6, 0);
	}

	bool Exec_LUI();
	bool Exec_AUIPC();

	bool Exec_JAL();
	bool Exec_JALR();

	bool Exec_BEQ();
	bool Exec_BNE();
	bool Exec_BLT();
	bool Exec_BGE();
	bool Exec_BLTU();
	bool Exec_BGEU();

	bool Exec_LB();
	bool Exec_LH();
	bool Exec_LW();
	bool Exec_LBU();
	bool Exec_LHU();

	bool Exec_SB();
	bool Exec_SH();
	bool Exec_SW();
	bool Exec_SBU();
	bool Exec_SHU();

	bool Exec_ADDI();
	bool Exec_SLTI();
	bool Exec_SLTIU();
	bool Exec_XORI();
	bool Exec_ORI();
	bool Exec_ANDI();
	bool Exec_SLLI();
	bool Exec_SRLI();
	bool Exec_SRAI();

	bool Exec_ADD();
	bool Exec_SUB();
	bool Exec_SLL();
	bool Exec_SLT();
	bool Exec_SLTU();

	bool Exec_XOR();
	bool Exec_SRL();
	bool Exec_SRA();
	bool Exec_OR();
	bool Exec_AND();

	bool Exec_FENCE();
	bool Exec_ECALL();
	bool Exec_EBREAK();

	bool Exec_CSRRW();
	bool Exec_CSRRS();
	bool Exec_CSRRC();
	bool Exec_CSRRWI();
	bool Exec_CSRRSI();
	bool Exec_CSRRCI();

	/*********************** Privileged Instructions ******************************/
	bool Exec_MRET();
	bool Exec_SRET();
	bool Exec_WFI();
	bool Exec_SFENCE();

	/**
	 * @brief Executes default ISA instruction
	 * @param  inst instruction to execute
	 * @return  true if PC is affected by instruction
	 */
	bool process_instruction(Instruction &inst);

	/**
	 * @brief Decodes opcode of instruction
	 * @return opcode of instruction
	 */
	opCodes decode();
};

#endif
