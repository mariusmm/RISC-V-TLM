/*!
 \file Debug.cpp
 \brief GDB connector
 \author Màrius Montón
 \date February 2021
 */
// SPDX-License-Identifier: GPL-3.0-or-later


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/algorithm/string.hpp>

#include "Debug.hpp"

namespace riscv_tlm {

    constexpr char nibble_to_hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    Debug::Debug(riscv_tlm::CPURV32 *cpu, Memory *mem) : sc_module(sc_core::sc_module_name("Debug")) {
        dbg_cpu32 = cpu;
        dbg_cpu64 = nullptr;
        register_bank32 = nullptr;
        register_bank64 = nullptr;
        dbg_mem = mem;
        cpu_type = riscv_tlm::RV32;

        int sock = socket(AF_INET, SOCK_STREAM, 0);

        int optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(1234);

        bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
        listen(sock, 1);

        socklen_t len = sizeof(addr);
        conn = accept(sock, reinterpret_cast<struct sockaddr *>(&addr), &len);

        handle_gdb_loop();
    }

    Debug::Debug(riscv_tlm::CPURV64 *cpu, Memory *mem) : sc_module(sc_core::sc_module_name("Debug")) {
        dbg_cpu32 = nullptr;
        dbg_cpu64 = cpu;
        register_bank32 = nullptr;
        register_bank64 = nullptr;
        dbg_mem = mem;
        cpu_type = riscv_tlm::RV64;

        int sock = socket(AF_INET, SOCK_STREAM, 0);

        int optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(1234);

        bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
        listen(sock, 1);

        socklen_t len = sizeof(addr);
        conn = accept(sock, reinterpret_cast<struct sockaddr *>(&addr), &len);

        handle_gdb_loop();
    }

    Debug::~Debug() = default;

    void Debug::send_packet(int m_conn, const std::string &msg) {
        std::string frame = "+$" + msg + "#" + compute_checksum_string(msg);

        memcpy(iobuf, frame.c_str(), frame.size());

        ::send(m_conn, iobuf, frame.size(), 0);
    }

    std::string Debug::receive_packet() {
        ssize_t nbytes = ::recv(conn, iobuf, bufsize, 0);

        if (nbytes == 0) {
            return "";
        } else if (nbytes == 1) {
            return std::string{"+"};
        } else {
            char *start = strchr(iobuf, '$');
            char *end = strchr(iobuf, '#');

            std::string message(start + 1, end - (start + 1));

            return message;
        }
    }

