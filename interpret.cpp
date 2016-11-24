/*
 *  interpret.cpp
 */

#include "include/source.h"
#include "include/gettoken.h"
#include "include/interpret.h"
#include "include/util.h"
using namespace std;

map<string, double> varmap;
stack<Token> stck;
stack<Token> opstck;

static Token check_and_register_var(Token tok)
{
	string symbol = tok.get_symbol();
	map<string, double>::iterator itr;
	itr = varmap.find(symbol);
	double v = 0.0;

	if (itr == varmap.end()) {
		varmap[symbol] = v;
	} else {
		v = itr->second;
	}
	return (Token (VALUE, symbol, v));
}

Token expression(vector<Token> tknlist)
{
	stack<Token> exstck;
	stack<Token> exopstck;

	while(true)
	{
		if (stack_stat) {
			print_stack(exstck, "exstck");
			print_stack(exopstck, "exopstck");
		}
		if (token_stat) 
			disp_tokenlist(tknlist, "tknlist");

		
		Token curtok = get_nexttoken(tknlist);  //current token 
		TokenType ctt = curtok.get_type();
		string csm = curtok.get_symbol();
		if (ctt == SYMBOL && csm[0] == '-') {
			string tmpsymbol = csm.substr(1, csm.length()-1);
			Token tmpToken = Token (SYMBOL, tmpsymbol, TOP);
			tmpToken = check_and_register_var(tmpToken);
			double v = tmpToken.get_value();
			curtok = Token (-1 * v);
			ctt = VALUE;
		}
		if (ctt == SYMBOL) {
			curtok = check_and_register_var(curtok);
			ctt = curtok.get_type();
		}
		if (ctt == NO_MORE_TOKEN) {
			Token optoken = get_top_elem(exopstck);
			TokenType optt = optoken.get_type();
			if (optt == NO_MORE_TOKEN || optt == ASSIGN) {
				exopstck.push(optoken);
				return exstck.top();
			}

			curtok = get_top_elem(exstck);
			ctt = curtok.get_type();
			exopstck.push(optoken);
		}
		if ((R_PAREN < ctt) && (ctt < SMALLER)){
			exopstck.push(curtok);
			continue;
		}
		if (ctt == L_PAREN) {
			int nLRaren = 1;
			vector<Token> tmpTokenList;
			while (true) {
				Token tok = get_nexttoken(tknlist);
				TokenType tt = tok.get_type();
				if (tt == L_PAREN) {
					nLRaren += 1;
				}
				if (tt == R_PAREN) {
					if (--nLRaren == 0)
						break;
				}
				if (tt == NO_MORE_TOKEN)
					return syntax_error("NO PAREN");
				tmpTokenList.insert(tmpTokenList.end(), tok);
			}
			if (tmpTokenList.size() > 1) {
				Token tok = expression(tmpTokenList);
				tok.set_type(VALUE);
				tknlist.insert(tknlist.begin(), tok);
				continue;
			} else 
				exstck.push(tmpTokenList[0].get_value());
			continue;
	}
	if (MINUS < ctt && ctt < ASSIGN) {
		exopstck.push(curtok);
		continue;
	}
	if (ctt == VALUE || ctt == VARIABLE) {
		Token tok = get_top_elem(exstck);
		Token optok = get_top_elem(exopstck);
		TokenType optokt = optok.get_type();

		if ((tok.get_type() == VALUE || tok.get_type() == VARIABLE) && MINUS < optokt && optokt < ASSIGN) {
			double a = curtok.get_value();
			double b = tok.get_value();
			exstck.push(a - b);
			continue;
		}
		if ((tok.get_type() == VALUE || tok.get_type() == VARIABLE) && (optok.get_type() == MULT || optok.get_type() == DIVIDE)) {
			double a = curtok.get_value();
			double b = tok.get_value();
			if (optok.get_type() == MULT) {
				exstck.push(a * b);
				continue;
			}
			if (optok.get_type() == DIVIDE) {
				exstck.push(b / a);
				continue;
			}
		}
		if ((tok.get_type() == VALUE || tok.get_type() == VARIABLE) && (optok.get_type() == PLUS || optok.get_type() == MINUS || optok.get_value() == MOD)) {
			Token nexttoken = get_nexttoken(tknlist);
			TokenType ntt = nexttoken.get_type();
			if (ntt == MULT || ntt == DIVIDE || ntt == R_PAREN) {
				exstck.push(tok);
				exopstck.push(optok);
				back_token(nexttoken, tknlist);
				exstck.push(curtok);
				continue;
			} else {
				double a = curtok.get_value();
				double b = tok.get_value();
				back_token(nexttoken, tknlist);
				if (optok.get_type() == PLUS) {
					exstck.push(a + b);
					continue;
				}
				if (optok.get_type() == MINUS) {
					exstck.push(b - a);
					continue;
				}
				if (optok.get_type() == MOD) {
					exstck.push((int)b % (int)a);
					continue;
				}
			}
		}
		if (tok.get_type() != NO_MORE_TOKEN) 
			exstck.push(tok);
		if (optok.get_type() != NO_MORE_TOKEN)
			exstck.push(optok);
		exstck.push(curtok);
	}
	}
	return (Token(INVALID));
}

