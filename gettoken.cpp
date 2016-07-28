//
// gettoken.cpp
//
#include <vector>
#include "gettoken.h"
#include "util.h"

using namespace std;

vector <Token> TokenList;
Token prevTok;

Token getNextToken (vector <Token> & tknList) 
{
	if (tknList.size() == 0) 
		return Token (NomoreToken);

	Token tok = tknList[0];
	tknList.erase (tknList.begin());

	return tok;
}


void backToken (Token tok, vector<Token> & tknList)
{
	tknList.insert(tknList.begin(), tok);
}

TokenType get1stTokenType (string src)
{
	vector<Token> tokenList;
	if (getTokenList(src, tokenList) < 1) 
		return Invalid;
	Token tok = getTokenN(0, tokenList);
	return tok.getType();
}

Token getTokenN(int n, vector <Token> tknList)
{
	if ((int)(tknList.size()) < n + 1)
		return Token (Nothing);

	return tknList[n];
}

int getTokenList (string s, vector<Token> &tknList)
{
	prevTok = Token (Start);
	tknList.clear();
	Token tok = getToken(s);
	while (tok.getType() != NomoreToken) {
		tknList.push_back(tok);
		prevTok = tok;
		tok = getToken ("");
	}
	return tknList.size();
}

void dispTokenList (vector<Token> tknList, string name)
{ 
	cout << "=== Token List(" << name << ") Top ===" << endl;
	vector<Token>::iterator itr;
	for (itr = tknList.begin(); itr != tknList.end(); ++itr) {
		Token tok = *itr;
		tok.printToken();
	}
	cout << "--- Token List End ---" << endl;
}

bool isCommandOrAssign (Token tok)
{
	TokenType tt = tok.getType();
	if (tt > StrLiteral) 
		return true;
	return false;
}

