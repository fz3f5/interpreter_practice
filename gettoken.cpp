/*
 *  gettoken.cpp
 */
#include <vector>
#include "include/gettoken.h"
#include "include/util.h"
using namespace std;

vector<Token> TokenList;
Token prevTok;

Token get_nexttoken(vector<Token> &list) 
{
	if (list.size() == 0) 
		return Token(NomoreToken);

	Token tok = list[0];
	list.erase(list.begin());

	return tok;
}

void back_token(Token tok, vector<Token> &list)
{
	list.insert(list.begin(), tok);
}

TokenType get_first_tokentype(string src)
{
	vector<Token> list;
	if (get_tokenlist(src, list) < 1) 
		return Invalid;
	Token tok = get_nth_token(0, list);
	return tok.get_type();
}

Token get_nth_token(int n, vector<Token> list)
{
	if ((int)(list.size()) < n + 1)
		return Token(Nothing);

	return list[n];
}

int get_tokenlist(string s, vector<Token> &list)
{
	prevTok = Token(Start);
	list.clear();
	Token tok = get_token(s);
	while (tok.get_type() != NomoreToken) {
		list.push_back(tok);
		prevTok = tok;
		tok = get_token("");
	}
	return list.size();
}

void disp_tokenlist(vector<Token> list, string name)
{ 
	cout << "=== Token List(" << name << ") Top ===" << endl;
	vector<Token>::iterator itr;
	for (itr = list.begin(); itr != list.end(); ++itr) {
		Token tok = *itr;
		tok.print_token();
	}
	cout << "--- Token List End ---" << endl;
}

bool is_command_or_assign(Token tok)
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

	if (is_command_or_assign(prevTok))
		prevTok.set_type(Start);

	string ss = trim_start(s);
	srcstr += ss;
	s = trim(srcstr);
	srcstr = s;
	if (srcstr.length() == 0) 
		return Token(NomoreToken);

	TokenPos tp = MIDDLE;
	if (prevTok.get_type() == Start)
		tp = TOP;
	else if (srcstr.length() < 2)
		tp = END_TOKEN;

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
				tp = END_TOKEN;
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
				return Token(atof(tokenstr.c_str()), END_TOKEN);
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
			tp = END_TOKEN;
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
		return Token (Def, tp);
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

string TokenTypeName[] = 
{ 
	"Start", 
	"Invalid", 
	"Nothing", 
	"NomoreToken", 
	"Value", 
	"Variable", 
	"Symbol", 
	"StrLiteral", 
	"LParen (", 
	"RParen )", 
	"Comma", 
	"*(Mult)", 
	"/(Divide)", 
	"+(Plus", 
	"-(Minus)", 
	"<(Smaller)", 
	">(Greater)", 
	"=(Equal)", 
	"!=(NotEqual)", 
	"=(Assign)", 
	"Cmd", 
	"End", 
	"Print", 
	"Println", 
	"Printspc", 
	"Call", 
	"Def", 
	"Enddef", 
	"If", 
	"Then", 
	"Else", 
	"Endif", 
	"For", 
	"To", 
	"Next" 
};

string tokenPosName[] = 
{
	"TOP", 
	"AFTER_L_PAREN", 
	"AFTER_ASSIGN", 
	"MIDDLE", 
	"END_TOKEN" 
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
