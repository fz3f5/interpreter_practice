//
// interpret.h
//
#ifndef INTERPRET_H
#define INTERPRET_H

#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include "gettoken.h"

int statement(string s = "");
template<class T> Token get_top_elem(T &Stck);
void print_stack(stack<Token> stck, string message);
void print_variable();
Token expression(vector<Token> ExTokenList);
int exec_source(string srcline);

extern map<string, double> varmap;
extern stack<Token> Stck;
extern stack<Token> OpStck;
extern bool disp_line;
extern bool f_direct_mode;

#endif