Token str_expression(vector<Token> tknlist) 
{
	string result = "";
	bool f_plus = true;

	while (true) {
		Token curtok = get_nexttoken(tknlist);
		TokenType ctt = curtok.get_type();

		if (ctt == SYMBOL) {
			curtok = check_and_register_var(curtok);
			ctt = curtok.get_type();
		}

		if (ctt == NO_MORE_TOKEN) {
			return (Token(STR_LITERAL, result));
		}

		if (ctt == STR_LITERAL) {
			if (f_plus) {
				result += curtok.get_symbol();
				f_plus = false;
			}
			continue;
		}
		if (ctt == VALUE) {
			if (f_plus) {
				ostringstream ostr;
				double v = curtok.get_value();
				ostr << v;
				result += ostr.str();
				f_plus = false;
			}
			continue;
		}
		if (ctt == PLUS) {
			f_plus = true;
			continue;
		}
	}
	return Token(INVALID);
}

static int callcnt = 0;

int do_call_statement()
{
	if (callcnt > 0) {
		cout << "call can not be nested" << endl;
		return -1;
	}
	callcnt += 1;

	int returnLine = currentline;
	Token token0 = get_nexttoken(tokenlist);
	Token token1 = get_nexttoken(tokenlist);
	string subroutineName = token1.get_symbol();

	vector<double> parameterList;
	while (true) {
		double value = 0.0;
		Token tok = get_nexttoken(tokenlist);
		TokenType tt = tok.get_type();
		if (tt == L_PAREN || tt == COMMA)
			continue;
		if (tt == R_PAREN || tt == NO_MORE_TOKEN)
			break;
		if (tt == VARIABLE || tt == SYMBOL) {
			tok = check_and_register_var(tok);
			value = tok.get_value();
			parameterList.insert(parameterList.end(), value);
		}
		if (tt == VALUE) {
			value = tok.get_value();
			parameterList.insert(parameterList.end(), value);
		}
	}
	int invokedLine = 0;
	string src = "";
	while (true) {
		src = sourcelist[invokedLine];
		int n = get_tokenlist(src, tokenlist);
		if (get_nexttoken(tokenlist).get_type() == DEF) {
			if (n < 2) 
				return syntax_error("Subroutine can not be found");
			string symbol = get_nexttoken(tokenlist).get_symbol();
			if (comp_str(symbol, subroutineName)) {
				break;
			} 
		}
		if (++invokedLine >= (int)(sourcelist.size()))
			return syntax_error("Subroutine can not be found");
	}
	
	src = sourcelist[invokedLine];
	if (disp_line)
		cout << "Run the " << src << endl;
	get_tokenlist(src, tokenlist);
	bool b_in_paren = false;
	while (true) {
		Token tok = get_nexttoken(tokenlist);
		TokenType tt = tok.get_type();
		if (tt == L_PAREN) {
			b_in_paren = true;
			continue;
		}
		if (tt == COMMA)
			continue; 
		if (tt == R_PAREN) {
			b_in_paren = false;
			break;
		}
		if (tt == NO_MORE_TOKEN) 
			break;
		if (tt == SYMBOL && b_in_paren == true) {
			string symbol = tok.get_symbol();
			varmap[symbol] = parameterList[0];
			if (parameterList.size() < 1)
				return syntax_error("The number of the argument and parameter do not match");
			parameterList.erase(parameterList.begin());
		}
	}
	
	currentline = invokedLine+1;
	while (true) {
		src = sourcelist[currentline];
		get_tokenlist(src, tokenlist);
		TokenType tt = get_nth_token(0, tokenlist).get_type();
		if (tt == ENDDEF) {
			if (disp_line)
				cout << "Run the " << src << endl;
			break;
		}
		if (currentline > (int)sourcelist.size() - 1)
			return 0;
		if (exec_source(sourcelist[currentline++]))
			return -1;
	}
	currentline = returnLine;
	return 0;
}

