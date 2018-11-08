#include<stdio.h>
#include<string.h>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<string>
#include<vector>
using namespace boost;

#include "program.h"
#include "reader.h"
#include "operation.h"

void Program::run() {
	Reader reader(this);
	//Run until we get a close() call
	out << "rshell" << endl;
	out << "Created by Alex Chen and Kyle Tran for CS100" << endl;
	
	while(true) {
		printInfo();
		out << "$ ";
		
		string line;
		getline(in, line);
		//Delete everything past the comment
		line = line.substr(0, line.find("#", 0));
		
		/*
		auto startIndex = 0;
		auto endIndex = line.find(";", startIndex);
		while(endIndex != string::npos) {
			const string statement = line.substr(startIndex, endIndex);
			reader.read(statement);
			
			startIndex = endIndex + 1;
			endIndex = line.find(";", startIndex);
			
			cout << "semicolon" << endl;
			if(endIndex == string::npos) {
				break;
			}
			
		}
		const string statement = line.substr(startIndex, endIndex);
		reader.read(statement);
		*/
		
		
		char_delimiters_separator<char> delimiters(false, "", ";");
		tokenizer<char_delimiters_separator<char>> t(line, delimiters);
		for(tokenizer<>::iterator i = t.begin(); i != t.end(); ++i) {
			string s = *i;
			trim(s);
			reader.read(s);
		}
		
	}
}

bool Reader::read(const string& statement) {
	//Keep vectors in case we get a Chain
	vector<Operation*> operations;
	vector<Connector*> connectors;
	
	//Store the arguments for the Command we are currently parsing
	//This vector includes the executable at the front. We'll find it when we execute
	vector<string> arguments;
	char_delimiters_separator<char> delimiters(false, "", " ");
	tokenizer<char_delimiters_separator<char>> t(statement, delimiters);
	for(tokenizer<>::iterator i = t.begin(); i != t.end(); ++i) {
		string s = *i;
		
		bool connector = false;
		//If we have a connector, then we are done parsing the current Command
		if(s == "&&") {
			connector = true;
			operations.push_back(createOperation(arguments));
			arguments.clear();
			connectors.push_back(new Success());
		} else if(s == "||") {
			connector = true;
			operations.push_back(createOperation(arguments));
			arguments.clear();
			connectors.push_back(new Failure());
		} else {
			//Otherwise, we have a regular argument
			arguments.push_back(s);
		}
		
		if(connector && connectors.size() > operations.size()) {
			//Make sure that we have a proper number of connectors
			parent->dbg << "Error: Only one connector allowed between pairs of commands" << endl;
		}
	}
	//Don't forget to check for a command at the end
	if(arguments.size() > 0) {
		parent->dbg << "Added last command" << endl;
		operations.push_back(createOperation(arguments));
		arguments.clear();
	}
	
	if(connectors.size() != (operations.size() - 1)) {
		//Make sure that we have a proper number of connectors
		parent->dbg << "Error: Only one connector allowed between pairs of commands" << endl;
	}
	Operation *op = 0;
	if(operations.size() == 1) {
		parent->dbg << "Operation Type: Command" << endl;
		op = operations.at(0);
	} else {
		parent->dbg << "Operation Type: Chain" << endl;
		op = new Chain(operations, connectors);
	}
	
	op->print(parent->dbg);
	//Execute the operation
	op->execute();
}
Operation* Reader::createOperation(const vector<string>& parameters) {
	cout << "Command: " << parameters.at(0) << endl;
	if(parameters.at(0) == "exit") {
		return new Exit(parent);
	} else {
		return new Command(parameters);
	}
}
