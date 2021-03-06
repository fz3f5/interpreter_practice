/*
 *  main.cpp 
 */
#include <clocale>
#include <unistd.h>
#include "include/source.h"
#include "include/gettoken.h"
#include "include/interpret.h"
#include "include/util.h"
using namespace std;


/* ----------------------------------------------------------------
 * ========================================================
 *  options                                                
 * ========================================================
 *  token_stat     - Display the token "true"
 *  stack_stat     - Display the stack "true"
 *  disp_line      - Display the line "true"
 *  f_direct_mode  - Change the direct execution mode "true"
 * 
 *
 * ========================================================
 *  usage
 * ========================================================
 *  "quit" or "exit"      - exit from the commandline
 *  "version"             - display the version
 *  "load"                - load the source file
 *  "list"                - display the source file
 *  "printstack" or "ps"  - outputs the current stack
 *  "printvar"   or "pv"  - outputs the variable table
 *  "stackstat"           - turn "ON" and "OFF" the stack output
 *  "tokenstat"           - turn "ON" and "OFF" the token output
 *  "displine"            - turn "ON" and "OFF" the line output
 *  "run"                 - run the source file
 * ------------------------------------------------------------------
 */

bool token_stat = false;
bool stack_stat = false;
bool disp_line = false; 
bool f_direct_mode = false;   
void init_run();
void init_load();
void usage(int status); 


int main(int argc, char **argv)
{
 	string s;
	char buff[MAX_LINE_LENGTH];

    /* performance (- . -); */
	printf("頑張っています..\nあと、少し..待って\n");
	for(int i = 0; i < 5; i++) {
		cout << "wait for " << i << "..." << endl;
		sleep(1);
		if(i == 4) cout << "起動中....." << endl;
	}
    sleep(3);
	

	if(argc > 1) {
		string filename = "load" + string(argv[1]);
		init_load();
		load_src_file(filename);
	}


	while(true)
	{
		cout << "->";
		cin.getline(buff, MAX_LINE_LENGTH);
		s = trim(string(buff));

		if (s.length() == 0)
			continue;
		if (comp_str(s, "quit") || comp_str(s, "exit"))  
			break;
		if (comp_str(s, "version")) {
			cout << "created by fz3f5, ver.1 " << endl;
			continue;
		}
		if (search_str(s, "load") != s.end()) { 
			init_load();
			load_src_file(s);
			continue;
		}
		if (search_str(s, "list") != s.end()) { 
			if(srclist.size() < 1) {
				cout << "file is not loaded..." << endl;
				cout << endl;
				usage(1);
			}
			disp_src_file();
			continue;
		}
		if (comp_str(s, "printstack") || comp_str(s, "ps")) { 
			print_stack(stck, "stck");
			print_stack(opstck, "opstck");
			continue;
		}
		if (comp_str(s, "printvar") || comp_str(s, "pv")) {  
			print_variable();
			continue;
		}
		if (comp_str(s, "stackstat") || comp_str(s, "ss")) { 
			stack_stat = !stack_stat;
			cout << (stack_stat ? "stackstat on" : "stackstat off") << endl;
			continue;
		}
		if (comp_str(s, "tokenstat") || comp_str(s, "ts")) { 
			token_stat = !token_stat;
			cout << (token_stat ? "tokenstat on" : "toknestat off") << endl;
			continue;
		}
		if (comp_str(s, "displine") || comp_str(s, "dl")) { 
			disp_line = !disp_line;
			cout << (disp_line ? "displine on" : "displine off") << endl;
			continue;
		}
		if (comp_str(s, "run")) {
			if (srclist.size() < 1) {
				cout << "please load the program." << endl;
				cout << endl;
				usage(1);
				continue;
			}
			init_run();
			statement();
			continue;
		}
		if (comp_str(s, "help")) {
			usage(0);
			continue;
		}

		f_direct_mode = true;
		statement(s);
	}
	
	return 0;
}
    

void init_load()
{
	token_stat = false;
	stack_stat = false;
	disp_line = false;
}

void init_run()
{
	curline = 0;
	varmap.clear();
	f_direct_mode = false;
}

void usage(int status) {
    fprintf(status ? stderr : stdout,
			 "* ========================================================\n"
			 "*  options\n"                                                
			 "* ========================================================\n"
			 "*  token_stat     - Display the token if true\n"
			 "*  stack_stat     - Display the stack if true\n"
			 "*  disp_line      - Display the line  if true\n"
			 "*  f_direct_mode  - Change the direct execution mode if true\n"
			 "*\n" 
			 "*\n"
			 "* ========================================================\n"
			 "*  usage\n"
			 "* ========================================================\n"
			 "*  quit or exit        - exit from the commandline\n"
			 "*  version             - display the version\n"
			 "*  load                - load the source file\n"
			 "*  list                - display the source file\n"
			 "*  printstack or ps    - outputs the current stack\n"
			 "*  printvar  or pv     - outputs the variable table\n"
			 "*  stackstat           - turn ON and OFF the stack output\n"
			 "*  tokenstat           - turn ON and OFF the token output\n"
			 "*  displine            - turn ON and OFF the line output\n"
			 "*  run                 - run the source file\n"
			 "* ---------------------------------------------------------\n"
			 "\n\n"
	);
}
