#include <stdio.h>

#ifdef CVE2
#define TRACE (*(unsigned char *)0x80001000)
#else
#define TRACE (*(unsigned char *)0x40000000)
#endif

void print(char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    TRACE = msg[i];
    i++;
  }
}

int main(void) {
  char msg[50];
  print("hello\n");
  sprintf(msg, "%i", 5);
  print(msg);

  asm volatile ("fence");
  asm volatile ("ecall");

  return 0;

}
