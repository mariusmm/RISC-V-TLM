# Another RISC-V ISA simulator.

**This code is suitable to hard refactor at any time**


This is another RISC-V ISA simulator, this is coded in SystemC + TLM-2.
It supports RV32IMCA Instruction set by now.

[![travis](https://travis-ci.org/mariusmm/RISC-V-TLM.svg?branch=master)](https://travis-ci.org/mariusmm/RISC-V-TLM)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0f7ccc8435f14ce2b241b3bfead772a2)](https://www.codacy.com/app/mariusmm/RISC-V-TLM?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mariusmm/RISC-V-TLM&amp;utm_campaign=Badge_Grade)
[![license](https://img.shields.io/badge/license-GNU--3.0-green.svg)](https://github.com/mariusmm/RISC-V-TLM/blob/master/LICENSE)
[![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/mariusmonton)

---
Table of Contents
=================
<!--ts-->
   * [Another RISC-V ISA simulator.](./README.md#another-risc-v-isa-simulator)
   * [Table of Contents](./README.md#table-of-contents)
      * [Description](./README.md#description)
         * [Structure](./README.md#structure)
      * [TODO](./README.md#todo)
      * [Compile](./README.md#compile)
      * [Docker container](./README.md#docker-container)
         * [How to use Docker](./README.md#how-to-use-docker)
      * [Test](./README.md#test)
         * [C code](./README.md#c-code)
         * [FreeRTOS](./README.md#freertos)
      * [Documentation](./README.md#documentation)
      * [Contribute](./README.md#contribute)
      * [License](./README.md#license)

<!-- Added by: marius, at: 2019-02-11T20:15+01:00 -->

<!--te-->

## Description

Brief description of the modules:
* CPU: Top entity that includes all other modules.
* Memory: Memory highly based on TLM-2 example with read file capability
* Registers: Implements the register file, PC register & CSR registers
* Execute: Executes ISA instructions
  * Executes C instruction extensions
  * Executes M instruction extensions
  * Executes A instruction extensions
* Instruction: Decodes instruction and acces to any instruction field
  * C_Instruction: Decodes Compressed instructions (C extension)
  * M_Instruction: Decodes Multiplication and Division instructions (M extension)
  * A_Instruction: Decodes Atomic instructions (A extension)
* Simulator: Top-level entity that builds & starts the simulation
* BusCtrl: Simple bus manager
* Trace: Simple trace peripheral
* Timer: Simple IRQ programable real-time counter peripheral

Helper classes:
* Performance: Performance indicators stores here (singleton class)
* Log: Log class to log them all (singleton class)

Current performance is about 1.500.000 instructions / sec in a Intel Core
i5-5200<span>@</span>2.2Ghz


### Structure
![Modules' hierarchy](https://github.com/mariusmm/RISC-V-TLM/blob/master/doc/Hierarchy.png)



## TODO
This is a preliminar and incomplete version.

Task to do:
- [x] Implement all missing instructions (Execute)
- [x] Implement CSRs (where/how?)
- [ ] Add full support to read file with memory contents (to memory.h)
   - [ ] .elf files
   - [x] .hex files (only partial .hex support)
- [ ]  Connect some TLM peripherals
     - [x] Debug module similiar to ARM's ITM
     - [ ] Some standard UART model
     - [ ] ...
- [ ] Implement interrupts
     - [x] implement timer (mtimecmp) & timer interrupt  
     - [ ] generic IRQ comtroller
- [x] Test, test, test & test. I'm sure there are a ~~lot of~~ some bugs in the code
     - [x] riscv-test almost complete (see [Test](https://github.com/mariusmm/RISC-V-TLM/wiki/Tests))
     - [ ] riscv-compliance WiP
* Improve structure and modules hierarchy
* Add 64 & 128 bits architecture (RV64I, RV128I)

## Compile
In order to compile the project you need SystemC-2.3.2 installed in your system.
Just change SYSTEMC path in Makefile.

```
$ make
```

Then, you need to modifiy your LD_LIBRARY_PATH environtment variable to add
path systemc library. In my case:
```
$ export LD_LIBRARY_PATH=/home/marius/Work/RiscV/code/systemc-2.3.2/lib-linux64
```

And then you can execute the simulator:
```
$ ./RISCV_TLM asm/BasicLoop.hex
```


## Docker container

There is a Docker container available with the latest release at https://hub.docker.com/r/mariusmm/riscv-tlm. 
This container has RISCV-TLM already build at /usr/src/riscv64/RISCV-TLM directory.

### How to use Docker
```
$ docker pull mariusmm/riscv-tlm
$ docker run -v <localfiles>:/tmp -it mariusmm/riscv-tlm /bin/bash

# cd /usr/src/riscv64/RISCV_TLM/
# ./RISCV_TLM /tmp/<your_hex_file>
```

I'm using docker image [zmors/riscv_gcc](https://hub.docker.com/r/zmors/riscv_gcc) to have a cross-compiler,  I'm using both docker images this way:
```
console1:
$ docker run -v /tmp:/PRJ -it zmors/riscv_gcc:1  bash

# cd /PRJ/func3
# make

console2:
$ docker run -v /tmp:/tmp -it mariusmm/riscv-tlm /bin/bash

# cd /usr/src/riscv64/RISC-V-TLM/ 
# ./RISCV-TLM /tmp/file.hex
...
```

Performance is not affected by running the simulator inside the container

## Test
See [Test page](Test) for more information.

In the asm directory there are some basic assembly examples.

I "compile" one file with the follwing command:
```
$ cd asm
$ riscv32-unknown-elf-as  EternalLoop.asm -o EternalLoop.o
$ riscv32-unknown-elf-ld -T ../my_linker_script.ld EternalLoop.o -o EternalLoop.elf
$ riscv32-unknown-elf-objcopy -O ihex EternalLoop.elf EternalLoop.hex
$ cd ..
$ ./RISCV_SCTLM asm/EternalLoop.hex
```
This example needs that you hit Ctr+C to stop execution.

### C code
The C directory contains simple examples in C. Each directory contains
an example, to compile it just:
```
$ make
```
and then execute the .hex file like the example before.

### FreeRTOS
FreeRTOS can run in this simulator!

In test/FreeRTOS/ directory there is portable files (port.c, portmacro.c portasm.S) and main file (freertos_test.c) ported to this RISC-V model.

## Documentation
The code is documented using doxygen. In the doc folder there is a Doxygen.cfg
file ready to be used.

## Contribute
There are several ways to contribute to this project:
* Test
* Pull request are welcome (see TODO list)
* Good documentation

##  License

Copyright (C) 2018 Màrius Montón ([\@mariusmonton](https://twitter.com/mariusmonton/))

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
