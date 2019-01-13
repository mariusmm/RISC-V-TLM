.global TIMER_CMP_INT
.global portSAVE_CONTEXT
.global portRESTORE_CONTEXT
.globl timer_ISR


#define STORE    sw
#define LOAD     lw
#define REGBYTES 4


TIMER_CMP_INT:
/* store execution context on the stack (register content) */
addi    sp, sp, -REGBYTES * 32
STORE   x1, 0x0(sp)
STORE   x4, 3 * REGBYTES(sp)
STORE   x5, 4 * REGBYTES(sp)
STORE   x6, 5 * REGBYTES(sp)
STORE   x7, 6 * REGBYTES(sp)
STORE   x10, 9 * REGBYTES(sp)
STORE   x11, 10 * REGBYTES(sp)
STORE   x12, 11 * REGBYTES(sp)
STORE   x13, 12 * REGBYTES(sp)
STORE   x14, 13 * REGBYTES(sp)
STORE   x15, 14 * REGBYTES(sp)
STORE   x16, 15 * REGBYTES(sp)
STORE   x17, 16 * REGBYTES(sp)
STORE   x28, 27 * REGBYTES(sp)
STORE   x29, 28 * REGBYTES(sp)
STORE   x30, 29 * REGBYTES(sp)
STORE   x31, 30 * REGBYTES(sp)

/* load interrupt/trap reason and call external C function to handle it */
csrr    a0, mcause
jal    timer_ISR 

/* re-store the saved context */
LOAD    x1, 0x0(sp)
LOAD    x4, 3 * REGBYTES(sp)
LOAD    x5, 4 * REGBYTES(sp)
LOAD    x6, 5 * REGBYTES(sp)
LOAD    x7, 6 * REGBYTES(sp)
LOAD    x10, 9 * REGBYTES(sp)
LOAD    x11, 10 * REGBYTES(sp)
LOAD    x12, 11 * REGBYTES(sp)
LOAD    x13, 12 * REGBYTES(sp)
LOAD    x14, 13 * REGBYTES(sp)
LOAD    x15, 14 * REGBYTES(sp)
LOAD    x16, 15 * REGBYTES(sp)
LOAD    x17, 16 * REGBYTES(sp)
LOAD    x28, 27 * REGBYTES(sp)
LOAD    x29, 28 * REGBYTES(sp)
LOAD    x30, 29 * REGBYTES(sp)
LOAD    x31, 30 * REGBYTES(sp)
addi    sp, sp, REGBYTES * 32
mret

