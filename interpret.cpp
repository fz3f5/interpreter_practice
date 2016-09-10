//
// interpret.cpp
//

#include "source.h"
#include "gettoken.h"
#include "interpret.h"
#include "util.h"

using namespace std;

map<string, double> varmap;
stack<Token> Stck;
stack<Token> OpStck;

static Token checkAndRegisterVar (Token token0)
{
	string symbol = token0.getSymbol();
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

Token expression (vector <Token> tknList)
{
	stack <Token> exstck;
	stack <Token> exopstck;

	while (true)
	{
		if (DispStack) {
			printstack(exstck, "exstck");
			printstack(exopstck, "exopstck");
		}
		if (DispToken) 
			dispTokenList(tknList, "tknList");

		
		Token currentToken = getNextToken(tknList);
		TokenType ctt = currentToken.getType();
		string csm = currentToken.getSymbol();
		if (ctt == Symbol && csm[0] == '-') {
			string tmpsymbol = csm.substr(1, csm.length()-1);
			Token tmpToken = Token (Symbol, tmpsymbol, Top);
			tmpToken = checkAndRegisterVar(tmpToken);
			double v = tmpToken.getValue();
			currentToken = Token (-1 * v);
			ctt = Value;
		}
		if (ctt == Symbol) {
			currentToken = checkAndRegisterVar(currentToken);
			ctt = currentToken.getType();
		}
		if (ctt == NomoreToken) {
			Token optoken = getTopElem(exopstck);
			TokenType optt = optoken.getType();
			if (optt == NomoreToken || optt == Assign) {
				exopstck.push(optoken);
				return exstck.top();
			}

			currentToken = getTopElem(exstck);
			ctt = currentToken.getType();
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
				Token tok = getNextToken(tknList);
				TokenType tt = tok.getType();
				if (tt == LParen) {
					nLRaren += 1;
				}
				if (tt == RParen) {
					if (--nLRaren == 0)
						break;
				}
				if (tt == NomoreToken)
					return syntaxError("NO PAREN");
				tmpTokenList.insert(tmpTokenList.end(), tok);
			}
			if (tmpTokenList.size() > 1) {
				Token tok = expression(tmpTokenList);
				tok.setType(Value);
				tknList.insert(tknList.begin(), tok);
				continue;
			} else 
				exstck.push(tmpTokenList[0].getValue());
			continue;
	}
	if (Minus < ctt && ctt < Assign) {
		exopstck.push(currentToken);
		continue;
	}
	if (ctt == Value || ctt == Variable) {
		Token tok = getTopElem(exstck);
		Token optok = getTopElem(exopstck);
		TokenType optokt = optok.getType();

		if ((tok.getType() == Value || tok.getType() == Variable) && Minus < optokt && optokt < Assign) {
			double a = currentToken.getValue();
			double b = tok.getValue();
			exstck.push(a - b);
			continue;
		}
		if ((tok.getType() == Value || tok.getType() == Variable) && (optok.getType() == Mult || optok.getType() == Divide)) {
			double a = currentToken.getValue();
			double b = tok.getValue();
			if (optok.getType() == Mult) {
				exstck.push(a * b);
				continue;
			}
			if (optok.getType() == Divide) {
				exstck.push(b / a);
				continue;
			}
		}
		if ((tok.getType() == Value || tok.getType() == Variable) && (optok.getType() == Plus || optok.getType() == Minus)) {
			Token nexttoken = getNextToken(tknList);
			TokenType ntt = nexttoken.getType();
			if (ntt == Mult || ntt == Divide || ntt == RParen) {
				exstck.push(tok);
				exopstck.push(optok);
				backToken(nexttoken, tknList);
				exstck.push(currentToken);
				continue;
			} else {
				double a = currentToken.getValue();
				double b = tok.getValue();
				backToken(nexttoken, tknList);
				if (optok.getType() == Plus) {
					exstck.push(a + b);
					continue;
				}
				if (optok.getType() == Minus) {
					exstck.push(b - a);
					continue;
				}
			}
		}
		if (tok.getType() != NomoreToken) 
			exstck.push(tok);
		if (optok.getType() != NomoreToken)
			exstck.push(optok);
		exstck.push(currentToken);
	}
	}
	return (Token(Invalid));
}

