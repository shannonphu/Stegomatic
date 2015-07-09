#include "provided.h"
#include <string>
#include <iostream>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
	// If hostIn empty string, return false
	if (hostIn == "")
		return false;
	hostOut = "";
	// Compress and encode message to hide
	vector<unsigned short> useToCompress;
	Compressor::compress(msg, useToCompress);
	string encodedMsg = BinaryConverter::encode(useToCompress);
	// Count lines before concatenating
	int preNumLines = 0;
	for (int i = 0; i != hostIn.length(); i++)
	{
		if ((hostIn[i] != '\r' && hostIn[i + 1] == '\n') || (hostIn[i] == '\r' && hostIn[i + 1] == '\n'))
			preNumLines++;
	}
	if (hostIn[hostIn.length() - 1] != '\n')					
		preNumLines++;
	// Concatenate hidden message to hostOut
	int numLines = 0;
	int lineAddedAtThisChar = 1;
	int startEncodedSubstr = 0;
	int currentSubStr = 1;
	for (int i = 0; i != hostIn.length(); i++)
	{
		if (hostIn[i] == ' ' || hostIn[i] == '\t')	// check if new line is after these spaces
		{
			bool trailing = true;
			int j;
			// Check if the space or tab is a trailing white space before newline
			for (j = i; j != hostIn.length() - 1 && hostIn[j] != '\n' && !(hostIn[j] == '\r' && hostIn[j + 1] == '\n'); j++)
			{
				if (hostIn[j] != '\t' && hostIn[j] != ' ')
				{
					trailing = false;
					break;
				}
			}
			if (trailing)
			{
				if (j < hostIn.length() - 1)
				{
					i = j - 1;
					continue;
				}
				else
					i = j;
			}
		}
		if (i != hostIn.length() - 1 && (hostIn[i] != '\n' && !(hostIn[i] == '\r' && hostIn[i + 1] == '\n')))
			hostOut += hostIn[i];
		else if (i == hostIn.length() - 1 && hostIn[i] != '\n')
		{
			hostOut += hostIn[i];
			numLines++;
		}
		else
		{
			numLines++;
			if (hostIn[i] == '\r' && hostIn[i + 1] == '\n')
				i++;
		}
		// Add substring to hostOut once reach newline character
		if (numLines == lineAddedAtThisChar)
		{
			int goTill = 0;
			int bounds = encodedMsg.length() % preNumLines;
			if (currentSubStr < bounds)
				goTill = (encodedMsg.length() / preNumLines) + 1;
			else
				goTill = encodedMsg.length() / preNumLines;
			if (startEncodedSubstr < encodedMsg.length())
				hostOut += encodedMsg.substr(startEncodedSubstr, goTill);
			hostOut += '\n';
			startEncodedSubstr += goTill;
			currentSubStr++;
			lineAddedAtThisChar++;
		}
	}

	return true;
}

bool Steg::reveal(const string& hostIn, string& msg) 
{
	string tabSpace;
	string result;
	vector<unsigned short> v;
	for (int i = 0; i != hostIn.length(); i++)
	{
		if (hostIn[i] == ' ' || hostIn[i] == '\t')	// check if new line is after these spaces
		{
			bool trailing = true;
			int j;
			for (j = i; j != hostIn.length() - 1 && hostIn[j] != '\n' && (hostIn[j] != '\r' && hostIn[j + 1] != '\n'); j++)
			{
				if (hostIn[j] != '\t' && hostIn[j] != ' ')
				{
					trailing = false;
					break;
				}
			}
			if (trailing)
			{
				result += hostIn.substr(i, j - i + 1);
				i = j;
				continue;
			}
		}
	}
	bool decode = BinaryConverter::decode(result, v);
	if (!decode)
		return false;
	bool decompress = Compressor::decompress(v, msg);
	if (!decompress)
		return false;
	return true;
}