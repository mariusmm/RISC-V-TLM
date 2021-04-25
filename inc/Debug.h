/*!
 \file Debug.h
 \brief GDB connector
 \author Màrius Montón
 \date February 2021
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"


#include "CPU.h"
#include "Memory.h"

class Debug: sc_core::sc_module {
public:

	Debug(CPU *cpu, Memory* mem);
	~Debug() override;

private:
	static std::string compute_checksum_string(const std::string &msg);
	void send_packet(int conn, const std::string &msg);
	std::string receive_packet();
	void handle_gdb_loop();

	static constexpr size_t bufsize = 1024 * 8;
	char iobuf[bufsize]{};
	int conn;
	CPU *dbg_cpu;
	Memory *dbg_mem;
	tlm::tlm_generic_payload dbg_trans;
	unsigned char pyld_array[128];
	std::unordered_set<uint32_t> breakpoints;
};


#endif /* INC_DEBUG_H_ */
