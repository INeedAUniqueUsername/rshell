#include "../src/operation.h"
#include "../src/program.h"
#include "../src/reader.h"

#include "gtest/gtest.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

//TO DO: Make old tests compatible with current implementation
//TO DO: Add comment tests (note changes in source)
TEST(CommandTest, EmptyString) {
	vector<string> t;
	Command test(t);
	
	EXPECT_EQ(false, test.execute());	
}
TEST(CommandTest, SuccessTest) {
	vector<string> t;
	t.push_back("ls");
	t.push_back("-a");
	Command test(t);
	
	EXPECT_EQ(true, test.execute());
}
TEST(CommandTest, NotBINProgram) {
	vector<string> t;
	t.push_back("test");
	
	Command test(t);
	
	EXPECT_EQ(false, test.execute());
		
	
}
TEST(CommandTest, IncompatibleArguments) {
	vector<string> t;
	t.push_back("ls");
	t.push_back("@");
	
	Command test(t);

	EXPECT_EQ(false, test.execute());
	

}

TEST(CommandTest, ExecuteTest) {
	vector<string> t;
	t.push_back("echo");
	t.push_back("hello world");

	Command test(t);

	testing::internal::CaptureStdout();
	test.execute();
	string output = testing::internal::GetCapturedStdout();

	EXPECT_EQ("hello world\n", output);
}
TEST(CommandTest, SourceTest) {
	Program* p = new Program();
	string line1 = "ls -a && exit || echo aaa";
	Reader parser1(p, line1);
	
	ostringstream s;	
	Operation* op = parser1.ParseLine();
	
	//op = parser.Parse("ls -a && exit || echo aaa");
	op->print(s);
	EXPECT_EQ(s.str(), "(ls -a) && exit || (echo aaa)");
	s.str("");
	
	string line2 = "ls      -a        &&     exit     || aaa";
	//op = parser.Parse("ls      -a        &&     exit     || aaa");
	Reader parser2(p, line2);

	op = parser2.ParseLine();
	op->print(s);
	EXPECT_EQ(s.str(), "(ls -a) && exit || (aaa)");
	s.str("");
	
	delete p;
	delete op;
}
TEST(ChainTest, SuccessEchoTest) {
	vector<Operation*> operations;
	vector<Connector*> connectors;

	vector<string> arguments0;
	vector<string> arguments1;
	

	arguments0.push_back("echo");
	arguments0.push_back("t0");
	arguments1.push_back("echo");
	arguments1.push_back("t1");
	
	Operation* t0 = new Command(arguments0);
	Operation* t1 = new Command(arguments1);

	Connector* c0 = new Success();

	operations.push_back(t0);
	operations.push_back(t1);

	connectors.push_back(c0);	
	
	Operation *op = new Chain(operations, connectors);	
	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("t0\nt1\n", output);
	
	delete op;
}
TEST(ChainTest, FailEchoTest) {
	vector<Operation*> operations;
	vector<Connector*> connectors;

	vector<string> arguments0;
	vector<string> arguments1;
	
	arguments1.push_back("echo");
	arguments1.push_back("t1");

	Connector* c0 = new Success();
	
	Operation* t0 = new Command(arguments0);
	Operation* t1 = new Command(arguments1);

	operations.push_back(t0);
	operations.push_back(t1);

	connectors.push_back(c0);

	Operation *op = new Chain(operations, connectors);
	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();

	
	EXPECT_EQ("", output);

	delete op;
}

TEST(ChainTest, SourceTest) {
	Program* p = new Program();
	string line1 = "ls -a      &&     ls";
	Reader parser1(p, line1);
	
	ostringstream s;	
	Operation* op;
	
	//op = parser.Parse("ls -a      &&     ls");
	op = parser1.ParseLine();
	op->print(s);
	EXPECT_EQ(s.str(), "(ls -a) && (ls)");
	s.str("");
	
	string line2 = "            ls -a             ";
	Reader parser2(p, line2);
	//op = parser.Parse("            ls -a             ");
	op = parser2.ParseLine();
	op->print(s);
	EXPECT_EQ(s.str(), "(ls -a)");
	s.str("");
	
	string line3 = "                    ls             -a";
	Reader parser3(p, line3);
	//op = parser.Parse("                    ls             -a");
	op = parser3.ParseLine();
	op->print(s);
	EXPECT_EQ(s.str(), "(ls -a)");
	s.str("");
	
	delete p;
	delete op;
}

