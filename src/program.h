#include<cstdlib>
#include<iostream>
#include<limits.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<boost/tokenizer.hpp>
#include<string>
#include<sstream>
using namespace std;

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "reader.h"
#include "operation.h"

class Program {
	friend Reader;
	private:
		//Customizable streams for debugging
		istream& in;
		ostream& out;
		ostream& dbg;
		
		void printInfo() {
			//Get the login. If we fail, then don't provide info
			char *login = getlogin();
			if(login) {
				out << '[';
				out << login;
				
				//Now get the hostname. If we fail, then don't provide info
				char *hostname = new char[HOST_NAME_MAX];
				if(!gethostname(hostname, HOST_NAME_MAX)) {
					out << "@";
					out << hostname;
					
					/*
					//Don't print cwd since it's not convenient on a small display
					
					//Now get the current working directory.
					char *cwd = new char[PATH_MAX];
					if(getcwd(cwd, PATH_MAX)) {
						out << " " << cwd;
						
						//If we got all the way here, then we have printed the info
					}
					delete cwd;
					*/
				}
				delete hostname;
				
				out << ']';
			}
			//delete login;		//For some reason, this always crashes the program
		}
	public:
		Program() : in(cin), out(cout), dbg(cerr) {}
		Program(istream& in, ostream& out, ostream& dbg) : in(in), out(out), dbg(dbg) {}
		void run();
		void error(const string& message) {
			out << message << endl;
		}
		void close() {
			//out << "rshell is exiting..." << endl;
			//End the program 
			exit(0);
		}
};

#endif
