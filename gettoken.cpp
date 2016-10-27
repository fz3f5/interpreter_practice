//
// gettoken.cpp
//
#include <vector>
#include "gettoken.h"
#include "util.h"

using namespace std;

vector<Token> TokenList;
Token prevTok;

Token get_nexttoken(vector <Token> & tknList) 
{
	if (tknList.size() == 0) 
		return Token (NomoreToken);

	Token tok = tknList[0];
	tknList.erase (tknList.begin());

	return tok;
}


void back_token (Token tok, vector<Token> & tknList)
{
	tknList.insert(tknList.begin(), tok);
}

TokenType get_first_tokentype(string src)
{
	vector<Token> tokenList;
	if (get_tokenlist(src, tokenList) < 1) 
		return Invalid;
	Token tok = get_nth_token(0, tokenList);
	return tok.get_type();
}

Token get_nth_token(int n, vector<Token> tknList)
{
	if ((int)(tknList.size()) < n + 1)
		return Token(Nothing);

	return tknList[n];
}

int get_tokenlist(string s, vector<Token> &tknList)
{
	prevTok = Token (Start);
	tknList.clear();
	Token tok = get_token(s);
	while (tok.get_type() != NomoreToken) {
		tknList.push_back(tok);
		prevTok = tok;
		tok = get_token("");
	}
	return tknList.size();
}

void disp_tokenlist(vector<Token> tknList, string name)
{ 
	cout << "=== Token List(" << name << ") Top ===" << endl;
	vector<Token>::iterator itr;
	for (itr = tknList.begin(); itr != tknList.end(); ++itr) {
		Token tok = *itr;
		tok.print_token();
	}
	cout << "--- Token List End ---" << endl;
}

bool isCommandOrAssign (Token tok)
{
	TokenType tt = tok.get_type();
	if (tt > StrLiteral) 
		return true;
	return false;
}

Token get_token(string s)
{
	static string srcstr;
	string tokenstr;

	if (isCommandOrAssign (prevTok))
		prevTok.set_type(Start);

	string ss = trim_start (s);
	srcstr += ss;
	s = trim (srcstr);
	srcstr = s;
	if (srcstr.length() == 0) 
		return Token (NomoreToken);

	TokenPos tp = Middle;
	if (prevTok.get_type() == Start)
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
	if (c1 == '-' && !(prevTok.is_starttoken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (Minus, tp);
	}
	if (c1 == '+' && !(prevTok.is_starttoken())) {
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
			if (c == ' ' || is_oparator(c) || is_paren(c) || c == ',') {
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
			if (c == ' ' || is_oparator(c) || is_paren(c) || c == ',') {
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
		return Token (StrLiteral, strip_str(tokenstr), tp);
	if (comp_str(tokenstr, "end"))
		return Token (End, tp);
	if (comp_str(tokenstr, "print"))
		return Token (Print, tp);
	if (comp_str(tokenstr, "println"))
		return Token (Println, tp);
	if (comp_str(tokenstr, "call"))
		return Token (Call, tp);
	if (comp_str(tokenstr, "def"))
		return Token (Sub, tp);
	if (comp_str(tokenstr, "enddef"))
		return Token (Enddef, tp);
	if (comp_str(tokenstr, "if"))
		return Token (If, tp);
	if (comp_str(tokenstr, "then"))
		return Token (Then, tp);
	if (comp_str(tokenstr, "else"))
		return Token (Else, tp);
	if (comp_str(tokenstr, "endif"))
		return Token (Endif, tp);
	if (comp_str(tokenstr, "for"))
		return Token (For, tp);
	if (comp_str(tokenstr, "to"))
		return Token (To, tp);
	if (comp_str(tokenstr, "next"))
		return Token (Next, tp);
	
	return Token (tokenstr, tp);
}

bool Token::is_starttoken()
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

void print_value_or_literal(Token tt, bool crlf)
{
	if (tt.get_type() == StrLiteral)
		cout << tt.get_symbol();
	else
		cout << tt.get_value();
	if (crlf)
		cout << endl;
	
}

string TokenTypeName[] = { "Start", "Invalid", "Nothing", "NomoreToken", "Value", "Variable", "Symbol", "StrLiteral", "LParen (", "RParen )", "Comma", "*(Mult)", "/(Divide)", "+(Plus", "-(Minus)", "<(Smaller)", ">(Greater)", "=(Equal)", "!=(NotEqual)", "=(Assign)", "Cmd", "End", "Print", "Println", "Printspc", "Call", "Sub", "Enddef", "If", "Then", "Else", "Endif", "For", "To", "Next" };

string tokenPosName[] = {
	"Top", "AfterLParen", "AfterAssign", "Middle", "EndToken" 
};

void Token::print_token()
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


