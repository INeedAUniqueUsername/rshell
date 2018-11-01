#include <string>
#include <unistd.h>

using namespsace std;

class Operation
{
	protected:
	public:
		Operation() {};

		virtual bool execute() = 0;
};

class Command : public Operation
{
	protected:
		string command;
	public:
		Command() : Operation() {};

		Command(string command) : Operation()
		{
			
		}

		bool execute() {
		}
};

class Exit : public Operation {
	private:
		Program *parent;
	public:
		Exit(Program *parent) : parent(parent) {
			
		}
		bool execute() {
			parent->close();
		}
}
