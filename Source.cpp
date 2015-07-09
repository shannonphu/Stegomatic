#include <iostream>
#include "hashtable.h"
#include "provided.h"
#include "http.h"
#include <cassert>
#include "vld.h"
using namespace std;

void testhashtable()
{
	HashTable<unsigned short, int> hash(10, 100);
	hash.set(2, 4);
	hash.set(1, 7);
	hash.set(7, 6);
	hash.set(2, 9);
	hash.set(9, 0);
	hash.set(1, 2);
	hash.set(7, 17);
	hash.set(0, 2);
	int x = 1;
	assert(!hash.get(-6, x) && x == 1);
	assert(hash.get(2, x) && x == 9);
	assert(hash.get(7, x) && x == 17);
	assert(hash.get(0, x) && x == 2);
	assert(hash.get(2, x) && x == 9);
	hash.set(2, 4);
	assert(hash.get(2, x) && x == 4);
	assert(!hash.isFull());
	assert(hash.touch(1));
	assert(hash.touch(1));
	assert(hash.touch(7));
	assert(hash.touch(0));
	assert(hash.touch(9));
	assert(!hash.touch(1111));
	assert(hash.set(77, 6));
//	cerr << endl;
//	cerr << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%% hash2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
//	cerr << endl;
	HashTable <unsigned short, int> hash2(10, 10);
	int y = 9;
	assert(!hash2.get(1, y) && y == 9);
	assert(!hash2.touch(55));
	assert(hash2.set(15, 6, true));
	assert(hash2.set(1, 9, true));
	unsigned short k;
	int v = 0;
	assert(!hash2.discard(k, v));
	assert(!hash2.get(2, y));
	assert(hash2.get(15, y) && y == 6);
	assert(hash2.set(3, 7));
	assert(hash2.set(4, 8));
	assert(hash2.discard(k, v));
	assert(k == 3 && v == 7);
	assert(hash2.discard(k, v) && k == 4 && v == 8);
	assert(!hash2.discard(k, v));
	assert(hash2.set(3, 9));
	assert(hash2.set(15, 8));
	assert(hash2.touch(3));
	assert(!hash2.touch(15));
	assert(hash2.set(5, 7));
	assert(hash2.touch(3));
	assert(hash2.set(8, 9));
	assert(hash2.set(111, 0));
	assert(hash2.set(20, 59));
	assert(hash2.set(100, 9));
	assert(hash2.set(6, 17));
	assert(hash2.set(55, 4, true));
	//	hash2.dump();
	assert(hash2.set(1, 14));
	assert(!hash2.set(11111, 44));
	//	hash2.dump();

	// strings

	HashTable<string, int> h3(5, 5);
	h3.set("lala", 6);
	h3.set("pa", 11);
	h3.set("shaun", 15);
	h3.set("wavea", 0);
	h3.set("sand", 4);
//	h3.dump();
	cerr << endl;
}

void testbinaryconverter()
{
	// test encoding
	vector<unsigned short> v;
	v.push_back(1);
	v.push_back(5);
	string msg = BinaryConverter::encode(v);
	string msg2;
	for (int i = 0; i < 15; i++)
		msg2 += " ";
	msg2 += "\t";
	for (int i = 0; i < 13; i++)
		msg2 += " ";
	msg2 += "\t";
	msg2 += " ";
	msg2 += "\t";
	assert(msg == msg2);

	// test decoding
	vector<unsigned short> v2;
	string msg3 = "               \t             \t \t";   
	assert(BinaryConverter::decode(msg3, v2));
	assert(v2[0] == 1 && v2[1] == 5);
}

void testcompression()
{
	// test compressing
	string s = "AAAAAAAAAB";
	vector<unsigned short> v3;
	Compressor::compress(s, v3);
	assert(v3[0] == 65 && v3[1] == 65 && v3[3] == 65 && v3[5] == 65 && v3[2] == 256 && v3[4] == 257 && v3[7] == 517 && v3[6] == 66);

	// test decompressing
	vector<unsigned short> v4;
	v4.push_back(65);
	v4.push_back(65);
	v4.push_back(256);
	v4.push_back(65);
	v4.push_back(65);
	v4.push_back(65);
	v4.push_back(256);
	v4.push_back(66);
	v4.push_back(257);
	string u;
	bool ki = Compressor::decompress(v4, u);
	assert(ki && u == "AAAAAAAAAB");
}

void teststeg()
{
	// test hide()
	//string use = "<html>   \nq	qq	 \nbbb			   \n\ngg \nbbb \n \nddd\neee </html>   ";
	string use = "hat  mat   \nbar \r\nlava \r\njan\r";
	string compressandencoded = "____-_---_-______--_-_-__---_--_";
	string another;
	for (int i = 0; i != compressandencoded.length(); i++)
	{
		if (compressandencoded[i] == '_')
			another += " ";
		else
			another += "\t";
	}
	string out;
	string s2 = "abc";
	bool b = Steg::hide(use, s2, out);
	string compare = "hat  mat";
	for (int i = 0; i < 9; i++)
		compare += " ";
	compare += "\t\t";
	for (int i = 0; i < 4; i++)
		compare += " ";
	compare += "\t\nbar";
	for (int i = 0; i < 9; i++)
		compare += " ";
	compare += "\t\t";
	for (int i = 0; i < 3; i++)
		compare += " ";
	compare += "\t \nlava";
	for (int i = 0; i < 9; i++)
		compare += " ";
	compare += "\t\t";
	for (int i = 0; i < 3; i++)
		compare += " ";
	compare += "\t\t\njan\r";
	for (int i = 0; i < 6; i++)
		compare += " ";
	compare += "\t";
	for (int i = 0; i < 8; i++)
		compare += " ";
	compare += "\t\n";
	assert(b);
	assert(out == compare);

	// test reveal()
	string msg5;
	Steg::reveal(out, msg5);
	assert(msg5 == "abc");	
}

