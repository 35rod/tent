#pragma once

#include <iostream>
#include <string>
#include "opcodes.hpp"
#include "span.hpp"

class Token {
	public:
		std::string text;
		TokenType kind;
		Span span;

		void print();

		Token(std::string tokenText, TokenType tokenKind, Span s);
};
