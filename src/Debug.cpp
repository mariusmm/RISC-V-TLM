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
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/algorithm/string.hpp>

#include "Debug.h"

constexpr char nibble_to_hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

Debug::Debug(sc_core::sc_module_name name, uint32_t PC, CPU *cpu, Memory* mem) {

	std::cout << "Debug constructor\n";

	dbg_cpu = cpu;
	dbg_mem = mem;

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	int ans = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
			sizeof(optval));

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1234);

	ans = bind(sock, (struct sockaddr *) &addr, sizeof(addr));

	ans = listen(sock, 1);

	socklen_t len = sizeof(addr);
	conn = accept(sock, (struct sockaddr *) &addr, &len);

	handle_gdb_loop();
}

Debug::~Debug() {

}

void Debug::send_packet(int conn, const std::string &msg) {
	std::string frame = "+$" + msg + "#" + compute_checksum_string(msg);

	memcpy(iobuf, frame.c_str(), frame.size());

	int nbytes = ::send(conn, iobuf, frame.size(), 0);
}

std::string Debug::receive_packet() {
	int nbytes = ::recv(conn, iobuf, bufsize, 0);

	if (nbytes == 0) {
		return "";
	} else if (nbytes == 1) {
		return std::string("+");
	} else {
		// 1) find $
		// 2) find #
		// 3) assert that two chars follow #

		char *start = strchr(iobuf, '$');
		char *end = strchr(iobuf, '#');

		std::string message(start + 1, end - (start + 1));

		{
			//std::string local_checksum = compute_checksum_string(message);
			//std::string recv_checksum(end + 1, 2);
			//assert(local_checksum == recv_checksum);
		}

		return message;
	}
}

