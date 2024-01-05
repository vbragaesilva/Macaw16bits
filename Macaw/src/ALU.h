#pragma once
#include "def.h"
struct ALU
{
private:
	Register* a;
	Register* b;
	Bus* bus;
	FlagRegister* F;
public:
	ALU() {
		a = nullptr;
		b = nullptr;
		bus = nullptr;
		F = nullptr;
	}
	void ConnectRegisters(Register& A, Register& B)
	{
		a = &A;
		b = &B;
	}
	void ConnectBusAndFlag(Bus& b, FlagRegister& f)
	{
		bus = &b;
		F = &f;
	}

	Word getA()
	{
		return a->Value;
	}
	Word getB()
	{
		return b->Value;
	}

	void OutSum()
	{
		Word result = a->Value + b->Value;
		int intResult = (int)a->Value + (int)b->Value;
		bus->Value = result;
		F->setFlag(FlagRegister::CARRY, (intResult > 0xffff) ? 1 : 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
	void OutSub()
	{
		Word result = a->Value - b->Value;
		bus->Value = result;
		int intResult = (int)a->Value + (int)((~b->Value) + 1);
		F->setFlag(FlagRegister::CARRY, (intResult > 0xffff) ? 1 : 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);

	}
	void OutMult()
	{
		Word result = a->Value * b->Value;
		int intResult = (int)a->Value * (int)b->Value;
		bus->Value = result;
		F->setFlag(FlagRegister::CARRY, (intResult > 0xffff) ? 1 : 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
	void OutAnd()
	{
		Word result = a->Value & b->Value;
		bus->Value = result;
		F->setFlag(FlagRegister::CARRY, 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
	void OutOr()
	{
		Word result = a->Value | b->Value;
		bus->Value = result;
		F->setFlag(FlagRegister::CARRY, 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
	void OutXor()
	{
		Word result = a->Value ^ b->Value;
		bus->Value = a->Value ^ b->Value;
		F->setFlag(FlagRegister::CARRY, 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
	void OutNot()
	{
		Word result = ~a->Value;
		bus->Value = result;
		F->setFlag(FlagRegister::CARRY, 0);
		F->setFlag(FlagRegister::ZERO, (bus->Value == 0) ? 1 : 0);
		F->setFlag(FlagRegister::NEGATIVE, (result > 0x7f) ? 1 : 0);
	}
};
