
#!/bin/bash

riscv32-unknown-elf-gcc -Wall -I. -O0 -static   -march=rv32imac -mabi=ilp32 --specs=nosys.specs timer.c timerasm.S -o timer.elf 
objcopy -Oihex timer.elf timer.hex
riscv32-unknown-elf-objdump timer.elf -d > dump