static int forcnt = 0;

int do_for_statement()
{
	if (forcnt > 0) {
		cout << "for can not be nested" << endl;
		return -1;
	}
	forcnt += 1;

	Token token0 = get_nexttoken(tokenlist);
	Token token1 = get_nexttoken(tokenlist);
	token1 = check_and_register_var(token1);
	string counterVarName = token1.get_symbol();
	Token token2 = get_nexttoken(tokenlist);
	vector<Token> exlist;
	while (true) {
		Token tok = get_nexttoken(tokenlist);
		if (tok.get_type() == NO_MORE_TOKEN)
			break;
		if (tok.get_type() == TO)
			break;
		exlist.insert(exlist.end(), tok);
	}
	double counter = 0;
	if (exlist.size() > 1)
		counter = expression(exlist).get_value();
	else 
		counter = exlist[0].get_value();
	exlist.clear();
	while (true) {
		Token tok = get_nexttoken(tokenlist);
		if (tok.get_type() == NO_MORE_TOKEN)
			break;
		exlist.insert(exlist.end(), tok);
	}
	double forto = 0;
	if (exlist.size() > 1)
		forto = expression(exlist).get_value();
	else
		forto = exlist[0].get_value();

	if (counter > forto)
		return syntax_error("Parameters of the for statement is incorrect");
	do {
		int tmpLine = currentline;
		if (tmpLine > (int)sourcelist.size() - 1)
			return 0;
		if (get_first_tokentype(sourcelist[tmpLine]) == NEXT) {
			currentline = tmpLine + 1;
			break;
		}
		string src = sourcelist[tmpLine++];
		if (exec_source(src))
			return 0;
		callcnt -= 1;
		varmap[counterVarName] = ++counter;
	} while (counter < forto);

	return 0;
}

static int ifcnt = 0;

