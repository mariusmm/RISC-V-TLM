li t1, 0xCAFE
li t2, 300
li t3, 0xBEEF
sb t1, -6(t2)
sb t3, -13(t2)
li t1, 23
lbu t1, -6(t2)
lbu t3, -13(t2)
ecall
