#define TRACE (*(unsigned char *)0x40000000)

void main(void) {
  char msg[] = "abcdefg\n";
  int i;

  for (i=0; i<5; i++) {
    TRACE = msg[i];
  }
}
