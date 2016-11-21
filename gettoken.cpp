/*
 *  gettoken.cpp
 */
#include <vector>
#include "include/gettoken.h"
#include "include/util.h"
using namespace std;

vector<Token> tokenlist;
Token prevtok;

Token get_nexttoken(vector<Token> &list) 
{
	if (list.size() == 0) 
		return Token(NO_MORE_TOKEN);

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
		return INVALID;
	Token tok = get_nth_token(0, list);
	return tok.get_type();
}

Token get_nth_token(int n, vector<Token> list)
{
	if ((int)(list.size()) < n + 1)
		return Token(NOTHING);

	return list[n];
}

int get_tokenlist(string s, vector<Token> &list)
{
	prevtok = Token(START);
	list.clear();
	Token tok = get_token(s);
	while (tok.get_type() != NO_MORE_TOKEN) {
		list.push_back(tok);
		prevtok = tok;
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
	if (tt > STR_LITERAL) 
		return true;
	return false;
}

Token get_token(string s)
{
	static string srcstr;
	string tokenstr;

	if (is_command_or_assign(prevtok))
		prevtok.set_type(START);

	string ss = trim_start(s);
	srcstr += ss;
	s = trim(srcstr);
	srcstr = s;
	if (srcstr.length() == 0) 
		return Token(NO_MORE_TOKEN);

	TokenPos tp = MIDDLE;
	if (prevtok.get_type() == START)
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
		return Token (EQUAL, tp);
	}
	if (c1 == '!' && c2 == '=') { // !=
		srcstr = srcstr.substr (2, srcstr.length()-2);
		return Token (NOT_EQUAL, tp);
	}
	if (c1 == '-' && !(prevtok.is_starttoken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (MINUS, tp);
	}
	if (c1 == '+' && !(prevtok.is_starttoken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (PLUS, tp);
	}
	if (c1 == '%' && !(prevtok.is_starttoken())) {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (MOD, tp);
	}
	if (c1 == '<') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (SMALLER, tp);
	}
	if (c1 == '>') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (GREATER, tp);
	}
	if (c1 == '/') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (DIVIDE, tp);
	}
	if (c1 == '*') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (MULT, tp);
	}
	if (c1 == '=') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (ASSIGN, tp);
	}
	if (c1 == '(') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (L_PAREN, tp);
	}
	if (c1 == ')') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (R_PAREN, tp);
	}
	if (c1 == ',') {
		srcstr = srcstr.substr (1, srcstr.length()-1);
		return Token (COMMA, tp);
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
	bool b_in_str_literal = false;
	if (c1 == 0x22) // 0x22 = \"
		b_in_str_literal = true;
 	  while (true) {
		if ((int)(srcstr.length()) == pos) {
			srcstr = "";
			tp = END_TOKEN;
			break;
		}
		char c = srcstr[pos++];
		if (b_in_str_literal) {
			if (c == 0x22) {
				tokenstr += c;
				int len = srcstr.length() - pos;
				srcstr = srcstr.substr(pos, len);
				b_in_str_literal = false;
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
		return Token (STR_LITERAL, strip_str(tokenstr), tp);
	if (comp_str(tokenstr, "end"))
		return Token (END, tp);
	if (comp_str(tokenstr, "print"))
		return Token (PRINT, tp);
	if (comp_str(tokenstr, "println"))
		return Token (PRINT_LN, tp);
	if (comp_str(tokenstr, "call"))
		return Token (CALL, tp);
	if (comp_str(tokenstr, "def"))
		return Token (DEF, tp);
	if (comp_str(tokenstr, "enddef"))
		return Token (ENDDEF, tp);
	if (comp_str(tokenstr, "if"))
		return Token (IF, tp);
	if (comp_str(tokenstr, "then"))
		return Token (THEN, tp);
	if (comp_str(tokenstr, "else"))
		return Token (ELSE, tp);
	if (comp_str(tokenstr, "endif"))
		return Token (ENDIF, tp);
	if (comp_str(tokenstr, "for"))
		return Token (FOR, tp);
	if (comp_str(tokenstr, "to"))
		return Token (TO, tp);
	if (comp_str(tokenstr, "next"))
		return Token (NEXT, tp);
	
	return Token (tokenstr, tp);
}

bool Token::is_starttoken()
{
	if (type == START || type == L_PAREN || type == ASSIGN || type == COMMA) 
		return true;
	else 
		return false;
}

void print_value_or_literal(Token tt, bool crlf)
{
	if (tt.get_type() == STR_LITERAL)
		cout << tt.get_symbol();
	else
		cout << tt.get_value();
	if (crlf)
		cout << endl;
	
}

string TokenTypeName[] = 
{ 
	"start", 
	"invalid", 
	"nothing", 
	"nomoreToken", 
	"value", 
	"variable", 
	"symbol", 
	"strLiteral", 
	"lparen (", 
	"rparen )", 
	"comma", 
	"*(mult)", 
	"/(divide)", 
	"+(plus)", 
	"-(minus)", 
	"<(smaller)", 
	">(greater)", 
	"=(equal)", 
	"!=(notequal)", 
	"=(assign)", 
	"cmd", 
	"end", 
	"print", 
	"println", 
	"printspc", 
	"call", 
	"def", 
	"enddef", 
	"if", 
	"then", 
	"else", 
	"endif", 
	"for", 
	"to", 
	"next" 
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
	if (type == VALUE) {
		cout << "Value:" << value << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	if (type == SYMBOL) {
		cout << "Symbol:" << symbol << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	if (type == STR_LITERAL) {
		cout << "StrLiteral:" << symbol << "[" << tokenPosName[position] << "]" << endl;
		return;
	}
	cout << TokenTypeName[type] << "[" << tokenPosName[position] << "]" << endl;
}
