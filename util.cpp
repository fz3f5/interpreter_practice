//
// util.cpp
//
#include "util.h"
#include <string.h> //Need to include after versin g++4.3

using namespace std;

bool compCharIgnCase (char c1, char c2) 
{
	return (tolower(c1) == tolower(c2));
}

bool compareIgnCase (string s1, string s2)
{
	if (s1.length() != s2.length())
		return false;
	return equal(s1.begin(), s1.end(), s2.begin(), compCharIgnCase);
}

string::iterator searchIgnCase (string &str, const string &substr) 
{
	return search (str.begin(), str.end(), substr.begin(), substr.end(), compCharIgnCase);
}

string trim (string s)
{
	string ss = "";
	bool instr = false;
	int len = s.length();
	for (int i = 0; i < len; i++) {
		char c = s[i];
		if ((c == 0x20 || c == 0x8) && !instr)
			continue;
		else {
			ss += c;
			instr = true;
		}
	}
	return ss;
}

string trimStart (string s)
{
	string ss = "";
	int len = s.length();
	bool notspace = false;
	for (int i = 0; i < len; i++) {
		char c = s[i];
    if (!notspace && (c == 0x20 || c == 0x8))
			continue;
		else {
			ss += c;
			notspace = true;
		}
	}
	return ss;
}

string StripStr (string s)
{
	int len = s.length();
	if (s[0] != 0x22 || s[len-1] != 0x22)
		cout << "string is invalid" << endl;
	string result = s.substr(1, len-2);
	return result;
}

void removeLastCRLF (char *buff)
{
	int len = strlen(buff);
	if (buff[len-1] == 0xd || buff[len-1] == 0xa)
		buff[len-1] = 0;
}

int isNumberString (string ss)
{
	string s = trim(ss);
	char c = s[0];
	if (c != '.' && c != '-' && !isdigit(c))
		return 0;
	if (s.length() == 1 && !isdigit(c))
		return 0;
	int len = s.length();
	int nPeriod = 0;
	if (c == '.')
		nPeriod = 1;
	if (len > 1) {
		for (int i = 1; i < len; i++) {
			char c = s[i];
			if (c != '.' && !isdigit(c))
				return 0;
			if (c == '.')
				if (++nPeriod > 1)
					return 0;
		}
		return 1;
	}
	return 1;
}

bool isOparator (char c)
{
	if (c == '<' || c == '>')
		return true;
	if (c == '!')
		return true;
	if (c == '-' || c == '+')
		return true;
	if (c == '*' || c == '/')
		return true;
	if (c == '=')
		return true;
	return false;
}


bool isParen (char c)
{
	if (c == '(' || c == ')')
		return true;
	return false;
}

int syntaxError (string msg)
{
	if (msg.length() > 0)
		cerr << "syntaxerror: " << msg << endl;
	else
		cerr << "syntaxerror" << endl;
	return -1;
}

string errorStr[] = {"Unknown Error.", "Not specified the file name.", "The file can not be opened." };

int dispError (ErrorID id, string s)
{
	cout << errorStr[id] << s << endl;
	return -1;
}
