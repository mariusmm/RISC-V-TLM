/*!
 \file Simulator.cpp
 \brief Top level simulation entity
 \author Màrius Montón
 \date September 2018
 */

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include <csignal>
#include <unistd.h>
#include <chrono>
#include <cstdint>

#include "CPU.h"
#include "BusCtrl.h"
#include "Trace.h"
#include "Timer.h"
#include "Debug.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


std::string filename;
bool debug_session = false;
bool mem_dump = false;
uint32_t dump_addr_st = 0;
uint32_t dump_addr_end = 0;

riscv_tlm::cpu_types_t cpu_type_opt = riscv_tlm::RV32;

/**
 * @class Simulator
 * This class instantiates all necessary modules, connects its ports and starts
 * the simulation.
 *
 * @brief Top simulation entity
 */
class Simulator : sc_core::sc_module {
public:
    riscv_tlm::CPU *cpu;
	riscv_tlm::Memory *MainMemory;
    riscv_tlm::BusCtrl *Bus;
    riscv_tlm::peripherals::Trace *trace;
    riscv_tlm::peripherals::Timer *timer;

	explicit Simulator(sc_core::sc_module_name const &name, riscv_tlm::cpu_types_t cpu_type_m): sc_module(name) {
		std::uint32_t start_PC;

		MainMemory = new riscv_tlm::Memory("Main_Memory", filename);
		start_PC = MainMemory->getPCfromHEX();

        cpu_type = cpu_type_m;

        if (cpu_type == riscv_tlm::RV32) {
            cpu = new riscv_tlm::CPURV32("cpu", start_PC, debug_session);
        } else if (cpu_type == riscv_tlm::RV32E20) {
            start_PC = 0x100080;
            cpu = new riscv_tlm::CPURV32("cpu", start_PC, debug_session);
        } else {
            cpu = new riscv_tlm::CPURV64("cpu", start_PC, debug_session);
        }

        Bus = new riscv_tlm::BusCtrl("BusCtrl");
		trace = new riscv_tlm::peripherals::Trace("Trace");
		timer = new riscv_tlm::peripherals::Timer("Timer");

		cpu->instr_bus.bind(Bus->cpu_instr_socket);
		cpu->mem_intf->data_bus.bind(Bus->cpu_data_socket);

        // Map peripherals to memory map
        auto port = Bus->register_peripheral(0x40000000, 0x40000000);
        Bus->peripherals_sockets[port].bind(trace->socket);
        port = Bus->register_peripheral(0x40004000, 0x4000400C);
        Bus->peripherals_sockets[port].bind(timer->socket);

        // All ports should be connected, if there is no peripheral, we map it to memory.
        for (auto i = 0; i < NR_OF_PERIPHERALS; i++) {
            if (Bus->peripherals_sockets[i].size() == 0) { 
                Bus->peripherals_sockets[i].bind(MainMemory->socket);
            }
        }

		timer->irq_line.bind(cpu->irq_line_socket);

		if (debug_session) {
            if (cpu_type == riscv_tlm::RV32) {
                riscv_tlm::Debug Debug(dynamic_cast<riscv_tlm::CPURV32*>(cpu), MainMemory);
            } else {
                riscv_tlm::Debug Debug(dynamic_cast<riscv_tlm::CPURV64*>(cpu), MainMemory);
            }
		}
	}

	~Simulator() override {
	    if (mem_dump) {
            MemoryDump();
        }
		delete MainMemory;
		delete cpu;
		delete Bus;
		delete trace;
		delete timer;
	}

private:
    void MemoryDump() const {
	    std::cout << "********** MEMORY DUMP ***********\n";

        if (dump_addr_st == 0) {
            dump_addr_st = cpu->getStartDumpAddress();
        }

        if (dump_addr_end == 0) {
            dump_addr_end = cpu->getEndDumpAddress();
        }

        std::cout << "from 0x" << std::hex << dump_addr_st << " to 0x" << dump_addr_end << "\n";
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay;
        std::uint32_t data[4];

        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(data));
        trans.set_data_length(4);
        trans.set_streaming_width(4); // = data_length to indicate no streaming
        trans.set_byte_enable_ptr(nullptr); // 0 indicates unused
        trans.set_dmi_allowed(false); // Mandatory initial value
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

