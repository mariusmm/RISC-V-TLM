/*!
 \file Trace.h
 \brief Basic TLM-2 Trace module
 \author Màrius Montón
 \date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __TRACE_H__
#define __TRACE_H__

#include <iostream>
#include <fstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

namespace riscv_tlm::peripherals {
    /**
    * @brief Simple trace peripheral
    *
    * This peripheral outputs to cout any character written to its unique register
    */
    class Trace : sc_core::sc_module {
    public:

        /**
        * @brief Bus socket
        */
        tlm_utils::simple_target_socket<Trace> socket;

        /**
        * @brief Constructor
        * @param name Module name
        */
        explicit Trace(sc_core::sc_module_name const &name);

        /**
        * @brief Destructor
        */
        ~Trace() override;

    private:

        // TLM-2 blocking transport method
        virtual void b_transport(tlm::tlm_generic_payload &trans,
                                 sc_core::sc_time &delay);

        void xtermLaunch(char *slaveName) const;

        void xtermKill();

        void xtermSetup();

        int ptSlave{};
        int ptMaster{};
        int xtermPid{};
    };
}
#endif
