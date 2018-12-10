#include<string>
#include<vector>
#include<cctype>
#include<stdexcept>
#include<sstream>
using namespace std;

#ifndef __READER_H__
#define __READER_H__

class Operation;
class Connector;

#include "program.h"
#include "operation.h"

enum TokenTypes {
	//Test Chains
	OpenBracket,
	CloseBracket,
	
	//Chains
	OpenParen,
	CloseParen,
	
	//Connectors
	ConnectorAnd,
	ConnectorOr,
	ConnectorSemicolon,
	
	Input,
	Pipe,
	OutputAppend,
	Output,
	
	//Arguments
	CharBackslash,
	CharQuote,
	CharSpace,
	StringArg,
	
	End
};
class Token {
	public:
		string value;
		TokenTypes type;
		Token(string value, TokenTypes type) : value(value), type(type) {}
};
class Reader {
	private:
		Program *parent;
		unsigned index = 0;
		string line;
		
		
		//Check the line at our current index to see if we have a certain substring
		bool LineContainsHere(const string& sub) {
			return line.substr(index, sub.length()) == sub;
		}
		//Increments the index for when we just accepted a token
		void UpdateIndex(const Token& t) {
			index += t.value.size();
		}
		
		bool IsArgChar(char c) {
			return isalpha(c) || c == '-' || c == '.' || c == '/';
		}
		void UnexpectedToken(Token t) {
			ostringstream s;
			s << "ERROR: Unexpected ";
			s << TokenName(t);
			throw invalid_argument(t.value);
		}
		string TokenName(Token t) {
			switch(t.type) {
				case TokenTypes::OpenBracket:
					return "open bracket";
				case TokenTypes::CloseBracket:	
					return "close bracket";
				case TokenTypes::OpenParen:	
					return "open parenthesis";
				case TokenTypes::CloseParen:	
					return "close parenthesis";
				case TokenTypes::ConnectorAnd:	
					return "connector &&";
				case TokenTypes::ConnectorOr:	
					return "connector ||";
				case TokenTypes::ConnectorSemicolon:	
					return "connector ;";
				case TokenTypes::Input:
					return "input descriptor";
				case TokenTypes::Pipe:	
					return "pipe";
				case TokenTypes::Output:
					return "output descriptor";
				case TokenTypes::Output:
					return "output descriptor";
				case TokenTypes::CharBackslash:	
					return "backslash";
				case TokenTypes::CharQuote:	
					return "quote";
				case TokenTypes::CharSpace:	
					return "space";
				case TokenTypes::StringArg:	
					return "string";
				case TokenTypes::End:	
					return "end of line";
				default:
					return t.value;
			}
		}
		//Read the token at the current position
		Token Read() {
			if(index >= line.size()) {
				return Token("", TokenTypes::End);
			} else if(LineContainsHere("[")) {
				return Token("[", TokenTypes::OpenBracket);
			} else if(LineContainsHere("]")) {
				return Token("]", TokenTypes::CloseBracket);
			} else if(LineContainsHere("(")) {
				return Token("(", TokenTypes::OpenParen);
			} else if(LineContainsHere(")")) {
				return Token(")", TokenTypes::CloseParen);
			} else if(LineContainsHere("&&")) {
				return Token("&&", TokenTypes::ConnectorAnd);
			} else if(LineContainsHere("||")) {
				return Token("||", TokenTypes::ConnectorOr);
			} else if(LineContainsHere(";")) {
				return Token(";", TokenTypes::ConnectorSemicolon);
			} else if(LineStartsHere("<")) {
				return Token("<", TokenTypes::Input);
			} else if(LineContainsHere("|")) {
				return Token("|", TokenTypes::Pipe);
			} else if(LineContainsHere(">>")) {
				return Token(">>", TokenTypes::OutputAppend);
			} else if(LineContainsHere(">"))  {
				return Token(">", TokenTypes::Output);
			} else if(LineContainsHere("\\")) {
				return Token("\\", TokenTypes::CharBackslash);
			} else if(LineContainsHere("\"")) {
				return Token("\"", TokenTypes::CharQuote);
			} else if(LineContainsHere(" ")) {
				return Token(" ", TokenTypes::CharSpace);
			} else if(IsArgChar(line.at(index))) {
				//Otherwise, we take any non-space chars and combine them into a string argument
				string result = "";
				result.push_back(line.at(index));
				int index_end = index;
				index_end++;
				char c = 0;
				while(index_end < line.size() &&
						IsArgChar(c = line.at(index_end))) {
					index_end++;
					result.push_back(c);
				}
				return Token(result, TokenTypes::StringArg);
			}
			throw new invalid_argument("Unknown token");
		}
		
		//General parsing method
		//Entry point to parsing the line
		//We expect to parse a Command, Chain or a Test (depending on the first token)
		Operation* Parse();
		
		//Subfunction of Parse
		//Parse the arguments of a command.
		//Creates an Exit command if the first parameter is Exit.
		//Creates a Test command if the first parameter is Test.
		//Ignores spaces
		//Terminates when we reach a Connector, Closing Parenthesis, Closing Bracket, or End.
		//Termination does not advance the index.
		//Creates an error if it reaches an Opening Parenthesis or Opening Bracket.
		Operation* ParseCommand();
		
		//Subfunction of Parse
		//Parses a quoted or unquoted argument, removing the surrounding quotes
		//Consecutive non-separated arguments are treated as one: "aa" == "a"a == a"a" == "a""a" == "a""""a" == "a""""""a"
		//An argument is a series of consecutive characters and strings delimited by CharSpace or End
		string ParseArgument();
	
		//Subfunction of ParseArgument
		//Parses a series of characters delimited by CharQuote
		//Note that when this gets called, the index should be directly after the opening quote
		//The result does not include the surrounding CharQuotes
		string ParseQuoted();
	
		//Parse an escaped character preceded by backslash
		//When called, the index should be directly after the backslash
		char ParseBackslash();
		
		
		//Parses a test command enclosed in brackets
		//When called, the index should be after the opening bracket
		Operation* ParseTestBracket();
	
		//Subfunction of Parse
		//The default parsing method,
		//Creates a Chain of Commands and Connectors
		//Ignores spaces
		//Terminates upon reaching an end of line or a Closing Parenthesis.
		//Termination advances the index past the Closing Parenthesis.
		//Creates an error upon reaching a Closing Bracket.
		//Creates an error upon reaching too many Connectors.
		//Creates a sub command upon reaching a string argument.
		//Creates a sub connector upon reaching a Connector.
		//Creates a sub chain upon reaching an opening parenthesis.
		//Creates a sub test upon reaching an opening bracket.
		Operation* ParseChain();
		
		//Subfunction of ParseChain
		//Parses a single connector and returns its type
		Connector* ParseConnector();
	public:
		Reader(Program *parent, const string& line) : parent(parent), index(0), line(line) {
			//Delete everything past the comment
			unsigned comment = this->line.find('#', 0);
			if(comment != string::npos) {
				this->line = line.substr(0, comment);
			}
			
			//We enclose the actual line in parentheses to make it a properly enclosed chain
			ostringstream result;
			result << "(" << line << ")";
			this->line = result.str();
		}
		
		
		//Parse the line to create an Operation
		Operation* ParseLine();
};

#endif
