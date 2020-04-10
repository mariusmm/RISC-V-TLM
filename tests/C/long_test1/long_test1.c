#include <string.h>
#include <stdio.h>



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
