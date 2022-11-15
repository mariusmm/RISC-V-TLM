#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE (512)


int main(void) {
  uint16_t *buffA, *buffB;  
  bool test_ok = true;
  uint8_t *buffC;
  
  printf("Malloc Test\n");

  printf("buffA: %p\n", (void *) buffA);
  printf("buffB: %p\n", (void *) buffB);
  printf("buffC: %p\n", (void *) buffC);
  
  buffA = malloc(BUFFER_SIZE * sizeof(uint16_t));
  buffB = malloc(BUFFER_SIZE * sizeof(uint16_t));

  printf("buffA: %p\n", (void *) buffA);
  printf("buffB: %p\n", (void *) buffB);
  printf("buffC: %p\n", (void *) buffC);
  
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
  
  free(buffA);
  free(buffB);

  printf("buffA: %p\n", (void *) buffA);
  printf("buffB: %p\n", (void *) buffB);
  printf("buffC: %p\n", (void *) buffC);
 
  /* buffC should reuse address previously used by buffA (?) */
  buffC = malloc(512);
  printf("buffC: %p\n", (void *) buffC);
  
  if (test_ok == true) {
    printf("Test were OK\n");
  }

  asm volatile ("fence");  
  asm volatile ("ecall");

  return 0;
}
