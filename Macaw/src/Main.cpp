#include "CPU.h"
#include <iostream>
#include <typeinfo>

int main()
{
	//std::cout << typeid(a).name() << '\n';
	
	CPU cpu(0, CPU::NoLog);
	/*

	*/
	cpu.PushDataString("Hello, World!\n\0");
	cpu.Assemble("C/fibo.oc", CPU::LOAD);
	cpu.Run();
	
	return 0;
}
