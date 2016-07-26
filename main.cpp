//
// main.cpp - anonymous program.
//
#include <clocale>
#include "source.h"
#include "gettoken.h"
#include "interpret.h"
#include "util.h"

using namespace std;

bool DispToken = false // Display the token "true"
bool DispStack = false // Display the stack "true"
bool DispLine = false // Display the Line "true"
bool fDirectMode = false // Change the DirectExecutionMode "true"      

void initRun();
void initLoad();

int main(int argc, char *argv[])
{
	if (argc > 1) {
		string filename = "load" + string(argv[1]);
		initLoad();
		loadSourceFile(filename);
	}
	
	string s;
	char buff[MAX_LINE_LENGTH];

	while(true)
	{
		cout << "->";
		cin.getline(buff, MAX_LINE_LENGTH]);
		s = trim(string(buff));
		if (s.length() == 0) // skip the blank
			continue;
		if (compareIgnCase(s, "quit"))  
			break;
		if (compareIgnCase(s, "version")) {
			cout << "anonymous, ver.0.01 " << endl;
			continue;
		}
		if (searchIgnCase(s, "load") != s.end()) { //Load the sourcefile
			initLoad();
			loadSourceFile(s);
			continue;
		}
		if (searchIgnCase(s, "list") != s.end()) { //Display the sourcefile
			dispSourceFile();
			continue;
		}
		if (compareIgnCase(s, "printstack") || compareIgnCase(s, "ps")) {  // Outputs the current stack
			printstack(Stck, "Stck");
			printstack(Opstck, "Opstck");
			continue;
		}
		if (compareIgnCase(s, "printvar") || compareIgnCase(s, "pv") {  // Outputs the Variable table
			printvarmap();
			continue;
		}
		if (compareIgnCase(s, "DispStack") || compareIgnCase(s, "ds")) { //Turn "ON" and "OFF" the stack output
			DispStack = !DispStack;
			cout << (DispStack ? "DispStack on" : "DispStack off") << endl;
			continue;
		}
		if (compareIgnCase(s, "DispToken") || compareIgnCase(s, "dt")) { //Turn "ON" and "OFF" the tokne output
			DispToken = !DsipToken;
			cout << (DispToken ? "DispToken on" : "DispToken off") << endl;
			continue;
		}
		if (compareIgnCase(s, "DispLine") || comapreIgnCase(s, "dl")) { // Turn "ON" and "OFF" the line output
			DispLine = !DipsLine;
			cout << (DispLine ? "DispLine on" : "DispLine off") << endl;
			continue;
		}

		if (compareIgnCase(s, "run")) {
			if (SourceList.size() < 1) {
				cout << "prease load the program." << endl;
				continue;
			}
			initRun();
			statement();
			continue;
		}

		// directmode
		fDirectMode = true;
		statement(s);
	}
	
	return 0;
}

void initLoad()
{
	DispToken = false;
	DispStack = false;
	DispLine = false;
}

void initRun()
{
	CurrentLine = 0;
	varmap.clear();
	fDirectMode = false;
}
