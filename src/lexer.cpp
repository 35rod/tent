#include "lexer.hpp"
#include "errors.hpp"

void Lexer::nextChar(int num) {
	curPos += num;

	if (curPos >= source.length()) {
		curChar = '\0';
	} else {
		curChar = source[curPos];
	}
}

char Lexer::peek() {
	if (curPos+1 >= source.length()) {
		return '\0';
	}

	return source[curPos+1];
}

void Lexer::skipWhitespace() {
	while (curChar == ' ' || curChar == '\t' || curChar == '\r') {
		nextChar();
	}
}

void Lexer::skipComment() {
	if (curChar == '`') {
		while (curChar != '\n') {
			nextChar();
		}
	}
}

#include "is_digit_incl.cpp"

Token Lexer::getToken() {
	skipWhitespace();
	skipComment();

	Token token("", "", lineNo);

	if (curChar == '+') {
		if (peek() == '+') {
			nextChar();
			token = Token("++", "INCREMENT", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("+=", "ADD_ASSIGN", lineNo);
		} else {
			token = Token("+", "ADD", lineNo);
		}
	} else if (curChar == '-') {
		if (peek() == '-') {
			nextChar();
			token = Token("--", "DECREMENT", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("-=", "SUB_ASSIGN", lineNo);
		} else {
			token = Token("-", "SUB", lineNo);
		}
	} else if (curChar == '*') {
		if (peek() == '*') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("**=", "POW_ASSIGN", lineNo);
			} else
				token = Token("**", "POW", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("*=", "MUL_ASSIGN", lineNo);
		} else {
			token = Token("*", "MUL", lineNo);
		}
	} else if (curChar == '/') {
		if (peek() == '/') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("//=", "FLOOR_DIV_ASSIGN", lineNo);
			} else
				token = Token("//", "FLOOR_DIV", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("/=", "DIV_ASSIGN", lineNo);
		} else {
			token = Token("/", "DIV", lineNo);
		}
	} else if (curChar == '%') {
		if (peek() == '=') {
			nextChar();
			token = Token("%=", "MOD_ASSIGN", lineNo);
		} else
			token = Token("%", "MOD", lineNo);
	} else if (curChar == '&') {
		if (peek() == '&') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("&&=", "AND_ASSIGN", lineNo);
			} else
				token = Token("&&", "AND", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("&=", "BIT_AND_ASSIGN", lineNo);
		} else
			token = Token("&", "BIT_AND", lineNo);
	} else if (curChar == '@') {
		token = Token("@", "INDEX", lineNo);
	} else if (curChar == '^') {
		if (peek() == '=') {
			nextChar();
			token = Token("^=", "BIT_XOR_ASSIGN", lineNo);
		} else
			token = Token("^", "BIT_XOR", lineNo);
	} else if (curChar == '|') {
		if (peek() == '|') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("||=", "OR_ASSIGN", lineNo);
			} else
				token = Token("||", "OR", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("|=", "BIT_OR_ASSIGN", lineNo);
		} else
			token = Token("|", "BIT_OR", lineNo);
	} else if (curChar == '<') {
		char peekChar = peek();
		if (peekChar == '<') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("<<=", "LSHIFT_ASSIGN", lineNo);
			} else
				token = Token("<<", "LSHIFT", lineNo);
		} else if (peekChar == '=') {
			nextChar();
			token = Token("<=", "LESSEQ", lineNo);
		} else
			token = Token("<", "LESS", lineNo);
	} else if (curChar == '>') {
		char peekChar = peek();
		if (peekChar == '>') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token(">>=", "RSHIFT_ASSIGN", lineNo);
			} else
				token = Token(">>", "RSHIFT", lineNo);
		} else if (peekChar == '=') {
			nextChar();
			token = Token(">=", "GREATEREQ", lineNo);
		} else
			token = Token(">", "GREATER", lineNo);
	} else if (curChar == '(') {
		token = Token("(", "OPEN_PAREN", lineNo);
	} else if (curChar == ')') {
		token = Token(")", "CLOSE_PAREN", lineNo);
	} else if (curChar == '{') {
		token = Token("{", "OPEN_BRAC", lineNo);
	} else if (curChar == '}') {
		token = Token("}", "CLOSE_BRAC", lineNo);
	} else if (curChar == ',') {
		token = Token(",", "COMMA", lineNo);
	} else if (curChar == '[') {
		token = Token("[", "OPEN_BRACKET", lineNo);
	} else if (curChar == ']') {
		token = Token("]", "CLOSE_BRACKET", lineNo);
	} else if (curChar == '!') {
		if (peek() == '!') {
			nextChar();
			token = Token("!!", "BIT_NOT", lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("!=", "NOTEQ", lineNo);
		} else
			token = Token("!", "NOT", lineNo);
	} else if (curChar == '=') {
		if (peek() == '=') {
			nextChar();
			token = Token("==", "EQEQ", lineNo);
		} else {
			token = Token("=", "ASSIGN", lineNo);
		}
	} else if (curChar == '\"') {
		nextChar();

		int startPos = curPos;

		while (curChar != '\"' && curChar != '\0')
			nextChar();
		if (curChar == '\0')
			MissingTerminatorError("Unterminated string literal", lineNo);

		token = Token(source.substr(startPos, curPos-startPos), "STR", lineNo);
	} else if (curChar == '\'') {
		nextChar();

		int startPos = curPos;

		while (curChar != '\'' && curChar != '\0')
			nextChar();
		if (curChar == '\0')
			MissingTerminatorError("Unterminated string literal", lineNo);

		if (curPos - startPos == 1)
			token = Token(source.substr(startPos, 1), "CHR", lineNo);
		else 
			token = Token(source.substr(startPos, curPos-startPos), "STR", lineNo);
	} else if (isalpha(curChar) || curChar == '_') {
		int startPos = curPos;

		char curChar = 0;
		while (isalnum(curChar = peek()) || curChar == '_') {
			nextChar();
		}

		std::string text = source.substr(startPos, curPos-startPos+1);

		std::string kind;

		if (text == "load") {
			kind = "LOAD";
		} else if (text == "set") {
			kind = "SET";
		} else if (text == "form") {
			kind = "FORM";
		} else if (text == "return") {
			kind = "RETURN";
		} else if (text == "if") {
			kind = "IF";
		} else if (text == "else") {
			kind = "ELSE";
		} else if (text == "while") {
			kind = "WHILE";
		} else if (text == "break") {
			kind = "BREAK";
		} else if (text == "continue") {
			kind = "CONTINUE";
		} else if (text == "true" || text == "false") {
			kind = "BOOL";
		} else {
			kind = "IDENT";
		}

		token = Token(text, kind, lineNo);
	} else if (is_hex_digit(curChar)) {
		bool (*is_digit_func) (char) = is_dec_digit;
		if (curChar == '0') {
			char peekChar = peek();
			switch (peekChar) {
			case 'x':
				is_digit_func = is_hex_digit;
				nextChar(2);
				break;
			case 'd':
				nextChar(2);
				break;
			case 'o':
				is_digit_func = is_oct_digit;
				nextChar(2);
				break;
			case 'b':
				is_digit_func = is_bin_digit;
				nextChar(2);
				break;
			default:
				if (isalnum(peekChar))
					SyntaxError("Illegal integer literal radix specifier: " + std::string(curChar, 1) + std::string(peekChar, 1), lineNo);
			}
		}

		int startPos = curPos;
		
		while (is_digit_func(peek())) {
			nextChar();
		}

		if (peek() == '.') {
			if (is_digit_func != is_dec_digit) {
				SyntaxError("Floating-point literals with specified radixes are not supported.", lineNo);
			}
			nextChar();

			while (isdigit(peek())) {
				nextChar();
			}

			token = Token(source.substr(startPos, curPos+1), "FLOAT", lineNo);
		} else {
			std::string intlit_type = "INT_DEC";
			if (is_digit_func == is_hex_digit)
				intlit_type = "INT_HEX";
			else if (is_digit_func == is_oct_digit)
				intlit_type = "INT_OCT";
			else if (is_digit_func == is_bin_digit)
				intlit_type = "INT_BIN";
			token = Token(source.substr(startPos, curPos-startPos+1), intlit_type, lineNo);
		}
	} else if (curChar == ';') {
		token = Token(";", "SEM", lineNo);
	} else if (curChar == '\n') {
		lineNo++;

		token = Token("\\n", "NEWLINE", lineNo);
	} else if (curChar == '\0') {
		token = Token("", "EOF", lineNo);
	}

	nextChar();

	return token;
}

void Lexer::getTokens() {
	while (true) {
		Token token = getToken();

		if (token.kind != "NEWLINE" && token.kind != "EOF") {
			tokens.push_back(token);
		} else {
			if (token.kind == "EOF") {
				break;
			}
		}
	}
}

Lexer::Lexer(std::string input) : source(input) {
	curPos = -1;
	lineNo = 1;
}
