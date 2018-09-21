
#include "CPU.h"

SC_HAS_PROCESS(CPU);
CPU::CPU(sc_module_name name, uint32_t PC): sc_module(name)
, instr_bus("instr_bus")
{
   register_bank = new Registers();
   exec = new Execute("Execute", register_bank);
   perf = Performance::getInstance();
   log = Log::getInstance();

   register_bank->setPC(PC);
   SC_THREAD(CPU_thread);
}

CPU::~CPU() {
  cout << "*********************************************" << endl;
  register_bank->dump();
  cout << "end time: " << sc_time_stamp() << endl;
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
  bool PC_not_affected = true;

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
        log->SC_log(Log::INFO) << "PC: " << hex << register_bank->getPC()
              << dec << endl;
        Instruction inst(INSTR);

        PC_not_affected = true;
        switch(inst.decode()) {
          case OP_LUI:
            exec->LUI(inst);
            break;
          case OP_AUIPC:
            exec->AUIPC(inst);
            break;
          case OP_JAL:
            exec->JAL(inst);
            PC_not_affected = false;
            break;
          case OP_JALR:
            exec->JALR(inst);
            PC_not_affected = false;
            break;
          case OP_BEQ:
            exec->BEQ(inst);
            PC_not_affected = false;
            break;
          case OP_BNE:
            exec->BNE(inst);
            PC_not_affected = false;
            break;
          case OP_BLT:
            exec->BLT(inst);
            PC_not_affected = false;
            break;
          case OP_BGE:
            exec->BGE(inst);
            PC_not_affected = false;
            break;
          case OP_BLTU:
            exec->BLTU(inst);
            PC_not_affected = false;
            break;
          case OP_BGEU:
            exec->BGEU(inst);
            PC_not_affected = false;
            break;
          case OP_LB:
            exec->LB(inst);
            break;
          case OP_LH:
            exec->LB(inst);
            break;
          case OP_LW:
            exec->LW(inst);
            break;
          case OP_LBU:
            exec->LBU(inst);
            break;
          case OP_LHU:
            exec->LHU(inst);
            break;
          case OP_SB:
            exec->SB(inst);
            break;
          case OP_SH:
            exec->SH(inst);
            break;
          case OP_SW:
            exec->SW(inst);
            break;
          case OP_ADDI:
            exec->ADDI(inst);
            break;
          case OP_SLTI:
            exec->SLTI(inst);
            break;
          case OP_SLTIU:
            exec->SLTIU(inst);
            break;
          case OP_XORI:
            exec->XORI(inst);
            break;
          case OP_ORI:
            exec->ORI(inst);
            break;
          case OP_ANDI:
            exec->ANDI(inst);
            break;
          case OP_SLLI:
            exec->SLLI(inst);
            break;
          case OP_SRLI:
            exec->SRLI(inst);
            break;
          case OP_SRAI:
            exec->SRAI(inst);
            break;
          case OP_ADD:
            exec->ADD(inst);
            break;
          case OP_SUB:
            exec->SUB(inst);
            break;
          case OP_SLL:
            exec->SLL(inst);
            break;
          case OP_SLT:
            exec->SLT(inst);
            break;
          case OP_SLTU:
            exec->SLTU(inst);
            break;
          case OP_XOR:
            exec->XOR(inst);
            break;
          case OP_SRL:
            exec->SRL(inst);
            break;
          case OP_SRA:
            exec->SRA(inst);
            break;
          case OP_OR:
            exec->OR(inst);
            break;
          case OP_AND:
            exec->AND(inst);
            break;
#if 0
          case OP_CSRRW:
            exec->CSRRW(inst);
            break;
          case OP_CSRRS:
            exec->CSRRS(inst);
            break;
          case OP_CSRRC:
            exec->CSRRC(inst);
            break;
#endif
          default:
            cout << endl << "Instruction not implemented: ";
            inst.dump();
            exec->NOP(inst);
        }
        perf->instructionsInc();

        if (PC_not_affected == true) {
          register_bank->incPC();
        }
      }
  } // while(1)
} // CPU_thread
