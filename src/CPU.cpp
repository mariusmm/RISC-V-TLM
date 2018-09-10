
#include "CPU.h"

SC_HAS_PROCESS(CPU);
CPU::CPU(sc_module_name name): sc_module(name)
, instr_bus("instr_bus")
//, exec("RISC_V_exec", &register_bank)
//, data_bus("data_bus")
 {
   register_bank = new Registers();
   exec = new RISC_V_execute("RISC_V_execute", register_bank);
   perf = Performance::getInstance();
   log = Log::getInstance();

   SC_THREAD(CPU_thread);
}

CPU::~CPU() {
  cout << "*********************************************" << endl;
  register_bank->dump();
  cout << sc_time_stamp() << endl;
  perf->dump();
  cout << "*********************************************" << endl;
}
/**
 * main thread for CPU simulation
 * @brief CPU mai thread
 */
void CPU::CPU_thread(void) {

  tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
  int32_t INSTR;
  sc_time delay = SC_ZERO_TIME;

  trans->set_command( tlm::TLM_READ_COMMAND );
  trans->set_data_ptr( reinterpret_cast<unsigned char*>(&INSTR) );
  trans->set_data_length( 4 );
  trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
  trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
  trans->set_dmi_allowed( false ); // Mandatory initial value
  trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

  register_bank->dump();

  while(1) {
      /* Get new PC value */
      trans->set_address( register_bank->getPC() );
      instr_bus->b_transport( *trans, delay);
      perf->codeMemoryRead();

      if ( trans->is_response_error() ) {
        SC_REPORT_ERROR("CPU base", "Read memory");
      } else {
        // cout << "INSTR: " << INSTR << endl;
        log->SC_log(Log::INFO) << "PC: " << register_bank->getPC() << endl;
        Instruction inst(INSTR);

        switch(inst.decode()) {
          case OP_LUI:
            exec->LUI(inst);
            break;
          case OP_AUIPC:
            exec->AUIPC(inst);
            break;
          case OP_JAL:
            exec->JAL(inst);
            break;
          case OP_BEQ:
            exec->BEQ(inst);
            break;
          case OP_BNE:
            exec->BNE(inst);
            break;
          case OP_ADDI:
            exec->ADDI(inst);
            break;
          case OP_ADD:
            exec->ADD(inst);
            break;
          case OP_SUB:
            exec->SUB(inst);
            break;
          default:
            exec->NOP(inst);
        }
        perf->instructionsInc();

        register_bank->incPC();

        /* Simulation control, we stop at 10 instructions (if no NOP found)*/
        if (register_bank->getPC() == 10*4) {
          cout << "*********************************************" << endl;
          register_bank->dump();
          cout << sc_time_stamp() << endl;
          cout << "*********************************************" << endl;

          perf->dump();

          sc_stop();
        }
      }
  } // while(1)
} // CPU_thread
