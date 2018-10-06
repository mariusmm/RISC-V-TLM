
#define TRACE (*(unsigned char *)0x40000000)

void main(void) {
  int i;
  for(i=0;i<10;i++) {
    TRACE = 'a' + i;
  }
}
