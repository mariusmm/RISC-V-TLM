/*!
 \file Memory.cpp
 \brief Basic TLM-2 memory model
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Memory.h"
#include "pysc.h"

namespace riscv_tlm {

    SC_HAS_PROCESS(Memory);

    Memory::Memory(sc_core::sc_module_name const &name, std::string const &filename) :
            sc_module(name), LATENCY(sc_core::SC_ZERO_TIME), py("Memory",this) {
        // Register callbacks for incoming interface method calls
        //socket.register_b_transport(this, &Memory::b_transport);
        //socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
        //socket.register_transport_dbg(this, &Memory::transport_dbg);

        dmi_allowed = false;
        program_counter = 0;
        

        logger = spdlog::get("my_logger");
        logger->debug("Using file {}", filename);

        //py = new py_module("Memory");

        target_socket_proxy *sock_p = py.get_target_socket("sock");
        socket.bind(sock_p->socket);
    }

    Memory::Memory(sc_core::sc_module_name const &name) :
            sc_module(name), LATENCY(sc_core::SC_ZERO_TIME), py("Memory",this) {
                /*
        socket.register_b_transport(this, &Memory::b_transport);
        socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
        socket.register_transport_dbg(this, &Memory::transport_dbg);

	dmi_allowed = false;
        program_counter = 0;

        logger = spdlog::get("my_logger");
        logger->debug("Memory instantiated wihtout file");
        */
    }

    Memory::~Memory() = default;

    std::uint32_t Memory::getPCfromHEX() {
        return program_counter;

    }

    unsigned int Memory::transport_dbg(tlm::tlm_generic_payload &trans){
        sc_abort(); // Not implimented
    }

    void Memory::trace(sc_core::sc_trace_file *tf){
        std::cout << "EFR trace memory" <<endl;
        py.trace(tf);
    }

}
