/*!
 \file C_extension.h
 \brief Implement C extensions part of the RISC-V
 \author Màrius Montón
 \date August 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef C_EXTENSION__H
#define C_EXTENSION__H

#include "systemc"
#include "extension_base.h"

typedef enum {
	OP_C_ADDI4SPN,
	OP_C_FLD,
	OP_C_LW,
	OP_C_FLW,
	OP_C_FSD,
	OP_C_SW,
	OP_C_FSW,

	OP_C_NOP,
	OP_C_ADDI,
	OP_C_JAL,
	OP_C_LI,
	OP_C_ADDI16SP,
	OP_C_LUI,
	OP_C_SRLI,
	OP_C_SRAI,
	OP_C_ANDI,
	OP_C_SUB,
	OP_C_XOR,
	OP_C_OR,
	OP_C_AND,
	OP_C_J,
	OP_C_BEQZ,
	OP_C_BNEZ,

	OP_C_SLLI,
	OP_C_FLDSP,
	OP_C_LWSP,
	OP_C_FLWSP,
	OP_C_JR,
	OP_C_MV,
	OP_C_EBREAK,
	OP_C_JALR,
	OP_C_ADD,
	OP_C_FSDSP,
	OP_C_SWSP,
	OP_C_FSWSP,

	OP_C_ERROR
} op_C_Codes;

typedef enum {
	C_ADDI4SPN = 0b000,
	C_FLD = 0b001,
	C_LW = 0b010,
	C_FLW = 0b011,
	C_FSD = 0b101,
	C_SW = 0b110,
	C_FSW = 0b111,

	C_ADDI = 0b000,
	C_JAL = 0b001,
	C_LI = 0b010,
	C_ADDI16SP = 0b011,
	C_SRLI = 0b100,
	C_2_SRLI = 0b00,
	C_2_SRAI = 0b01,
	C_2_ANDI = 0b10,
	C_2_SUB = 0b11,
	C_3_SUB = 0b00,
	C_3_XOR = 0b01,
	C_3_OR = 0b10,
	C_3_AND = 0b11,
	C_J = 0b101,
	C_BEQZ = 0b110,
	C_BNEZ = 0b111,

	C_SLLI = 0b000,
	C_FLDSP = 0b001,
	C_LWSP = 0b010,
	C_FLWSP = 0b011,
	C_JR = 0b100,
	C_FDSP = 0b101,
	C_SWSP = 0b110,
	C_FWWSP = 0b111,
} C_Codes;

/**
 * @brief Instruction decoding and fields access
 */
class C_extension: public extension_base {
public:

	/**
	 * @brief Constructor, same as base clase
	 */
	using extension_base::extension_base;

	/**
	 * @brief Access to opcode field
	 * @return return opcode field
	 */
	inline std::int32_t opcode() const override {
		return static_cast<std::int32_t>(m_instr.range(1, 0));
	}

	inline std::int32_t get_rdp() const {
		return static_cast<std::int32_t>(m_instr.range(4, 2) + 8);
	}

	/**
	 * @brief Access to rs1 field
	 * @return rs1 field
	 */
	inline std::int32_t get_rs1() const override {
		return static_cast<std::int32_t>(m_instr.range(11, 7));
	}

	inline void set_rs1(std::int32_t value) override {
		m_instr.range(11, 7) = value;
	}

	inline std::int32_t get_rs1p() const {
		return static_cast<std::int32_t>(m_instr.range(9, 7) + 8);
	}

	/**
	 * @brief Access to rs2 field
	 * @return rs2 field
	 */
	inline std::int32_t get_rs2() const override {
		return static_cast<std::int32_t>(m_instr.range(6, 2));
	}

	inline void set_rs2(std::int32_t value) override {
		m_instr.range(6, 2) = value;
	}

	inline std::int32_t get_rs2p() const {
		return static_cast<std::int32_t>(m_instr.range(4, 2) + 8);
	}

