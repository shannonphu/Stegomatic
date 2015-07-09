#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	string result;
	for (int i = 0; i != numbers.size(); i++)
	{
		string temp = convertNumberToBitString(numbers[i]);
		for (int j = 0; j != temp.length(); j++)
		{
			if (temp[j] == '0')
				result += " ";
			else if (temp[j] == '1')
				result += "\t";
		}
	}
	return result;  
}

bool BinaryConverter::decode(const string& bitString, vector<unsigned short>& numbers)
{
	// if bitString is not a multiple of 16 characters
	if (bitString.length() % 16 != 0)
		return false;
	// Empty vector passing in
	numbers.clear();
	int k = 0;
	// Create a string of 0's and 1's from spaces and tabs to eventually convert i
	for (int i = 0; i != bitString.length(); i+=16)
	{
		string a;
		for (int j = i; j < i + 16; j++)
		{
			if (bitString[j] != char(32) && bitString[j] != '\t')
				return false;
			else if (bitString[j] == char(32))
				a += "0";
			else if (bitString[j] == '\t')
				a += "1";
		}
		numbers.push_back(0);
		bool b = convertBitStringToNumber(a, numbers[k]);
		if (!b)
			return false;
		k++;
	}
	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}
