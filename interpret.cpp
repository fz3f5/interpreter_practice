/*
 *  interpret.cpp
 */

#include "include/source.h"
#include "include/gettoken.h"
#include "include/interpret.h"
#include "include/util.h"
using namespace std;

map<string, double> varmap;
stack<Token> Stck;
stack<Token> OpStck;

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
	return (Token (Value, symbol, v));
}

Token expression(vector<Token> tknList)
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
			disp_tokenlist(tknList, "tknList");

		
		Token currentToken = get_nexttoken(tknList);
		TokenType ctt = currentToken.get_type();
		string csm = currentToken.get_symbol();
		if (ctt == Symbol && csm[0] == '-') {
			string tmpsymbol = csm.substr(1, csm.length()-1);
			Token tmpToken = Token (Symbol, tmpsymbol, TOP);
			tmpToken = check_and_register_var(tmpToken);
			double v = tmpToken.get_value();
			currentToken = Token (-1 * v);
			ctt = Value;
		}
		if (ctt == Symbol) {
			currentToken = check_and_register_var(currentToken);
			ctt = currentToken.get_type();
		}
		if (ctt == NomoreToken) {
			Token optoken = get_top_elem(exopstck);
			TokenType optt = optoken.get_type();
			if (optt == NomoreToken || optt == Assign) {
				exopstck.push(optoken);
				return exstck.top();
			}

			currentToken = get_top_elem(exstck);
			ctt = currentToken.get_type();
			exopstck.push(optoken);
		}
		if ((RParen < ctt) && (ctt < Smaller)){
			exopstck.push(currentToken);
			continue;
		}
		if (ctt == LParen) {
			int nLRaren = 1;
			vector<Token> tmpTokenList;
			while (true) {
				Token tok = get_nexttoken(tknList);
				TokenType tt = tok.get_type();
				if (tt == LParen) {
					nLRaren += 1;
				}
				if (tt == RParen) {
					if (--nLRaren == 0)
						break;
				}
				if (tt == NomoreToken)
					return syntax_error("NO PAREN");
				tmpTokenList.insert(tmpTokenList.end(), tok);
			}
			if (tmpTokenList.size() > 1) {
				Token tok = expression(tmpTokenList);
				tok.set_type(Value);
				tknList.insert(tknList.begin(), tok);
				continue;
			} else 
				exstck.push(tmpTokenList[0].get_value());
			continue;
	}
	if (Minus < ctt && ctt < Assign) {
		exopstck.push(currentToken);
		continue;
	}
	if (ctt == Value || ctt == Variable) {
		Token tok = get_top_elem(exstck);
		Token optok = get_top_elem(exopstck);
		TokenType optokt = optok.get_type();

		if ((tok.get_type() == Value || tok.get_type() == Variable) && Minus < optokt && optokt < Assign) {
			double a = currentToken.get_value();
			double b = tok.get_value();
			exstck.push(a - b);
			continue;
		}
		if ((tok.get_type() == Value || tok.get_type() == Variable) && (optok.get_type() == Mult || optok.get_type() == Divide)) {
			double a = currentToken.get_value();
			double b = tok.get_value();
			if (optok.get_type() == Mult) {
				exstck.push(a * b);
				continue;
			}
			if (optok.get_type() == Divide) {
				exstck.push(b / a);
				continue;
			}
		}
		if ((tok.get_type() == Value || tok.get_type() == Variable) && (optok.get_type() == Plus || optok.get_type() == Minus)) {
			Token nexttoken = get_nexttoken(tknList);
			TokenType ntt = nexttoken.get_type();
			if (ntt == Mult || ntt == Divide || ntt == RParen) {
				exstck.push(tok);
				exopstck.push(optok);
				back_token(nexttoken, tknList);
				exstck.push(currentToken);
				continue;
			} else {
				double a = currentToken.get_value();
				double b = tok.get_value();
				back_token(nexttoken, tknList);
				if (optok.get_type() == Plus) {
					exstck.push(a + b);
					continue;
				}
				if (optok.get_type() == Minus) {
					exstck.push(b - a);
					continue;
				}
			}
		}
		if (tok.get_type() != NomoreToken) 
			exstck.push(tok);
		if (optok.get_type() != NomoreToken)
			exstck.push(optok);
		exstck.push(currentToken);
	}
	}
	return (Token(Invalid));
}

