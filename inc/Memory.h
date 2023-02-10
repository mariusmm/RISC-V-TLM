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
//#include "tlm_utils/tlm_target_socket.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "pysc.h"

namespace riscv_tlm {
/**
 * @brief Basic TLM-2 memory
 */
    class Memory : sc_core::sc_module {
    public:
        // TLM-2 socket, defaults to 32-bits wide, base protocol
        tlm::tlm_target_socket<> socket;

        /* 16 MBytes */
        enum {
            SIZE = 0x1000000
        };
        const sc_core::sc_time LATENCY;

        Memory(sc_core::sc_module_name const &name, std::string const &filename);

        explicit Memory(const sc_core::sc_module_name &name);

        ~Memory() override;

        void trace(sc_core::sc_trace_file *tf);

        /**
         * @brief Returns Program Counter read from hexfile
         * @return Initial PC
         */
        virtual std::uint32_t getPCfromHEX();
        // *********************************************
        // TLM-2 debug transport method
        // *********************************************
        virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans);
    
    private:
        py_module<Memory> py;
        /**
         * @brief Memory array in bytes
         */
        //std::array<uint8_t, Memory::SIZE> mem{};

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

   };
}
#endif /* __MEMORY_H__ */
