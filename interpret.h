//
// interpreter.h
//
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include "gettoken.h"

int statement (string s = "");
template <class T> Token getTopElem(T &Stck);
void printstack(stack<Token> Stck, string msg);
void printvarmap();
Token expression(vector<Token> ExTokenList);
int execSrcLine(string sourceline);

extern map<string, double> varmap;
extern stack<Token> Stck;
extern stack<Token> OpStck;
extern bool DispLine;
extern bool fDirectMode;

#endif