    void Debug::handle_gdb_loop() {
        std::cout << "Handle_GDB_Loop\n";

        if (dbg_cpu32 != nullptr) {
            register_bank32 = dbg_cpu32->getRegisterBank();
        } else {
            register_bank64 = dbg_cpu64->getRegisterBank();
        }

        while (true) {
            std::string msg = receive_packet();

            if (msg.empty() ) {
                std::cout << "remote connection seems to be closed, terminating ..."
                          << std::endl;
                break;
            } else if (msg == "+") {
                // NOTE: just ignore this message, nothing to do in this case
            } else if (boost::starts_with(msg, "qSupported")) {
                send_packet(conn, "PacketSize=256;swbreak+;hwbreak+;vContSupported+;multiprocess-");
            } else if (msg == "vMustReplyEmpty") {
                send_packet(conn, "");
            } else if (msg == "Hg0") {
                send_packet(conn, "OK");
            } else if (msg == "Hc0") {
                send_packet(conn, "");
            } else if (msg == "qTStatus") {
                send_packet(conn, "");
            } else if (msg == "?") {
                send_packet(conn, "S05");
            } else if (msg == "qfThreadInfo") {
                send_packet(conn, "");
            } else if (boost::starts_with(msg, "qL")) {
                send_packet(conn, "");
            } else if (msg == "Hc-1") {
                send_packet(conn, "OK");
            } else if (msg == "qC") {
                send_packet(conn, "-1");
            } else if (msg == "qAttached") {
                send_packet(conn, "0");  // 0 process started, 1 attached to process
            } else if (msg == "g") {

                std::stringstream stream;
                stream << std::setfill('0') << std::hex;
                for (int i = 1; i < 32; i++) {
                    if (cpu_type == riscv_tlm::RV32) {
                        stream << std::setw(8) << register_bank32->getValue(i);
                    }
                }
                send_packet(conn, stream.str());
            } else if (boost::starts_with(msg, "p")) {
                long n = strtol(msg.c_str() + 1, nullptr, 16);
                std::uint64_t reg_value = 0;
                if (n < 32) {
                    if (cpu_type == riscv_tlm::RV32) {
                        reg_value = register_bank32->getValue(n);
                    } else {
                        reg_value = register_bank64->getValue(n);
                    }
                } else if (n == 32) {
                    if (cpu_type == riscv_tlm::RV32) {
                        reg_value = register_bank32->getPC();
                    } else {
                        reg_value = register_bank64->getPC();
                    }
                } else {
                    // see: https://github.com/riscv/riscv-gnu-toolchain/issues/217
                    // risc-v register 834
                    if (cpu_type == riscv_tlm::RV32) {
                        reg_value = register_bank32->getCSR(n - 65);
                    } else {
                        reg_value = register_bank64->getCSR(n - 65);
                    }
                }
                std::stringstream stream;
                stream << std::setfill('0') << std::hex;
                if (cpu_type == riscv_tlm::RV32) {
                    stream << std::setw(8) << htonl(reg_value);
                } else {
                    stream << std::setw(16) << htonl(reg_value);
                }
                send_packet(conn, stream.str());
            } else if (boost::starts_with(msg, "P")) {
                char *pEnd;
                long reg = strtol(msg.c_str() + 1, &pEnd, 16);
                int val = strtol(pEnd + 1, nullptr, 16);
                if (cpu_type == riscv_tlm::RV32) {
                    register_bank32->setValue(reg + 1, val);
                } else {
                    register_bank64->setValue(reg + 1, val);
                }
                send_packet(conn, "OK");
            } else if (boost::starts_with(msg, "m")) {
                char *pEnd;
                long addr = strtol(msg.c_str() + 1, &pEnd, 16);
                int len = strtol(pEnd + 1, &pEnd, 16);

                dbg_trans.set_data_ptr(pyld_array);
                dbg_trans.set_command(tlm::TLM_READ_COMMAND);
                dbg_trans.set_address(addr);
                dbg_trans.set_data_length(len);
                dbg_mem->transport_dbg(dbg_trans);

                std::stringstream stream;
                stream << std::setfill('0') << std::hex;
                for (auto &c: pyld_array) {
                    stream << std::setw(2) << (0xFF & c);
                }

                send_packet(conn, stream.str());

            } else if (boost::starts_with(msg, "M")) {
                printf("M TBD\n");
                send_packet(conn, "OK");
            } else if (boost::starts_with(msg, "X")) {
                send_packet(conn, "");  // binary data unsupported
            } else if (msg == "qOffsets") {
                send_packet(conn, "Text=0;Data=0;Bss=0");
            } else if (msg == "qSymbol::") {
                send_packet(conn, "OK");
            } else if (msg == "vCont?") {
                send_packet(conn, "vCont;cs");
            } else if (msg == "c") {
                bool breakpoint_hit = false;
                bool bkpt = false;
                do {
                    std::uint64_t currentPC;

                    if (cpu_type == riscv_tlm::RV32) {
                        bkpt = dbg_cpu32->CPU_step();
                        currentPC = register_bank32->getPC();
                    } else {
                        bkpt = dbg_cpu64->CPU_step();
                        currentPC = register_bank64->getPC();
                    }

                    auto search = breakpoints.find(currentPC);
                    if (search != breakpoints.end()) {
                        breakpoint_hit = true;
                    }
                } while ((breakpoint_hit == false) && (bkpt == false));

                // std::cout << "Breakpoint hit at 0x" << std::hex << register_bank->getPC() << std::endl;
                send_packet(conn, "S05");
            } else if (msg == "s") {

                bool breakpoint;
                if (cpu_type == riscv_tlm::RV32) {
                    dbg_cpu32->CPU_step();
                } else {
                    dbg_cpu64->CPU_step();
                }

                std::uint64_t currentPC;
                if (cpu_type == riscv_tlm::RV32) {
                    currentPC = register_bank32->getPC();
                } else {
                    currentPC = register_bank64->getPC();
                }

                auto search = breakpoints.find(currentPC);
                if (search != breakpoints.end()) {
                    breakpoint = true;
                } else {
                    breakpoint = false;
                }

                if (breakpoint) {
                    send_packet(conn, "S03");
                } else {
                    send_packet(conn, "S05");
                }

            } else if (boost::starts_with(msg, "vKill")) {
                send_packet(conn, "OK");
                break;
            } else if (boost::starts_with(msg, "Z1")) {
                char *pEnd;
                long addr = strtol(msg.c_str() + 3, &pEnd, 16);
                breakpoints.insert(addr);
                std::cout << "Breakpoint set to address 0x" << std::hex << addr << std::endl;
                send_packet(conn, "OK");
            } else if (boost::starts_with(msg, "z1")) {
                char *pEnd;
                long addr = strtol(msg.c_str() + 3, &pEnd, 16);
                breakpoints.erase(addr);
                send_packet(conn, "OK");
            } else if (boost::starts_with(msg, "z0")) {
                char *pEnd;
                long addr = strtol(msg.c_str() + 3, &pEnd, 16);
                breakpoints.erase(addr);
                send_packet(conn, "");
            } else if (boost::starts_with(msg, "Z0")) {
                char *pEnd;
                long addr = strtol(msg.c_str() + 3, &pEnd, 16);
                breakpoints.insert(addr);
                std::cout << "Breakpoint set to address 0x" << std::hex << addr << std::endl;
                send_packet(conn, "OK");
            } else {
                std::cout << "unsupported message '" << msg
                          << "' detected, terminating ..." << std::endl;
                break;
            }
        }
    }

    std::string Debug::compute_checksum_string(const std::string &msg) {
        unsigned sum = 0;
        for (auto c: msg) {
            sum += unsigned(c);
        }
        sum = sum % 256;

        char low = nibble_to_hex[sum & 0xf];
        char high = nibble_to_hex[(sum & (0xf << 4)) >> 4];

        return {high, low};
    }

}