Token str_expression(vector<Token> tknList) 
{
	string StrResult = "";
	bool f_plus = true;

	while (true) {
		Token currentToken = get_nexttoken(tknList);
		TokenType ctt = currentToken.get_type();

		if (ctt == Symbol) {
			currentToken = check_and_register_var(currentToken);
			ctt = currentToken.get_type();
		}

		if (ctt == NomoreToken) {
			return (Token(StrLiteral, StrResult));
		}

		if (ctt == StrLiteral) {
			if (f_plus) {
				StrResult += currentToken.get_symbol();
				f_plus = false;
			}
			continue;
		}
		if (ctt == Value) {
			if (f_plus) {
				ostringstream ostr;
				double v = currentToken.get_value();
				ostr << v;
				StrResult += ostr.str();
				f_plus = false;
			}
			continue;
		}
		if (ctt == Plus) {
			f_plus = true;
			continue;
		}
	}
	return Token(Invalid);
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
	Token token0 = get_nexttoken(TokenList);
	Token token1 = get_nexttoken(TokenList);
	string subroutineName = token1.get_symbol();

	vector<double> parameterList;
	while (true) {
		double paraValue = 0.0;
		Token tok = get_nexttoken(TokenList);
		TokenType tt = tok.get_type();
		if (tt == LParen || tt == Comma)
			continue;
		if (tt == RParen || tt == NomoreToken)
			break;
		if (tt == Variable || tt == Symbol) {
			tok = check_and_register_var(tok);
			paraValue = tok.get_value();
			parameterList.insert(parameterList.end(), paraValue);
		}
		if (tt == Value) {
			paraValue = tok.get_value();
			parameterList.insert(parameterList.end(), paraValue);
		}
	}
	int invokedLine = 0;
	string src = "";
	while (true) {
		src = sourcelist[invokedLine];
		int n = get_tokenlist(src, TokenList);
		if (get_nexttoken(TokenList).get_type() == Sub) {
			if (n < 2) 
				return syntax_error("Subroutine can not be found");
			string symbol = get_nexttoken(TokenList).get_symbol();
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
	get_tokenlist(src, TokenList);
	bool bInParen = false;
	while (true) {
		Token tok = get_nexttoken(TokenList);
		TokenType tt = tok.get_type();
		if (tt == LParen) {
			bInParen = true;
			continue;
		}
		if (tt == Comma)
			continue; 
		if (tt == RParen) {
			bInParen = false;
			break;
		}
		if (tt == NomoreToken) 
			break;
		if (tt == Symbol && bInParen == true) {
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
		get_tokenlist(src, TokenList);
		TokenType tt = get_nth_token(0, TokenList).get_type();
		if (tt == Enddef) {
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

	Token token0 = get_nexttoken(TokenList);
	Token token1 = get_nexttoken(TokenList);
	token1 = check_and_register_var(token1);
	string counterVarName = token1.get_symbol();
	Token token2 = get_nexttoken(TokenList);
	vector<Token> exTokenList;
	while (true) {
		Token tok = get_nexttoken(TokenList);
		if (tok.get_type() == NomoreToken)
			break;
		if (tok.get_type() == To)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	double counter = 0;
	if (exTokenList.size() > 1)
		counter = expression(exTokenList).get_value();
	else 
		counter = exTokenList[0].get_value();
	exTokenList.clear();
	while (true) {
		Token tok = get_nexttoken(TokenList);
		if (tok.get_type() == NomoreToken)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	double forto = 0;
	if (exTokenList.size() > 1)
		forto = expression(exTokenList).get_value();
	else
		forto = exTokenList[0].get_value();

	if (counter > forto)
		return syntax_error("Parameters of the for statement is incorrect");
	do {
		int tmpLine = currentline;
		if (tmpLine > (int)sourcelist.size() - 1)
			return 0;
		if (get_first_tokentype(sourcelist[tmpLine]) == Next) {
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
	Token tok = get_nexttoken(TokenList);
	double exp = 0.0;
	vector<Token> exTokenList; 
	while (true) {
		tok = get_nexttoken(TokenList);
		TokenType tt = tok.get_type();
		if (tt == Then)
			break;
		if (tt == NomoreToken)
			break;
		if (Minus < tt && tt < Assign)
			optok = tok;
		exTokenList.insert(exTokenList.end(), tok);
	}
	if (exTokenList.size() > 1)
		exp = expression(exTokenList).get_value();
	else 
		return syntax_error("Conditional expression is invalid");
	
	TokenType optt = optok.get_type();
	bool bResult = true;
	if (optt == Greater && exp > 0.0)
		bResult = false;
	if (optt == Smaller && exp < 0.0)
		bResult = false;
	if (optt == Equal && fabs(exp - 0.0) > 1.0e-7)
		bResult = false;
	if (optt == NotEqual && fabs(exp - 0.0) < 1.0e-7)
		bResult = false;
	if (bResult) {
		while (true) {
			if (currentline > (int)sourcelist.size() - 1)
				return 0;
			string src = sourcelist[currentline];
			TokenType tt = get_first_tokentype(src);
			if (tt == Endif) {
				currentline += 1;
				return 0;
			}
			if (tt == Else) {
				currentline += 1;
				while (true) {
					src = sourcelist[currentline++];
					if (get_first_tokentype(src) == Endif) {
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
			if (get_first_tokentype(src) == Else) {
				while (true) {
					if (currentline > (int)sourcelist.size() - 1)
						return 0;
					src = sourcelist[currentline];
					if (get_first_tokentype(src) == Endif) {
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
		disp_tokenlist(TokenList, "TokenList");
	
	Token token0 = get_nexttoken(TokenList);
	string symbol = token0.get_symbol();
	token0 = check_and_register_var(token0);
	Token token1 = get_nexttoken(TokenList);

	vector<Token> exTokenList;
	while (true) {
		Token tok = get_nexttoken(TokenList);
		if (tok.get_type() == NomoreToken)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	Token tok = Token(0.0);
	if (exTokenList.size() > 0)
		tok = expression(exTokenList);
	Stck.push(tok);

	if (stack_stat) {
		print_stack(Stck, "Stck");
		print_stack(OpStck, "OpStck");
	}
	Token tv = get_top_elem(Stck);
	double v = tv.get_value();
	varmap[symbol] = v;
	if (f_direct_mode) 
		cout << v << endl;
	if (stack_stat) {
		print_stack(Stck, "Stck");
		print_stack(OpStck, "Opstck");
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
	get_tokenlist(line, TokenList);

	if (token_stat)
		disp_tokenlist(TokenList, "TokenList");

	Token token0 = get_nth_token(0, TokenList);
	TokenType tt0 = token0.get_type();
	if (tt0 == Nothing)
		return -1;
	if (tt0 == End)
		return 1;
	
	if (tt0 == Symbol) {
		string symbol = token0.get_symbol();
		if (comp_str(symbol, "printvar")) {
			print_variable();
			return 0;
		}
		if (comp_str(symbol, "printstack")) {
			print_stack(Stck, "Stck");
			print_stack(OpStck, "OpStck");
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
  
	Token token1 = get_nth_token(1, TokenList);
	TokenType tt1 = token1.get_type();
	if (tt1 == Nothing) {
		if (token0.get_type() == Value) {
			cout << token0.get_value() << endl;
			return 0;
		} else {
			vector<Token> exTokenList;
			bool fStrLiteral = false;
			Token tok = get_nexttoken(TokenList);
			exTokenList.insert(exTokenList.end(), tok);
			if (tok.get_type() == StrLiteral)
				Stck.push(str_expression(exTokenList));
			else 
				Stck.push(expression(exTokenList));
		}
	} else {
		if (tt0 == If) {
			if (do_if_statement())
				return -1;
			return 0;
		} else if (tt0 == Call) {
			if (do_call_statement())
				return -1;
			return 0;
		} else if (tt0 == For) {
			if (do_for_statement())
				return -1;
			return 0;
		} else if (tt1 == Assign) {
			do_assign();
			return 0;
		} else {
			vector<Token> exTokenList;
			bool fStrLiteral = false;
			while (true) {
				Token tok = get_nexttoken(TokenList);
				if (tok.get_type() == Print || tok.get_type() == Println)
					continue;
				if (tok.get_type() == NomoreToken)
					break;
				if (tok.get_type() == StrLiteral)
					fStrLiteral = true;
					exTokenList.insert(exTokenList.end(), tok);
			}
	    if (exTokenList.size() > 0) {
				if (fStrLiteral)
					Stck.push(str_expression(exTokenList));
				else 
					Stck.push(expression(exTokenList));
			} else {
				if (fStrLiteral)
					Stck.push(exTokenList[0]);
				else
					Stck.push(exTokenList[0].get_value());
			}
		}
	}
	if (tt0 == Print)
		print_value_or_literal(Stck.top());
	if (tt0 == Println)
		print_value_or_literal(Stck.top(), true);
	if (f_direct_mode == true && tt0 != Print && tt0 != Println)
		print_value_or_literal(Stck.top(), true);

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

template<class T> Token get_top_elem(T &Stck)      // change &Stck > &stck ????  Stck is global var?
{
	if(Stck.size() < 1) 
		return Token(NomoreToken);   
	Token tok = Stck.top();
	Stck.pop();
	return tok;
}

extern string TokenTypeName[];
extern string tokenPosName[];

void print_stack(stack<Token> stck, string message)
{
	stack<Token> tmp;

	tmp = stck;
	cout << "\n";
	cout << "=== stack " << message << " Top ===" << endl;
	int n = (int)tmp.size();
	for (int i = 0; i < n; i++) {
		Token t = tmp.top();
		TokenType tt = t.get_type();
		if (tt == Variable)
			cout << TokenTypeName[t.get_type()] << "(" << t.get_symbol() << ")" << t.get_value() << endl;
		else if (tt == Symbol)
			cout << TokenTypeName[t.get_type()] << ":" << t.get_symbol() << endl;
		else if (tt == Assign)
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