TEST(PrecendenceTest, EchoTest) {
	//TO DO: Implement these
	//(echo a && (echo b || (echo c || (echo d)))) -> a\nb\n
	//echo A && echo B || echo C && echo D -> A\nB\nD\n
	//(echo A && echo B) || (echo C && echo D) -> A\nB\n

	Program* p = new Program();
	string line = "(echo a && (echo b || (echo c || (echo d))))";
	Reader parser1(p, line);
	Operation* op;

	op = parser1.ParseLine();
	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("a\nb\n", output);

	line = "echo A && echo B || echo C && echo D";
	Reader parser2(p, line);
	op = parser2.ParseLine();
	testing::internal::CaptureStdout();
	op->execute();
	output = testing::internal::GetCapturedStdout();

	EXPECT_EQ("A\nB\nD\n", output);

	line = "(echo A && echo B) || (echo C && echo D)";
	Reader parser3(p, line);

	op = parser3.ParseLine();
	testing::internal::CaptureStdout();
	op->execute();
	output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("A\nB\n", output);
	delete p;
	delete op;
}
TEST(TestCommandTest, PrintTest) {
	
	string flag = "-e";
	string arg = "/src/operation.h";
	TestCommand* op = new TestCommand(flag, arg);
	//Print the source
	testing::internal::CaptureStdout();
	op->print(cout);
	string output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("[ -e /src/operation.h ]", output);
	delete op;
}
TEST(TestCommandTest, TrueExecuteTest) {
	
	string flag = "-e";
	string arg = ".";
	TestCommand* op = new TestCommand(flag, arg);
	//Print the source
	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("(True)\n", output);
	delete op;
}
TEST(TestCommandTest, FalseExecuteTest) {
	
	string flag = "-e";
	string arg = "zzz";
	TestCommand* op = new TestCommand(flag, arg);
	//Print the source
	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();
	
	EXPECT_EQ("(False)\n", output);
	delete op;
}
TEST(PipeTest, lsTest) {
	vector<string> t1;
	t1.push_back("ls");
	t1.push_back("-l");

	Command* test1 = new Command(t1);

	vector<string> t2;
	t2.push_back("wc");
	t2.push_back("-l");

	Command* test2 = new Command(t2);

	PipeOperation* op = new PipeOperation(test1, test2);

	testing::internal::CaptureStdout();
	op->execute();
	string output = testing::internal::GetCapturedStdout();

	EXPECT_EQ("18\n", output);
	//delete test1;
	//delete test2;
	delete op;
}
TEST(InputTest, InputFileTest) {
	vector<string> t1;
	t1.push_back("wc");
	t1.push_back("-m");
	string fileName = "./tests/testInput.txt";

	Command* op = new Command(t1);

	InputOperation* input = new InputOperation(op, fileName);

	testing::internal::CaptureStdout();
	input->execute();
	string output = testing::internal::GetCapturedStdout();

	EXPECT_EQ("4\n", output);
	delete op;
	//delete input;
}
TEST(OutputTest, OutputFileTest) {
	vector<string> t1;
	t1.push_back("echo");
	t1.push_back("test");
	string fileName = "./tests/testOutput1.txt";

	Command* op = new Command(t1);

	OutputOperation* output = new OutputOperation(false, op, fileName);

	output->execute();

	char c;
	string inputText;
	ifstream in(fileName);
	if (in.is_open()) {
		while (in >> c) {
			inputText += c;
		}
	}

	EXPECT_EQ("test", inputText);
	delete op;

}
int main (int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
