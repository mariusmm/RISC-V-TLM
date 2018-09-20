#include "Memory.h"

SC_HAS_PROCESS(Memory);
Memory::Memory(sc_module_name name, string filename): sc_module(name)
  ,socket("socket")
  ,LATENCY(SC_ZERO_TIME) {
  // Register callbacks for incoming interface method calls
  socket.register_b_transport(       this, &Memory::b_transport);
  socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
  socket.register_transport_dbg(     this, &Memory::transport_dbg);

  memset(mem, 0, SIZE*sizeof(uint8_t));
  // readCustomHexFile("memory.hex");
  readHexFile(filename);

  SC_THREAD(invalidation_process);
}

Memory::Memory(sc_module_name name, bool use_file): sc_module(name)
  ,socket("socket")
  ,LATENCY(SC_ZERO_TIME) {
    socket.register_b_transport(       this, &Memory::b_transport);
    socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
    socket.register_transport_dbg(     this, &Memory::transport_dbg);

    memset(mem, 0, SIZE*sizeof(int));

    SC_THREAD(invalidation_process);
  }


uint32_t Memory::getPCfromHEX() {
  return program_counter;

}
void Memory::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
  tlm::tlm_command cmd = trans.get_command();
  sc_dt::uint64    adr = trans.get_address();
  unsigned char*   ptr = trans.get_data_ptr();
  unsigned int     len = trans.get_data_length();
  unsigned char*   byt = trans.get_byte_enable_ptr();
  unsigned int     wid = trans.get_streaming_width();

  // Obliged to check address range and check for unsupported features,
  //   i.e. byte enables, streaming, and bursts
  // Can ignore extensions

  // *********************************************
  // Generate the appropriate error response
  // *********************************************

  if (adr >= sc_dt::uint64(SIZE)) {
    trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
    return;
  }
  if (byt != 0) {
    trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
    return;
  }
  if (len > 4 || wid < len) {
    trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
    return;
  }

  //cout << "MEM: addr=" << hex << adr << endl << endl;
  //cout << "MEM: data=" << hex << mem[adr] << endl;

  // Obliged to implement read and write commands
  if ( cmd == tlm::TLM_READ_COMMAND )
    memcpy(ptr, &mem[adr], len);
  else if ( cmd == tlm::TLM_WRITE_COMMAND )
    memcpy(&mem[adr], ptr, len);

  // Illustrates that b_transport may block
  wait(delay);

  // Reset timing annotation after waiting
  delay = SC_ZERO_TIME;

  // *********************************************
  // Set DMI hint to indicated that DMI is supported
  // *********************************************

  trans.set_dmi_allowed(true);

  // Obliged to set response status to indicate successful completion
  trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

bool Memory::get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                tlm::tlm_dmi& dmi_data)
{
  // Permit read and write access
  dmi_data.allow_read_write();

  // Set other details of DMI region
  dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>( &mem[0] ) );
  dmi_data.set_start_address( 0 );
  dmi_data.set_end_address( SIZE*4-1 );
  dmi_data.set_read_latency( LATENCY );
  dmi_data.set_write_latency( LATENCY );

  return true;
}


void Memory::invalidation_process()
{
  // Invalidate DMI pointers periodically
  for (int i = 0; i < 4; i++)
  {
    wait(LATENCY*8);
    socket->invalidate_direct_mem_ptr(0, SIZE-1);
  }
}


unsigned int Memory::transport_dbg(tlm::tlm_generic_payload& trans)
{
  tlm::tlm_command cmd = trans.get_command();
  sc_dt::uint64    adr = trans.get_address();
  unsigned char*   ptr = trans.get_data_ptr();
  unsigned int     len = trans.get_data_length();

  // Calculate the number of bytes to be actually copied
  unsigned int num_bytes = (len < (SIZE - adr) * 4) ? len : (SIZE - adr) * 4;

  if ( cmd == tlm::TLM_READ_COMMAND )
    memcpy(ptr, &mem[adr], num_bytes);
  else if ( cmd == tlm::TLM_WRITE_COMMAND )
    memcpy(&mem[adr], ptr, num_bytes);

  return num_bytes;
}

void Memory::readHexFile(string filename) {
    ifstream hexfile;
    string line;
    int byte_count;
    int address;
    int i = 0;
    int extended_address = 0;

    hexfile.open(filename);

    if (hexfile.is_open()) {
      while(getline(hexfile, line) ) {
          /* # is a comentary in the file */
        if (line[0] == ':') {
           if (line.substr(7,2) == "00")
          {
            /* Data */
            byte_count = stol(line.substr(1,2), nullptr, 16);
            address = stol(line.substr(3,4), nullptr, 16);
            address = address + extended_address;
            
            for (i=0; i < byte_count; i++) {
              mem[address+i] = stol(line.substr(9+(i*2), 2), nullptr, 16);
              }
          } else if (line.substr(7,2) == "02") {
            /* Extended segment address */
            extended_address = stol(line.substr(9,4), nullptr, 16) * 16;
          } else if (line.substr(7,2) == "03") {
            /* Start segment address */
              uint32_t code_segment;
              code_segment = stol(line.substr(9,4), nullptr, 16) * 16; /* ? */
              program_counter = stol(line.substr(13,4), nullptr, 16);
              program_counter = program_counter + code_segment;
          }
        }
      }
      hexfile.close();
    } else {
      SC_REPORT_ERROR("Memory", "Open file error");
    }

#if 0
    for(int i = 50;i<100; i++) {
      cout << "Dump address: 0x" << hex << extended_address + i << ": 0x" <<
      mem[extended_address+i] << dec << endl;
    }
#endif
}
