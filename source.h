//
// source.h
//
#ifndef SOURCE_H
#define SOURCE_H
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <iomanip>

using namespace std;

extern vector <string> SourceList;
extern int CurrentLine;
int loadSourceFile(string cmdline);
void dispSourceFile();

#endif
