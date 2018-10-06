#define TRACE (*(unsigned char *)0x40000000)

int func1(int a, int* b) {
  return a + (*b);
}


void main(void) {
  int x1, x2, x3;

  x1 = 6;
  x2 = 7;

  x3 = func1(x1, &x2);

  if (x3 == 13) {
    TRACE = 'O';
    TRACE = 'K';
    TRACE = '\n';  
  } else {
    TRACE = 'E';
    TRACE = 'R';
    TRACE = '\n';
  }
}
