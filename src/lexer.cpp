#include "lexer.hpp"
#include "errors.hpp"

void Lexer::nextChar() {
    curPos++;

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

Token Lexer::getToken() {
    skipWhitespace();
    skipComment();

    Token token("", "", lineNo);

    if (curChar == '+') {
        token = Token("+", "ADD", lineNo);
    } else if (curChar == '-') {
        token = Token("-", "SUB", lineNo);
    } else if (curChar == '*') {
        if (peek() == '*') {
            char lastChar = curChar;
            nextChar();
            token = Token(std::string(1, lastChar) + curChar, "POW", lineNo);
        } else {
            token = Token("*", "MUL", lineNo);
        }
    } else if (curChar == '/') {
        token = Token("/", "DIV", lineNo);
    } else if (curChar == '%') {
        token = Token("%", "MOD", lineNo);
    } else if (curChar == '&') {
        token = Token("&", "BIN_AND", lineNo);
    } else if (curChar == '^') {
        token = Token("^", "BIN_XOR", lineNo);
    } else if (curChar == '|') {
        token = Token("|", "BIN_OR", lineNo);
    } else if (curChar == '<') {
        if (peek() == '<') {
            char lastChar = curChar;
            nextChar();
            token = Token(std::string(1, lastChar) + curChar, "LSHIFT", lineNo);
        }
    } else if (curChar == '>') {
        if (peek() == '>') {
            char lastChar = curChar;
            nextChar();
            token = Token(std::string(1, lastChar) + curChar, "RSHIFT", lineNo);
        }
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
    } else if (curChar == '=') {
        if (peek() == '=') {
            char lastChar = curChar;
            nextChar();
            token = Token(std::string(1, lastChar) + curChar, "EQEQ", lineNo);
        } else {
            token = Token("=", "EQ", lineNo);
        }
    } else if (curChar == '\"') {
        nextChar();

        int startPos = curPos;

        while (curChar != '\"' && curChar != '\0') {
            nextChar();
        }
	  if (curChar == '\0')
		  MissingTerminatorError("unterminated string literal", lineNo);

        token = Token(source.substr(startPos, curPos-startPos), "STR", lineNo);
    } else if (isdigit(curChar)) {
        int startPos = curPos;
        
        while (isdigit(peek())) {
            nextChar();
        }

        if (peek() == '.') {
            nextChar();

            while (isdigit(peek())) {
                nextChar();
            }

            token = Token(source.substr(startPos, curPos+1), "FLOAT", lineNo);
        } else {
            token = Token(source.substr(startPos, curPos-startPos+1), "INT", lineNo);
        }
    } else if (isalpha(curChar)) {
        int startPos = curPos;

        while (isalnum(peek())) {
            nextChar();
        }

        std::string text = source.substr(startPos, curPos-startPos+1);

        std::string kind;

        if (text == "set") {
            kind = "SET";
        } else if (text == "form") {
            kind = "FORM";
        } else if (text == "while") {
            kind = "WHILE";
        } else if (text == "true" || text == "false") {
            kind = "BOOL";
        } else {
            kind = "IDENT";
        }

        token = Token(text, kind, lineNo);
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
