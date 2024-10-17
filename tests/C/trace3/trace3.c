#ifdef CVE2
#define TRACE (*(unsigned char *)0x80001000)
#else
#define TRACE (*(unsigned char *)0x40000000)
#endif


int main(void) {
  char msg[] = "abcdefg\n";
  int i;

  for (i=0; i<5; i++) {
    TRACE = msg[i];
  }
  asm volatile ("rdcycle t0");
  asm volatile ("rdtime t4");
  asm volatile ("fence");
  asm volatile ("ecall");
}
