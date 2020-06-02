/*!
 \file Performance.cpp
 \brief Class to store performance of CPU
 \author Màrius Montón
 \date Aug 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Performance.h"

Performance* Performance::getInstance() {
	if (instance == 0) {
		instance = new Performance();
	}

	return instance;
}

Performance::Performance() {
	data_memory_read = 0;
	data_memory_write = 0;
	code_memory_read = 0;
	code_memory_write = 0;
	register_read = 0;
	register_write = 0;
	instructions_executed = 0;
}

void Performance::dump() {
	std::cout << std::dec << "# data memory reads: " << data_memory_read
			<< std::endl;
	std::cout << "# data memory writes: " << data_memory_write << std::endl;
	std::cout << "# code memory reads: " << code_memory_read << std::endl;
	std::cout << "# code memory writes: " << code_memory_write << std::endl;
	std::cout << "# registers read: " << register_read << std::endl;
	std::cout << "# registers write: " << register_write << std::endl;
	std::cout << "# instructions executed: " << instructions_executed
			<< std::endl;
}

Performance *Performance::instance = 0;
