//
// util.h
//
#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>

using namespace std;

#define MAX_LINE_LENGTH 256

enum error_id { UNKNOWN_ERROR, NO_FILE_NAME, FILE_NOT_OPEN, DUMMY };
typedef error_id ErrorID;

// bool isParen (char c);
bool comp_char (char c1, char c2);
bool comp_str (string s1, string s2);
string::iterator search_str(string &str, const string &substr);
string trim (string s);
string trim_start (string s);   
string strip_str (string s);
int is_number_string(string ss);
int syntax_error (string message = "");
bool is_oparator (char c);
bool is_paren (char c);
void remove_last_CRLF (char *);
int disp_error(ErrorID id=UNKNOWN_ERROR, string s="");

#endif
