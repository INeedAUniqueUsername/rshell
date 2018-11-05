#include "program.h"

int main() {
	Program *p = new Program(cin, cout, cerr);
	p->run();
}
