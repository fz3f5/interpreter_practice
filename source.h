//
// source.h
//
#ifndef SOURCE_H
#ifndef SOURCE_H
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <strin>
#include <cctype>
#include <iomanip>

using namespace std;

extern vector <string> SourceList;
extern int CurrentLine;
int loadSourceFile(string cmdline);
void dispSourceFile();

#endif
