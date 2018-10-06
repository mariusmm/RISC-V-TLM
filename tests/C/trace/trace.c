
#define TRACE (*(unsigned char *)0x40000000)

void main(void) {
  TRACE = 'H';
  TRACE = 'e';
}
