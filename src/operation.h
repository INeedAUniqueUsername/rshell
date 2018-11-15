#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
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
	private:
		bool executeCommand() {
			char* parmList[arguments.size() + 1];
			for (int i = 0; i < arguments.size(); i++) {
				parmList[i] = (char*) arguments[i].c_str();
			}
			parmList[arguments.size()] = NULL;
			return execvp(parmList[0], const_cast<char**>(parmList)) == 0;
		}
	protected:
		vector<string> arguments;
	public:
		Command(vector<string> arguments) : arguments(arguments) { }

		bool execute() { //Executes the first argument in arguments as the program and the rest as paramaters
			pid_t pid;
	
			if ((pid = fork()) < 0) {
				perror("fork() error");
			} else if (pid == 0) {
				if(executeCommand()) {
					_exit(0);
					return true;
				} else {
					perror("execvp error");
					_exit(-1);
					return false;
				}
			} else if(pid > 0) {
				int status;
				waitpid(pid, &status, 0);
				if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
					return true;
				} else {
					return false;
				}
			}
		}
		void print(ostream& out) {
			//Print out our arguments
			for(unsigned i = 0; i < arguments.size() - 1; i++) {
				out << arguments.at(i) << " ";
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
			out << "exit";
		}
};

#endif
