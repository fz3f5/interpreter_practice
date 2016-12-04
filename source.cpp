/*
 *  source.cpp
 */
#include <string.h> /* Need to include after "g++-4.3" version */
#include "include/source.h"
#include "include/interpret.h"
#include "include/util.h"
using namespace std;

vector<string> sourcelist;
int curline;


int load_source_file(string cmdline)
{
	char buff[MAX_LINE_LENGTH];
	strcpy(buff, cmdline.c_str());
	char seps[12] = " ,\t\n"; // separate　" " or "," or "\t" or "\n"
	char *token = strtok(buff, seps); // seps == load 
	if(token == NULL) { 
		return disp_error();
	}
	/* get the file name */
	token = strtok(NULL, seps);   // file name ??
	if(token == NULL) {
		return disp_error(NO_FILE_NAME);
	}
	char filename[MAX_LINE_LENGTH];
	strcpy(filename, token);
	if(strstr(filename, ".aaa") == NULL) {
		strcat(filename, ".aaa");
	}
	ifstream fin(filename, ios::in);  // open the inputfile
	if(!fin) {
		return disp_error(FILE_NOT_OPEN, filename);
	}
	sourcelist.clear();
	int lines = 0;

	while (!fin.eof()) {
		fin.getline(buff, MAX_LINE_LENGTH);
		remove_last_CRLF(buff);
		string s = string(buff);
		if (s.length() > 0) {
			sourcelist.push_back(s);
			lines += 1;
		}
	}
	fin.close();
	cout << "load the " << filename << "( " << lines << " lines)" << endl;
	return 0;
}

void disp_src_file()
{
	vector<string>::iterator itr;
	int num = 0;
	for (itr = sourcelist.begin(); itr != sourcelist.end(); ++itr) {
		cout << setw(3) << ++num << ":";
		cout << *itr << " " << endl;
	}
}
