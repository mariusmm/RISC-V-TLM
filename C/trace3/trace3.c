#define TRACE (*(unsigned char *)0x40000000)



void main(void) {
  char msg[] = "Hello world\n";
  int i;

  for (i=0; msg[i] != '\0'; i++) {
    TRACE = msg[i];
  }
}
