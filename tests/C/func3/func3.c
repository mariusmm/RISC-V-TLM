#include <string.h>

#define TRACE (*(unsigned char *)0x40000000)

void print(char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    TRACE = msg[i];
    i++;
  }
}

int func1(int a, int* b) {
  return a - (*b);
}


int main(void) {
  int x1, x2, x3;
  int aux[5] = {0};
  int aux2[5];

  TRACE = 'a';
  TRACE = 'b';

  x1 = 6;
  x2 = 7;

  x3 = func1(x1, &x2);

  if (x3 == (6-7)) {
    print("\nOK\n");
  } else {
    print("\nERROR\n");
  }


  memcpy(aux, aux2, 5);

  asm volatile ("fence");
  asm volatile ("ecall");

  return 0;
}
