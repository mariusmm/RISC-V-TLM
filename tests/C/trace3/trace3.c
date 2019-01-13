#define TRACE (*(unsigned char *)0x40000000)

int main(void) {
  char msg[] = "abcdefg\n";
  int i;

  for (i=0; i<5; i++) {
    TRACE = msg[i];
  }
  asm volatile ("rdcycle t0");
  asm volatile ("rdtime t4");
  asm volatile ("ecall");
}
