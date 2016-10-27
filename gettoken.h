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
	Token() { value = 0.0; };
	Token(TokenType t, string s, double v, TokenPos pos = Middle) {
		type = t;
		symbol = s;
		value = v;
		position = pos;
	};
	Token(TokenType t, double v, TokenPos pos = Middle) {
		type = t;
		value = v;
		position = pos;
	};
	Token(double v, TokenPos pos = Middle) {
		value = v;
		type = Value;
		position = pos;
	};
	Token(TokenType t, TokenPos pos = Middle) {
		type = t;
		value = 0.0;
		position = pos;
	};
	Token(TokenType t, string s, TokenPos pos = Middle) {
		type = t;
		symbol = s;
		value = 0.0;
		position = pos;
	};
	Token(string s, TokenPos pos = Middle) {
		symbol = s;
		type = Symbol;
		value = 0.0;
		position = pos;
	};
	
	TokenType get_type() { return type; };
	void set_type(TokenType t) { type = t; };
	string get_symbol() { return symbol; };
	double get_value() { return value; };
	void set_value(double v) { value = v; };
	TokenPos get_position() { return position; };
	bool is_starttoken();
	void print_token();
};

extern vector <Token> TokenList;
extern bool token_stat;
extern bool stack_stat;

Token get_nexttoken(vector<Token> & tknList);
void back_token(Token tok, vector<Token> & tknList);
TokenType get_first_tokentype(string src);
//void back_token (Token tok);
Token get_nth_token(int n, vector<Token> tknList);
int get_tokenlist(string s, vector<Token> & tknList);
Token get_token(string s);
void disp_tokenlist(vector<Token> tknList, string name);
void print_value_or_literal(Token tt, bool crlf=false);

#endif