void Debug::handle_gdb_loop() {
	std::cout << "Handle_GDB_Loop\n";

	Registers *register_bank = dbg_cpu->getRegisterBank();

	while (true) {
		std::string msg = receive_packet();

//		std::cout << "msg: " << msg << std::endl;
		if (msg.size() == 0) {
			std::cout << "remote connection seems to be closed, terminating ..."
					<< std::endl;
			break;
		} else if (msg == "+") {
			// NOTE: just ignore this message, nothing to do in this case
		} else if (boost::starts_with(msg, "qSupported")) {
			printf("qSupported\n");
			send_packet(conn, "PacketSize=1024;swbreak-;hwbreak+;vContSupported+");
		} else if (msg == "vMustReplyEmpty") {
			printf("vMustReplyEmpty\n");
			send_packet(conn, "");
		} else if (msg == "Hg0") {
			printf("Hq0\n");
			send_packet(conn, "OK");
		} else if (msg == "Hc0") {
			printf("Hc0\n");
			send_packet(conn, "");
		} else if (msg == "qTStatus") {
			printf("qTStatus\n");
			send_packet(conn, "");
		} else if (msg == "?") {
			printf("?\n");
			send_packet(conn, "S05");
		} else if (msg == "qfThreadInfo") {
			printf("qThreadInfo\n");
			send_packet(conn, "");
		} else if (boost::starts_with(msg, "qL")) {
			printf("qL\n");
			send_packet(conn, "");
		} else if (msg == "Hc-1") {
			printf("Hc-1\n");
			send_packet(conn, "OK");
		} else if (msg == "qC") {
			printf("qC\n");
			send_packet(conn, "-1");
		} else if (msg == "qAttached") {
			printf("qAttached\n");
			send_packet(conn, "0");  // 0 process started, 1 attached to process
		} else if (msg == "g") {
//			std::cout << msg << '\n';

			std::stringstream stream;
			stream << std::setfill('0') << std::hex;
			for (int i = 1; i < 32; i++) {
				stream << std::setw(8) << register_bank->getValue(i);
			}
			send_packet(conn, stream.str());
		} else if (boost::starts_with(msg, "p")) {
			std::cout << "P :  "  << msg << std::endl;
			long n = strtol(msg.c_str() + 1, 0, 16);

			int reg_value;
			if (n < 32) {
				reg_value = register_bank->getValue(n);
			} else if (n == 32) {
				reg_value = register_bank->getPC();
			} else {
				// see: https://github.com/riscv/riscv-gnu-toolchain/issues/217
				// risc-v register 834
				reg_value = register_bank->getCSR(n - 65);
			}
			std::stringstream stream;
			stream << std::setfill('0') << std::hex;
			stream << std::setw(8) << htonl(reg_value);
			send_packet(conn, stream.str());
		} else if (boost::starts_with(msg, "P")) {
			printf("P\n");
			char * pEnd;
			long reg = strtol(msg.c_str() + 1, &pEnd, 16);
			std::cout << "n: " << reg;
			int val = strtol(pEnd + 1, 0, 16);
			std::cout << "= " << val << std::endl;
			register_bank->setValue(reg + 1, val);
			send_packet(conn, "OK");
		} else if (boost::starts_with(msg, "m")) {
			printf("m\n");

			char * pEnd;
			long addr = strtol(msg.c_str() + 1, &pEnd, 16);;
			int len = strtol(pEnd + 1, &pEnd, 16);
			std::cout << "msg m: " << msg << std::endl;
			std::cout << std::hex << "addr: " << addr << " , " << len << std::endl;

			dbg_trans.set_data_ptr(pyld_array);
			dbg_trans.set_command(tlm::TLM_READ_COMMAND);
			dbg_trans.set_address(addr);
			dbg_trans.set_data_length(len);
			dbg_mem->transport_dbg(dbg_trans);

			std::stringstream stream;
			stream << std::setfill('0') << std::hex;
			for (auto &c :  pyld_array) {
				stream << std::setw(2) << (0xFF & c);
			}

			send_packet(conn, stream.str());

		} else if (boost::starts_with(msg, "M")) {
			printf("M\n");
			//memory_access_t m = parse_memory_access(msg);
			//std::string data = msg.substr(msg.find(":") + 1);
			//write_memory(m.start, m.nbytes, data);
			send_packet(conn, "OK");
		} else if (boost::starts_with(msg, "X")) {
			printf("X\n");
			send_packet(conn, "");  // binary data unsupported, gdb will send
			// text based message M
		} else if (msg == "qOffsets") {
			printf("qOffsets\n");
			// NOTE: seems to be additional offsets wrt. the exec. elf file
			send_packet(conn, "Text=0;Data=0;Bss=0");
		} else if (msg == "qSymbol::") {
			printf("qSymbol\n");
			send_packet(conn, "OK");
		} else if (msg == "vCont?") {
			printf("vCont?\n");
			send_packet(conn, "vCont;cs");
		} else if (msg == "c") {
			printf("c\n");
			//try {
				//  core.run();
				//if (core.status == CoreExecStatus::HitBreakpoint) {
				//    send_packet(conn, "S05");
				//    core.status = CoreExecStatus::Runnable;
				//} else if (core.status == CoreExecStatus::Terminated) {
				//send_packet(conn, "S03");
				//} else {
				//   assert(false && "invalid core status (apparently still marked as runnable)");
				//}
			//} catch (std::exception &e) {
			//	send_packet(conn, "S04");
			//}
			bool breakpoint_hit = false;
			bool bkpt = false;
			do {
//				std::cout << "PC: " << std::hex <<  register_bank->getPC() << std::endl;
				bkpt =  dbg_cpu->CPU_step();
				uint32_t currentPC = register_bank->getPC();

				auto search = breakpoints.find(currentPC);
				if (search != breakpoints.end()) {
					breakpoint_hit = true;
				}
			} while ((breakpoint_hit == false) && (bkpt == false));

			std::cout << "Breakpoint hit at 0x" << std::hex << register_bank->getPC() << std::endl;

			//send_packet(conn, "S05");
			send_packet(conn, "S03");
		} else if (msg == "s") {
			printf("s\n");
			bool breakpoint;

			dbg_cpu->CPU_step();

			uint32_t currentPC = register_bank->getPC();
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
			printf("vKill\n");
			send_packet(conn, "OK");
			break;
		} else if (boost::starts_with(msg, "Z1")) {
			char * pEnd;
			long addr = strtol(msg.c_str() + 3, &pEnd, 16);;
			std::cout << "msg m: " << msg << std::endl;
			breakpoints.insert(addr);
			std::cout << "Breakpoint set to addres 0x"<< std::hex << addr << std::endl;
			send_packet(conn, "OK");
		} else if (boost::starts_with(msg, "z1")) {
			send_packet(conn, "OK");
		} else if (boost::starts_with(msg, "z0")) {

		} else if (boost::starts_with(msg, "Z0")) {
			char * pEnd;
			long addr = strtol(msg.c_str() + 3, &pEnd, 16);;
			std::cout << "msg m: " << msg << std::endl;
			breakpoints.insert(addr);
			std::cout << "Breakpoint set to address 0x"<< std::hex << addr << std::endl;
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
	for (auto c : msg) {
		sum += unsigned(c);
	}
	sum = sum % 256;

	char low = nibble_to_hex[sum & 0xf];
	char high = nibble_to_hex[(sum & (0xf << 4)) >> 4];

	return {high, low};
}