        /* Filename in format name.elf.hex should be name.signature_output */
        std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
        std::string base_name = base_filename.substr(0, base_filename.find('.'));
        std::string local_name = base_name + ".signature.output";
        std::cout << "filename is " << local_name << '\n';

        std::ofstream signature_file;
        signature_file.open(local_name);

        for(unsigned int i = dump_addr_st; i < dump_addr_end; i = i+4) {
            //trans.set_address(dump_addr + (i*4));
            trans.set_address(i);
            MainMemory->b_transport(0, trans, delay);
            signature_file << std::hex << std::setfill('0') << std::setw(8) << data[0] <<  "\n";
        }

        signature_file.close();
       }

private:
    riscv_tlm::cpu_types_t cpu_type;
};

Simulator *top;
std::shared_ptr<spdlog::logger> logger;

void intHandler(int dummy) {
	delete top;
	(void) dummy;
	//sc_stop();
	exit(-1);
}

void process_arguments(int argc, char *argv[]) {

	int c;
	long int debug_level;

	debug_session = false;
    cpu_type_opt = riscv_tlm::RV32;

	while ((c = getopt(argc, argv, "DTE:B:L:f:R:?")) != -1) {
		switch (c) {
		case 'D':
			debug_session = true;
			break;
        case 'T':
            mem_dump = true;
            break;
        case 'B':
            dump_addr_st = std::strtoul (optarg, nullptr, 16);
            break;
        case 'E':
            dump_addr_end = std::strtoul(optarg, nullptr, 16);
            break;
		case 'L':
			debug_level = std::strtol(optarg, nullptr, 10);

            if (debug_level >= 0) {
                spdlog::filename_t log_filename = SPDLOG_FILENAME_T("newlog.txt");
                logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("my_logger", log_filename, true);
                logger->set_pattern("%v");
                logger->set_level(spdlog::level::info);
            } else {
                logger = nullptr;
            }

			switch (debug_level) {
			case 3:
                logger->set_level(spdlog::level::info);
				break;
			case 2:
                logger->set_level(spdlog::level::warn);
				break;
			case 1:
                logger->set_level(spdlog::level::debug);
				break;
			case 0:
                logger->set_level(spdlog::level::err);
				break;
			default:
                logger->set_level(spdlog::level::info);
				break;
			}
			break;
		case 'f':
			filename = std::string(optarg);
			break;
        case 'R':
            if (strcmp(optarg, "32") == 0) {
                cpu_type_opt = riscv_tlm::RV32;
                std::cout << "RV32" << std::endl;
            } else if (strcmp(optarg, "32E20") == 0) {
                cpu_type_opt = riscv_tlm::RV32E20;
            } else {
                cpu_type_opt = riscv_tlm::RV64;
            }
            break;
		case '?':
			std::cout << "Call ./RISCV_TLM -D -L <debuglevel> (0..3) filename.hex"
					<< std::endl;
			break;
		default:
			std::cout << "unknown" << std::endl;
		}
	}

	if (filename.empty()) {
		filename = std::string(argv[optind]);
	}

	std::cout << "file: " << filename << '\n';
}

int sc_main(int argc, char *argv[]) {

  Performance *perf = Performance::getInstance();

	/* Capture Ctrl+C and finish the simulation */
	signal(SIGINT, intHandler);

	/* SystemC time resolution set to 1 ns*/
	sc_core::sc_set_time_resolution(1, sc_core::SC_NS);

	/* Parse and process program arguments. -f is mandatory */
	process_arguments(argc, argv);

    if (logger == nullptr) {
        spdlog::filename_t log_filename = SPDLOG_FILENAME_T("/dev/null");
        logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("my_logger", log_filename, true);
        logger->set_pattern("%v");
        logger->set_level(spdlog::level::info);
    }

	top = new Simulator("top", cpu_type_opt);

	auto start = std::chrono::steady_clock::now();
	sc_core::sc_start();
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
    double instructions = static_cast<double>(perf->getInstructions()) / elapsed_seconds.count();

	std::cout << "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::cout << "Simulated " << int(std::round(instructions)) << " instr/sec" << std::endl;

	if (!mem_dump)
    {
        std::cout << "Press Enter to finish" << std::endl;
        std::cin.ignore();
    }

	// call all destructors, clean exit.
	delete top;

	return 0;
}
