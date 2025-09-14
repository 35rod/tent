#pragma once

#include <iostream>
#include <string>
#include "opcodes.hpp"

class Token {
	public:
		std::string text;
		TokenType kind;
		int lineNo;

		void print();
		
		Token(std::string tokenText, TokenType tokenKind, int line);
};
