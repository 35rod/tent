#pragma once

#include <cstdint>

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

	PRINT,
	PRINTLN,

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

	VAR
};