Token strexpression (vector<Token> tknList) 
{
	string StrResult = "";
	bool fPlus = true;

	while (true) {
		Token currentToken = getNextToken(tknList);
		TokenType ctt = currentToken.getType();

		if (ctt == Symbol) {
			currentToken = checkAndRegisterVar(currentToken);
			ctt = currentToken.getType();
		}

		if (ctt == NomoreToken) {
			return (Token(StrLiteral, StrResult));
		}

		if (ctt == StrLiteral) {
			if (fPlus) {
				StrResult += currentToken.getSymbol();
				fPlus = false;
			}
			continue;
		}
		if (ctt == Value) {
			if (fPlus) {
				ostringstream ostr;
				double v = currentToken.getValue();
				ostr << v;
				StrResult += ostr.str();
				fPlus = false;
			}
			continue;
		}
		if (ctt == Plus) {
			fPlus = true;
			continue;
		}
	}
	return Token(Invalid);
}

static int callCount = 0;

int doCallStatement()
{
	if (callCount > 0) {
		cout << "call can not be nested" << endl;
		return -1;
	}
	callCount += 1;

	int returnLine = CurrentLine;
	Token token0 = getNextToken(TokenList);
	Token token1 = getNextToken(TokenList);
	string subroutineName = token1.getSymbol();

	vector<double> parameterList;
	while (true) {
		double paraValue = 0.0;
		Token tok = getNextToken(TokenList);
		TokenType tt = tok.getType();
		if (tt == LParen || tt == Comma)
			continue;
		if (tt == RParen || tt == NomoreToken)
			break;
		if (tt == Variable || tt == Symbol) {
			tok = checkAndRegisterVar(tok);
			paraValue = tok.getValue();
			parameterList.insert(parameterList.end(), paraValue);
		}
		if (tt == Value) {
			paraValue = tok.getValue();
			parameterList.insert(parameterList.end(), paraValue);
		}
	}
	int invokedLine = 0;
	string src = "";
	while (true) {
		src = SourceList[invokedLine];
		int n = getTokenList(src, TokenList);
		if (getNextToken(TokenList).getType() == Sub) {
			if (n < 2) 
				return syntaxError("Subroutine can not be found");
			string symbol = getNextToken(TokenList).getSymbol();
			if (compareIgnCase(symbol, subroutineName)) {
				break;
			} 
		}
		if (++invokedLine >= (int)(SourceList.size()))
			return syntaxError("Subroutine can not be found");
	}
	
	src = SourceList[invokedLine];
	if (DispLine)
		cout << "Run the " << src << endl;
	getTokenList(src, TokenList);
	bool bInParen = false;
	while (true) {
		Token tok = getNextToken(TokenList);
		TokenType tt = tok.getType();
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
			string symbol = tok.getSymbol();
			varmap[symbol] = parameterList[0];
			if (parameterList.size() < 1)
				return syntaxError("The number of the argument and parameter do not match");
			parameterList.erase(parameterList.begin());
		}
	}
	
	CurrentLine = invokedLine+1;
	while (true) {
		src = SourceList[CurrentLine];
		getTokenList(src, TokenList);
		TokenType tt = getTokenN(0, TokenList).getType();
		if (tt == Enddef) {
			if (DispLine)
				cout << "Run the " << src << endl;
			break;
		}
		if (CurrentLine > (int)SourceList.size() - 1)
			return 0;
		if (execSrcLine(SourceList[CurrentLine++]))
			return -1;
	}
	CurrentLine = returnLine;
	return 0;
}

static int forCount = 0;

