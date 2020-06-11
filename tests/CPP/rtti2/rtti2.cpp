/* 
 * RTTI example from https://www.tenouk.com/cpluscodesnippet/moreonruntimetypeinfo.html
 */

// run time type information

#include <iostream>
#include <typeinfo>

using namespace std;

class Base
{
public:
    virtual void funct(){}    
};

class Derived:public Base{};

int main(void)
{
    Derived* Test1 = new Derived;
    Base* Test2 = Test1;
    
    cout<<"The type name of Test1 is: ";
    cout<<typeid(Test1).name()<<endl;
    
    cout<<"The type name of *Test1 is: ";
    cout<<typeid(*Test1).name()<<endl;
    
    cout<<"The type name of Test2 is: ";
    cout<<typeid(Test2).name()<<endl;
    
    cout<<"The type name of *Test2 is: ";
    cout<<typeid(*Test2).name()<<endl;
    
    delete Test1;
    
    asm volatile ("ecall");
    
    return 0;    
}
