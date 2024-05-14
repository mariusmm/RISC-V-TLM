# Another RISC-V ISA simulator.

**This code is suitable to hard refactor at any time**


This is another RISC-V ISA simulator, this is coded in SystemC + TLM-2.
It supports RV32IMAC and RV64IMAC Instruction set.

[![travis](https://api.travis-ci.com/mariusmm/RISC-V-TLM.svg?branch=master)](https://app.travis-ci.com/github/mariusmm/RISC-V-TLM)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/0f7ccc8435f14ce2b241b3bfead772a2)](https://app.codacy.com/gh/mariusmm/RISC-V-TLM/dashboard)
[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/18772.svg)](https://scan.coverity.com/projects/mariusmm-risc-v-tlm)
[![license](https://img.shields.io/badge/license-GNU--3.0-green.svg)](https://github.com/mariusmm/RISC-V-TLM/blob/master/LICENSE)
![last commit](https://img.shields.io/github/last-commit/Mariusmm/RISC-V-TLM)
![commit activity](https://img.shields.io/github/commit-activity/w/Mariusmm/RISC-V-TLM)
[![Docker](https://img.shields.io/docker/cloud/automated/mariusmm/riscv-tlm.svg?style=flat)](https://hub.docker.com/r/mariusmm/riscv-tlm)
[![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/mariusmonton)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7181526.svg)](https://doi.org/10.5281/zenodo.7181526)

---
Table of Contents
=================
<!--ts-->
   * [Another RISC-V ISA simulator.](./README.md#another-risc-v-isa-simulator)
   * [Table of Contents](./README.md#table-of-contents)
      * [Description](./README.md#description)
         * [Structure](./README.md#structure)
         * [Memory Map](./README.md#memory-map)
      * [TODO](./README.md#todo)
      * [Compile](./README.md#compile)
      * [Cross-compiler](./README.md#cross-compiler)
      * [Debug](./README.md#debug)
      * [Docker container](./README.md#docker-container)
         * [How to use Docker](./README.md#how-to-use-docker)
      * [Test](./README.md#test)
         * [C code](./README.md#c-code)
         * [FreeRTOS](./README.md#freertos)
      * [Documentation](./README.md#documentation)
         * [Publications](./README.md#publications)
      * [Contribute](./README.md#contribute)
      * [Authors and credits](./README.md#Authors-and-credits)
      * [License](./README.md#license)

<!-- Added by: marius, at: 2019-02-11T20:15+01:00 -->

<!--te-->

## Description

Brief description of the modules:
* CPU: Top entity that includes all other modules.
* Memory: Memory highly based on TLM-2 example with read file capability
* Registers: Implements the register file, PC register & CSR registers
* Instruction: Decodes instruction type and keeps instruction field
* BASE_ISA: Executes Base ISA, Zifencei and Zicsr.
  * C_extension: Decodes & Executes Compressed instructions (C extension)
  * M_extension: Decodes & Executes Multiplication and Division instructions (M extension)
  * A_extension: Decodes & Executes Atomic instructions (A extension)
* Simulator: Top-level entity that builds & starts the simulation
* BusCtrl: Simple bus manager
* Trace: Simple trace peripheral
* Timer: Simple IRQ programable real-time counter peripheral
* Debug: GDB server for remote debugging (Beta)

Helper classes:
* Performance: Performance indicators stores here (singleton class)
* Log: Log class to log them all (singleton class)

Current performance is about 3.000.000 instructions / sec in a Intel Core
i5-5200<span>@</span>2.2Ghz and about 4.500.000 instructions / sec in a Intel Core i7-8550U<span>@</span>1.8Ghz.

Trace perihperal creates a xterm window where it prints out all received data. 

### Structure
![Modules' hierarchy](doc/Hierarchy.png)


### Memory map

| Base | Module | Description | 
| ---- | :----: | ---- |
| 0x40000000 | Trace | Output data to xterm | 
| 0x40004000 | Timer | LSB Timer |
| 0x40004004 | Timer | MSB Timer |
| 0x40004008 | Timer | MSB Timer Comparator |
| 0x4000400C | Timer | LSB Timer Comparator |


## TODO
This is a preliminar and incomplete version.

Task to do:
- [x] Implement all missing instructions (Execute)
- [x] Implement CSRs ~~(where/how?)~~
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
     - [x] riscv-compliance 
* [ ] Improve structure and modules hierarchy
* [X] Add 64 architecture (RV64I)
* [x] Debug capabilities
* [ ] Add [Trace v2.0](https://github.com/riscv-non-isa/riscv-trace-spec) support 

## Compile
In order to compile the project you need SystemC-2.3.3/4 installed in your system, and Boost Library Headers.

```sh
# Following Environment are needed to configure the project
export SYSTEMC_HOME=<Path to SystemC Library Installation>
# Check the SystemC Installation path for this setting
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
# Optional BOOST_ROOT if not specified one should have Boost Header Package Installed
export BOOST_ROOT=<Path to extracted boost library sources>
# Clone the repo with submodules initialized
git clone --recurse-submodules https://github.com/mariusmm/RISC-V-TLM.git

# If already cloned one need to run the following command to initialize the git submodule from within the RISC-V-TLM cloned directory
git submodule update --init --recursive
# Configure, Build and deploy spdlog dependency
cd spdlog
# Configure spdlog submodule
cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release
# Build spdlog
cmake --build _builds --config Release
# Install spdlog
cmake --build _builds --target install
cd ..
# Setup SPDLOG_HOME to point to spdlog dependency installation path for the built library, here PWD is the RISC-V-TLM project cloned directory
export SPDLOG_HOME=$PWD/spdlog/install
```

Then, you need to modifiy your LD_LIBRARY_PATH environtment variable to add
path systemc library. In my case:
```sh
export LD_LIBRARY_PATH=/home/marius/Work/RiscV/code/systemc-2.3.2/lib-linux64
```

And then you can execute the simulator:
```sh
./RISCV_TLM asm/BasicLoop.hex
```

### Using cmake

It is possible to use cmake:
```sh
# Create and cd into the build directory for RISC-V-TLM project
mkdir build
cd build
# Configure the project using CMake
cmake -DCMAKE_BUILD_TYPE=Release ..
# For additional configuration one can refer the CMake documentation on additional configuration options.
make
```

note that SystemC must be compiled with cmake:
```sh
cd <systemc directory>
mkdir build
cd build
cmake ../ -DCMAKE_CXX_STANDARD=17
make
```

### Arguments
-L loglevel: 3 for detailed (INFO) log, 0 to ERROR log level

-f filename .hex binary filename to use

-D Enter in Debug mode, simulator starts gdb server (Beta)

-R 32 or 64 to choose 32-bit or 64-bit architecture

## Cross-compiler
It is possible to use gcc as risc-v compiler. Follow the instructions (from https://github.com/riscv/riscv-gnu-toolchain):
~~~sh
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv32gc --with-abi=ilp32
make
# ...
# wait for long time ...
# ...
export PATH=$PATH:/opt/riscv/bin
~~~

In test/C/long_test/ example there is a Makefile that compiles a project with any .c files and links them against new-lib nano. 
There is a Helper_functions.c file with defiitions of all missing functions needed by the library (**_read()**, **_close()**, **_fstat_r()**, 
**_lseek_r()**, **_isatty_r()**, **_write()**). All of them are defined empty except **_write()** that is written to use the Trace perihperal. 
The definition of the function **_write()** allows developer to use printf() as usual and the stdout will be redirected to the Trace perihperal.

## Debug
It is possible to debug an application running in RISC-V-TLM simulator. 
It is tested with riscv32-unknown-elf-gdb version 8.3.0.20190516-git and eclipse.
Configure a "C/C++ Remote Application" debug configuration as the figure

![Modules' hierarchy](doc/DebugConfiguration.png)
 
gdbinit.txt file must contain: 

```
set debug remote 1
set arch riscv:rv32
```

With this configuration, eclipse debuggins is almost normal (I experienced some problems wiith "step-over" and "step-into" commands)

## Docker container

There is a Docker container available with the latest release at https://hub.docker.com/r/mariusmm/riscv-tlm. 
This container has RISCV-TLM already build at /usr/src/riscv64/RISCV-TLM directory.

### How to use Docker
```sh
docker pull mariusmm/riscv-tlm
docker run -v <path_to_RISCV-V-TLM>/:/tmp -u $UID -e DISPLAY=$DISPLAY --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw"  -it mariusmm/riscv-tlm /bin/bash

./RISC-V-TLM/build/RISCV_TLM /tmp/<your_hex_file>
```

or you can call binary inside docker image directly:

```sh
docker run -v <path_to_RISCV-V-TLM>/:/tmp -u $UID -e DISPLAY=$DISPLAY --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw"  -it mariusmm/riscv-tlm /usr/src/riscv64/RISC-V-TLM/RISCV_TLM /tmp/<your_hex_file>
```

I'm using docker image [zmors/riscv_gcc](https://hub.docker.com/r/zmors/riscv_gcc) to have a cross-compiler,  I'm using both docker images this way:

```sh
# console1:
docker run -v /tmp:/PRJ -it zmors/riscv_gcc:1  bash

# cd /PRJ/func3
# make

# console2:
docker run -v /tmp:/tmp -u $UID -e DISPLAY=$DISPLAY --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" -it mariusmm/riscv-tlm /bin/bash

# Run following commands as root/su user
cd /usr/src/riscv64/RISC-V-TLM/ 
./RISCV-TLM /tmp/file.hex
# ...
```

or 

```sh
# ...
# console 2:
docker run -v /tmp/tmp -it  mariusmm/riscv-tlm /usr/src/riscv64/RISC-V-TLM/RISCV_TLM /tmp/file.hex
```

Performance is not affected by running the simulator inside the container

## Test
See [Test page](Test) for more information.

In the asm directory there are some basic assembly examples.

I "compile" one file with the follwing command:
```sh
cd asm
riscv32-unknown-elf-as  EternalLoop.asm -o EternalLoop.o
riscv32-unknown-elf-ld EternalLoop.o -o EternalLoop.elf
riscv32-unknown-elf-objcopy -O ihex EternalLoop.elf EternalLoop.hex
cd ..
./RISCV_SCTLM asm/EternalLoop.hex
```
This example needs that you hit Ctr+C to stop execution.

### C code
The C directory contains simple examples in C. Each directory contains
an example, to compile it just:
```sh
make
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
* RTL-Level simulation


## Authors and credits
RISC-V-TLM is managed by Màrius Montón.

If you find this code useful, please consider citing:
```
@inproceedings{montonriscvtlm2020,
        title = {A {RISC}-{V} {SystemC}-{TLM} simulator},
        booktitle = {Workshop on {Computer} {Architecture} {Research} with {RISC}-{V} ({CARRV 2020}),
        author = {Montón, Màrius},
        year = {2020}
}
```

## Publications
I've published a paper describing the RISC-V simulator in [CARRV 2020](https://carrv.github.io/2020/) conference ([pdf](http://mariusmonton.com/wp-uploads/2020/05/CARRV2020_paper_7_Monton.pdf)).

##  License

Copyright (C) 2018, 2019, 2020, 2021, 2022 Màrius Montón ([@mariusmonton](https://twitter.com/mariusmonton/))

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
