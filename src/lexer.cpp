#include "lexer.hpp"
#include "errors.hpp"
#include "opcodes.hpp"

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
	if (curChar == '~') {
		while (curChar != '\n') {
			nextChar();
		}
	}
}

#include "is_digit_incl.cpp"

Token Lexer::getToken() {
	skipWhitespace();
	skipComment();

	Token token("", TokenType::INVALID_TOKEN, lineNo);

	if (curChar == '+') {
		if (peek() == '+') {
			nextChar();
			token = Token("++", TokenType::INCREMENT, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("+=", TokenType::ADD_ASSIGN, lineNo);
		} else {
			token = Token("+", TokenType::ADD, lineNo);
		}
	} else if (curChar == '-') {
		if (peek() == '-') {
			nextChar();
			token = Token("--", TokenType::DECREMENT, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("-=", TokenType::SUB_ASSIGN, lineNo);
		} else {
			token = Token("-", TokenType::SUB, lineNo);
		}
	} else if (curChar == '*') {
		if (peek() == '*') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("**=", TokenType::POW_ASSIGN, lineNo);
			} else
				token = Token("**", TokenType::POW, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("*=", TokenType::MUL_ASSIGN, lineNo);
		} else {
			token = Token("*", TokenType::MUL, lineNo);
		}
	} else if (curChar == '/') {
		if (peek() == '/') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("//=", TokenType::FLOOR_DIV_ASSIGN, lineNo);
			} else
				token = Token("//", TokenType::FLOOR_DIV, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("/=", TokenType::DIV_ASSIGN, lineNo);
		} else {
			token = Token("/", TokenType::DIV, lineNo);
		}
	} else if (curChar == '%') {
		if (peek() == '=') {
			nextChar();
			token = Token("%=", TokenType::MOD_ASSIGN, lineNo);
		} else
			token = Token("%", TokenType::MOD, lineNo);
	} else if (curChar == '&') {
		if (peek() == '&') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("&&=", TokenType::AND_ASSIGN, lineNo);
			} else
				token = Token("&&", TokenType::AND, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("&=", TokenType::BIT_AND_ASSIGN, lineNo);
		} else
			token = Token("&", TokenType::BIT_AND, lineNo);
	} else if (curChar == '.') {
		token = Token(".", TokenType::DOT, lineNo);
	} else if (curChar == '@') {
		token = Token("@", TokenType::INDEX, lineNo);
	} else if (curChar == '$') {
		token = Token("$", TokenType::ITER, lineNo);
	} else if (curChar == '^') {
		if (peek() == '=') {
			nextChar();
			token = Token("^=", TokenType::BIT_XOR_ASSIGN, lineNo);
		} else
			token = Token("^", TokenType::BIT_XOR, lineNo);
	} else if (curChar == '|') {
		if (peek() == '|') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("||=", TokenType::OR_ASSIGN, lineNo);
			} else
				token = Token("||", TokenType::OR, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("|=", TokenType::BIT_OR_ASSIGN, lineNo);
		} else
			token = Token("|", TokenType::BIT_OR, lineNo);
	} else if (curChar == '<') {
		char peekChar = peek();
		if (peekChar == '<') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token("<<=", TokenType::LSHIFT_ASSIGN, lineNo);
			} else
				token = Token("<<", TokenType::LSHIFT, lineNo);
		} else if (peekChar == '=') {
			nextChar();
			token = Token("<=", TokenType::LESSEQ, lineNo);
		} else
			token = Token("<", TokenType::LESS, lineNo);
	} else if (curChar == '>') {
		char peekChar = peek();
		if (peekChar == '>') {
			nextChar();
			if (peek() == '=') {
				nextChar();
				token = Token(">>=", TokenType::RSHIFT_ASSIGN, lineNo);
			} else
				token = Token(">>", TokenType::RSHIFT, lineNo);
		} else if (peekChar == '=') {
			nextChar();
			token = Token(">=", TokenType::GREATEREQ, lineNo);
		} else
			token = Token(">", TokenType::GREATER, lineNo);
	} else if (curChar == '(') {
		token = Token("(", TokenType::OPEN_PAREN, lineNo);
	} else if (curChar == ')') {
		token = Token(")", TokenType::CLOSE_PAREN, lineNo);
	} else if (curChar == '{') {
		token = Token("{", TokenType::OPEN_BRAC, lineNo);
	} else if (curChar == '}') {
		token = Token("}", TokenType::CLOSE_BRAC, lineNo);
	} else if (curChar == ',') {
		token = Token(",", TokenType::COMMA, lineNo);
	} else if (curChar == '[') {
		token = Token("[", TokenType::OPEN_BRACKET, lineNo);
	} else if (curChar == ']') {
		token = Token("]", TokenType::CLOSE_BRACKET, lineNo);
	} else if (curChar == '!') {
		if (peek() == '!') {
			nextChar();
			token = Token("!!", TokenType::BIT_NOT, lineNo);
		} else if (peek() == '=') {
			nextChar();
			token = Token("!=", TokenType::NOTEQ, lineNo);
		} else
			token = Token("!", TokenType::NOT, lineNo);
	} else if (curChar == '=') {
		if (peek() == '=') {
			nextChar();
			token = Token("==", TokenType::EQEQ, lineNo);
		} else {
			token = Token("=", TokenType::ASSIGN, lineNo);
		}
	} else if (curChar == '\"') {
		nextChar();

		int startPos = curPos;

		while (curChar != '\"' && curChar != '\0')
			nextChar();
		if (curChar == '\0')
			MissingTerminatorError("Unterminated string literal", lineNo);

		token = Token(source.substr(startPos, curPos-startPos), TokenType::STR, lineNo);
	} else if (curChar == '\'') {
		nextChar();

		int startPos = curPos;

		while (curChar != '\'' && curChar != '\0')
			nextChar();
		if (curChar == '\0')
			MissingTerminatorError("Unterminated string literal", lineNo);

		if (curPos - startPos == 1)
			token = Token(source.substr(startPos, 1), TokenType::CHR, lineNo);
		else 
			token = Token(source.substr(startPos, curPos-startPos), TokenType::STR, lineNo);
	} else if (isalpha(curChar) || curChar == '_') {
		int startPos = curPos;

		char curChar = 0;
		while (isalnum(curChar = peek()) || curChar == '_') {
			nextChar();
		}

		std::string text = source.substr(startPos, curPos-startPos+1);

		TokenType kind;

		if (text == "int") {
			kind = TokenType::TYPE_INT;
		} else if (text == "str") {
			kind = TokenType::TYPE_STR;
		} else if (text == "float") {
			kind = TokenType::TYPE_FLOAT;
		} else if (text == "bool") {
			kind = TokenType::TYPE_BOOL;
		} else if (text == "vec") {
			kind = TokenType::TYPE_VEC;
		} else if (text == "load") {
			kind = TokenType::LOAD;
		} else if (text == "form") {
			kind = TokenType::FORM;
		} else if (text == "return") {
			kind = TokenType::RETURN;
		} else if (text == "class") {
			kind = TokenType::CLASS;
		} else if (text == "if") {
			kind = TokenType::IF;
		} else if (text == "else") {
			kind = TokenType::ELSE;
		} else if (text == "while") {
			kind = TokenType::WHILE;
		} else if (text == "for") {
			kind = TokenType::FOR;
		} else if (text == "break") {
			kind = TokenType::BREAK;
		} else if (text == "continue") {
			kind = TokenType::CONTINUE;
		} else if (text == "true" || text == "false") {
			kind = TokenType::BOOL;
		} else {
			kind = TokenType::IDENT;
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

			token = Token(source.substr(startPos, curPos+1), TokenType::FLOAT, lineNo);
		} else {
			TokenType intlit_type = TokenType::INT_DEC;
			if (is_digit_func == is_hex_digit)
				intlit_type = TokenType::INT_HEX;
			else if (is_digit_func == is_oct_digit)
				intlit_type = TokenType::INT_OCT;
			else if (is_digit_func == is_bin_digit)
				intlit_type = TokenType::INT_BIN;
			token = Token(source.substr(startPos, curPos-startPos+1), intlit_type, lineNo);
		}
	} else if (curChar == ';') {
		token = Token(";", TokenType::SEM, lineNo);
	} else if (curChar == '\n') {
		lineNo++;

		token = Token("\\n", TokenType::NEWLINE, lineNo);
	} else if (curChar == '\0') {
		token = Token("", TokenType::EOF_TOK, lineNo);
	}

	nextChar();

	return token;
}

void Lexer::getTokens() {
	while (true) {
		Token token = getToken();

		if (token.kind != TokenType::NEWLINE && token.kind != TokenType::EOF_TOK) {
			tokens.push_back(token);
		} else {
			if (token.kind == TokenType::EOF_TOK) {
				break;
			}
		}
	}
}

Lexer::Lexer(std::string input) : source(input) {
	curPos = -1;
	lineNo = 1;
}
