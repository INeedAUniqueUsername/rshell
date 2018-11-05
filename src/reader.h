#include<string>
#include<vector>
using namespace std;

#ifndef __READER_H__
#define __READER_H__

class Operation;

#include "program.h"
#include "operation.h"

class Reader {
	friend Program;
	private:
		Program *parent;
	public:
		Reader(Program *parent) : parent(parent) {
			
		}
		void read(const string& statement);
		Operation* createOperation(const vector<string>& parameters);
};

#endif