int doForStatement()
{
	if (forCount > 0) {
		cout << "for can not be nested" << endl;
		return -1;
	}
	forCount += 1;

	Token token0 = getNextToken(TokenList);
	Token token1 = getNextToken(TokenList);
	token1 = checkAndRegisterVar(token1);
	string counterVarName = token1.getSymbol();
	Token token2 = getNextToken(TokenList);
	vector<Token> exTokenList;
	while (true) {
		Token tok = getNextToken(TokenList);
		if (tok.getType() == NomoreToken)
			break;
		if (tok.getType() == To)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	double counter = 0;
	if (exTokenList.size() > 1)
		counter = expression(exTokenList).getValue();
	else 
		counter = exTokenList[0].getValue();
	exTokenList.clear();
	while (true) {
		Token tok = getNextToken(TokenList);
		if (tok.getType() == NomoreToken)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	double forto = 0;
	if (exTokenList.size() > 1)
		forto = expression(exTokenList).getValue();
	else
		forto = exTokenList[0].getValue();

	if (counter > forto)
		return syntaxError("Parameters of the for statement is incorrect");
	do {
		int tmpLine = CurrentLine;
		if (tmpLine > (int)SourceList.size() - 1)
			return 0;
		if (get1stTokenType(SourceList[tmpLine]) == Next) {
			CurrentLine = tmpLine + 1;
			break;
		}
		string src = SourceList[tmpLine++];
		if (execSrcLine(src))
			return 0;
		callCount -= 1;
		varmap[counterVarName] = ++counter;
	} while (counter < forto);

	return 0;
}

static int ifCount = 0;

int doIfStatement()
{
	if (ifCount > 0) {
		cout << "If can not be nested" << endl;
		return -1;
	}
	ifCount += 1;

	Token optok;
	Token tok = getNextToken(TokenList);
	double exp = 0.0;
	vector<Token> exTokenList; 
	while (true) {
		tok = getNextToken(TokenList);
		TokenType tt = tok.getType();
		if (tt == Then)
			break;
		if (tt == NomoreToken)
			break;
		if (Minus < tt && tt < Assign)
			optok = tok;
		exTokenList.insert(exTokenList.end(), tok);
	}
	if (exTokenList.size() > 1)
		exp = expression(exTokenList).getValue();
	else 
		return syntaxError("Conditional expression is invalid");
	
	TokenType optt = optok.getType();
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
			if (CurrentLine > (int)SourceList.size() - 1)
				return 0;
			string src = SourceList[CurrentLine];
			TokenType tt = get1stTokenType(src);
			if (tt == Endif) {
				CurrentLine += 1;
				return 0;
			}
			if (tt == Else) {
				CurrentLine += 1;
				while (true) {
					src = SourceList[CurrentLine++];
					if (get1stTokenType(src) == Endif) {
						CurrentLine += 1;
						return 0;
					}
				}
				return 0;
			}
      if (execSrcLine(SourceList[CurrentLine++])) 
				return 0;
		}
	} else {
		while (true) {
			string src = SourceList[CurrentLine++];
			if (get1stTokenType(src) == Else) {
				while (true) {
					if (CurrentLine > (int)SourceList.size() - 1)
						return 0;
					src = SourceList[CurrentLine];
					if (get1stTokenType(src) == Endif) {
						CurrentLine += 1;
						return 0;
					}
					if (execSrcLine(SourceList[CurrentLine++]))
						return 0;
			  }
      }
    }
	}
}


int doAssign()
{
	if (DispToken)
		dispTokenList(TokenList, "TokenList");
	
	Token token0 = getNextToken(TokenList);
	string symbol = token0.getSymbol();
	token0 = checkAndRegisterVar(token0);
	Token token1 = getNextToken(TokenList);

	vector<Token> exTokenList;
	while (true) {
		Token tok = getNextToken(TokenList);
		if (tok.getType() == NomoreToken)
			break;
		exTokenList.insert(exTokenList.end(), tok);
	}
	Token tok = Token(0.0);
	if (exTokenList.size() > 0)
		tok = expression(exTokenList);
	Stck.push(tok);

	if (DispStack) {
		printstack(Stck, "Stck");
		printstack(OpStck, "OpStck");
	}
	Token tv = getTopElem(Stck);
	double v = tv.getValue();
	varmap[symbol] = v;
	if (fDirectMode) 
		cout << v << endl;
	if (DispStack) {
		printstack(Stck, "Stck");
		printstack(OpStck, "Opstck");
	}
	return 0;
}

