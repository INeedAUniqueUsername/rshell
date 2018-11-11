#include "program.h"

#include<sstream>
using namespace std;
int main() {
	ostringstream s;
	Program *p = new Program(cin, cout, s);
	p->run();
}
