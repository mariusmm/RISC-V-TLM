#include <string.h>
#include <stdio.h>

#define TRACE (*(unsigned char *)0x40000000)
int _read(int file, char* ptr, int len) {
    return 0;
}

int _close(int fd){
    return 0;
}

int _fstat_r(int fd) {
    return 0;
}

int _lseek_r(struct _reent *ptr, FILE *fp, long offset, int whence){
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd) {
    return 0;
}

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

int main(void) {

  int aux[5] = {0, 1, 2, 3, 4};
  int aux2[5];
  int counter = 0;
  
  printf("Long test Start\n");
   
  
  while(1) {
      
    memcpy(aux2, aux, sizeof(int)*5);
    counter++;
    
    for(int i = 0; i < 5; i++) {
        if (aux[i] != aux2[i]) {
        printf("ERROR %d\n", i);
        asm volatile ("ecall");
        }
    }
    
    if (counter > 10000) {
        printf(".\n");	
        counter = 0;
    }
  }
  printf("OK!\n");	



  asm volatile ("ecall");

  return 0;
}
