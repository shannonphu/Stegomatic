#include "provided.h"
#include "http.h"
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string contents;
	bool u = HTTP().get(url, contents);
	if (!u)
		return false;
	bool b = Steg::hide(contents, msg, host);
	if (!b)
		return false;
	return true;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string contents;
	bool u = HTTP().get(url, contents);
	if (!u)
		return false;
	bool b = Steg::reveal(contents, msg);
	if (!b)
		return false;
	return true;
}
