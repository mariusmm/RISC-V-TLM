#include "Timer.h"

SC_HAS_PROCESS(Timer);
Timer::Timer(sc_module_name name): sc_module(name)
  ,socket("timer_socket"), m_mtime(0), m_mtimecmp(0) {

    socket.register_b_transport(this, &Timer::b_transport);

    SC_THREAD(run);
  }

void Timer::run() {

  tlm::tlm_generic_payload* irq_trans = new tlm::tlm_generic_payload;
  sc_time delay = SC_ZERO_TIME;

  irq_trans->set_command(tlm::TLM_WRITE_COMMAND);
  irq_trans->set_data_ptr(NULL);
  irq_trans->set_data_length(0);
  irq_trans->set_streaming_width(0);
  irq_trans->set_byte_enable_ptr(0);
  irq_trans->set_dmi_allowed(false);
  irq_trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  irq_trans->set_address( 0x01);

  while(true) {
    wait(timer_event);
    irq_line->b_transport(*irq_trans, delay);
  }
}

void Timer::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    addr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    //unsigned char*   byt = trans.get_byte_enable_ptr();
    //unsigned int     wid = trans.get_streaming_width();

    uint32_t aux_value = 0;
    uint64_t notify_time = 0;

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
            break;
        case TIMERCMP_MEMORY_ADDRESS_HI:
            m_mtimecmp.range(63,32) = aux_value;

            // notify needs relative time, mtimecmp works in absolute time
            notify_time = m_mtimecmp  - m_mtime;
            // cout << "time: " << sc_time_stamp() << ". Timer: IRQ will be at "
            //   << dec << notify_time + m_mtime << " ns." << endl;
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
