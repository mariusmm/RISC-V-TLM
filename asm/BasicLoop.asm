# Basic loop
  ADDI t1, zero, 10 # t1 to 10
  ADDI t2, zero, 1 #t2 to 1
loop:
  SUB t1, t1, t2
  BNE t1, zero, loop
# END
