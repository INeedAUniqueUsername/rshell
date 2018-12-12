#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
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
		virtual bool execute(int pipeIn[] = 0, int pipeOut[] = 0) = 0;
		virtual void print(ostream& out) = 0;
	protected:
		void closeIn(int pipeIn[] = 0) {
			if(pipeIn) {
				close(pipeIn[0]);
				close(pipeIn[1]);
			}
		}
		void handlePipes(int pipeIn[] = 0, int pipeOut[] = 0) {
			/*
			if(pipeIn)
				cout << "pipeIn" << endl;
			if(pipeOut)
				cout << "pipeOut" << endl;
				*/
			
			
			if(pipeIn != 0 && pipeOut != 0) {
				//This should not happen
				if(pipeIn == pipeOut) {
					//input from pipeIn
					dup2(pipeIn[0], 0);
					//output to pipeOut
					dup2(pipeOut[1], 1);
					//close the pipe
					close(pipeIn[0]);
					close(pipeOut[1]);
				} else {
					//input from pipeIn
					dup2(pipeIn[0], 0);
					close(pipeIn[0]);
					close(pipeIn[1]);
					
					//output to pipeOut
					dup2(pipeOut[1], 1);
					close(pipeOut[0]);
					close(pipeOut[1]);
				}
			} else if(pipeIn != 0) {
				//input from pipeIn
				dup2(pipeIn[0], 0);
				close(pipeIn[0]);
				close(pipeIn[1]);
				
			} else if(pipeOut != 0) {
				//output to pipeOut
				dup2(pipeOut[1], 1);
				close(pipeOut[0]);
				close(pipeOut[1]);
				
			} else {
				
			}
		}
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) {
			//First operation always executes
			bool result = operations.at(0)->execute(pipeIn, pipeOut);
			
			for(unsigned i = 1; i < operations.size(); i++) {
				//We execute this command if the previous result satisfies the connector
				if(connectors.at(i - 1)->status(result)) {
					result = operations.at(i)->execute(pipeIn, pipeOut);
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) {
			//TO DO: Setup the input
			char inputChar;
			ifstream in;
			in.open(file);

			int pipeFile[2];

			if (pipe(pipeFile) == -1) {
				perror("pipe error");
				exit(-1);
				in.close();
				return false;
			}
			
			//Send everything to pipe-out
			//Don't close pipe-in since the user will dup and close it for us
			if (in.is_open()) {
				char c;
				while(in >> c) {
					write(pipeFile[1], &c, sizeof(inputChar));
				}
				//Done sending to input, so close
				close(pipeFile[1]);
				in.close();
			} else {
				perror("unable to open file");
				return false;
			}

			//Execute the operation itself
			bool result = source->execute(pipeFile, pipeOut);
			
			//Done with the pipes
			close(pipeFile[0]);
			close(pipeFile[1]);
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) {
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) {
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) {
			/*
			0 = stdin
			1 = stdout
			2 = stderr

			writer output -> reader input
			*/

			int pipeMid[2];

			if (pipe(pipeMid) == -1) {
				perror("pipe error");
				exit(-1);
				return false;
			}
			
			writer->execute(pipeIn, pipeMid);
			
			//Somehow this does not terminate
			reader->execute(pipeMid, pipeOut);
			
			close(pipeMid[0]);
			close(pipeMid[1]);
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

		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) { //Executes the first argument in arguments as the program and the rest as paramaters
			pid_t pid;
	
			if ((pid = fork()) < 0) {
				perror("fork() error");
			} else if (pid == 0) {
				
				handlePipes(pipeIn, pipeOut);
				
				if(executeCommand()) {
					_exit(0);
					return true;
				} else {
					perror("execvp error");
					_exit(-1);
					return false;
				}
			} else if(pid > 0) {
				closeIn(pipeIn);
				
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
		bool execute(int pipeIn[] = 0, int pipeOut[] = 0);
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

		bool executeCommand() { //Returns true if the file exists (stat() == 0)
			struct stat buf;
			bool result = false;
			if (flag == "-e") {
				result = (stat(arg.c_str(), &buf) == 0);
			} else if (flag == "-f") {
				stat(arg.c_str(), &buf);
				result = (buf.st_mode & S_IFREG);
			} else if (flag == "-d") {
				stat(arg.c_str(), &buf);
				result = (buf.st_mode & S_IFDIR);
			}
			
			if(result) {
				cout << "(True)" << endl;
			} else {
				cout << "(False)" << endl;
			}
			return result;
		}

		bool execute(int pipeIn[] = 0, int pipeOut[] = 0) { //Executes the first argument in arguments as the program and the rest as paramaters
			pid_t pid;
	
			if ((pid = fork()) < 0) {
				perror("fork() error");
			} else if (pid == 0) {
				
				handlePipes(pipeIn, pipeOut);
				if(executeCommand()) {
					_exit(0);
					return true;
				} else {
					perror("execvp error");
					_exit(-1);
					return false;
				}
			} else if(pid > 0) {
				closeIn(pipeIn);
				
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
			//NOTE: The brackets must be one space away from the arguments
			out << "[ " << flag << " " << arg << " ]";
		}
};

#endif
