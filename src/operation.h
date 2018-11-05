#include <string>
#include <unistd.h>
using namespace std;

#ifndef __OPERATION_H__
#define __OPERATION_H__

class Program;
#include "program.h"

class Connector {
	public:
		virtual bool status(bool result) = 0;
};
class Success : public Connector {
	
	public:
		Success() {}
		bool status(bool result) {
			return result;
		}
};
class Failure : public Connector {
	public:
		Failure() {}
		bool status(bool result) {
			return !result;
		}
};
class Operation {
	public:
		virtual bool execute() = 0;
};
class Chain : public Operation {
	protected:
		vector<Operation*> operations;
		vector<Connector*> connectors;
	public:
		Chain(vector<Operation*> operations, vector<Connector*> connectors) : operations(operations), connectors(connectors) {}
		bool execute() {
			
		}
};

class Command : public Operation {
	protected:
		vector<string> parameters;
	public:
		Command(vector<string> parameters) : parameters(parameters) { }

		bool execute() {
			pid_t pid;
			char *const parmList[] = 
			{(char*)"-l",(char*) "-a", NULL};
			
			if ((pid = fork()) == -1) {
				perror("fork() error");
			}
			else if (pid == 0) {
				execvp("ls", parmList);
				printf("Return not expected. Must be an execvp() error.\\n");
			}
		}
};

class Exit : public Operation {
	private:
		Program *parent;
	public:
		Exit(Program *parent) : parent(parent) {}
		bool execute();
};

#endif
