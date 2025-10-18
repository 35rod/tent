#pragma once

#include <cstdint>
#include <string>

typedef uint16_t TokenTypeSize;

/* by the way, the order these are in is really important to the
 * program working properly. if they are rearranged, the program
 * will probably cause a lot of random (and, dangerously, silent)
 * bugs that will probably be really hard to figure out.
 * Please don't rearrange these. If you have to add more operators
 * or types of tokens, add them at the bottom.
 */
enum class TokenType: TokenTypeSize {
	PUSH_INT,
	PUSH_FLOAT,
	PUSH_STRING,
	PUSH_BOOL,

	NOT, BIT_NOT,
	INCREMENT, DECREMENT,
	NEGATE,

	ADD, SUB,
	MOD, POW, MUL, DIV,
	AND, OR,
	FLOOR_DIV,
	BIT_AND, BIT_XOR, BIT_OR,
	LSHIFT, RSHIFT,

	INDEX,

	LESS, LESSEQ, GREATER, GREATEREQ,
	EQEQ, NOTEQ,

	ASSIGN,
	
	ADD_ASSIGN, SUB_ASSIGN,
	MOD_ASSIGN, POW_ASSIGN, MUL_ASSIGN, DIV_ASSIGN,
	AND_ASSIGN, OR_ASSIGN,
	FLOOR_DIV_ASSIGN,
	BIT_AND_ASSIGN, BIT_XOR_ASSIGN, BIT_OR_ASSIGN,
	LSHIFT_ASSIGN, RSHIFT_ASSIGN,
	
	OPEN_PAREN, CLOSE_PAREN,
	OPEN_BRAC, CLOSE_BRAC,
	OPEN_BRACKET, CLOSE_BRACKET,
	COMMA, SEM, NEWLINE,
	LOAD,
	SET,
	FORM, RETURN,
	IF, ELSE,
	WHILE, BREAK, CONTINUE,
	INT_HEX, INT_DEC, INT_OCT, INT_BIN,
	FLOAT,
	BOOL,
	STR, CHR,
	IDENT,

	INVALID_TOKEN,
	EOF_TOK,

	VAR,

	JUMP_IF_FALSE, JUMP, INLINE, RETURN_INLINE,

	FOR, ITER,

	CALL, CALL_INLINE,

	DOT,

	CLASS,

	TYPE_INT,
	TYPE_STR,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_VEC,
	TYPE_DIC,

	COLON
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::PUSH_INT: return "integer";
        case TokenType::PUSH_FLOAT: return "float";
        case TokenType::PUSH_STRING: return "string";
        case TokenType::PUSH_BOOL: return "boolean";

        case TokenType::NOT: return "logical NOT operator";
        case TokenType::BIT_NOT: return "bitwise NOT operator";
        case TokenType::INCREMENT: return "increment operator (++)";
        case TokenType::DECREMENT: return "decrement operator (--)";
        case TokenType::NEGATE: return "negation operator (-)";

        case TokenType::ADD: return "addition operator (+)";
        case TokenType::SUB: return "subtraction operator (-)";
        case TokenType::MOD: return "modulus operator (%)";
        case TokenType::POW: return "power operator (**)";
        case TokenType::MUL: return "multiplication operator (*)";
        case TokenType::DIV: return "division operator (/)";

        case TokenType::AND: return "logical AND operator (&&)";
        case TokenType::OR: return "logical OR operator (||)";
        case TokenType::FLOOR_DIV: return "floor division operator (//)";

        case TokenType::BIT_AND: return "bitwise AND operator (&)";
        case TokenType::BIT_XOR: return "bitwise XOR operator (^)";
        case TokenType::BIT_OR: return "bitwise OR operator (|)";

        case TokenType::LSHIFT: return "left shift operator (<<)";
        case TokenType::RSHIFT: return "right shift operator (>>)";
        case TokenType::INDEX: return "index operator (@)";

        case TokenType::LESS: return "less than operator (<)";
        case TokenType::LESSEQ: return "less than or equal operator (<=)";
        case TokenType::GREATER: return "greater than operator (>)";
        case TokenType::GREATEREQ: return "greater than or equal operator (>=)";

