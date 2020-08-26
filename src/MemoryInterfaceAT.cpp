/*!
 \file MemoryInterfaceAT.cpp
 \brief CPU to Memory Interface class AT model
 \author Màrius Montón
 \date Aug 2020
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MemoryInterfaceAT.h"


MemoryInterfaceAT::MemoryInterfaceAT() :
		request_in_progress(0),
		m_peq(this, &MemoryInterfaceAT::peq_cb) {
	data_bus.register_nb_transport_bw(this, &MemoryInterfaceAT::nb_transport_bw);

	  // build transaction pool (4 transactions)
	  for (unsigned int i = 0; i < 4; i++ )
	  {
	    m_mm.enqueue ();
	  }

}


/**
 * Access data memory to get data
 * @param  addr address to access to
 * @param size size of the data to read in bytes
 * @return data value read
 */
uint32_t MemoryInterfaceAT::readDataMem(uint32_t addr, int size) {
	uint32_t data;
	tlm::tlm_generic_payload *trans;
	sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
	tlm::tlm_phase phase;

	trans = m_mm.dequeue();
	trans->acquire();

	trans->set_command(tlm::TLM_READ_COMMAND);
	trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data));
	trans->set_data_length(size);
	trans->set_streaming_width(4); // = data_length to indicate no streaming
	trans->set_byte_enable_ptr(0); // 0 indicates unused
	trans->set_dmi_allowed(false); // Mandatory initial value
	trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	trans->set_address(addr);

	if (request_in_progress) {
		sc_core::wait(end_request_event);
	}

	request_in_progress = trans;
	phase = tlm::BEGIN_REQ;

	tlm::tlm_sync_enum status;
	status = data_bus->nb_transport_fw(*trans, phase, delay);

	if (status == tlm::TLM_UPDATED) {
		// The timing annotation must be honored
		m_peq.notify(*trans, phase, delay);
	} else if (status == tlm::TLM_COMPLETED) {
		request_in_progress = 0;

		// Allow the memory manager to free the transaction object
		trans->release();
	}
	return data;
}

/**
 * Acces data memory to write data
 * @brief
 * @param addr addr address to access to
 * @param data data to write
 * @param size size of the data to write in bytes
 */
void MemoryInterfaceAT::writeDataMem(uint32_t addr, uint32_t data, int size) {
	tlm::tlm_generic_payload *trans;
	tlm::tlm_phase phase;

	sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
	trans = m_mm.dequeue ();
    trans->acquire();


	trans->set_command(tlm::TLM_WRITE_COMMAND);
	trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data));
	trans->set_data_length(size);
	trans->set_streaming_width(4); // = data_length to indicate no streaming
	trans->set_byte_enable_ptr(0); // 0 indicates unused
	trans->set_dmi_allowed(false); // Mandatory initial value
	trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	trans->set_address(addr);

	if (request_in_progress) {
		sc_core::wait(end_request_event);
	}

	request_in_progress = trans;
	phase = tlm::BEGIN_REQ;

	tlm::tlm_sync_enum status;
	status = data_bus->nb_transport_fw( *trans, phase, delay );

	if (status == tlm::TLM_UPDATED) {
		// The timing annotation must be honored
		m_peq.notify(*trans, phase, delay);
	} else if (status == tlm::TLM_COMPLETED) {
		request_in_progress = 0;

		// Allow the memory manager to free the transaction object
		trans->release();
	}

}

tlm::tlm_sync_enum MemoryInterfaceAT::nb_transport_bw(
		tlm::tlm_generic_payload &trans,
		tlm::tlm_phase &phase,
		sc_core::sc_time &delay) {

	m_peq.notify( trans, phase, delay );

	return tlm::TLM_ACCEPTED;
}

void MemoryInterfaceAT::peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase)
   {
       if (phase == tlm::END_REQ || (&trans == request_in_progress
                                     && phase == tlm::BEGIN_RESP)) {
           // The end of the BEGIN_REQ phase
           request_in_progress = 0;
           end_request_event.notify();
       } else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP)
           SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");

       if (phase == tlm::BEGIN_RESP) {
           //check_transaction( trans );

           // Send final phase transition to target
           tlm::tlm_phase fw_phase = tlm::END_RESP;
           sc_core::sc_time delay = sc_core::sc_time(60000, sc_core::SC_PS);
           data_bus->nb_transport_fw( trans, fw_phase, delay );
           // Ignore return value

           // Allow the memory manager to free the transaction object
           trans.release();
       }
   }


