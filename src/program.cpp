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

void Reader::read(const string& statement) {
	vector<Operation*> operations;
	vector<Connector*> connectors;
	vector<string> parameters;
	char_delimiters_separator<char> delimiters(false, "", " ");
	tokenizer<char_delimiters_separator<char>> t(statement, delimiters);
	for(tokenizer<>::iterator i = t.begin(); i != t.end(); ++i) {
		string s = *i;
		
		bool connector = false;
		if(s == "&&") {
			connector = true;
			operations.push_back(createOperation(parameters));
			connectors.push_back(new Success());
		} else if(s == "||") {
			connector = true;
			operations.push_back(createOperation(parameters));
			connectors.push_back(new Failure());
		}
		
		if(connector) {
			if(connectors.size() > operations.size()) {
				
			}
		}
	}
}
Operation* Reader::createOperation(const vector<string>& parameters) {
	if(parameters.at(0) == "exit") {
		return new Exit(parent);
	} else {
		return new Command(parameters);
	}
}
