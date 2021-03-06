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
template<class T> Token get_top_elem(T &stck);
void print_stack(stack<Token> stck, string msg);
void print_variable();
Token expression(vector<Token> ExTokenList);
int exec_src(string srcline);

extern map<string, double> varmap;
extern stack<Token> stck;
extern stack<Token> opstck;
extern bool disp_line;
extern bool f_direct_mode;

#endif
