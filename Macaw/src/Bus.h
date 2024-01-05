#pragma once
#include "def.h"
struct Bus
{
	Word Value;
	Bus()
	{
		Value = 0;
	}
	void Reset()
	{
		Value = 0;
	}
	Byte Lower()
	{
		return (Value & 0b0000000011111111);
	}
	Byte Upper()
	{
		return (Value & 0b1111111100000000)>>8;
	}
};
