#include<stdio.h>
#include<string.h>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<string>
#include<vector>
#include<stdexcept>
using namespace boost;
using namespace std;

//#define PRINT(x) cout << x << endl;
#define PRINT(x)

class Exit;
class Reader;
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
		
		Operation* op = 0;
		try {
			op = parser.ParseLine();
		} catch(const invalid_argument& e) {
			out << e.what() << endl;
		}
		if(op) {
			op->execute();
		} else {
			out << "Command not executed." << endl;
		}
	}
}
Operation* Reader::ParseLine() {
	PRINT("Line: " + line);
	Operation* result = Parse();
	index = 0;
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
		case TokenTypes::OpenParen:
			//We have a chain
			UpdateIndex(t);
			return ParseChain();
		case TokenTypes::CharSpace:
			//We don't care about spaces here
			UpdateIndex(t);
			PRINT("Space");
			goto Start;
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			//We have the first command of a chain
			return ParseChain();
		case TokenTypes::End:
			//If the line is empty, don't do anything
			return 0;
		default:
			//Unexpected
			UpdateIndex(t);
			UnexpectedToken(t);
			return 0;
	}
	return 0;
}
Operation* Reader::ParseChain() {
	vector<Operation*> operations;
	vector<Connector*> connectors;
	
	bool pipe = false;	//Whether we must connect the next and previous operations via pipe
	
	Parse:
		
	Token t = Read();
	PRINT("ParseChain: Index = " << index);
	switch(t.type) {
		case TokenTypes::OpenBracket:
			//We have a test command
			PRINT("ParseChain: Sub test");
			//Increment the index past the bracket since the function expects that
			UpdateIndex(t);
			operations.push_back(ParseTestBracket());
			goto ParsedOperation;
			
		case TokenTypes::OpenParen:
			//We have a subchain
			PRINT("ParseChain: Subchain");
			UpdateIndex(t);
			operations.push_back(ParseChain());
			goto ParsedOperation;
		case TokenTypes::CloseParen:
			PRINT("ParseChain: CloseParen");
			//If we reach the closing parenthesis, then we are done
			UpdateIndex(t);
			goto Done;
			
		case TokenTypes::ConnectorAnd:
		case TokenTypes::ConnectorOr:
		case TokenTypes::ConnectorSemicolon:
			PRINT("ParseChain: Connector");
			if(connectors.size() >= operations.size() || pipe) {
				//We shouldn't have more connectors than operations
				throw invalid_argument("ERROR: Unexpected connector");
				return 0;
			} else {
				connectors.push_back(ParseConnector());
			}
			goto Parse;
			
		case TokenTypes::Input: {
			UpdateIndex(t);
			SkipSpaces();
			string file = ParseArgument();
			if(operations.empty()) {
				UnexpectedToken(t);
			}
			Operation* result = new InputOperation(operations.back(), file);
			operations.pop_back();
			operations.push_back(result);
			goto ParsedOperation;
		}
		case TokenTypes::Pipe:
			pipe = true;
			UpdateIndex(t);
			goto Parse;
		case TokenTypes::OutputAppend: {
			UpdateIndex(t);
			SkipSpaces();
			string file = ParseArgument();
			if(operations.empty()) {
				UnexpectedToken(t);
			}
			Operation* result = new AppendOperation(operations.back(), file);
			operations.pop_back();
			operations.push_back(result);
			goto ParsedOperation;
		}
		case TokenTypes::Output: {
			UpdateIndex(t);
			SkipSpaces();
			string file = ParseArgument();
			if(operations.empty()) {
				UnexpectedToken(t);
			}
			Operation* result = new OutputOperation(operations.back(), file);
			operations.pop_back();
			operations.push_back(result);
			goto ParsedOperation;
		}
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			//We have the first argument of a command
			PRINT("ParseChain: Argument");
			operations.push_back(ParseCommand());
			goto ParsedOperation;
		case TokenTypes::CharSpace:
			//Ignore the space
			PRINT("ParseChain: Space");
			UpdateIndex(t);
			goto Parse;
			
		case TokenTypes::End:
			//We didn't expect this
			throw invalid_argument("ERROR: Unexpected end of line in ParseChain");
			return 0;
			
		default:
			//Unexpected
			UpdateIndex(t);
			UnexpectedToken(t);
			return 0;
	}
	
	Done:
	
	//If we have a hanging connector, then ERROR
	if(connectors.size() >= operations.size()) {
		throw invalid_argument("ERROR: Hanging connector");
		return 0;
	} else if(operations.size() == 0) {
		//This chain is empty
		throw invalid_argument("ERROR: Empty chain");
		return 0;
	} else if(operations.size() == 1) {
		return operations[0];
	} else {
		return new Chain(operations, connectors);
	}
	
	ParsedOperation:
	//Check if we have parsed the receiver of a pipe
	if(pipe) {
		Operation* receiver = operations.back();
		operations.pop_back();
		Operation* giver = operations.back();
		operations.pop_back();
		operations.push_back(new PipeOperation(giver, receiver));
		pipe = false;
	}
	goto Parse;
	
}
Connector* Reader::ParseConnector() {
	Token t = Read();
	switch(t.type) {
		case TokenTypes::ConnectorAnd:
			UpdateIndex(t);
			return new Success();
		case TokenTypes::ConnectorOr:
			UpdateIndex(t);
			return new Failure();	
		case TokenTypes::ConnectorSemicolon:
			UpdateIndex(t);
			return new Any();
		default:
			throw invalid_argument("ERROR: Expected connector");
			return 0;
	}
}
Operation* Reader::ParseCommand() {
	vector<string> args;
	Read:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::ConnectorSemicolon:
		case TokenTypes::ConnectorAnd:
		case TokenTypes::ConnectorOr:
		case TokenTypes::CloseParen:
		case TokenTypes::CloseBracket:
		case TokenTypes::End:
			//Delimited by end of line, closing parenthesis, or closing connector
			goto Done;
		case TokenTypes::CharSpace:
			UpdateIndex(t);
			goto Read;
			
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			args.push_back(ParseArgument());
			goto Read;
	}
	
	Done:
	string exe = args.at(0);
	if(exe == "exit") {
		if(args.size() > 1) {
			throw invalid_argument("ERROR: too many arguments for exit command");
		} else {
			return new Exit(parent);
		}
		
	} else if(exe == "test") {
		if(args.size() == 1) {
			throw invalid_argument("ERROR: test expression expected");
		} else if(args.size() == 2) {
			return new TestCommand(args.at(1));
		} else if(args.size() == 3) {
			return new TestCommand(args.at(1), args.at(2));
		} else {
			throw invalid_argument("ERROR: too many arguments in test expression");
		}
		//throw invalid_argument("TO DO: not implemented");
	} else {
		return new Command(args);
	}
}
string Reader::ParseArgument() {
	PRINT("ParseArgument");
	string result = "";
	Read:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::CharBackslash:
			UpdateIndex(t);
			result.push_back(ParseBackslash());
			goto Read;
		case TokenTypes::CharQuote:
			UpdateIndex(t);
			result += ParseQuoted();
			goto Read;
		case TokenTypes::OpenParen:
		case TokenTypes::CloseParen:
		case TokenTypes::OpenBracket:
		case TokenTypes::CloseBracket:
		case TokenTypes::ConnectorSemicolon:
		case TokenTypes::ConnectorAnd:
		case TokenTypes::ConnectorOr:
		case TokenTypes::Pipe:
		case TokenTypes::End:
		case TokenTypes::CharSpace:
			//Delimited by end of line, space, or connector
			goto Done;
		default:
			UpdateIndex(t);
			result += t.value;
			goto Read;
	}
	
	Done:
		
	PRINT("ParseArgument: Result = " << result)
	return result;
}
string Reader::ParseQuoted() {
	//We build a string from characters until we find a closing quote
	string result = "";
	Read:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::CharBackslash:
			UpdateIndex(t);
			result.push_back(ParseBackslash());
			goto Read;
		case TokenTypes::CharQuote:
			UpdateIndex(t);
			return result;
		case TokenTypes::End:
			throw invalid_argument("ERROR: Unexpected end of line");
		default:
			UpdateIndex(t);
			result += t.value;
			goto Read;
	}
}
char Reader::ParseBackslash() {
	//Escapes a character
	char result = line.at(index);
	index++;
	return result;
}
Operation* Reader::ParseTestBracket() {
	vector<string> args;
	Read:
	Token t = Read();
	switch(t.type) {
		case TokenTypes::CloseBracket:
			UpdateIndex(t);
			goto Done;
		
		case TokenTypes::CharBackslash:
		case TokenTypes::CharQuote:
		case TokenTypes::StringArg:
			args.push_back(ParseArgument());
			goto Read;
			
		case TokenTypes::CharSpace:
			UpdateIndex(t);
			goto Read;
			
		default:
			UnexpectedToken(t);
			return 0;
	}
	
	Done:
	if(args.size() == 0) {
		throw invalid_argument("ERROR: test expression expected");
	} else if(args.size() == 1) {
		return new TestCommand(args.at(0));
	} else if(args.size() == 2) {
		return new TestCommand(args.at(0), args.at(1));
	} else {
		throw invalid_argument("ERROR: too many arguments in test expression");
	}
	return 0;
}

void Reader::SkipSpaces() {
	Token t = Read();
	while(t.type == TokenTypes::CharSpace) {
		UpdateIndex(t);
		t = Read();
	}
}
