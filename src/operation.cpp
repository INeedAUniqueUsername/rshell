#include "operation.h"
bool Exit::execute(int pipeIn[], int pipeOut[]) {
	parent->close();
}
