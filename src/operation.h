#include <string>
#include <unistd.h>
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
char *convert(const string &s) { //Function to convert string to return char pointer
	char *pc = new char[s.size()+1];
	strcpy(pc, s.c_str());
	return pc;
}

class Command : public Operation {
	protected:
		vector<string> arguments;
	public:
		Command(vector<string> arguments) : arguments(arguments) { }

		bool execute() { //Executes the first argument in arguments as the program and the rest as paramaters
			pid_t pid;
				
			char* parmList[arguments.size()];
			for (int i = 1; i < arguments.size(); i++) {
				parmList[i-1] = (char*) arguments[i].c_str();
			}
			parmList[arguments.size()-1] = NULL;
	
			if ((pid == fork()) < 0) {
				perror("fork() error");				
			}
			else if (pid == 0) {
				execvp(arguments[0].c_str(), const_cast<char**>(parmList));
				//Create a Failure object here
				printf("Return not expected. Must be an execvp() error. \\n");
				
				/*execvp(arguments[0].c_str(), parmList);
				printf("Return not expected. Must be an execvp() error.\\n");*/
			}	
			/*
			pid_t pid;
			//char parmList[arguments.size() + 1]; //Declare parmList array to be used with execvp
			vector <const char*> vc;
			transform(arguments.begin(), arguments.end(), back_inserter(vc), convert); //Performs convert on arguments and fill vc using a back_inserter iterator
			if ((pid == fork()) == -1) {
				perror("fork() error");
			}
			else if (pid == 0) {
				execvp(vc[0], const_cast<char**>(&vc[1])); //vc[0] is the program to be executed, 2nd argument casts to use vc as array of char pointers 
				printf("Return not expected. Must be an execvp() error.\\n"); //Should not get print if execvp is successful
			}*/
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
