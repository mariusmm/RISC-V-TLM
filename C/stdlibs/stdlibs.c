#include <stdio.h>

#define TRACE (*(unsigned char *)0x40000000)

void print(char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    TRACE = msg[i];
    i++;
  }
}

void main(void) {
  char msg[50];
  print("hello");
  sprintf(msg, "%i", 5);
}
