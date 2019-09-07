#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE (512)
#define TRACE (*(unsigned char *)0x40000000)

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}


int main(void) {
  uint16_t *buffA, *buffB;  
  bool test_ok = true;
  
  printf("Malloc Test\n");

  buffA = malloc(BUFFER_SIZE * sizeof(uint16_t));
  buffB = malloc(BUFFER_SIZE * sizeof(uint16_t));

  if ( (buffA == NULL ) || (buffB == NULL) ) {
	  printf("Error malloc\n");
  }

  for(int i=0;i<BUFFER_SIZE;i++) {
     buffA[i] = i;
  }
 
 
  memcpy(buffB, buffA, BUFFER_SIZE * sizeof(uint16_t));

  for(int i=0;i<BUFFER_SIZE;i++) {
    if (buffA[i] != buffB[i]) {   
        printf("Error at postion %d\n", i);
        test_ok = false;
        break;
    }
  }
  
  if (test_ok == true) {
    printf("Test were OK\n");
  }
  
  asm volatile ("ecall");

  return 0;
}
