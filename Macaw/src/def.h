#pragma once
#ifndef printd
#include <cstdint>
#include <iostream>
#include <string>
#include <Windows.h>
#include <chrono>
#include <vector>


#define printd(x) std::cout << x << std::endl
#define printsl(x) std::cout << x
//#define is ==
#endif 

using Byte = uint8_t;
using Word = uint16_t;
using String = std::string;

/*
static String DecToBin(int num)
{
	int n = num;
	String r;
	while (n != 0)
	{
		r = ((n % 2 == 0) ? '0' : '1') + r;
		n /= 2;
	}
	return r;
}
static String DecToBinFilled(int input, int length)
{
	std::string s = DecToBin(input);
	while (s.length() < length)
	{
		s = "0" + s;
	}
	return s;
}
*/
static unsigned int BinToDec(const String& s)
{
	return std::stoi(s, nullptr, 2);
}

template <typename T> static int indexOfVector(std::vector<T>& vec, const T& c)
{
	const auto vector_begin = vec.begin();
	const auto vector_end = vec.end();
	const auto index = std::find(vector_begin, vector_end, c) - vector_begin;
	const auto length = vector_end - vector_begin;
	if (index == length)
	{
		return -1;
	}
	return (int)index;
}

static int indexFirstString(const String& s, const char& c)
{
	int index = -1;
	for (int i = 0; i < s.length(); i++)
	{
		char current = s[i];
		if (current == c)
		{
			index = i;
			break;
		}
	}
	return index;
}
static int indexLastString(const String& s, const char& c)
{
	int index = -1;
	for (int i = 0; i < s.length(); i++)
	{
		char current = s[i];
		if (current == c)
		{
			index = i;
		}
	}

}
static std::vector<int> findAll(std::vector<uint8_t> vec, const uint8_t& c)
{
	int cc = 0;
	std::vector<int> indexes;
	int index = indexOfVector(vec, c);
	while (index != -1)
	{
		indexes.push_back(index + cc);
		vec.erase(vec.begin() + index);
		cc++;
		index = indexOfVector(vec, c);
	}

	return indexes;
}

static bool isNum(const String& a)
{
	std::vector<Byte> digits;
	digits.push_back('1');
	digits.push_back('2');
	digits.push_back('3');
	digits.push_back('4');
	digits.push_back('5');
	digits.push_back('6');
	digits.push_back('7');
	digits.push_back('8');
	digits.push_back('9');
	digits.push_back('0');
	int s = 99;
	for (int i = 0; i < a.size(); i++)
	{
		s = min((int)indexOfVector(digits, (Byte)a[i]), s);
	}
	if (s >= 0 && s <= 9)
	{
		return true;
	}
	else
	{
		return false;
	}
}
static int toNumber(const String& s)
{
	if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
	{
		int number;
		if (isNum(s.substr(2))) {
			number = std::stoi(s.substr(2), nullptr, 2);
		}
		else{
			std::cout << "Cannot convert \"" << s << "\" into a number!" << std::endl;
			return 0;
		}
		return number;
	}
	else if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		int number;
		if(isNum(s.substr(2))) {
			number = std::stoi(s.substr(2), nullptr, 16);
		}
		else{
			std::cout << "Cannot convert \"" << s << "\" into a number!" << std::endl;
			return 0;
		}
		return number;
	}
	else 
	{
		int number;
		if(isNum(s)) {
			number = std::stoi(s, nullptr, 10);
		}
		else{
			std::cout << "Cannot convert \"" << s << "\" into a number!" << std::endl;
			return 0;
		}
		return number;
	}
}

static bool checkByte(const String& s, const Word& lineNumber, const String& section="Unknown")
{
	if (toNumber(s) > 255)
	{
		std::cout << "[" << section << ":Line " << lineNumber << "] Byte out of range!" << std::endl;
		return false;
	}
	return true;
}
static bool checkWord(const String& s, const Word& lineNumber, const String& section="Unknown")
{
	if (toNumber(s) > 65535)
	{
		std::cout << "[" << section << ":Line " << lineNumber << "] Word out of range!" << std::endl;
		return false;
	}
	return true;
}

static String toUpper(const String& s)
{
	String tmp;
	for (int i = 0; i < s.length(); i++)
	{
		tmp += (char)toupper(s[i]);
	}
	return tmp;
}

static void usleep(__int64 usec)
{
	if (usec == 0) return;
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (timer == 0) return;
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}





// !printd
//auto start = std::chrono::high_resolution_clock::now();
//auto end = std::chrono::high_resolution_clock::now();
//auto elaps = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();




