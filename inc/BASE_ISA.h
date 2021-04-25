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
#include "Log.h"


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
	 * @brief Access to funct7 field
	 * @return funct7 field
	 */
	inline int32_t get_funct7() const {
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
	inline int32_t get_imm_I() const {
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
	inline int32_t get_imm_S() const {
		int32_t aux = 0;

		aux = m_instr.range(31, 25) << 5;
		aux |= m_instr.range(11, 7);

		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	/**
	 * @brief Gets immediate field value for U-type
	 * @return immediate_U field
	 */
	inline int32_t get_imm_U() const {
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
	inline int32_t get_imm_B() const {
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
	inline int32_t get_imm_J() const {
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
	inline int32_t get_shamt() const {
		return static_cast<int32_t>(m_instr.range(25, 20));
	}

	/**
	 * @brief Returns CSR field for CSR instructions
	 * @return value corresponding to instr(31:20)
	 */
	inline int32_t get_csr() const {
		int32_t aux = 0;

		aux = static_cast<int32_t>(m_instr.range(31, 20));

		return aux;
	}

	/**
	 * @brief Access to opcode field
	 * @return return opcode field
	 */
	inline int32_t opcode() const override {
		return static_cast<int32_t>(m_instr.range(6, 0));
	}

	bool Exec_LUI() const;
	bool Exec_AUIPC() const;

	bool Exec_JAL() const;
	bool Exec_JALR() const;

	bool Exec_BEQ() const;
	bool Exec_BNE() const;
	bool Exec_BLT() const;
	bool Exec_BGE() const;
	bool Exec_BLTU() const;
	bool Exec_BGEU() const;

	bool Exec_LB() const;
	bool Exec_LH() const;
	bool Exec_LW() const;
	bool Exec_LBU() const;
	bool Exec_LHU() const;

	bool Exec_SB() const;
	bool Exec_SH() const;
	bool Exec_SW() const;
	bool Exec_SBU() const;
	bool Exec_SHU() const;

	bool Exec_ADDI() const;
	bool Exec_SLTI() const;
	bool Exec_SLTIU() const;
	bool Exec_XORI() const;
	bool Exec_ORI() const;
	bool Exec_ANDI() const;
	bool Exec_SLLI();
	bool Exec_SRLI() const;
	bool Exec_SRAI() const;

	bool Exec_ADD() const;
	bool Exec_SUB() const;
	bool Exec_SLL() const;
	bool Exec_SLT() const;
	bool Exec_SLTU() const;

	bool Exec_XOR() const;
	bool Exec_SRL() const;
	bool Exec_SRA() const;
	bool Exec_OR() const;
	bool Exec_AND() const;

	bool Exec_FENCE() const;
	bool Exec_ECALL() const;
	bool Exec_EBREAK();

	bool Exec_CSRRW() const;
	bool Exec_CSRRS() const;
	bool Exec_CSRRC() const;
	bool Exec_CSRRWI() const;
	bool Exec_CSRRSI() const;
	bool Exec_CSRRCI() const;

	/*********************** Privileged Instructions ******************************/
	bool Exec_MRET() const;
	bool Exec_SRET() const;
	bool Exec_WFI() const;
	bool Exec_SFENCE() const;

	/**
	 * @brief Executes default ISA instruction
	 * @param  inst instruction to execute
	 * @return  true if PC is affected by instruction
	 */
	bool process_instruction(Instruction *inst, bool *breakpoint = nullptr);

	/**
	 * @brief Decodes opcode of instruction
	 * @return opcode of instruction
	 */
	opCodes decode();
};

#endif
