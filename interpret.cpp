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
stack<Token> Opstck;

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
			
