#pragma once
#include "def.h"

struct Register
{
	Word Value;
	Bus* bus;
	int size = sizeof(Word);
	Register()
	{
		Value = 0;
		bus = nullptr;
	}
	void Connect(Bus& b)
	{
		bus = &b;
	}
	void virtual Out()
	{
		bus->Value = Value;
	}
	void In()
	{
		Value = bus->Value;
	}
	void OutLower()
	{
		bus->Value = (Value & 0b0000000011111111);
	}
	void OutHigher()
	{
		bus->Value = (Value & 0b1111111100000000) >> 8;
	}

	
};
//2 bytes

struct ProgramCounter : public Register
{
	ProgramCounter() 
	{
		Value = 0;
	}
	void Increment()
	{
		Value++;
	}
	void Jump()
	{
		Value = bus->Value;
	}
	void operator++()
	{
		Increment();
	}
	void operator++(int)
	{
		Increment();
	}
};
//2 bytes

struct InstructionRegister
{
private:
	Bus* bus;
	std::vector<Byte> Value;
public:
	InstructionRegister()
	{
		bus = nullptr;
		Value.push_back(0);
		Value.push_back(0);
		Value.push_back(0);
		Value.push_back(0);
	}
	void Connect(Bus& b)
	{
		bus = &b;
	}

	void SetByte(const Byte& index, const Byte& value)
	{
		Value[index] = value;
	}
	void InByte(const Byte& index)
	{
		Value[index] = bus->Lower();
	}
	Byte GetByte(const Byte& index)
	{
		return Value[index];
	}

	void OutWordLittle(const Byte& index) const
	{
		bus->Value = (Value[index + 1] << 8) + Value[index];
	}
	void OutByte(const Byte& index)
	{
		bus->Value = Value[index];
	}

	Byte operator[](const Byte& index) const
	{
		return Value[index];
	}
	Byte& operator[](const Byte& index)
	{
		return Value[index];
	}


};
// 4 bytes

struct FlagRegister
{
public:
	struct Flags
	{
		bool carry : 1;
		bool zero : 1;
		bool negative : 1;
		bool interrupt : 1;
	};
	enum FlagNumber
	{
		CARRY, ZERO, NEGATIVE, INTERRUPT 
	};
private:
	//uint8_t size = 4;
	//Byte value;
	Flags flags;
public:
	FlagRegister()
	{
		flags.carry = 0;
		flags.zero = 0;
		flags.negative = 0;
		flags.interrupt = 0;
	}

	void Reset()
	{
		flags.carry = 0;
		flags.zero = 0;
		flags.negative = 0;
		flags.interrupt = 0;
	}
	bool getFlag(Byte index)
	{
		if (index == CARRY) return flags.carry;
		if (index == ZERO) return flags.zero;
		if (index == NEGATIVE) return flags.negative;
		if (index == INTERRUPT) return flags.interrupt;
		std::cout << "Wrong flag" << std::endl;
		return 0;
	}
	void setFlag(Byte index, bool value)
	{
		if (index == CARRY) flags.carry = value;
		if (index == ZERO) flags.zero = value;
		if (index == NEGATIVE) flags.negative = value;
		if (index == INTERRUPT) flags.interrupt = value;
	}

};
//1 byte(actually 0.5bytes)

struct StepCounter
{
public:
	Byte Value;
public:
	StepCounter()
	{
		Value = 0;
	}
	void Reset()
	{
		Value = 0;
	}
	void Increment()
	{
		Value++;
	}
	void EndOperation()
	{
		Value = UINT8_MAX;
	}

	void operator++()
	{
		Increment();
	}

	void operator++(int)
	{
		Increment();
	}
};
//1 byte

struct OutputRegister : public Register
{
	OutputRegister()
	{
		bus = nullptr;
	}
	void Output() {
		std::cout << "-> " << Value << std::endl;
	}
};



