#include "Timer.h"

SC_HAS_PROCESS(Timer);
Timer::Timer(sc_module_name name): sc_module(name)
  ,socket("timer_socket"), m_mtime(0), m_mtimecmp(0), irq_value(false) {

    socket.register_b_transport(this, &Timer::b_transport);

    SC_THREAD(run);
  }

/**
 * @brief Waits for event timer_event and triggers an IRQ
 *
 */
void Timer::run() {
  while(true) {

    // timer_event.notify( sc_time(10000, SC_NS) );

    wait(timer_event);

    if (timer_irq.read() == true) {
      timer_irq.write(false);
      // cout << "time: " << sc_time_stamp() <<". bla bla " << endl;
    } else {
      timer_irq.write(true);
      cout << "time: " << sc_time_stamp() << ". Timer interrupt!" << endl;
      // notify in 20 ns to low irq signal
      timer_event.notify( sc_time(20, SC_NS));
    }
  }
}

/**
 *
 * @brief TLM-2.0 socket implementaiton
 * @param trans TLM-2.0 transaction
 * @param delay transactino delay time
 */
void Timer::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    addr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    uint32_t aux_value = 0;
    uint64_t notify_time = 0;
    unsigned int     len = trans.get_data_length();
    //unsigned char*   byt = trans.get_byte_enable_ptr();
    //unsigned int     wid = trans.get_streaming_width();

    // cout << "accessing TIMER 0x" << hex << addr << endl;
    if (cmd == tlm::TLM_WRITE_COMMAND) {
      memcpy(&aux_value, ptr, len);
      switch (addr) {
        case TIMER_MEMORY_ADDRESS_LO:
            m_mtime.range(31,0) = aux_value;
            break;
        case TIMER_MEMORY_ADDRESS_HI:
            m_mtime.range(63,32) = aux_value;
            break;
        case TIMERCMP_MEMORY_ADDRESS_LO:
            m_mtimecmp.range(31,0) = aux_value;
            // timer_event.notify(SC_ZERO_TIME);
            break;
        case TIMERCMP_MEMORY_ADDRESS_HI:
            m_mtimecmp.range(63,32) = aux_value;
            // notify needs relative time, mtimecmp works in absolute time
            notify_time = m_mtimecmp  - m_mtime;
            // cout << "time: " << sc_core::sc_time_stamp()
            //     << ": interrupt will be in " << dec << notify_time
            //     << " ns" << endl;
            timer_event.notify( sc_time(notify_time, SC_NS) );
            break;
      }
    } else { // TLM_READ_COMMAND
      switch (addr) {
        case TIMER_MEMORY_ADDRESS_LO:
            m_mtime = sc_time_stamp().value();
            aux_value = m_mtime.range(31,0);
            break;
        case TIMER_MEMORY_ADDRESS_HI:
            aux_value = m_mtime.range(63,32);
            break;
        case TIMERCMP_MEMORY_ADDRESS_LO:
            aux_value = m_mtimecmp.range(31,0);
            break;
        case TIMERCMP_MEMORY_ADDRESS_HI:
            aux_value = m_mtimecmp.range(63,32);
            break;
      }
      memcpy(ptr, &aux_value, len);
    }

    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}
