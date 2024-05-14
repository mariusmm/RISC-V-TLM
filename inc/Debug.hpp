/*!
 \file Debug.hpp
 \brief GDB connector
 \author Màrius Montón
 \date February 2021
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

#include "CPU.hpp"
#include "Memory.hpp"

namespace riscv_tlm::CPU {

    /**
     * @brief GDB connector
    */
    class Debug : sc_core::sc_module {
    public:

        Debug(riscv_tlm::CPU::CPURV32 *cpu, peripherals::Memory *mem);
        Debug(riscv_tlm::CPU::CPURV64 *cpu, peripherals::Memory *mem);

        ~Debug() override;

    private:
        static std::string compute_checksum_string(const std::string &msg);

        void send_packet(int m_conn, const std::string &msg);

        std::string receive_packet();

        void handle_gdb_loop();

        static constexpr size_t bufsize = 1024 * 8;
        char iobuf[bufsize]{};
        int conn;
        riscv_tlm::CPU::CPURV32 *dbg_cpu32;
        riscv_tlm::CPU::CPURV64 *dbg_cpu64;
        Registers<std::uint32_t> *register_bank32;
        Registers<std::uint64_t> *register_bank64;
        peripherals::Memory *dbg_mem;
        tlm::tlm_generic_payload dbg_trans;
        unsigned char pyld_array[128]{};
        std::unordered_set<uint32_t> breakpoints;
        riscv_tlm::CPU::cpu_types_t cpu_type;
    };
}

#endif /* INC_DEBUG_H_ */
