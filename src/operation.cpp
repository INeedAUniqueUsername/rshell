#include "operation.h"
bool Exit::execute() {
	parent->close();
}
