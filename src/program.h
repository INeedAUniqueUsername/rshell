#include<cstdlib>
#include<iostream>
#include<limits.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<boost/tokenizer.hpp>
#include<string>
using namespace std;

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "reader.h"
#include "operation.h"

class Program {
	friend Reader;
	private:
		bool active;
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
					
					//Now get the current working directory.
					char *cwd = new char[PATH_MAX];
					if(getcwd(cwd, PATH_MAX)) {
						out << " " << cwd;
						
						//If we got all the way here, then we have printed the info
					}
					delete cwd;
				}
				delete hostname;
				
				out << ']';
			}
			//delete login;		//For some reason, this always crashes the program
		}
	public:
		Program(istream& in, ostream& out, ostream& dbg) : in(in), out(out), dbg(dbg) {
			
		}
		void run();
		void close() {
			//End the program 
		}
};

#endif
