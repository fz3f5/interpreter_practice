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

extern vector <string> sourcelist;
extern int currentline;
int load_source_file(string cmdline);
void disp_source_file();

#endif