int do_if_statement()
{
	if (ifcnt > 0) {
		cout << "If can not be nested" << endl;
		return -1;
	}
	ifcnt += 1;

	Token optok;
	Token tok = get_nexttoken(tokenlist);
	double exp = 0.0;
	vector<Token> exlist; 
	while (true) {
		tok = get_nexttoken(tokenlist);
		TokenType tt = tok.get_type();
		if (tt == THEN)
			break;
		if (tt == NO_MORE_TOKEN)
			break;
		if (MINUS < tt && tt < ASSIGN)
			optok = tok;
		exlist.insert(exlist.end(), tok);
	}
	if (exlist.size() > 1)
		exp = expression(exlist).get_value();
	else 
		return syntax_error("Conditional expression is invalid");
	
	TokenType optt = optok.get_type();
	bool bResult = true;
	if (optt == GREATER && exp > 0.0)
		bResult = false;
	if (optt == SMALLER && exp < 0.0)
		bResult = false;
	if (optt == EQUAL && fabs(exp - 0.0) > 1.0e-7)
		bResult = false;
	if (optt == NOT_EQUAL && fabs(exp - 0.0) < 1.0e-7)
		bResult = false;
	if (bResult) {
		while (true) {
			if (currentline > (int)sourcelist.size() - 1)
				return 0;
			string src = sourcelist[currentline];
			TokenType tt = get_first_tokentype(src);
			if (tt == ENDIF) {
				currentline += 1;
				return 0;
			}
			if (tt == ELSE) {
				currentline += 1;
				while (true) {
					src = sourcelist[currentline++];
					if (get_first_tokentype(src) == ENDIF) {
						currentline += 1;
						return 0;
					}
				}
				return 0;
			}
      if (exec_source(sourcelist[currentline++])) 
				return 0;
		}
	} else {
		while (true) {
			string src = sourcelist[currentline++];
			if (get_first_tokentype(src) == ELSE) {
				while (true) {
					if (currentline > (int)sourcelist.size() - 1)
						return 0;
					src = sourcelist[currentline];
					if (get_first_tokentype(src) == ENDIF) {
						currentline += 1;
						return 0;
					}
					if (exec_source(sourcelist[currentline++]))
						return 0;
			  }
      }
    }
	}
}


int do_assign()
{
	if (token_stat)
		disp_tokenlist(tokenlist, "tokenlist");
	
	Token token0 = get_nexttoken(tokenlist);
	string symbol = token0.get_symbol();
	token0 = check_and_register_var(token0);
	Token token1 = get_nexttoken(tokenlist);

	vector<Token> exlist;
	while (true) {
		Token tok = get_nexttoken(tokenlist);
		if (tok.get_type() == NO_MORE_TOKEN)
			break;
		exlist.insert(exlist.end(), tok);
	}
	Token tok = Token(0.0);
	if (exlist.size() > 0)
		tok = expression(exlist);
	stck.push(tok);

	if (stack_stat) {
		print_stack(stck, "stck");
		print_stack(opstck, "opsttck");
	}
	Token tv = get_top_elem(stck);
	double v = tv.get_value();
	varmap[symbol] = v;
	if (f_direct_mode) 
		cout << v << endl;
	if (stack_stat) {
		print_stack(stck, "stck");
		print_stack(opstck, "opstck");
	}
	return 0;
}

