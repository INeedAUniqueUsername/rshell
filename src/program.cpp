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
	//Run until we get a close() call
	out << "rshell" << endl;
	out << "Created by Alex Chen and Kyle Tran for CS100" << endl;
	
	while(true) {
		printInfo();
		out << "$ ";
		
		string line;
		getline(in, line);
		Reader parser(this, line);
		
	}
}
Operation* Reader::parseLine() {
	//Delete everything past the comment
	unsigned comment = line.find('#', 0);
	if(comment != string::npos) {
		line = line.substr(0, comment);
	}
	Operation* op = parse();
	return op;
}
Operation* Reader::parse() {
	
}
//Create an operation based on the arguments we just processed. We always have at least one argument.
Operation* Reader::createOperation(const vector<string>& arguments) {
	//parent->dbg << "Command: " << arguments.at(0) << endl;
	if(arguments.at(0).compare("exit") == 0) {
		//Exit is a special command
		return new Exit(parent);
	} else {
		return new Command(arguments);
	}
}
