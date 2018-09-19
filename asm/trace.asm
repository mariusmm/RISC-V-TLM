.equ TRACE_BASE, 0x40000000

#msg:
#	.string "Hello World\n"

.section .text
.globl _start

_start:
  li a2, TRACE_BASE
  li t0, 'H'
  sb t0, 0(a2)
  li t0, 'e'
  sb t0, 0(a2)
  li t0, 'l'
  sb t0, 0(a2)
  li t0, 'l'
  sb t0, 0(a2)
  li t0, 'o'
  sb t0, 0(a2)
  li t0, ' '
  sb t0, 0(a2)
  li t0, 'w'
  sb t0, 0(a2)
  li t0, 'o'
  sb t0, 0(a2)
  li t0, 'r'
  sb t0, 0(a2)
  li t0, 'l'
  sb t0, 0(a2)
  li t0, 'd'
  sb t0, 0(a2)
  li t0, '\n'
  sb t0, 0(a2)
