#include "types.hpp"

#include <cstdint>
#include <unordered_map>

#include "opcodes.hpp"

int64_t ipow(int64_t base, uint8_t exp) {
    static const uint8_t highest_bit_set[] = {
        0, 1, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 255, // anything past 63 is a guaranteed overflow with base > 1
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
    };

    int64_t result = 1;

    switch (highest_bit_set[exp]) {
    case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
        if (base == 1) {
            return 1;
        }
        
        if (base == -1) {
            return 1 - 2 * (exp & 1);
        }
        
        return 0;
    case 6:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
	  [[fallthrough]];
    case 5:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
	  [[fallthrough]];
    case 4:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
	  [[fallthrough]];
    case 3:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
	  [[fallthrough]];
    case 2:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
	  [[fallthrough]];
    case 1:
        if (exp & 1) result *= base;
	  [[fallthrough]];
    default:
        return result;
    }
}

bool isRightAssoc(const TokenType& op) {
	switch (op) {
	case TokenType::ASSIGN:
	case TokenType::ADD_ASSIGN:
	case TokenType::SUB_ASSIGN:
	case TokenType::MOD_ASSIGN:
	case TokenType::POW_ASSIGN:
	case TokenType::MUL_ASSIGN:
	case TokenType::DIV_ASSIGN:
	case TokenType::AND_ASSIGN:
	case TokenType::OR_ASSIGN:
	case TokenType::FLOOR_DIV_ASSIGN:
	case TokenType::BIT_AND_ASSIGN:
	case TokenType::BIT_XOR_ASSIGN:
	case TokenType::BIT_OR_ASSIGN:
	case TokenType::LSHIFT_ASSIGN:
	case TokenType::RSHIFT_ASSIGN:
	case TokenType::POW:
		return true;
	default:
		return false;
	}
}

bool getCompoundAssignOp(const TokenType& op, TokenType& out) {
	static const std::unordered_map<TokenType, TokenType> compoundToBinary = {
		{TokenType::ADD_ASSIGN, TokenType::ADD},
		{TokenType::SUB_ASSIGN, TokenType::SUB},
		{TokenType::MOD_ASSIGN, TokenType::MOD},
		{TokenType::POW_ASSIGN, TokenType::POW},
		{TokenType::MUL_ASSIGN, TokenType::MUL},
		{TokenType::DIV_ASSIGN, TokenType::DIV},
		{TokenType::AND_ASSIGN, TokenType::AND},
		{TokenType::OR_ASSIGN, TokenType::OR},
		{TokenType::FLOOR_DIV_ASSIGN, TokenType::FLOOR_DIV},
		{TokenType::BIT_AND_ASSIGN, TokenType::BIT_AND},
		{TokenType::BIT_XOR_ASSIGN, TokenType::BIT_XOR},
		{TokenType::BIT_OR_ASSIGN, TokenType::BIT_OR},
		{TokenType::LSHIFT_ASSIGN, TokenType::LSHIFT},
		{TokenType::RSHIFT_ASSIGN, TokenType::RSHIFT},
	};

	auto it = compoundToBinary.find(op);
	if (it == compoundToBinary.end()) {
		return false;
	}

	out = it->second;
	return true;
}
