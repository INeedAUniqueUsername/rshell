#include<string>
#include<vector>
#include<cctype>
#include<stdexcept>
using namespace std;

#ifndef __READER_H__
#define __READER_H__

class Operation;

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
			} else if(LineContainsHere("\\")) {
				return Token("\\", TokenTypes::CharBackslash);
			} else if(LineContainsHere("\"")) {
				return Token("\"", TokenTypes::CharQuote);
			} else if(LineContainsHere(" ")) {
				return Token(" ", TokenTypes::CharSpace);
			} else if(isalpha(line.at(index))) {
				string result = "";
				result.push_back(line.at(index));
				index++;
				char c = 0;
				while(index < line.size() &&
						isalpha((c = line.at(index)))
						) {
					index++;
					result.push_back(c);
				}
				return Token(result, TokenTypes::StringArg);
			}
			throw new invalid_argument("Unknown token");
		}
		
		//General parsing method
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
		//Creates a sub test chain upon reaching an opening bracket.
		Operation* ParseChain();
		
		//Subfunction of ParseChain and ParseTestChain
		//Parses a single connector and returns its type
		Connector* ParseConnector();
		
		//Subfunction of Parse
		//Creates a Test chain of Commands and Connectors within brackets
		//Ignores spaces
		//Terminates upon reaching a Closing Bracket or End.
		//Termination advances the index past the Closing Bracket.
		//Creates an error upon reaching a Closing Parenthesis.
		//Creates an error upon reaching too many Connectors.
		//Creates a sub command upon reaching a string argument.
		//Creates a sub connector upon reaching a Connector.
		//Creates a sub chain upon reaching an opening parenthesis.
		//Creates a sub test chain upon reaching an Opening Bracket.
		Operation* ParseTestChain();
	public:
		Reader(Program *parent, string line) : parent(parent), index(0), line(line) { }
		
		//Parse the line to create an Operation
		Operation* parseLine();
};

#endif
