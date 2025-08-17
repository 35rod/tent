#include "lexer.hpp"

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
    if (curChar == '~') {
        while (curChar != '\n') {
            nextChar();
        }
    }
}

Token Lexer::getToken() {
    skipWhitespace();
    skipComment();

    Token token("", "");

    if (curChar == '+') {
        token = Token("+", "ADD");
    } else if (curChar == '-') {
        token = Token("-", "SUB");
    } else if (curChar == '*') {
        token = Token("*", "MUL");
    } else if (curChar == '/') {
        token = Token("/", "DIV");
    } else if (curChar == '(') {
        token = Token("(", "OPEN_PAREN");
    } else if (curChar == ')') {
        token = Token(")", "CLOSE_PAREN");
    } else if (curChar == '{') {
        token = Token("{", "OPEN_BRAC");
    } else if (curChar == '}') {
        token = Token("}", "CLOSE_BRAC");
    } else if (curChar == ',') {
        token = Token(",", "COMMA");
    } else if (curChar == '=') {
        if (peek() == '=') {
            char lastChar = curChar;
            nextChar();
            token = Token(std::string(1, lastChar) + curChar, "EQEQ");
        } else {
            token = Token("=", "EQ");
        }
    } else if (curChar == '\"') {
        nextChar();

        int startPos = curPos;

        while (curChar != '\"') {
            nextChar();
        }

        token = Token(source.substr(startPos, curPos-startPos), "STR");
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

            token = Token(source.substr(startPos, curPos+1), "FLOAT");
        } else {
            token = Token(source.substr(startPos, curPos-startPos+1), "INT");
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
        } else {
            kind = "IDENT";
        }

        token = Token(text, kind);
    } else if (curChar == ';') {
        token = Token(";", "SEM");
    } else if (curChar == '\n') {
        token = Token("\\n", "NEWLINE");
    } else if (curChar == '\0') {
        token = Token("", "EOF");
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
            break;
        }
    }
}

Lexer::Lexer(std::string input) : source(input) {
    curPos = -1;
}