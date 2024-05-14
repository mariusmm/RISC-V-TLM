/*!
 \file MemoryInterface.cpp
 \brief CPU to Memory Interface class
 \author Màrius Montón
 \date May 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MemoryInterface.hpp"

namespace riscv_tlm::peripherals {

    MemoryInterface::MemoryInterface() :
            data_bus("data_bus") {}

/**
 * Access data memory to get data
 * @param  addr address to access to
 * @param size size of the data to read in bytes
 * @return data value read
 */
    std::uint32_t MemoryInterface::readDataMem(std::uint32_t addr, int size) {
        std::uint32_t data;
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;

        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_data_ptr(reinterpret_cast<unsigned char *>(&data));
        trans.set_data_length(size);
        trans.set_streaming_width(4); // = data_length to indicate no streaming
        trans.set_byte_enable_ptr(nullptr); // 0 indicates unused
        trans.set_dmi_allowed(false); // Mandatory initial value
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        trans.set_address(addr);

        data_bus->b_transport(trans, delay);

        if (trans.is_response_error()) {
            std::stringstream error_msg;
            error_msg << "Read memory: 0x" << std::hex << addr;
            SC_REPORT_ERROR("Memory", error_msg.str().c_str());
        }

        return data;
    }

/**
 * Acces data memory to write data
 * @brief
 * @param addr addr address to access to
 * @param data data to write
 * @param size size of the data to write in bytes
 */
    void MemoryInterface::writeDataMem(std::uint32_t addr, std::uint32_t data, int size) {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;

        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_data_ptr(reinterpret_cast<unsigned char *>(&data));
        trans.set_data_length(size);
        trans.set_streaming_width(4); // = data_length to indicate no streaming
        trans.set_byte_enable_ptr(nullptr); // 0 indicates unused
        trans.set_dmi_allowed(false); // Mandatory initial value
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        trans.set_address(addr);

        data_bus->b_transport(trans, delay);

        if (trans.is_response_error()) {
            std::stringstream error_msg;
            error_msg << "Write memory: 0x" << std::hex << addr;
            SC_REPORT_ERROR("Memory", error_msg.str().c_str());
        }
    }
}