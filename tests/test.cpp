#include "../src/operation.h"
#include "../src/program.h"
#include "../src/reader.h"

#include "gtest/gtest.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

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
	//test.print(cout);
	string output = testing::internal::GetCapturedStdout();
	
	//cout << output;

	EXPECT_EQ("hello world\n", output);
}
TEST(CommandTest, SourceTest) {
	Program* p = new Program();
	Reader parser(p);
	
	ostringstream s;	
	Operation* op;
	
	op = parser.parse("ls -a && exit || echo aaa");
	op->print(s);
	EXPECT_EQ(s.str(), "ls -a && exit || echo aaa");
	s.str("");
	
	op = parser.parse("ls      -a        &&     exit     || aaa");
	op->print(s);
	EXPECT_EQ(s.str(), "ls -a && exit || aaa");
	s.str("");
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
}
TEST(ChainTest, FailEchoTest) {
	vector<Operation*> operations;
	vector<Connector*> connectors;

	vector<string> arguments0;
	vector<string> arguments1;
	
	//arguments0.push_back("ff");
	//arguments0.push_back("t0");
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


}
TEST(ChainTest, SourceTest) {
	Program* p = new Program();
	Reader parser(p);
	
	ostringstream s;	
	Operation* op;
	
	op = parser.parse("ls -a      &&     ls");
	op->print(s);
	EXPECT_EQ(s.str(), "ls -a && ls");
	s.str("");
	
	op = parser.parse("            ls -a             ");
	op->print(s);
	EXPECT_EQ(s.str(), "ls -a");
	s.str("");
	
	op = parser.parse("                    ls             -a");
	op->print(s);
	EXPECT_EQ(s.str(), "ls -a");
	s.str("");
}

int main (int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
