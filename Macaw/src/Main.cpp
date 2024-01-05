#include "CPU.h"
#include <iostream>
#include <typeinfo>

int main()
{
	//std::cout << typeid(a).name() << '\n';
	CPU cpu(50, CPU::NoLog);
	

	/*

	cpu.PushDataString("Hello, World!\n\0");
	cpu.Assemble("C/allchars.oc", CPU::LOAD_BYTES);
	cpu.Run();
	return 0;
	*/
}