int execSrcLine(string sourceline)
{
	string line = trim(sourceline);
	if (line.length() < 1)
		return 1;
	if (DispLine)
		cout << "Run the " << line << endl;
	if (line.length() > 1 && line[0] == '/' && line[1] == '/')
		return 0;
	getTokenList(line, TokenList);

	if (DispToken)
		dispTokenList(TokenList, "TokenList");

	Token token0 = getTokenN(0, TokenList);
	TokenType tt0 = token0.getType();
	if (tt0 == Nothing)
		return -1;
	if (tt0 == End)
		return 1;
	
	if (tt0 == Symbol) {
		string symbol = token0.getSymbol();
		if (compareIgnCase(symbol, "printvar")) {
			printvarmap();
			return 0;
		}
		if (compareIgnCase(symbol, "printstack")) {
			printstack(Stck, "Stck");
			printstack(OpStck, "OpStck");
			return 0;
		}
		if (compareIgnCase(symbol, "list")) {
			dispSourceFile();
			return 0;
		}
		if (compareIgnCase(symbol, "DispStack") || compareIgnCase(symbol, "ds")) {
			DispStack = !DispStack;
			cout << (DispStack ? "DispStack on" : "DispStack off") << endl;
			return 0;
		}
		if (compareIgnCase(symbol, "DispToken") || compareIgnCase(symbol, "dt")) {
			DispToken = !DispToken;
			cout << (DispToken ? "DispToken on" : "DispToken off") << endl;
			return 0;
		}
		if (compareIgnCase(symbol, "DispLine") || compareIgnCase(symbol, "dl")) {
			DispLine = !DispLine;
			cout << (DispLine ? "DispLine on" : "DispLine off") << endl;
			return 0;
		}
	}
  
	Token token1 = getTokenN(1, TokenList);
	TokenType tt1 = token1.getType();
	if (tt1 == Nothing) {
		if (token0.getType() == Value) {
			cout << token0.getValue() << endl;
			return 0;
		} else {
			vector<Token> exTokenList;
			bool fStrLiteral = false;
			Token tok = getNextToken(TokenList);
			exTokenList.insert(exTokenList.end(), tok);
			if (tok.getType() == StrLiteral)
				Stck.push(strexpression(exTokenList));
			else 
				Stck.push(expression(exTokenList));
		}
	} else {
		if (tt0 == If) {
			if (doIfStatement())
				return -1;
			return 0;
		} else if (tt0 == Call) {
			if (doCallStatement())
				return -1;
			return 0;
		} else if (tt0 == For) {
			if (doForStatement())
				return -1;
			return 0;
		} else if (tt1 == Assign) {
			doAssign();
			return 0;
		} else {
			vector<Token> exTokenList;
			bool fStrLiteral = false;
			while (true) {
				Token tok = getNextToken(TokenList);
				if (tok.getType() == Print || tok.getType() == Println)
					continue;
				if (tok.getType() == NomoreToken)
					break;
				if (tok.getType() == StrLiteral)
					fStrLiteral = true;
					exTokenList.insert(exTokenList.end(), tok);
			}
	    if (exTokenList.size() > 0) {
				if (fStrLiteral)
					Stck.push(strexpression(exTokenList));
				else 
					Stck.push(expression(exTokenList));
			} else {
				if (fStrLiteral)
					Stck.push(exTokenList[0]);
				else
					Stck.push(exTokenList[0].getValue());
			}
		}
	}
	if (tt0 == Print)
		printTokenValOrLiteral(Stck.top());
	if (tt0 == Println)
		printTokenValOrLiteral(Stck.top(), true);
	if (fDirectMode == true && tt0 != Print && tt0 != Println)
		printTokenValOrLiteral(Stck.top(), true);

	return 0;
}


int statement (string statementLine)
{
	if (fDirectMode) {
		execSrcLine(statementLine);
		return 0;
	}

	while (true) {
		if (CurrentLine > (int)SourceList.size() - 1)
			return 0;
		callCount = 0;
		forCount = 0;
		ifCount = 0;
		string str = SourceList[CurrentLine++];
		if (execSrcLine(str))
			return 0;
	}

	return 0;
}

template<class T> Token getTopElem(T &Stck)
{
	if (Stck.size() < 1) 
		return Token(NomoreToken);   
	Token tok = Stck.top();
	Stck.pop();
	return tok;
}

extern string TokenTypeName[];
extern string tokenPosName[];

void printstack (stack<Token> Stck, string msg)
{
	stack<Token> tmpstck;

	tmpstck = Stck;
	cout << "=== stack " << msg << " Top ===" << endl;
	int n = (int)tmpstck.size();
	for (int i = 0; i < n; i++) {
		Token t = tmpstck.top();
		TokenType tt = t.getType();
		if (tt == Variable)
			cout << TokenTypeName[t.getType()] << "(" << t.getSymbol() << ")" << t.getValue() << endl;
		else if (tt == Symbol)
			cout << TokenTypeName[t.getType()] << ":" << t.getSymbol() << endl;
		else if (tt == Assign)
			cout << TokenTypeName[t.getType()] << endl;
		else 
			cout << TokenTypeName[t.getType()] << ":" << t.getValue() << endl;
		tmpstck.pop();
	}
	cout << "--- stack bottom ---" << endl;
}

void printvarmap()
{
	map<string, double>::iterator itr;
	cout << "=== Variable map List ===" << endl;
	for (itr = varmap.begin(); itr != varmap.end(); itr++)
		cout << itr->first << ":" << itr->second << endl;
	cout << "--- Variable map List ---" << endl;
}


		 
		
