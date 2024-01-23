#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
struct TokenizedOCVectors
{
	std::vector<std::vector<String>> data, code;
};
struct OCVectors
{
	std::vector<String> data, code;
};

struct Writer
{
private:
	
public:
	void WriteByteArray(const std::vector<uint8_t>& bytes, const char* path)
	{
		std::ofstream File;
		File.open(path, std::ios::binary | std::ios::out);
		for (uint8_t element : bytes)
		{
			File << element;
		}
		File.close();
	}
};

struct Reader
{
private:
	std::fstream fs;
public:
	std::streamsize length;
	bool fail = false;
	Reader(const char* path)
	{
		LoadFile(path);
	}
	Reader()
	{
		length = 0;
	}
	~Reader()
	{
		fs.close();
	}
	void LoadFile(const char* path)
	{
		fs.open(path, std::ios::in | std::ios::ate | std::ios::binary);
		fs.seekg(0, std::ios::end);
		length = fs.tellg();
		if (fs.fail())
		{
			fail = true;
			std::cout << "Fail to load File: \"" << path << "\"" << '\n';
			//abort();
			exit(1);
		}

	}
	std::vector<uint8_t> ReadFile()
	{
		if (fs.is_open())
		{
			fs.seekg(0, std::ios::beg);
			char* raw_buffer = new char[length];
			fs.read(raw_buffer, length);
			std::vector<uint8_t> bytes(raw_buffer, raw_buffer + length);
			delete[] raw_buffer;
			return bytes;
		}
		std::cout << "erro no vetor b(1)" << '\n';
		abort();
	}
};
/*
struct BinFile
{
	enum FileType
	{
		V = 1
	};
	std::deque<uint8_t> bytes;
	FileType type;
	BinFile(std::deque<uint8_t> fullBytes)
	{
		type = (FileType)fullBytes[0];
		fullBytes.pop_front();
		bytes = fullBytes;

		//for (std::streamsize i = 0; i < fullBytes.size(); i++)
		//{
			//if (i != 0) {
				//bytes.push_back(fullBytes[i]);
			//}
		//}

	}
};
*/
struct AssemblerLineReader
{
	std::vector<String> getLines(const char* path)
	{
		std::vector<String> linesArray;
		String text;
		std::ifstream stream(path);
		while (getline (stream, text))
		{
			linesArray.push_back(text);
		}
		stream.close();
		return linesArray;
	}
	TokenizedOCVectors tokenize(std::vector<String> vec)
	{		
		constexpr char COMMENT_SYMBOL = '#';
		std::vector<String> cleanedLines = cleanString(vec, COMMENT_SYMBOL);
		OCVectors tempOCVectors = getSubVectors(cleanedLines);
		std::vector<std::vector<String>> data_tokens = tokenizeData(tempOCVectors.data);
		std::vector<std::vector<String>> code_tokens = tokenizeCode(tempOCVectors.code);
		
		return {data_tokens, code_tokens};
	}

	std::vector<String> cleanString(std::vector<String> linesVector, const char& commentMark)
	{
		std::vector<String> cleanedLines;
		for (int lineIndex = 0; lineIndex < linesVector.size(); lineIndex++)
		{
			String currentLine = removeWhiteSpacesFromStringStart(linesVector[lineIndex]);
			if (currentLine[0] == commentMark || currentLine[0] == 0) continue;
			int hashPosition = indexFirstString(currentLine, commentMark);
			if (hashPosition >= 0) currentLine = currentLine.substr(0, hashPosition);
			cleanedLines.push_back(currentLine);
		}
		return cleanedLines;
	}
	OCVectors getSubVectors(std::vector<String>& tmpLine)
	{
		std::vector<String> tmpData;
		std::vector<String> tmpCode;
		int _dataIndex = indexOfVector(tmpLine, (String)".data");
		int _codeIndex = indexOfVector(tmpLine, (String)".code");
		for (int lineIndex = 0; lineIndex < tmpLine.size(); lineIndex++)
		{
			String currentLine = tmpLine[lineIndex];
			if (_dataIndex < lineIndex && lineIndex < _codeIndex)
			{
				tmpData.push_back(currentLine);
			}
			else if (lineIndex > _codeIndex)
			{
				tmpCode.push_back(currentLine);
			}
		}
		return { tmpData, tmpCode };
	}
	std::vector<std::vector<String>> tokenizeData(std::vector<String> dataVector)
	{
		std::vector<std::vector<String>> dataTokens;
		for (int lineIndex = 0; lineIndex < dataVector.size(); lineIndex++)
		{
			String dataLine = dataVector[lineIndex];
			std::vector<String> tokens;
			String s;
			bool insideQuotes = false;

			for (int column = 0; column <= dataLine.length(); column++)
			{
				char character = dataLine[column];
				char previousChar = column == 0 ? 0 : dataLine[column - 1];
				if (character == '"' && previousChar != '\\')
				{
					insideQuotes = insideQuotes ? 0 : 1;
				}
				if (character == '\\')
				{
					continue;
				}
				if (column == dataLine.length())
				{
					if (s != "")
					{
						tokens.push_back(s);
					}
					break;
				}
				if ((char)dataLine[column] != ' ' || insideQuotes)
				{
					s += (dataLine[column]);
				}
				else
				{
					if (s != "")
					{
						tokens.push_back(s);
					}
					s.clear();
				}
			}
			dataTokens.push_back(tokens);
		}
		return dataTokens;
	}
	std::vector<std::vector<String>> tokenizeCode(std::vector<String> codeVector)
	{
		std::vector<std::vector<String>> codeTokens;
		for (int lineIndex = 0; lineIndex < codeVector.size(); lineIndex++)
		{
			String commandLine = codeVector[lineIndex];
			std::vector<String> tokens;
			String s;
			for (int column = 0; column <= commandLine.length(); column++)
			{
				if (column == commandLine.length())
				{
					if (s != "")
					{
						tokens.push_back(s);
					}
					break;
				}
				if ((char)commandLine[column] != ' ')
				{
					s += toupper(commandLine[column]);
				}
				else
				{
					if (s != "")
					{
						tokens.push_back(s);
					}
					s.clear();
				}
			}
			codeTokens.push_back(tokens);
		}
		return codeTokens;
	}

	void copyVector(const std::vector<std::vector<String>>& original, std::vector<std::vector<String>>& copy)
	{
		for (std::vector<String> el : original)
		{
			copy.push_back(el);
		}

	}
	
};
