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

extern vector <string> srclist;
extern int curline;
int load_src_file(string cmdline);
void disp_src_file();

#endif
