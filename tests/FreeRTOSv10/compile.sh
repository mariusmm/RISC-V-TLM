riscv32-unknown-elf-gcc -static -march=rv32imac -mabi=ilp32 --specs=nosys.specs  freertos_test.c port.c list.c  queue.c tasks.c timers.c heap_4.c portasm.S  -o freertos_test.o
objcopy -Oihex freertos_test.o  freertos_test.hex