int exec_source(string srcline)
{
	string line = trim(srcline);
	if (line.length() < 1)
		return 1;
	if (disp_line)
		cout << "Run the " << line << endl;
	if (line.length() > 1 && line[0] == '/' && line[1] == '/')
		return 0;
	get_tokenlist(line, tokenlist);

	if (token_stat)
		disp_tokenlist(tokenlist, "tokenlist");

	Token token0 = get_nth_token(0, tokenlist);
	TokenType tt0 = token0.get_type();
	if (tt0 == NOTHING)
		return -1;
	if (tt0 == END)
		return 1;
	
	if (tt0 == SYMBOL) {
		string symbol = token0.get_symbol();
		if (comp_str(symbol, "printvar")) {
			print_variable();
			return 0;
		}
		if (comp_str(symbol, "printstack")) {
			print_stack(stck, "stck");
			print_stack(opstck, "opstck");
			return 0;
		}
		if (comp_str(symbol, "list")) {
			disp_source_file();
			return 0;
		}
		if (comp_str(symbol, "stackstat") || comp_str(symbol, "ss")) {
			stack_stat = !stack_stat;
			cout << (stack_stat ? "stackstat on" : "stackstat off") << endl;
			return 0;
		}
		if (comp_str(symbol, "tokenstat") || comp_str(symbol, "ts")) {
			token_stat = !token_stat;
			cout << (token_stat ? "tokenstat on" : "tokenstat off") << endl;
			return 0;
		}
		if (comp_str(symbol, "displine") || comp_str(symbol, "dl")) {
			disp_line = !disp_line;
			cout << (disp_line ? "displine on" : "displine off") << endl;
			return 0;
		}
	}
  
	Token token1 = get_nth_token(1, tokenlist);
	TokenType tt1 = token1.get_type();
	if (tt1 == NOTHING) {
		if (token0.get_type() == VALUE) {
			cout << token0.get_value() << endl;
			return 0;
		} else {
			vector<Token> exlist;
			bool f_str_literal = false;
			Token tok = get_nexttoken(tokenlist);
			exlist.insert(exlist.end(), tok);
			if (tok.get_type() == STR_LITERAL)
				stck.push(str_expression(exlist));
			else 
				stck.push(expression(exlist));
		}
	} else {
		if (tt0 == IF) {
			if (do_if_statement())
				return -1;
			return 0;
		} else if (tt0 == CALL) {
			if (do_call_statement())
				return -1;
			return 0;
		} else if (tt0 == FOR) {
			if (do_for_statement())
				return -1;
			return 0;
		} else if (tt1 == ASSIGN) {
			do_assign();
			return 0;
		} else {
			vector<Token> exlist;
			bool f_str_literal = false;
			while (true) {
				Token tok = get_nexttoken(tokenlist);
				if (tok.get_type() == PRINT || tok.get_type() == PRINT_LN)
					continue;
				if (tok.get_type() == NO_MORE_TOKEN)
					break;
				if (tok.get_type() == STR_LITERAL)
					f_str_literal = true;
					exlist.insert(exlist.end(), tok);
			}
	    if (exlist.size() > 0) {
				if (f_str_literal)
					stck.push(str_expression(exlist));
				else 
					stck.push(expression(exlist));
			} else {
				if (f_str_literal)
					stck.push(exlist[0]);
				else
					stck.push(exlist[0].get_value());
			}
		}
	}
	if (tt0 == PRINT)
		print_value_or_literal(stck.top());
	if (tt0 == PRINT_LN)
		print_value_or_literal(stck.top(), true);
	if (f_direct_mode == true && tt0 != PRINT && tt0 != PRINT_LN)
		print_value_or_literal(stck.top(), true);

	return 0;
}


int statement(string line)
{
	if (f_direct_mode) {
		exec_source(line);
		return 0;
	}

	while (true) {
		if (currentline > (int)sourcelist.size() - 1)
			return 0;
		callcnt = 0;
		forcnt = 0;
		ifcnt = 0;
		string str = sourcelist[currentline++];
		if (exec_source(str))
			return 0;
	}

	return 0;
}

template<class T> Token get_top_elem(T &stck)     
{
	if(stck.size() < 1) 
		return Token(NO_MORE_TOKEN);   
	Token tok = stck.top();
	stck.pop();
	return tok;
}

extern string TokenTypeName[];
extern string tokenPosName[];

void print_stack(stack<Token> stck, string msg)
{
	stack<Token> tmp;

	tmp = stck;
	cout << "\n";
	cout << "=== stack " << msg << " Top ===" << endl;
	int n = (int)tmp.size();
	for (int i = 0; i < n; i++) {
		Token t = tmp.top();
		TokenType tt = t.get_type();
		if (tt == VARIABLE)
			cout << TokenTypeName[t.get_type()] << "(" << t.get_symbol() << ")" << t.get_value() << endl;
		else if (tt == SYMBOL)
			cout << TokenTypeName[t.get_type()] << ":" << t.get_symbol() << endl;
		else if (tt == ASSIGN)
			cout << TokenTypeName[t.get_type()] << endl;
		else 
			cout << TokenTypeName[t.get_type()] << ":" << t.get_value() << endl;
		tmp.pop();
	}
	cout << "--- stack bottom ---" << endl;
	cout << "\n";
}

void print_variable()
{
	map<string, double>::iterator itr;
	cout << "\n";
	cout << "=== Variable List ===" << endl;
	for (itr = varmap.begin(); itr != varmap.end(); itr++)
		cout << itr->first << ":" << itr->second << endl;
	cout << "--- Variable List ---" << endl;
	cout << "\n";
}
