#include <iostream>
using namespace std;


int main(void) {

  int aux[5] = {0, 1, 2, 3, 4};
  int aux2[5];
  int counter = 0;
  
  cout << "Test Start" << endl;
   

  for ( const auto &x : aux ) std::cout << x << ' ';
    std::cout << std::endl;

  cout << "Test End" << endl;
  asm volatile ("ecall");

  return 0;
}
