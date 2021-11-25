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
#include "Memory.h"
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

/**
 * @class Simulator
 * This class instantiates all necessary modules, connects its ports and starts
 * the simulation.
 *
 * @brief Top simulation entity
 */
class Simulator : sc_core::sc_module {
public:
    CPU *cpu;
	Memory *MainMemory;
	BusCtrl *Bus;
	Trace *trace;
	Timer *timer;

	explicit Simulator(sc_core::sc_module_name const &name): sc_module(name) {
		std::uint32_t start_PC;

		MainMemory = new Memory("Main_Memory", filename);
		start_PC = MainMemory->getPCfromHEX();

		cpu = new CPU("cpu", start_PC, debug_session);

		Bus = new BusCtrl("BusCtrl");
		trace = new Trace("Trace");
		timer = new Timer("Timer");

		cpu->instr_bus.bind(Bus->cpu_instr_socket);
		cpu->mem_intf->data_bus.bind(Bus->cpu_data_socket);

		Bus->memory_socket.bind(MainMemory->socket);
		Bus->trace_socket.bind(trace->socket);
		Bus->timer_socket.bind(timer->socket);

		timer->irq_line.bind(cpu->irq_line_socket);

		if (debug_session) {
			Debug debug(cpu, MainMemory);
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
    void MemoryDump() {
	    std::cout << "********** MEMORY DUMP ***********\n";
        tlm::tlm_generic_payload trans;
        tlm::tlm_dmi dmi_data;
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
            MainMemory->b_transport(trans, delay);
            signature_file << std::hex << std::setfill('0') << std::setw(8) << data[0] <<  "\n";
        }

        signature_file.close();
       }
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
	int debug_level;

	debug_session = false;

	while ((c = getopt(argc, argv, "DTE:B:L:f:?")) != -1) {
		switch (c) {
		case 'D':
			debug_session = true;
			break;
        case 'T':
            mem_dump = true;
            break;
        case 'B':
            dump_addr_st = std::strtoul (optarg, 0, 16);
            break;
        case 'E':
            dump_addr_end = std::strtoul(optarg, 0, 16);
            break;
		case 'L':
			debug_level = std::atoi(optarg);

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

    spdlog::filename_t filename = SPDLOG_FILENAME_T("newlog.txt");
    logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("my_logger", filename);
    logger->set_pattern("%v");
    logger->set_level(spdlog::level::info);

	/* Parse and process program arguments. -f is mandatory */
	process_arguments(argc, argv);

	top = new Simulator("top");

	auto start = std::chrono::steady_clock::now();
	sc_core::sc_start();
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
    double instructions = static_cast<double>(perf->getInstructions()) / elapsed_seconds.count();

	std::cout << "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::cout << "Simulated " << int(std::round(instructions)) << " instr/sec" << std::endl;

	if (!mem_dump) {
        std::cout << "Press Enter to finish" << std::endl;
        std::cin.ignore();
    }

	// call all destructors, clean exit.
	delete top;

	return 0;
}
