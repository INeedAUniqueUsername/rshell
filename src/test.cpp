#include "operation.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;


int main() {
	vector<string> t;
	t.push_back("ls");
	t.push_back("-a");
	Command test(t);
	test.execute();
	test.print(cout);
	return 0;
}
