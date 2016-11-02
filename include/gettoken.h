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
{
	 START, 
	 INVALID, 
	 NOTHING, 
	 NO_MORE_TOKEN, 
	 VALUE, 
	 VARIABLE, 
	 SYMBOL, 
	 STR_LITERAL, 
	 L_PAREN,
	 R_PAREN, 
	 COMMA, 
	 MULT, 
	 DIVIDE, 
	 PLUS, 
	 MINUS, 
	 SMALLER, 
	 GREATER, 
	 EQUAL, 
	 NOT_EQUAL, 
	 ASSIGN, 
	 CMD, 
	 END,
	 PRINT, 
	 PRINT_LN, 
	 PRINTSPC, 
	 CALL, 
	 DEF, 
	 ENDDEF, 
	 IF, 
	 THEN, 
	 ELSE, 
	 ENDIF, 
	 FOR, 
	 TO, 
	 NEXT 
} TokenType;

typedef enum tokenPos
{ 
	TOP, 
	AFTER_L_PAREN, 
	AFTER_ASSIGN, 
	MIDDLE, 
	END_TOKEN 
} TokenPos;


class Token {
	TokenType type;
	string symbol;
	double value;
	TokenPos position;
public:
	Token() { value = 0.0; };
	Token(TokenType t, string s, double v, TokenPos pos = MIDDLE) {
		type = t;
		symbol = s;
		value = v;
		position = pos;
	};
	Token(TokenType t, double v, TokenPos pos = MIDDLE) {
		type = t;
		value = v;
		position = pos;
	};
	Token(double v, TokenPos pos = MIDDLE) {
		value = v;
		type = VALUE;
		position = pos;
	};
	Token(TokenType t, TokenPos pos = MIDDLE) {
		type = t;
		value = 0.0;
		position = pos;
	};
	Token(TokenType t, string s, TokenPos pos = MIDDLE) {
		type = t;
		symbol = s;
		value = 0.0;
		position = pos;
	};
	Token(string s, TokenPos pos = MIDDLE) {
		symbol = s;
		type = SYMBOL;
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

Token get_nexttoken(vector<Token> &list);
void back_token(Token tok, vector<Token> &list);
TokenType get_first_tokentype(string src);
//void back_token (Token tok);
Token get_nth_token(int n, vector<Token> list);
int get_tokenlist(string s, vector<Token> &list);
Token get_token(string s);
void disp_tokenlist(vector<Token> list, string name);
void print_value_or_literal(Token tt, bool crlf=false);

#endif
