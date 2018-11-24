#include<stdio.h>
#include<string.h>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<string>
#include<vector>
#include<stdexcept>
using namespace boost;

#define DEBUG_THROW(x) throw new invalid_argument(x);

#include "program.h"
#include "reader.h"
#include "operation.h"

void Program::run() {
	//Run until we get a close() call
	out << "rshell" << endl;
	out << "Created by Alex Chen and Kyle Tran for CS100" << endl;
	
	while(true) {
		printInfo();
		out << "$ ";
		
		string line;
		getline(in, line);
		Reader parser(this, line);
		parser.ParseLine()->execute();
	}
}

Operation* Reader::ParseLine() {
	if(index > 0) {
		DEBUG_THROW("Please create a new Reader");
	}
	Operation* result = Parse();
	return result;
}
Operation* Reader::Parse() {

	Start:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::OpenBracket:
			//We have a test command
			UpdateIndex(t);
			return ParseTestBracket();
		case TokenTypes::CloseBracket:
			//Unexpected, so we ignore and continue
			UpdateIndex(t);
			DEBUG_THROW("unexpected closing bracket");
			goto Start;
		case TokenTypes::OpenParen:
			//We have a chain
			UpdateIndex(t);
			return ParseChain();
		case TokenTypes::CloseParen:
			//Unexpected, so we ignore and continue
			UpdateIndex(t);
			DEBUG_THROW("unexpected closing parenthesis");
			goto Start;
		case TokenTypes::ConnectorAnd:
		case TokenTypes::ConnectorOr:
		case TokenTypes::ConnectorSemicolon:
			//Unexpected (since we do not parse Chains), so we ignore and continue
			UpdateIndex(t);
			DEBUG_THROW("unexpected connector");
			goto Start;
		case TokenTypes::CharSpace:
			//We don't care about spaces here
			UpdateIndex(t);
			goto Start;
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			//We have the first command of a chain
			return ParseChain();
		case TokenTypes::End:
			//If the line is empty, don't do anything
			return 0;
		
	}
	return 0;
}
Operation* Reader::ParseChain() {
	vector<Operation*> operations;
	vector<Connector*> connectors;
	
	Parse:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::OpenBracket:
			//We have a test command
			operations.add(ParseTestBracket());
			goto Parse;
		case TokenTypes::CloseBracket:
			//We didn't expect this
			DEBUG_THROW("unexpected closing bracket");
			goto Parse;
			
		case TokenTypes::OpenParen:
			//We have a subchain
			UpdateIndex(t);
			operations.add(ParseChain());
			goto Parse;
		case TokenTypes::CloseParen:
			//If we reach the closing parenthesis, then we are done
			UpdateIndex(t);
			goto Done;
			
		case TokenTypes::ConnectorAnd:
		case TokenTypes::ConnectorOr:
		case TokenTypes::ConnectorSemicolon:
			if(connectors.size() >= operations.size()) {
				//We shouldn't have more connectors than operations
				//But if we do, then we just ignore the extras
				DEBUG_THROW("ERROR: unexpected connector");
			} else {
				connectors.add(ParseConnector());
			}
			goto Parse;
			
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			//We have the first argument of a command
			operations.add(ParseCommand());
			goto Parse;
		case TokenTypes::CharSpace:
			//Ignore the space
			UpdateIndex(t);
			goto Parse;
			
		case TokenTypes::End:
			//We didn't expect this
			DEBUG_THROW("ERROR: unexpected end of line");
			goto Done;
	}
	
	Done:
	
	//If we have a hanging connector, then ERROR
	if(connectors.size() >= operations.size()) {
		DEBUG_THROW("ERROR: Unexpected hanging connector");
		connectors.add(ParseConnector());
	} else if(operations.size() == 0) {
		//This chain is empty
		DEBUG_THROW("ERROR: Empty chain");
	} else if(operations.size() == 1) {
		return operations[0];
	} else {
		return new Chain(operations, connectors);
	}
}
Connector* Reader::ParseConnector() {
	Token t = Read();
	switch(t.type) {
		case TokenTypes::ConnectorAnd:
			UpdateIndex(t)
			return Success();
		case TokenTypes::ConnectorOr:
			UpdateIndex(t)
			return Failure();	
		case TokenTypes::ConnectorSemicolon:
			UpdateIndex(t)
			return Any();
		default:
			DEBUG_THROW("expected connector);
	}
}
Operation* Reader::ParseCommand() {
	
}
string Reader::ParseArgument() {
	
}
string Reader::ParseQuoted() {
	Token t
}
char Reader::ParseBackslash() {
	char result = line.at(index);
	index++;
	return result;
}
Connector* Reader::ParseConnector() {
	Token t = Read();
	switch(t.type) {
		case TokenTypes::ConnectorAnd:
			UpdateIndex(t)
			return Success();
		case TokenTypes::ConnectorOr:
			UpdateIndex(t)
			return Failure();	
		case TokenTypes::ConnectorSemicolon:
			UpdateIndex(t)
			return Any();
		default:
			DEBUG_THROW("expected connector);
	}
}
Operation* Reader::ParseTestBracket() {
	return 0;
}

