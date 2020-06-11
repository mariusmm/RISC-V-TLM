    
// dynamic_cast
#include <iostream>
#include <exception>
using namespace std;

class Base_Class { virtual void dummy() {} };
class Derived_Class: public Base_Class { int a; };

int main () {
  try {
 	Base_Class * ptr_a = new Derived_Class;
 	Base_Class * ptr_b = new Base_Class;
 	Derived_Class * ptr_c;

    	ptr_c = dynamic_cast< Derived_Class *>(ptr_a);
    	if (ptr_c ==0) cout << "Null pointer on first type-cast" << endl;

    	ptr_c = dynamic_cast< Derived_Class *>(ptr_b);
    	if (ptr_c ==0) cout << "Null pointer on second type-cast" << endl;

  	} catch (exception& my_ex) {cout << "Exception: " << my_ex.what();}

  asm volatile ("ecall");
  return 0;
}
