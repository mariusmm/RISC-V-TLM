/*!
 \file M_extension.h
 \brief Implement M extensions part of the RISC-V
 \author Màrius Montón
 \date November 2018
*/
// SPDX-License-Identifier: GPL-3.0-or-later

#include "M_extension.h"
namespace riscv_tlm {
    // RV32
    template<>
    void M_extension<std::uint32_t>::Exec_M_MULH() const {
        unsigned int rd, rs1, rs2;
        signed_T multiplier, multiplicand;
        std::int64_t result;
        signed_T ret_value;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = this->regs->getValue(rs1);
        multiplicand = this->regs->getValue(rs2);


        result = static_cast<std::int64_t>(multiplier) * static_cast<std::int64_t>(multiplicand);
        ret_value = static_cast<std::int32_t>((result >> 32) & 0x00000000FFFFFFFF);

        this->regs->setValue(rd, ret_value);

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULH: x{:d}({:d}) * x{:d}({:d}) -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, multiplier, rs2, multiplicand, rd, result);
    }

    template<>
    void M_extension<std::uint32_t>::Exec_M_MULHSU() const {
        unsigned int rd, rs1, rs2;
        std::int32_t multiplier;
        std::uint32_t multiplicand;
        std::int64_t result;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = static_cast<std::int32_t>(this->regs->getValue(rs1));
        multiplicand = this->regs->getValue(rs2);

        result = static_cast<std::int64_t>(multiplier * static_cast<std::uint64_t>(multiplicand));
        result = (result >> 32) & 0x00000000FFFFFFFF;
        this->regs->setValue(rd, static_cast<std::int32_t>(result));

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULHSU: x{:d} * x{:d} -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, rs2, rd, result);
    }

    template<>
    void M_extension<std::uint32_t>::Exec_M_MULHU() const {
        unsigned int rd, rs1, rs2;
        std::uint32_t multiplier, multiplicand;
        std::uint64_t result;
        std::int32_t ret_value;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = static_cast<std::int32_t>(this->regs->getValue(rs1));
        multiplicand = static_cast<std::int32_t>(this->regs->getValue(rs2));

        result = static_cast<std::uint64_t>(multiplier) * static_cast<std::uint64_t>(multiplicand);
        ret_value = static_cast<std::int32_t>((result >> 32) & 0x00000000FFFFFFFF);
        this->regs->setValue(rd, ret_value);

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULHU: x{:d} * x{:d} -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, rs2, rd, result);
    }


    // RV64
    // I need to use SystemC bigint with 128 bits to perform 64 x 64 bits multiplication and keep the high half
    template<>
    void M_extension<std::uint64_t>::Exec_M_MULH() const {
        unsigned int rd, rs1, rs2;
        signed_T multiplier, multiplicand;
        signed_T result;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = this->regs->getValue(rs1);
        multiplicand = this->regs->getValue(rs2);

        sc_dt::sc_bigint<128> mul = multiplier;
        sc_dt::sc_bigint<128> muld = multiplicand;
        sc_dt::sc_bigint<128> res = mul * muld;
        result = res.range(127, 64).to_int64();

        this->regs->setValue(rd, result);

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULH: x{:d}({:d}) * x{:d}({:d}) -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, multiplier, rs2, multiplicand, rd, result);
    }

    template<>
    void M_extension<std::uint64_t>::Exec_M_MULHSU() const {
        unsigned int rd, rs1, rs2;
        signed_T multiplier;
        unsigned_T multiplicand;
        signed_T result;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = this->regs->getValue(rs1);
        multiplicand = this->regs->getValue(rs2);

        sc_dt::sc_bigint<128> mul = multiplier;
        sc_dt::sc_bigint<128> muld = multiplicand;
        sc_dt::sc_bigint<128> res = mul * muld;
        result = res.range(127, 64).to_int64();

        this->regs->setValue(rd, result);

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULHSU: x{:d} * x{:d} -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, rs2, rd, result);
    }

    template<>
    void M_extension<std::uint64_t>::Exec_M_MULHU() const {
        unsigned int rd, rs1, rs2;
        unsigned_T multiplier, multiplicand;
        unsigned_T result;

        rd = this->get_rd();
        rs1 = this->get_rs1();
        rs2 = this->get_rs2();

        multiplier = this->regs->getValue(rs1);
        multiplicand = this->regs->getValue(rs2);

        sc_dt::sc_bigint<128> mul = multiplier;
        sc_dt::sc_bigint<128> muld = multiplicand;
        sc_dt::sc_bigint<128> res = mul * muld;
        result = res.range(127, 64).to_uint64();

        this->regs->setValue(rd, result);

        this->logger->debug("{} ns. PC: 0x{:x}. M.MULHU: x{:d} * x{:d} -> x{:d}({:d})",
                            sc_core::sc_time_stamp().value(),
                            this->regs->getPC(),
                            rs1, rs2, rd, result);
    }

}