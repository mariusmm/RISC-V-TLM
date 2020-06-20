/* 
 * RTTI example from https://www.geeksforgeeks.org/g-fact-33/
 */

#include<iostream> 
using namespace std; 
class B { virtual void fun() {} }; 
class D: public B { }; 
  
int main() 
{ 
    B *b = new D; 
    D *d = dynamic_cast<D*>(b); 
    if(d != NULL) 
        cout << "works" << endl; 
    else
        cout << "cannot cast B* to D*" << endl; 

    
    asm volatile ("ecall");

    return 0; 
    
    
} 
