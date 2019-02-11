#include <string.h>
#include <stdio.h>

#define TRACE (*(unsigned char *)0x40000000)

void print(char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    TRACE = msg[i];
    i++;
  }
}

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

int func1(int a, int* b) {
  return a - (*b);
}


int main(void) {
  int x1, x2, x3;
  int aux[5] = {0, 1, 2, 3, 4};
  int aux2[5];

  print("Hello\n");
  x1 = 6;
  x2 = 7;
  x3 = func1(x1, &x2);

  memcpy(aux2, aux, sizeof(int)*5);

  for(int i = 0; i < 5; i++) {
    if (aux[i] != aux2[i]) {
      print("ERROR\n");
      asm volatile ("ecall");
    }
  }

  print("OK!\n");	
  asm volatile ("ecall");

  return 0;
}
