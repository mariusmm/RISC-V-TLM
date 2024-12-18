
#define TRACE (*(unsigned char *)0x80001000)

int main(void) {
  TRACE = 'H';
  TRACE = 'e';

  asm volatile ("fence");
  asm volatile ("ecall");

}
