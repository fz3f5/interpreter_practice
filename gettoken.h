//
// gettoken.h
//
#ifndef GETTOKEN_H
#define GETTOKEN_H
#include <iostream>
#include <algorithm>
#include <string>
#include <cctype>
#include <stack>

using namespace std;

typedef enum tokenType 
{ Start, Invalid, Nothing, NomoreToken, Value, Variable, Symbol, StrLiteral, LParen, RParen, Comma, Mult, Divide, Plus, Minus, Smaller, Greater, Equal, NotEqual, Assign, Cmd, End, Print, Println, Printspc, Call, Sub, Enddef, If, Then, Else, Endif, For, To, Next 
} TokenType;

typedef enum tokenPos { Top, AfterLParen, AfterAssign, Middle, EndToken } TokenPos;

class Token {
	TokenType type;
	string symbol;
	double value;
	TokenPos position;
public:
	Token () { value = 0.0; };
	Token (TokenType t, string s, double v, TokenPos pos = Middle) {
		type = t;
		symbol = s;
		value = v;
		position = pos;
	};
	Token (TokenType t, double v, TokenPos pos = Middle) {
		type = t;
		value = v;
		position = pos;
	};
	Token (double v, TokenPos pos = Middle) {
		value = v;
		type = Value;
		position = pos;
	};
	Token (TokenType t, TokenPos pos = Middle) {
		type = t;
		value = 0.0;
		position = pos;
	};
	Token (TokenType t, string s, TokenPos pos = Middle) {
		type = t;
		symbol = s;
		value = 0.0;
		position = pos;
	};
	Token (string s, TokenPos pos = Middle) {
		symbol = s;
		type = Symbol;
		value = 0.0;
		position = pos;
	};
	
	TokenType getType() { return type; };
	void setType(TokenType t) { type = t; };
	string getSymbol() { return symbol; };
	double getValue() { return value; };
	void setValue(double v) { value = v; };
	TokenPos getPosition() { return position; };
	bool isStartToken();
	void printToken();
};

extern vector <Token> TokenList;
extern bool token_stat;
extern bool stack_stat;

Token getNextToken (vector<Token> & tknList);
void backToken (Token tok, vector<Token> & tknList);
TokenType get1stTokenType (string src);

void backToken (Token tok);
Token getTokenN (int n, vector<Token> tknList);
int getTokenList (string s, vector<Token> & tknList);
Token getToken (string s);
void dispTokenList (vector<Token> tknList, string name);
void printTokenValOrLiteral (Token tt, bool crlf=false);

#endif
