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
		Operation* createOperation(const vector<string>& parameters);
	public:
		Reader(Program *parent) : parent(parent) {
			
		}
		bool read(const string& statement);
};

#endif
