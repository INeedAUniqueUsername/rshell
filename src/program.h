#include<cstdlib>
#include<iostream>
#include<limits.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
using namespace std;

class Reader;

char* getline_c() {
	string line;
	getline(cin, line);
	char *result = new char[line.size()];
	const char* line_c = line.c_str();
	strcpy(result, line_c);
}
class Program {
	private:
		bool active;
	public:
		Program() {
			
		}
		void run() {
			
			string info = "";
			{
				//Get the login. If we fail, then don't provide info
				char *login = getlogin();
				if(!login) {
					login = (char*)"";
				}
				
				//Get the hostname. If we fail, then don't provide info
				char *hostname = new char[HOST_NAME_MAX];
				if(gethostname(hostname, HOST_NAME_MAX)) {
					delete hostname;
					hostname = (char*)"";
				}
				if(login && hostname) {
					info += login;
					info += "@";
					info += hostname;
				}
			}
			
			//Run until we get a close() call
			while(true) {
				if(info) {
					cout << "[" << info;
					char *cwd = new char[PATH_MAX];
					if(getcwd(cwd, PATH_MAX)) {
						cout << " " << cwd;
					}
					cout << "]";
					delete cwd;
				}
				cout << "$ ";
				
				string line;
				cin >> line;
				//Delete everything past the comment
				line = line.substr(0, line.find("#", 0));
				cout << "Actual: " << line << endl;
			}
			
		}
		void close() {
			//End the program 
		}
};

class Reader {
	private:
		Program *parent;
	public:
		Reader(Program *parent) : parent(parent) {
			
		}
		void read(string statement) {
			
		}
};
