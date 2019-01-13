
#define TRACE (*(unsigned char *)0x40000000)

int main(void) {
  TRACE = 'H';
  TRACE = 'e';

  asm volatile ("ecall");
}