void testwebsteg()
{
	//// test hide()
	// string text;
	// HTTP().set("http://cs.ucla.edu", "this is the cs32 class website.\nthis a very hard class.");
	// bool b = WebSteg::hideMessageInPage("http://cs.ucla.edu", "hello there!", text);
	// if (!b)
	//	 cout << "error hiding!" << endl;

	// // test reveal()
	// string msg;
	// if (!WebSteg::revealMessageInPage("http://cs.ucla.edu", msg))
	//	 cout << "error revealing!" << endl;
	// assert(msg == "hello there!");

	 string msga = "I am cool. \nI love apples.\n";
	 HTTP().set("http://cs.ucla.edu", msga);
	 string msgb = "Yep. Too cool for you.";
	 string outbound;
	 bool u = WebSteg::hideMessageInPage("http://cs.ucla.edu", msgb, outbound);
	 assert(u);
	 HTTP().set("http://cs.ucla.edu", outbound);
	 string msgc;
	 bool t = WebSteg::revealMessageInPage("http://cs.ucla.edu", msgc);
	 assert(msgc == "Yep. Too cool for you.");

	 string originalPageText =
		 "<html>\n"
		 "<head>\n"
		 " <title>My Pretend Web Page</title>\n"
		 "</head>\n"
		 "<body>\n"
		 "<h2>My Pretend Web Page<h1>\n"
		 "<p>\n"
		 "I wish I were creative enough to have something interesting \n"
		 "to say here.  I'm not, though, so this is a boring page.\n"
		 "</p>\n"
		 "<p>\n"
		 "Oh, well.\n"
		 "</p>\n"
		 "</body>\n"
		 "</html>\n"
		 ;
	 HTTP().set("http://boring.com", originalPageText);
	 string plan =
		 "Lefty and Mugsy enter the bank by the back door.\n"
		 "Shorty waits out front in the getaway car.\n"
		 "Don't let the teller trip the silent alarm.\n"
		 ;
	 string newPageText;
	 if (!WebSteg::hideMessageInPage("http://boring.com", plan, newPageText))
		 cout << "Error hiding!" << endl;
	 HTTP().set("http://boring.com", newPageText);
	 string msg;
	 if (!WebSteg::revealMessageInPage("http://boring.com", msg))
		 cout << "Error revealing!" << endl;
	 assert(msg == plan);
}

int main()
{
	testhashtable();
	cerr << "passed hash table tests" << endl;
	testbinaryconverter();
	cerr << "passed binary conversion tests" << endl;
	testcompression();
	cerr << "passed compressor tests" << endl;
	teststeg();
	cerr << "passed steg tests" << endl;
	testwebsteg();
	cerr << "passed websteg tests" << endl;
}

// We have not yet produced the test driver main routine for you.

//#include "provided.h"
//#include "http.h"
//#include <iostream>
//#include <string>
//#include <vector>
//#include <cassert>
//using namespace std;
//
//int main()
//{
//	//cout << "Test driver not yet written." << endl;
//
//	//// string text;
//	//// if ( ! WebSteg::hideMessageInPage("http://cs.ucla.edu", "Hello there!", text))
//	////	cout << "Error hiding!" << endl;
//	//// string msg;
//	//// if ( ! WebSteg::revealMessageInPage("http://cs.ucla.edu", msg))
//	//// 	cout << "Error revealing!" << endl;
//
//	string message = "AAAAAAAAAB";  // AAAAAAAAAB
//	vector<unsigned short> numbers;
//	Compressor::compress(message, numbers);
//	for (int i = 0; i < numbers.size(); i++)
//		cout << numbers[i] << endl;   // prints out the vector of unsigned short that represents message
//	string b = "blah";
//	assert(Compressor::decompress(numbers, b));
//	assert(b == message); // b should equal message
//	string encoded = BinaryConverter::encode(numbers);
//	//cout << encoded << endl;   // encoded should be a series of spaces and tabs
//	string hostIn = "<html>   \nQ \tQQ \t \nBBB\t\t\t   \n\nGG \nBBB \n\t\nDDD\nEEE </html>   ";
//	string hostOut = "hello world";
//	assert(Steg::hide(hostIn, "", hostOut));  // can hide empty string
//	string decoded;
//	assert(Steg::reveal(hostOut, decoded));
//	assert(decoded == "");  // decoded string should be empty string
//	assert(Steg::hide(hostIn, message, hostOut));  // hide the first message "AAAAAAAAAB"
//	//cout << hostOut << endl;  // page now contains encoded msg
//	assert(Steg::reveal(hostOut, decoded));
//	assert(message == decoded);
//	string url1 = "http://a.com";
//	string url2 = "http://b.com";
//	HTTP().set(url2, hostIn);
//	string outputPage;
//	assert(WebSteg::hideMessageInPage(url2, message, outputPage));
//	HTTP().set(url1, outputPage);
//	//cout << outputPage;
//	string outputMsg;
//	assert(WebSteg::revealMessageInPage(url1, outputMsg));
//	//cout << outputMsg << endl;
//	cout << "passed all tests" << endl;
//}