	inline std::int32_t get_funct3() const override {
		return static_cast<std::int32_t>(m_instr.range(15, 13));
	}

	inline void set_funct3(std::int32_t value) override {
		m_instr.range(15, 13) = value;
	}

	/**
	 * @brief Access to immediate field for I-type
	 * @return immediate_I field
	 */
	inline std::int32_t get_imm_I() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr.range(31, 20));

		/* sign extension (optimize) */
		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	inline void set_imm_I(std::int32_t value) {
		m_instr.range(31, 20) = value;
	}

	/**
	 * @brief Access to immediate field for S-type
	 * @return immediate_S field
	 */
	inline std::int32_t get_imm_S() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr.range(31, 25) << 5);
		aux |= static_cast<std::int32_t>(m_instr.range(11, 7));

		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	inline void set_imm_S(std::int32_t value) {
		sc_dt::sc_uint<32> aux = value;

		m_instr.range(31, 25) = aux.range(11, 5);
		m_instr.range(11, 7) = aux.range(4, 0);
	}

	/**
	 * @brief Access to immediate field for U-type
	 * @return immediate_U field
	 */
	inline std::int32_t get_imm_U() const {
		return static_cast<std::int32_t>(m_instr.range(31, 12));
	}

	inline void set_imm_U(std::int32_t value) {
		m_instr.range(31, 12) = (value << 12);
	}

	/**
	 * @brief Access to immediate field for B-type
	 * @return immediate_B field
	 */
	inline std::int32_t get_imm_B() const {
		std::int32_t aux = 0;

		aux |= static_cast<std::int32_t>(m_instr[7] << 11);
		aux |= static_cast<std::int32_t>(m_instr.range(30, 25) << 5);
		aux |= static_cast<std::int32_t>(m_instr[31] << 12);
		aux |= static_cast<std::int32_t>(m_instr.range(11, 8) << 1);

		if (m_instr[31] == 1) {
			aux |= (0b11111111111111111111) << 12;
		}

		return aux;
	}

	inline void set_imm_B(std::int32_t value) {
		sc_dt::sc_uint<32> aux = value;

		m_instr[31] = aux[12];
		m_instr.range(30, 25) = aux.range(10, 5);
		m_instr.range(11, 7) = aux.range(4, 1);
		m_instr[6] = aux[11];
	}
	/**
	 * @brief Access to immediate field for J-type
	 * @return immediate_J field
	 */
	inline std::int32_t get_imm_J() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 11);
		aux |= static_cast<std::int32_t>(m_instr[11] << 4);
		aux |= static_cast<std::int32_t>(m_instr[10] << 9);
		aux |= static_cast<std::int32_t>(m_instr[9] << 8);
		aux |= static_cast<std::int32_t>(m_instr[8] << 10);
		aux |= static_cast<std::int32_t>(m_instr[7] << 6);
		aux |= static_cast<std::int32_t>(m_instr[6] << 7);
		aux |= static_cast<std::int32_t>(m_instr.range(5, 3) << 1);
		aux |= static_cast<std::int32_t>(m_instr[2] << 5);

		if (m_instr[12] == 1) {
			aux |= 0b11111111111111111111 << 12;
		}

		return aux;
	}

	inline void set_imm_J(std::int32_t value) {
		sc_dt::sc_uint<32> aux = (value << 20);

		m_instr[31] = aux[20];
		m_instr.range(30, 21) = aux.range(10, 1);
		m_instr[20] = aux[11];
		m_instr.range(19, 12) = aux.range(19, 12);
	}

	inline std::int32_t get_imm_L() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr.range(12, 10) << 3);
		aux |= static_cast<std::int32_t>(m_instr[6] << 2);
		aux |= static_cast<std::int32_t>(m_instr[5] << 6);

		return aux;
	}

	inline std::int32_t get_imm_LWSP() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 5);
		aux |= static_cast<std::int32_t>(m_instr.range(6, 4) << 2);
		aux |= static_cast<std::int32_t>(m_instr.range(3, 2) << 6);

		return aux;
	}

	inline std::int32_t get_imm_ADDI () const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 5);
		aux |= static_cast<std::int32_t>(m_instr.range(6, 2));

		if (m_instr[12] == 1) {
			aux |= 0b11111111111111111111111111 << 6;
		}
		return aux;
	}

	inline std::int32_t get_imm_ADDI4SPN() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr.range(12, 11) << 4);
		aux |= static_cast<std::int32_t>(m_instr.range(10, 7) << 6);
		aux |= static_cast<std::int32_t>(m_instr[6] << 2);
		aux |= static_cast<std::int32_t>(m_instr[5] << 3);

		return aux;
	}

	inline std::int32_t get_imm_ADDI16SP() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 9);
		aux |= static_cast<std::int32_t>(m_instr[6] << 4);
		aux |= static_cast<std::int32_t>(m_instr[5] << 6);
		aux |= static_cast<std::int32_t>(m_instr[4] << 8);
		aux |= static_cast<std::int32_t>(m_instr[3] << 7);
		aux |= static_cast<std::int32_t>(m_instr[2] << 5);

		if (m_instr[12] == 1) {
			aux |= 0b1111111111111111111111 << 10;
		}
		return aux;
	}

	inline std::int32_t get_imm_CSS() const {
		std::int32_t aux = 0;
		aux = static_cast<std::int32_t>(m_instr.range(12, 9) << 2);
		aux |= static_cast<std::int32_t>(m_instr.range(8, 7) << 6);

		return aux;
	}

	inline std::int32_t get_imm_CB() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 8);
		aux |= static_cast<std::int32_t>(m_instr[11] << 4);
		aux |= static_cast<std::int32_t>(m_instr[10] << 3);
		aux |= static_cast<std::int32_t>(m_instr[6] << 7);
		aux |= static_cast<std::int32_t>(m_instr[5] << 6);
		aux |= static_cast<std::int32_t>(m_instr[4] << 2);
		aux |= static_cast<std::int32_t>(m_instr[3] << 1);
		aux |= static_cast<std::int32_t>(m_instr[2] << 5);

		if (m_instr[12] == 1) {
			aux |= 0b11111111111111111111111 << 9;
		}

		return aux;
	}

	inline std::int32_t get_imm_LUI() const {
		std::int32_t aux = 0;

		aux = static_cast<std::int32_t>(m_instr[12] << 17);
		aux |= static_cast<std::int32_t>(m_instr.range(6, 2) << 12);

		if (m_instr[12] == 1) {
			aux |= 0b111111111111111 << 17;
		}

		return aux;
	}

	inline std::int32_t get_csr() const {
		return get_imm_I();
	}

	/**
	 * @brief Decodes opcode of instruction
	 * @return opcode of instruction
	 */
	op_C_Codes decode() const;

	bool Exec_C_JR();
	bool Exec_C_MV();
	bool Exec_C_LWSP();
	bool Exec_C_ADDI4SPN();
	bool Exec_C_SLLI();
	bool Exec_C_ADDI16SP();
	bool Exec_C_SWSP();
	bool Exec_C_BEQZ();
	bool Exec_C_BNEZ();
	bool Exec_C_LI();
	bool Exec_C_SRLI();
	bool Exec_C_SRAI();
	bool Exec_C_ANDI();
	bool Exec_C_ADD();
	bool Exec_C_SUB();
	bool Exec_C_XOR();
	bool Exec_C_OR();
	bool Exec_C_AND();

	bool Exec_C_ADDI() const;
	bool Exec_C_JALR();
	bool Exec_C_LW();
	bool Exec_C_SW();
	bool Exec_C_JAL(int m_rd);
	bool Exec_C_EBREAK();

	bool process_instruction(Instruction &inst, bool *breakpoint = nullptr);
};

#endif
