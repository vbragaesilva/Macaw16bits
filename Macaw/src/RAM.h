#pragma once
#include "def.h"
#include "Bus.h"
#include <vector>
struct RAM
{
public:
	Word AddressReg;
	enum RamStatus {
		Success = 0, OutOfBoundaries
	};
private:
	std::vector<Byte*> buffers;
	Byte bank;
	Bus* bus;
	Word NextAddressAvailable[4];
public:
	RAM()
		:AddressReg(0), bank(0), bus(nullptr)
	{
		buffers.push_back(new Byte[0x10000]);
		buffers.push_back(new Byte[0x10000]);
		buffers.push_back(new Byte[0x10000]);
		buffers.push_back(new Byte[0x10000]);
		Reset();
	}
	~RAM()
	{
		for (auto el : buffers)
		{
			delete[] el;
		}
	}
	void ConnectBus(Bus& b)
	{
		bus = &b;
	}
	void Reset()
	{
		for (auto el : buffers)
		{
			for (int i = 0; i < 0x10000; i++)
			{
				el[i] = 0;
			}
		}
		NextAddressAvailable[0] = 0;
		NextAddressAvailable[1] = 0;
	}
	void SetByte(const Word& index, const Byte& b)
	{
		buffers[bank][index] = b;
	}
	void SetWord(const Word& index, const Word& w)
	{
		buffers[bank][index] = (w & 0b0000000011111111);
		buffers[bank][index + 1] = (w & 0b1111111100000000) >> 8;
	}

	void SetBank(const Byte& n)
	{
		if (n > 3)return;
		bank = n;
	}

	void OutByte()
	{
		bus->Value = buffers[bank][AddressReg];
	}
	void OutWordLittle()
	{
		bus->Value = (buffers[bank][AddressReg + 1] << 8) + buffers[bank][AddressReg];
	}

	void InWord()
	{
		buffers[bank][AddressReg] = bus->Lower();
		buffers[bank][AddressReg+1] = bus->Upper();
	}
	void InLower()
	{
		buffers[bank][AddressReg] = bus->Lower();
	}
	void InUpper()
	{
		buffers[bank][AddressReg] = bus->Upper();
	}
	void MIn()// Memory Address IN
	{
		AddressReg = bus->Value;
	}

	void PushByte(const Byte& b)
	{
		SetByte(NextAddressAvailable[bank], b);
		NextAddressAvailable[bank]++;
	}
	void PushWord(const Word& w)
	{
		SetWord(NextAddressAvailable[bank], w);
		NextAddressAvailable[bank]++;
		NextAddressAvailable[bank]++;
	}

	Byte operator[](Word index) const
	{
		return buffers[bank][index];
	}
	Byte& operator[](Word index) {
		return buffers[bank][index];
	}

};
