/*!
 \file Memory.h
 \brief Basic TLM-2 memory model
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/multi_passthrough_target_socket.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace riscv_tlm {
/**
 * @brief Basic TLM-2 memory
 */
    class Memory : public sc_core::sc_module {
    public:
        // TLM-2 socket, defaults to 32-bits wide, base protocol
        tlm_utils::multi_passthrough_target_socket<Memory> socket;

        /* 16 MBytes */
        enum {
            SIZE = 0x1000000
        };
        const sc_core::sc_time LATENCY;

        Memory(sc_core::sc_module_name const &name, std::string const &filename) noexcept;

        explicit Memory(const sc_core::sc_module_name &name) noexcept;

        ~Memory() noexcept override = default;

        /**
         * @brief Returns Program Counter read from hexfile
         * @return Initial PC
         */
        virtual std::uint32_t getPCfromHEX() noexcept;

        // TLM-2 blocking transport method
        virtual void b_transport(int portNo, tlm::tlm_generic_payload &trans,
                                 sc_core::sc_time &delay) noexcept;

        // *********************************************
        // TLM-2 forward DMI method
        // *********************************************
        virtual bool get_direct_mem_ptr(int portNo, tlm::tlm_generic_payload &trans,
                                        tlm::tlm_dmi &dmi_data) noexcept;

        // *********************************************
        // TLM-2 debug transport method
        // *********************************************
        virtual unsigned int transport_dbg(int portNo, tlm::tlm_generic_payload &trans) noexcept;

    private:

        /**
         * @brief Memory array in bytes
         */
        std::array<uint8_t, Memory::SIZE> mem{};

        /**
         * @brief Log class
         */
        std::shared_ptr<spdlog::logger> logger;

        /**
         * @brief Program counter (PC) read from hex file
         */
        std::uint32_t program_counter;

        /**
         * @brief DMI can be used?
         */
        bool dmi_allowed;

        /**
         * @brief Read Intel hex file
         * @param filename file name to read
         */
        void readHexFile(const std::string &filename) noexcept;
    };
}
#endif /* __MEMORY_H__ */
