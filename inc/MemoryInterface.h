/*!
 \file MemoryInterface.h
 \brief CPU to Memory Interface class
 \author Màrius Montón
 \date May 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_MEMORYINTERFACE_H_
#define INC_MEMORYINTERFACE_H_

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

#include "memory.h"
#include "Log.h"

/**
 * @brief Memory Interface
 */
class MemoryInterface {
public:

	tlm_utils::simple_initiator_socket<MemoryInterface> data_bus;

	MemoryInterface();
	uint32_t readDataMem(uint32_t addr, int size);
	void writeDataMem(uint32_t addr, uint32_t data, int size);
};

#endif /* INC_MEMORYINTERFACE_H_ */
