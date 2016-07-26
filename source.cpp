/*
 *  source.cpp
 */
#include "source.h"
#include "interpret.h"
#include "util.h"

using namespace std;


// sourcelist
vector <string> SourceList;

// correntline
int CurrentLine;

int loadSourceFile(string cmdline)
{
	char buff[MAX_LINE_LENGTH];
	strcpy(buff, cmdline.c_str());
	char seps[12] = " ,\t\n"; // separate　" " or "," or "\t" or "\n"
	char *token = strtok(buff, seps); // seps == load ?
	if (token == NULL) 
		return dispError();

	// get the file name
	token = strtok(NULL, seps);   // file name ??
	if (token == NULL)
		return dispError(NoFileName);
	char filename[MAX_LINE_LENGTH];
	strcpy(filename, token);
	if (strstr(filename, ".ano") == NULL)
		strcat(filename, ".ano");
	ifstream fin(filename, ios::in);  // open the inputfile
	if (!fin)
		return dispError(FileNotOpen, filename);
	SourceList.clear();
	int lines = 0;
	while (!fin.eof()) {
		fin.getline(buff, MAX_LINE_LENGTH];
		removeLastCRLF(buff);
		if (s.length() > 0) {
			SourceList.push_back(s);
			lines += 1;
		}
	}
	fin.close();
	cout << "load the " << filename << "(" << lines << "lines)" << endl;
	return 0;
}

void dispSourceFile()
{
	vector<string>::iterator itr;
	int linenumber = 0;
	for (itr = SourceList.begin(); itr != SourceList.end(); ++itr) {
		cout << setw(3) << ++linenumber << ":";
		cout << *itr << " " << endl;
	}
}
	
	
