#include <string.h>
#include <stdio.h>


int main(void) {

  int counter = 0;
  volatile int value1, value2, value3;
  volatile char buffer[100];
  volatile char buffer2[100];
  
  printf("Long test Start\n");
  
  value1 = 5;
  value2 = 7; 
  
  while(1) {
    

    value3 += value1 * value2;
    sprintf(buffer, "value %d * %d = %d\n", value1, value2, value3);
    
    strcpy(buffer2, buffer);

    counter++;
    
    if (counter > 10000) {
        printf("%s\n", buffer2);	
        counter = 0;
    }

  }
  printf("OK!\n");	

  asm volatile ("fence");
  asm volatile ("ecall");

  return 0;
}
