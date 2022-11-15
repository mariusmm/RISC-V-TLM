#include <stdio.h>

#define TRACE (*(unsigned char *)0x40000000)

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