        case TokenType::EQEQ: return "equality operator (==)";
        case TokenType::NOTEQ: return "not equal operator (!=)";

        case TokenType::ASSIGN: return "assignment operator (=)";
        case TokenType::ADD_ASSIGN: return "addition assignment operator (+=)";
        case TokenType::SUB_ASSIGN: return "subtraction assignment operator (-=)";
        case TokenType::MOD_ASSIGN: return "modulus assignment operator (%=)";
        case TokenType::POW_ASSIGN: return "power assignment operator (**=)";
        case TokenType::MUL_ASSIGN: return "multiplication assignment operator (*=)";
        case TokenType::DIV_ASSIGN: return "division assignment operator (/=)";
        case TokenType::AND_ASSIGN: return "logical AND assignment (&&=)";
        case TokenType::OR_ASSIGN: return "logical OR assignment (||=)";
        case TokenType::FLOOR_DIV_ASSIGN: return "floor division assignment (//=)";
        case TokenType::BIT_AND_ASSIGN: return "bitwise AND assignment (&=)";
        case TokenType::BIT_XOR_ASSIGN: return "bitwise XOR assignment (^=)";
        case TokenType::BIT_OR_ASSIGN: return "bitwise OR assignment (|=)";
        case TokenType::LSHIFT_ASSIGN: return "left shift assignment (<<=)";
        case TokenType::RSHIFT_ASSIGN: return "right shift assignment (>>=)";

        case TokenType::OPEN_PAREN: return "opening parenthesis (()";
        case TokenType::CLOSE_PAREN: return "closing parenthesis ())";
        case TokenType::OPEN_BRAC: return "opening brace ({)";
        case TokenType::CLOSE_BRAC: return "closing brace (})";
        case TokenType::OPEN_BRACKET: return "opening bracket ([)";
        case TokenType::CLOSE_BRACKET: return "closing bracket (])";
        case TokenType::COMMA: return "comma (,)";
        case TokenType::SEM: return "semicolon (;)";
        case TokenType::NEWLINE: return "newline";
        case TokenType::LOAD: return "load keyword";
        case TokenType::SET: return "set keyword";
        case TokenType::FORM: return "form keyword";
        case TokenType::RETURN: return "return keyword";
        case TokenType::IF: return "if keyword";
        case TokenType::ELSE: return "else keyword";
        case TokenType::WHILE: return "while keyword";
        case TokenType::BREAK: return "break keyword";
        case TokenType::CONTINUE: return "continue keyword";

        case TokenType::INT_HEX: return "hex integer";
        case TokenType::INT_DEC: return "decimal integer";
        case TokenType::INT_OCT: return "octal integer";
        case TokenType::INT_BIN: return "binary integer";
        case TokenType::FLOAT: return "float";
        case TokenType::BOOL: return "boolean";
        case TokenType::STR: return "string";
        case TokenType::CHR: return "character";
        case TokenType::IDENT: return "identifier";

        case TokenType::INVALID_TOKEN: return "invalid token";
        case TokenType::EOF_TOK: return "end of file";
        case TokenType::VAR: return "variable";
        case TokenType::JUMP_IF_FALSE: return "jump if false";
        case TokenType::JUMP: return "jump";
        case TokenType::INLINE: return "inline keyword";
        case TokenType::RETURN_INLINE: return "return inline";
        case TokenType::FOR: return "for keyword";
        case TokenType::ITER: return "iterator operator ($)";
        case TokenType::CALL: return "function call";
        case TokenType::CALL_INLINE: return "inline function call";
        case TokenType::DOT: return "dot operator (.)";
        case TokenType::CLASS: return "class keyword";

        case TokenType::TYPE_INT: return "int type";
        case TokenType::TYPE_STR: return "string type";
        case TokenType::TYPE_FLOAT: return "float type";
        case TokenType::TYPE_BOOL: return "boolean type";
        case TokenType::TYPE_VEC: return "vector type";
        case TokenType::TYPE_DIC: return "dictionary type";

        case TokenType::COLON: return "colon (:)";

        default: return "unknown token";
    }
}
