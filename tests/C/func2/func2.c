#define TRACE (*(unsigned char *)0x40000000)

void print(char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    TRACE = msg[i];
    i++;
  }
}

int func1(int a, int* b) {
  return a + (*b);
}


int main(void) {
  int x1, x2, x3;

  x1 = 6;
  x2 = 7;

  x3 = func1(x1, &x2);

  if (x3 == 13) {
    print("OK\n");
  } else {
    print("ERROR\n");
  }

  asm volatile ("ecall");

}
