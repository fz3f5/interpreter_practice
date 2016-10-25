//
// main.cpp 
//
#include <clocale>
#include "source.h"
#include "gettoken.h"
#include "interpret.h"
#include "util.h"

using namespace std;

/*  options 
 *  token_stat     - Display the token "true"
 *  stack_stat     - Display the stack "true"
 *  disp_line      - Display the line "true"
 *  f_direct_mode  - Change the direct execution mode "true"
 *
 * 
 *  usage
 *  "quit" or "exit"   - exit from the commandline
 *  "version"          - display the version
 *  "load"             - load the source file
 *  "list"             - display the source file
 *  "printstack" or "ps" - outputs the current stack
 *  "printvar" or "pv" - outputs the variable table
 *  "stackstat"        - turn "ON" and "OFF" the stack output
 *  "tokenstat"        - turn "ON" and "OFF" the token output
 *  "displine"         - turn "ON" and "OFF" the line output
 *  "run"              - run the source file
 */

bool TokenStat = false;
bool StackStat = false;
bool DispLine = false; 
bool fDirectMode = false;   

void initRun();
void initLoad();

int main(int argc, char **argv)
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
		cin.getline(buff, MAX_LINE_LENGTH);
		s = trim(string(buff));
		if (s.length() == 0)
			continue;
		if (compareIgnCase(s, "quit") || compareIgnCase(s, "exit"))  
			break;
		if (compareIgnCase(s, "version")) {
			cout << "created by fz3f5, ver.1 " << endl;
			continue;
		}
		if (searchIgnCase(s, "load") != s.end()) { 
			initLoad();
			loadSourceFile(s);
			continue;
		}
		if (searchIgnCase(s, "list") != s.end()) { 
			dispSourceFile();
			continue;
		}
		if (compareIgnCase(s, "printstack") || compareIgnCase(s, "ps")) { 
			printstack(Stck, "Stck");
			printstack(OpStck, "OpStck");
			continue;
		}
		if (compareIgnCase(s, "printvar") || compareIgnCase(s, "pv")) {  
			printvarmap();
			continue;
		}
		if (compareIgnCase(s, "StackStat") || compareIgnCase(s, "ss")) { 
			StackStat = !StackStat;
			cout << (StackStat ? "StackStat on" : "StackStat off") << endl;
			continue;
		}
		if (compareIgnCase(s, "TokenStat") || compareIgnCase(s, "ts")) { 
			TokenStat = !TokenStat;
			cout << (TokenStat ? "TokenStat on" : "TokenStat off") << endl;
			continue;
		}
		if (compareIgnCase(s, "DispLine") || compareIgnCase(s, "dl")) { 
			DispLine = !DispLine;
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

		fDirectMode = true;
		statement(s);
	}
	
	return 0;
}

void initLoad()
{
	TokenStat = false;
	StackStat = false;
	DispLine = false;
}

void initRun()
{
	CurrentLine = 0;
	varmap.clear();
	fDirectMode = false;
}
