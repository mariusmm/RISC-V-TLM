/*!
 \file MemoryInterface.h
 \brief CPU to Memory Interface class AT model
 \author Màrius Montón
 \date Aug 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INC_MEMORYINTERFACEAT_H_
#define INC_MEMORYINTERFACEAT_H_

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "MemoryInterface.h"
#include "memory.h"
#include "Log.h"

#include <queue>

/// memory managed queue class
/// implements memory management IF
class tg_queue_c: public tlm::tlm_mm_interface {
public:

	/// tg_queue_c constructor
	tg_queue_c(void) {

	}

	/// enqueue entry (create)
	void enqueue(void) {
		tlm::tlm_generic_payload *transaction_ptr =
				new tlm::tlm_generic_payload(this); /// transaction pointer
		unsigned char *data_buffer_ptr = new unsigned char[4]; /// data buffer pointer

		transaction_ptr->set_data_ptr(data_buffer_ptr);

		m_queue.push(transaction_ptr);
	}

	/// transaction pointer
	/// dequeue entry
	tlm::tlm_generic_payload*
	dequeue(void) {
		tlm::tlm_generic_payload *transaction_ptr = m_queue.front();

		m_queue.pop();

		return transaction_ptr;
	}

	/// release entry
	/// transaction pointer
	void release(tlm::tlm_generic_payload *transaction_ptr) {
		transaction_ptr->release();
	}

	/// queue empty
	bool is_empty(void) {
		return m_queue.empty();
	}

	/// queue size
	size_t size(void) {
		return m_queue.size();
	}

	/// return transaction to the pool
	void free(tlm::tlm_generic_payload *transaction_ptr) {
		transaction_ptr->reset();
		m_queue.push(transaction_ptr);
	}

private:

	std::queue<tlm::tlm_generic_payload*> m_queue;        /// queue
};


/**
 * @brief Memory Interface with AT model
 */
class MemoryInterfaceAT : public MemoryInterface {

public:
	MemoryInterfaceAT();

	tlm_utils::simple_initiator_socket<MemoryInterfaceAT> data_bus;

	tlm::tlm_generic_payload *request_in_progress;
	tlm_utils::peq_with_cb_and_phase<MemoryInterfaceAT> m_peq;

	uint32_t readDataMem(uint32_t addr, int size);
	void writeDataMem(uint32_t addr, uint32_t data, int size);

	tlm::tlm_sync_enum nb_transport_bw(
			tlm::tlm_generic_payload &trans,
			tlm::tlm_phase &phase,
			sc_core::sc_time &delay);

    sc_core::sc_event end_request_event;
    sc_core::sc_event end_protocol;

	void peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase);

	tg_queue_c m_mm;
};

#endif /* INC_MEMORYINTERFACEAT_H_ */
