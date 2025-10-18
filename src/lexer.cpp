#include "lexer.hpp"

#include <cctype>

#include "opcodes.hpp"
#include "types.hpp"
#include "errors.hpp"

void Lexer::nextChar(int num) {
    for (int i = 0; i < num; i++) {
        curPos++;

        if (curPos >= source.length()) {
            curChar = '\0';
            return;
        }

        curChar = source[curPos];
        
        if (curChar == '\n') {
            lineNo++;
            colNo = 0;
        } else {
            colNo++;
        }
    }
}

char Lexer::peek() {
    if (curPos + 1 >= source.length()) return '\0';
    return source[curPos+1];
}

void Lexer::skipWhitespace() {
    while (curChar == ' ' || curChar == '\t' || curChar == '\r') {
        nextChar();
    }
}

void Lexer::skipComment() {
    if (curChar == '~') {
        while (curChar != '\n' && curChar != '\0') {
            nextChar();
        }
    }
}

#include "is_digit_incl.cpp"

Token Lexer::getToken() {
    skipWhitespace();
    skipComment();

    Token token("", TokenType::INVALID_TOKEN, lineNo, colNo);

    switch (curChar) {
        case '+':
            if (peek() == '+') {
                nextChar();
                token = Token("++", TokenType::INCREMENT, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("+=", TokenType::ADD_ASSIGN, lineNo, colNo);
            } else {
                token = Token("+", TokenType::ADD, lineNo, colNo);
            }
            break;
        case '-':
            if (peek() == '-') {
                nextChar();
                token = Token("--", TokenType::DECREMENT, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("-=", TokenType::SUB_ASSIGN, lineNo, colNo);
            } else {
                token = Token("-", TokenType::SUB, lineNo, colNo);
            }
            break;
        case '*':
            if (peek() == '*') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token("**=", TokenType::POW_ASSIGN, lineNo, colNo);
                } else
                    token = Token("**", TokenType::POW, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("*=", TokenType::MUL_ASSIGN, lineNo, colNo);
            } else {
                token = Token("*", TokenType::MUL, lineNo, colNo);
            }
            break;
        case '/':
            if (peek() == '/') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token("//=", TokenType::FLOOR_DIV_ASSIGN, lineNo, colNo);
                } else
                    token = Token("//", TokenType::FLOOR_DIV, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("/=", TokenType::DIV_ASSIGN, lineNo, colNo);
            } else {
                token = Token("/", TokenType::DIV, lineNo, colNo);
            }
            break;
        case '%':
            if (peek() == '=') {
                nextChar();
                token = Token("%=", TokenType::MOD_ASSIGN, lineNo, colNo);
            } else
                token = Token("%", TokenType::MOD, lineNo, colNo);
            break;
        case '&':
            if (peek() == '&') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token("&&=", TokenType::AND_ASSIGN, lineNo, colNo);
                } else
                    token = Token("&&", TokenType::AND, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("&=", TokenType::BIT_AND_ASSIGN, lineNo, colNo);
            } else
                token = Token("&", TokenType::BIT_AND, lineNo, colNo);
            break;
        case '.':
            token = Token(".", TokenType::DOT, lineNo, colNo);
            break;
        case '@':
            token = Token("@", TokenType::INDEX, lineNo, colNo);
            break;
        case '$':
            token = Token("$", TokenType::ITER, lineNo, colNo);
            break;
        case '^':
            if (peek() == '=') {
                nextChar();
                token = Token("^=", TokenType::BIT_XOR_ASSIGN, lineNo, colNo);
            } else
                token = Token("^", TokenType::BIT_XOR, lineNo, colNo);
            break;
        case '|':
            if (peek() == '|') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token("||=", TokenType::OR_ASSIGN, lineNo, colNo);
                } else
                    token = Token("||", TokenType::OR, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("|=", TokenType::BIT_OR_ASSIGN, lineNo, colNo);
            } else
                token = Token("|", TokenType::BIT_OR, lineNo, colNo);
            break;
        case '<': {
            char peekChar = peek();
            if (peekChar == '<') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token("<<=", TokenType::LSHIFT_ASSIGN, lineNo, colNo);
                } else
                    token = Token("<<", TokenType::LSHIFT, lineNo, colNo);
            } else if (peekChar == '=') {
                nextChar();
                token = Token("<=", TokenType::LESSEQ, lineNo, colNo);
            } else
                token = Token("<", TokenType::LESS, lineNo, colNo);
            break;
        }
        case '>': {
            char peekChar = peek();
            if (peekChar == '>') {
                nextChar();
                if (peek() == '=') {
                    nextChar();
                    token = Token(">>=", TokenType::RSHIFT_ASSIGN, lineNo, colNo);
                } else
                    token = Token(">>", TokenType::RSHIFT, lineNo, colNo);
            } else if (peekChar == '=') {
                nextChar();
                token = Token(">=", TokenType::GREATEREQ, lineNo, colNo);
            } else
                token = Token(">", TokenType::GREATER, lineNo, colNo);
            break;
        }
        case '(':
            token = Token("(", TokenType::OPEN_PAREN, lineNo, colNo);
            break;
        case ')':
            token = Token(")", TokenType::CLOSE_PAREN, lineNo, colNo);
            break;
        case '{':
            token = Token("{", TokenType::OPEN_BRAC, lineNo, colNo);
            break;
        case '}':
            token = Token("}", TokenType::CLOSE_BRAC, lineNo, colNo);
            break;
        case ',':
            token = Token(",", TokenType::COMMA, lineNo, colNo);
            break;
        case '[':
            token = Token("[", TokenType::OPEN_BRACKET, lineNo, colNo);
            break;
        case ']':
            token = Token("]", TokenType::CLOSE_BRACKET, lineNo, colNo);
            break;
        case '!':
            if (peek() == '!') {
                nextChar();
                token = Token("!!", TokenType::BIT_NOT, lineNo, colNo);
            } else if (peek() == '=') {
                nextChar();
                token = Token("!=", TokenType::NOTEQ, lineNo, colNo);
            } else
                token = Token("!", TokenType::NOT, lineNo, colNo);
            break;
        case '=':
            if (peek() == '=') {
                nextChar();
                token = Token("==", TokenType::EQEQ, lineNo, colNo);
            } else {
                token = Token("=", TokenType::ASSIGN, lineNo, colNo);
            }
            break;
        case '\"': {
            int literalStartCol = colNo;
            std::string lineSrc = getLineText(source, lineNo);
            nextChar();
            int startPos = curPos;

            while (curChar != '\"' && curChar != '\0')
                nextChar();
            
            if (curChar == '\0') {
                MissingTerminatorError(
                    "Unterminated string literal",
                    lineNo,
                    literalStartCol,
                    filename,
                    "Did you forget a closing double quote ('\"')?",
                    lineSrc
                ).print();
                exit(1);
            }

            token = Token(source.substr(startPos, curPos-startPos), TokenType::STR, lineNo, colNo);
            break;
        }
        case '\'': {
            int literalStartCol = colNo;
            std::string lineSrc = getLineText(source, lineNo);
            nextChar();
            int startPos = curPos;

            while (curChar != '\'' && curChar != '\0')
                nextChar();
            
            if (curChar == '\0') {
                MissingTerminatorError(
                    "Unterminated character/string literal",
                    lineNo,
                    literalStartCol,
                    filename,
                    "Did you forget a closing single quote (')?.",
                    lineSrc
                ).print();
                exit(1);
            }

            if (curPos - startPos == 1)
                token = Token(source.substr(startPos, 1), TokenType::CHR, lineNo, colNo);
            else 
                token = Token(source.substr(startPos, curPos-startPos), TokenType::STR, lineNo, colNo);
            break;
        }
        case ':':
            token = Token(":", TokenType::COLON, lineNo, colNo);
            break;
        case ';':
            token = Token(";", TokenType::SEM, lineNo, colNo);
            break;
        case '\n':
            lineNo++;

            token = Token("\\n", TokenType::NEWLINE, lineNo, colNo);
            break;
        case '\0':
            token = Token("", TokenType::EOF_TOK, lineNo, colNo);
            break;
        default:
            if (isalpha(curChar) || curChar == '_') {
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
                } else if (text == "inline") {
                    kind = TokenType::INLINE;
                }else if (text == "return") {
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

                token = Token(text, kind, lineNo, colNo);
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
                        if (isalnum(peekChar)) {
                            SyntaxError(
                                "Illegal integer literal radix specifier: " +
                                std::string("0") + peekChar,
                                lineNo,
                                colNo,
                                filename,
                                "Use 0x for hex, 0b for binary, 0o for octal, and 0d for decimal.",
                                getLineText(source, lineNo)
                            ).print();
                            exit(1);
                        }
                    }
                }

                int startPos = curPos;
                
                while (is_digit_func(peek())) {
                    nextChar();
                }

                if (peek() == '.') {
                    if (is_digit_func != is_dec_digit) {
                        SyntaxError(
                            "Floating-point literals with specified radixes are not supported.",
                            lineNo,
                            colNo,
                            filename,
                            "Only normal decimal floats are valid.",
                            getLineText(source, lineNo)
                        ).print();
                        exit(1);
                    }

                    nextChar();

                    while (isdigit(peek()))
                        nextChar();

                    token = Token(source.substr(startPos, curPos+1), TokenType::FLOAT, lineNo, colNo);
                } else {
                    TokenType intlit_type = TokenType::INT_DEC;

                    if (is_digit_func == is_hex_digit)
                        intlit_type = TokenType::INT_HEX;
                    else if (is_digit_func == is_oct_digit)
                        intlit_type = TokenType::INT_OCT;
                    else if (is_digit_func == is_bin_digit)
                        intlit_type = TokenType::INT_BIN;

                    token = Token(source.substr(startPos, curPos-startPos+1), intlit_type, lineNo, colNo);
                }
            }
            break;
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

Lexer::Lexer(std::string input, std::string file)
: source(input), curPos(-1), curChar('\0'), filename(file) {}
