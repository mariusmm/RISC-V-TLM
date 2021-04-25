/*!
 \file Memory.cpp
 \brief Basic TLM-2 memory model
 \author Màrius Montón
 \date August 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Memory.h"

SC_HAS_PROCESS(Memory);
Memory::Memory(sc_core::sc_module_name const &name, std::string const &filename) :
		sc_module(name), socket("socket"), LATENCY(sc_core::SC_ZERO_TIME) {
	// Register callbacks for incoming interface method calls
	socket.register_b_transport(this, &Memory::b_transport);
	socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
	socket.register_transport_dbg(this, &Memory::transport_dbg);

	mem = new uint8_t[SIZE];
	//memset(mem, 0, SIZE*sizeof(uint8_t));

	memory_offset = 0;
    program_counter = 0;
	readHexFile(filename);

	log = Log::getInstance();
	log->SC_log(Log::INFO) << "Using file: " << filename << std::endl;
}

Memory::Memory(sc_core::sc_module_name const& name) :
		sc_module(name), socket("socket"), LATENCY(sc_core::SC_ZERO_TIME) {
	socket.register_b_transport(this, &Memory::b_transport);
	socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
	socket.register_transport_dbg(this, &Memory::transport_dbg);
	memory_offset = 0;
	program_counter = 0;

	mem = new uint8_t[SIZE];

	log = Log::getInstance();
	log->SC_log(Log::INFO) << "Memory instantiated without file" << std::endl;
}

Memory::~Memory() {
	delete[] mem;
}

uint32_t Memory::getPCfromHEX() {
	return program_counter;

}
void Memory::b_transport(tlm::tlm_generic_payload &trans,
		sc_core::sc_time &delay) {
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 adr = trans.get_address();
	unsigned char *ptr = trans.get_data_ptr();
	unsigned int len = trans.get_data_length();
	unsigned char *byt = trans.get_byte_enable_ptr();
	unsigned int wid = trans.get_streaming_width();

	adr = adr - memory_offset;

	// Obliged to check address range and check for unsupported features,
	//   i.e. byte enables, streaming, and bursts
	// Can ignore extensions

	// *********************************************
	// Generate the appropriate error response
	// *********************************************
	if (adr >= sc_dt::uint64(SIZE)) {
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return;
	}
	if (byt != nullptr) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
	}
	if (len > 4 || wid < len) {
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}


	// Obliged to implement read and write commands
	if (cmd == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &mem[adr], len);
	else if (cmd == tlm::TLM_WRITE_COMMAND)
		memcpy(&mem[adr], ptr, len);

	// Illustrates that b_transport may block
	//sc_core::wait(delay);

	// Reset timing annotation after waiting
	delay = sc_core::SC_ZERO_TIME;

	// *********************************************
	// Set DMI hint to indicated that DMI is supported
	// *********************************************

	if (memory_offset == 0) {
		trans.set_dmi_allowed(true);
	} else {
		trans.set_dmi_allowed(false);
	}

	// Obliged to set response status to indicate successful completion
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

bool Memory::get_direct_mem_ptr(tlm::tlm_generic_payload &trans,
		tlm::tlm_dmi &dmi_data) {
  (void) trans;

	if (memory_offset != 0) {
		return false;
	}


	// Permit read and write access
	dmi_data.allow_read_write();

	// Set other details of DMI region
	dmi_data.set_dmi_ptr(reinterpret_cast<unsigned char*>(&mem[0]));
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(SIZE * 4 - 1);
	dmi_data.set_read_latency(LATENCY);
	dmi_data.set_write_latency(LATENCY);

	return true;
}

unsigned int Memory::transport_dbg(tlm::tlm_generic_payload &trans) {
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 adr = trans.get_address();
	unsigned char *ptr = trans.get_data_ptr();
	unsigned int len = trans.get_data_length();

	if (adr >= sc_dt::uint64(SIZE)) {
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return 0;
	}

	// Calculate the number of bytes to be actually copied
	unsigned int num_bytes = (len < (SIZE - adr) * 4) ? len : (SIZE - adr) * 4;

	if (cmd == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &mem[adr], num_bytes);
	else if (cmd == tlm::TLM_WRITE_COMMAND)
		memcpy(&mem[adr], ptr, num_bytes);

	return num_bytes;
}

void Memory::readHexFile(std::string const& filename) {
	std::ifstream hexfile;
	std::string line;

	hexfile.open(filename);

	if (hexfile.is_open()) {
		uint32_t extended_address = 0;

		while (getline(hexfile, line)) {
			if (line[0] == ':') {
				if (line.substr(7, 2) == "00") {
					/* Data */
				  int byte_count;
				  uint32_t address;
					byte_count = std::stoi(line.substr(1, 2), nullptr, 16);
					address = std::stoi(line.substr(3, 4), nullptr, 16);
					address = address + extended_address;
					//cout << "00 address 0x" << hex << address << endl;
					for (int i = 0; i < byte_count; i++) {
						mem[address + i] = stol(line.substr(9 + (i * 2), 2),
								nullptr, 16);
					}
				} else if (line.substr(7, 2) == "02") {
					/* Extended segment address */
					extended_address = stol(line.substr(9, 4), nullptr, 16)
							* 16;
					std::cout << "02 extended address 0x" << std::hex
							<< extended_address << std::dec << std::endl;
				} else if (line.substr(7, 2) == "03") {
					/* Start segment address */
					uint32_t code_segment;
					code_segment = stol(line.substr(9, 4), nullptr, 16) * 16; /* ? */
					program_counter = stol(line.substr(13, 4), nullptr, 16);
					program_counter = program_counter + code_segment;
					std::cout << "03 PC set to 0x" << std::hex
							<< program_counter << std::dec << std::endl;
				} else if (line.substr(7, 2) == "04") {
					/* Start segment address */
					memory_offset = stol(line.substr(9, 4), nullptr, 16) << 16;
					extended_address = 0;
					std::cout << "04 address set to 0x" << std::hex
							<< extended_address << std::dec << std::endl;
					std::cout << "04 offset set to 0x" << std::hex
							<< memory_offset << std::dec << std::endl;
				} else if (line.substr(7, 2) == "05") {
					program_counter = stol(line.substr(9, 8), nullptr, 16);
					std::cout << "05 PC set to 0x" << std::hex
							<< program_counter << std::dec << std::endl;
				}
			}
		}
		hexfile.close();
	} else {
		SC_REPORT_ERROR("Memory", "Open file error");
	}
}
