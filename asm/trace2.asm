.equ TRACE_BASE, 0x40000000

.section .text
.globl _start
_start:

        la a0, msg

1:      li a2, TRACE_BASE
#        addi a0, a0, 1
#        j 1b


#.section .rodata
msg:
	    .string "Hello World\n"
