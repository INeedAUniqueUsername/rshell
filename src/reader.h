#include<string>
#include<vector>
using namespace std;

#ifndef __READER_H__
#define __READER_H__

class Operation;

#include "program.h"
#include "operation.h"

class Reader {
	private:
		Program *parent;
		
		//Create a Command given arguments
		Operation* createOperation(const vector<string>& arguments);
	public:
		Reader(Program *parent) : parent(parent) { }
		
		//Parses a line of statements and executes it, returning the result
		bool readLine(const string& line);
		
		//Parse a statement and execute it, returning the result
		bool read(const string& statement);
		
		//Parse a statement to create an Operation
		Operation* parse(const string& statement);
};

#endif
