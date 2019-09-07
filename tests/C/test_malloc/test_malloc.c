#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE (512)
#define TRACE (*(unsigned char *)0x40000000)

extern int  errno;

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

int _read (int   file, char *ptr, int   len) {
  return  0;
}  

int _close (int   file) {
  errno = EBADF;
  return -1;
} 

int _fstat (int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return  0;
}   

int _isatty (int   file) {
  return  1;
} 

int _lseek (int file, int offset, int whence) {
  return  0;
} 

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
  
  asm volatile ("ecall");

  return 0;
}