Token getToken (string s)
{
	static string srcstr;
	string tokenstr;

	if (isCommandOrAssign (prevTok))
		prevTok.setType (Start);

	string ss = trimStart (s);
	srcstr += ss;
	s = trim (srcstr);
	srcstr = s;
	if (srcstr.length() == 0) 
		return Token (NomoreToken);

	TokenPos tp = Middle;
	if (prevTok.getType() == Start)
		tp = Top;
	else if (srcstr.length() < 2)
		tp = EndToken;

	int pos = 0; // current position
	char c1 = srcstr[pos++];
	char c2 = 0;
	if (srcstr.length() > 1)
		c2 = srcstr[pos];
	if (c1 == '=' && c2 == '=') { // ==
		srcstr = srcstr.substr (2, srcstr.length()-2);
		return Token (Equal, tp);
	}
	if (c1 == '!' && c2 == '=') { // !=
		srcstr = srcstr.substr (2, srcstr.length()-2);
		return Token (NotEqual, tp);
	}
	if (c1 == '-' && !(prevTok.isStartToken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Minus, tp);
	}
	if (c1 == '+' && !(prevTok.isStartToken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Plus, tp);
	}
	if (c1 == '<') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Smaller, tp);
	}
	if (c1 == '>') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Greater, tp);
	}
	if (c1 == '/') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Divide, tp);
	}
	if (c1 == '*') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Mult, tp);
	}
	if (c1 == '=') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Assign, tp);
	}
	if (c1 == '(') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (LParen, tp);
	}
	if (c2 == ')') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (RParen, tp);
	}
	if (c1 == ',') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Comma, tp);
	}
	if (c1 == '-' && isalpha(c2)) { // minus symbol
		tokenstr = c1;
		while (true) {
			if ((int)(srcstr.length()) == pos) {
				srcstr = "";
				tp = EndToken;
				break;
			}
			char c = srcstr[pos++];
			if (c == ' ' || isOparator(c) || isParen(c) || c == ',') {
				--pos;
				int len = srcstr.length() - pos;
				srcstr = srcstr.substr(pos, len);
				break;
			} else {
				tokenstr += c;
			}
		}
		return Token (tokenstr, tp);
	}

	if (c1 == '-' || c1 == '+' || c1 == '.' || isdigit(c1)) {
		tokenstr = c1;
		while (true) {
			if ((int)(srcstr.length()) == pos) {
				srcstr = "";
				return Token(atof(tokenstr.c_str()), EndToken);
			}
			char c = srcstr[pos++];
			if (c == '.' || isdigit(c)) 
				tokenstr += c;
			else {
				--pos;
				int len = srcstr.length() - pos;
				srcstr = srcstr.substr(pos, len);
				return Token (atof(tokenstr.c_str()));
			}
		}
	}

	tokenstr = c1;
	bool bInStrLiteral = false;
	if (c1 == 0x22) // 0x22 = \"
		bInStrLiteral = true;
 	  while (true) {
		if ((int)(srcstr.length()) == pos) {
			srcstr = "";
			tp = EndToken;
			break;
		}
		char c = srcstr[pos++];
		if (bInStrLiteral) {
			if (c == 0x22) {
				tokenstr += c;
				int len = srcstr.length() - pos;
				srcstr = srcstr.substr(pos, len);
				bInStrLiteral = false;
				break;
			} else {
				tokenstr += c;
			}
		} else {
			if (c == ' ' || isOparator(c) || isParen(c) || c == ',') {
				--pos;
				int len = srcstr.length() - pos;
				srcstr = srcstr.substr(pos, len);
				break;
			} else {
				tokenstr += c;
			}
		}
	}
	
	if (tokenstr[0] == 0x22) // 0x22 = \"
		return Token (StrLiteral, StripStr(tokenstr), tp);
	if (compareIgnCase(tokenstr, "end"))
		return Token (End, tp);
	if (compareIgnCase(tokenstr, "print"))
		return Token (Print, tp);
	if (compareIgnCase(tokenstr, "println"))
		return Token (Println, tp);
	if (compareIgnCase(tokenstr, "call"))
		return Token (Call, tp);
	if (compareIgnCase(tokenstr, "sub"))
		return Token (Sub, tp);
	if (compareIgnCase(tokenstr, "endsub"))
		return Token (Endsub, tp);
	if (compareIgnCase(tokenstr, "if"))
		return Token (If, tp);
	if (compareIgnCase(tokenstr, "then"))
		return Token (Then, tp);
	if (compareIgnCase(tokenstr, "else"))
		return Token (Else, tp);
	if (compareIgnCase(tokenstr, "endif"))
		return Token (Endif, tp);
	if (compareIgnCase(tokenstr, "for"))
		return Token (For, tp);
	if (compareIgnCase(tokenstr, "to"))
		return Token (To, tp);
	if (compareIgnCase(tokenstr, "next"))
		return Token (Next, tp);
	
	return Token (tokenstr, tp);
}

bool Token::isStartToken()
{
	if (type == Start)
		return true;
	if (type == LParen)
		return true;
	if (type == Assign)
		return true;
	if (type == Comma)
		return true;
	return false;
}

void printTokenValOrLiteral (Token tt, bool crlf)
{
	if (tt.getType() == StrLiteral)
		cout << tt.getSymbol();
	else
		cout << tt.getValue();
	if (crlf)
		cout << endl;
	
}

string TokenTypeName[] = { "Start", "Invalid", "Nothing", "NomoreToken", "Value", "Variable", "Symbol", "StrLiteral", "LParen (", "RParen )", "Comma", "*(Mult)", "/(Divide)", "+(Plus", "-(Minus)", "<(Smaller)", ">(Greater)", "=(Equal)", "!=(NotEqual)", "=(Assign)", "Cmd", "End", "Print", "Println", "Printspc", "Call", "Sub", "Endsub", "If", "Then", "Else", "Endif", "For", "To", "Next" };

string tokenPosName[] = {
	"Top", "AfterLParen", "AfterAssign", "Middle", "EndToken" 
};

void Token::printToken()
{
	if (type == Value) {
		cout << "Value:" << value << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	if (type == Symbol) {
		cout << "Symbol:" << symbol << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	if (type == StrLiteral) {
		cout << "StrLiteral:" << symbol << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	cout << TokenTypeName[type] << "[" << tokenPosName[position] << "]" << endl;
}


