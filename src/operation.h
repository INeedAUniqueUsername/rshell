#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <sys/stat.h>
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
class Any : public Connector {
	public:
		Any() {}
		bool status(bool result) {
			return true;
		}
		void print(ostream& out) {
			out << ";";
		}
};
class Operation {
	public:
		virtual bool execute() = 0;
		virtual void print(ostream& out) = 0;
};
/*
class Pair : public Operation {
	protected:
		Operation *op1, *op2;
		Connector* connector;
	public:
		Pair(Operation *op1, Operation *op2, Connector *connector) : op1(op1), op2(op2), connector(connector) {}
		bool execute() {
			bool result = op1->execute();
			if(connector->status(result)) {
				result = op2->execute();
			}
			return result;
		}
};
*/
class Chain : public Operation {
	protected:
		vector<Operation*> operations;
		vector<Connector*> connectors;
		
		//We assume that we always have one less Connector than our Operations
	public:
		Chain(vector<Operation*> operations, vector<Connector*> connectors) : operations(operations), connectors(connectors) {}
		~Chain() {
			for (int i = 0; i < operations.size(); i ++) {
				delete operations[i];
			}
			for (int i = 0; i < connectors.size(); i++) {
				delete connectors[i];
			}
			operations.clear();
			connectors.clear();	
		}
		bool execute() {
			//First operation always executes
			bool result = operations.at(0)->execute();
			
			for(unsigned i = 1; i < operations.size(); i++) {
				//We execute this command if the previous result satisfies the connector
				if(connectors.at(i - 1)->status(result)) {
					result = operations.at(i)->execute();
				}
			}
			return result;
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

class InputOperation : public Operation {
	private:
		Operation* source;
		string file;
	public:
		InputOperation(Operation* source, string file) : source(source), file(file) {}
		~InputOperation() {
			delete source;
		}
		bool execute() {
			//TO DO: Setup the input
			
			//Execute the operation itself
			bool result = source->execute();
			
			//TO DO: Finish the input
			
			
			return result;
		}
		void print(ostream& out) {
			source->print(out);
			out << " < " << file;
		}
};
class OutputOperation : public Operation {
	private:
		Operation* source;
		string file;
	public:
		OutputOperation(Operation* source, string file) : source(source), file(file) {}
		~OutputOperation() {
			delete source;
		}
		bool execute() {
			//TO DO: Setup the output (delete the file and create a new one)
			
			//Execute the operation itself
			bool result = source->execute();
			
			//TO DO: Finish the output
			
			
			return result;
		}
		void print(ostream& out) {
			source->print(out);
			out << " > " << file;
		}
};
class AppendOperation : public Operation {
	private:
		Operation* source;
		string file;
	public:
		AppendOperation(Operation* source, string file) : source(source), file(file) {}
		~AppendOperation() {
			delete source;
		}
		bool execute() {
			//TO DO: Setup the output (create the file if it does not exist and append to it)
			
			//Execute the operation itself
			bool result = source->execute();
			
			//TO DO: Finish the output
			
			
			return result;
		}
		void print(ostream& out) {
			source->print(out);
			out << " >> " << file;
		}
};
class PipeOperation : public Operation {
	private:
		Operation *writer, *reader;
		
	public:
		PipeOperation(Operation *writer, Operation *reader) : writer(writer), reader(reader) {}
		~PipeOperation() {
			delete writer;
			delete reader;
		}
		bool execute() {
			/*
			0 = stdin
			1 = stdout
			2 = stderr

			writer output -> reader input
			*/

			int fd[2];

			//pipe(fd);
			pid_t pid;

			if (pipe(fd) == -1) {
				perror("pipe error");
				exit(-1);
				return false;
			}

			pid = fork();

			if (pid == -1) {
				perror("fork() error");
				exit(-1);
				return false;
			} else if (pid == 0) { //Child
				if(close(STDIN_FILENO) == -1) {
					perror("pre-operation stdin close error reader");
					_exit(-1);
				}
				if(dup(fd[0]) == -1) {
					perror("pre-operation pipe-in dup error in reader");
					_exit(-1);
				}
				
				if(close(fd[1]) == -1) {
					perror("pre-operation pipe-out close error in reader");
					_exit(-1);
				}
				int result = 0;
				
				char buf;
				while(read(fd[0], &buf, 1) > 0) {
					write(STDOUT_FILENO, &buf, 1);
				}
				
				/*
				cout << "executing reader command" << endl;
				if(reader->execute()) {
					result = 0;
				} else {
					cout << "reader command failed" << endl;
					result = -1;
				}
				*/
				
				if(close(fd[0]) == -1) {
					perror("pre-operation pipe-in close error in reader");
					_exit(-1);	
				}
				_exit(result);
			} else { //Parent
			
				//Close stdout
				if(close(STDOUT_FILENO) == -1) {
					perror("pre-operation stdin close error in writer");
					return false;
				}
				
				//stdout <= pipe-out
				if(dup(fd[1]) == -1) {
					perror("pre-operation pipe-out dup error in writer");
					return false;
				}
				
				//close pipe-in
				if(close(fd[0]) == -1) {
					perror("pre-operation pipe-in close error in writer");
				}
				
				//cout << "executing writer command" << endl;
				if(writer->execute()) {
					
				} else {
					//cout << "writer command failed" << endl;
				}
			
				//close pipe-out
				if(close(fd[1]) == -1) {
					perror("pre-operation pipe-out close error in writer");
				}
			
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
			writer->print(out);
			out << " | ";
			reader->print(out);
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
			out << "(";
			for(unsigned i = 0; i < arguments.size() - 1; i++) {
				out << arguments.at(i) << " ";
			}
			out << arguments.at(arguments.size() - 1);
			out << ")";
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
class TestCommand : public Operation {
	protected:
		string flag, arg;
	public:
		TestCommand(const string& arg) : flag("-e"), arg(arg) {};
		TestCommand(const string& flag, const string& arg) : flag(flag), arg(arg) {};

		bool execute() { //Returns true if the file exists (stat() == 0)
			struct stat buf;
			if (flag == "-e") {
				if (stat(arg.c_str(), &buf) == 0) {
					cout << "(True)" << endl;
					return true;
				}
				else {
					cout << "(False)" << endl;
					return false;
				}
			}
			else if (flag == "-f") {
				stat(arg.c_str(), &buf);
				if (buf.st_mode & S_IFREG) { //if a file
					cout << "(True)" << endl;
					return true;
				} else {
					cout << "(False)" << endl;
					return false;
				} 
			}
			else if (flag == "-d") {
				stat(arg.c_str(), &buf);
				if (buf.st_mode & S_IFDIR) {
					cout << "(True)" << endl;
					return true;
				} else {
					cout << "(False)" << endl;
					return false;
				}
			}
			
		}

		void print(ostream& out) {
			//NOTE: The brackets must be one space away from the arguments
			out << "[ " << flag << " " << arg << " ]";
		}
};

#endif
