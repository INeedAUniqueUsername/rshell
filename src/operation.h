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
		virtual void print(ostream& out) = 0;
};
class Success : public Connector {
	
	public:
		Success() {}
		bool status(bool result) {
			return result;
		}
		void print(ostream& out) {
			out << "&&";
		}
};
class Failure : public Connector {
	public:
		Failure() {}
		bool status(bool result) {
			return !result;
		}
		void print(ostream& out) {
			out << "||";
		}
};
class Operation {
	public:
		virtual bool execute() = 0;
		virtual void print(ostream& out) = 0;
};
class Chain : public Operation {
	protected:
		vector<Operation*> operations;
		vector<Connector*> connectors;
		
		//We assume that we always have one less Connector than our Operations
	public:
		Chain(vector<Operation*> operations, vector<Connector*> connectors) : operations(operations), connectors(connectors) {}
		bool execute() {
			bool result = true;
			//Alternate between sub-operations and connectors in sequence
			for(unsigned i = 0; i < operations.size() - 1; i++) {
				result = operations.at(i)->execute();
				//If connector says to stop, then return the result of the latest operation
				if(!connectors.at(i)->status(result)) {
					return result;
				}
			}
			//Otherwise, return the result of the last Operation
			return operations.at(operations.size() - 1)->execute();
		}
		void print(ostream& out) {
			//Print out Operations and Connectors
			for(unsigned i = 0; i < operations.size() - 1; i++) {
				operations.at(i)->print(out);
				out << " ";
				connectors.at(i)->print(out);
				out << " ";
			}
			operations.at(operations.size() - 1)->print(out);
		}
};

class Command : public Operation {
	protected:
		vector<string> arguments;
	public:
		Command(vector<string> arguments) : arguments(arguments) { }

		bool execute() {
			/*
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
			*/
		}
		void print(ostream& out) {
			//Print out our arguments
			for(unsigned i = 0; i < arguments.size() - 1; i++) {
				out << arguments.at(i);
			}
			out << arguments.at(arguments.size() - 1);
		}
};

class Exit : public Operation {
	private:
		Program *parent;
	public:
		Exit(Program *parent) : parent(parent) {}
		bool execute();
		void print(ostream& out) {
			out << "Exit";
		}
};

#endif
