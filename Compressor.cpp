#include "provided.h"
#include <string>
#include "HashTable.h"
#include <vector>
using namespace std;

unsigned int computeHash(unsigned short input)			
{
	return input;
}

unsigned int computeHash(string input)
{
	int total = 0;
	for (int i = 0; i != input.length(); i++)
		total += (i + 1) * input[i];
	return total;
}

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	numbers.clear();
	int C = s.length() / 2 + 512;
	if (C > 16384)
		C = 16384;
	HashTable<string, unsigned short> H(C * 2, C);
	for (unsigned int j = 0; j < 256; j++)
	{
		string y(1, static_cast<char>(j));
		H.set(y, j, true);
	}
	int nextFreeID = 256;
	string runSoFar;
	for (int c = 0; c != s.length(); c++)
	{
		string expandedRun = runSoFar + s[c];
		unsigned short p;
		if (H.get(expandedRun, p))
			runSoFar = expandedRun;
		else
		{
			unsigned short x;
			H.get(runSoFar, x);
			numbers.push_back(x);
			H.touch(runSoFar);
			runSoFar = "";
			unsigned short cv;
			string ch(1, static_cast<char>(s[c]));
			H.get(ch, cv);
			numbers.push_back(cv);
			if (!H.isFull())
			{
				H.set(expandedRun, nextFreeID);
				nextFreeID++;
			}
			else
			{
				string n;
				unsigned short k;
				H.discard(n, k);
				H.set(expandedRun, k);
			}
		}
	}
	if (runSoFar != "")
	{
		unsigned short x;
		H.get(runSoFar, x);
		numbers.push_back(x);
	}
	numbers.push_back(C);
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	s = "";
	int C = numbers[numbers.size() - 1];
	HashTable<unsigned short, string> H(C * 2, C);
	for (unsigned short j = 0; j < 256; j++)
	{
		string y(1, static_cast<char>(j));
		H.set(j, y, true);
	}
	int nextFreeID = 256;
	string runSoFar;
	string output;
	for (int us = 0; us != numbers.size() - 1; us++)
	{
		if (numbers[us] <= 255)
		{
			string z;
			H.get(numbers[us], z);
			s += z;
			if (runSoFar == "")
			{
				runSoFar += z;
				continue;
			}
			else
			{
				string expandedRun = runSoFar + z;
				if (!H.isFull())
				{
					H.set(nextFreeID, expandedRun);
					nextFreeID++;
				}
				else
				{
					unsigned short y;
					string n;
					H.discard(y, n);
					H.set(y, expandedRun);
				}
				runSoFar = "";
			}
		}
		else
		{
			string S;
			if (!H.get(numbers[us], S))
				return false;
			H.touch(numbers[us]);
			s += S;
			runSoFar = S;
		}
	}
	return true;
}
