#pragma once
#include <string>
#include <vector>
#include <regex>

// Token types for the lexer
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    SEPARATOR,
    STRING_LITERAL,
    COMMENT,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int lineNumber;
};

// Tokenize input code into a vector of tokens
std::vector<Token> tokenize(const std::string& code);

// Convert TokenType to string for display
std::string tokenTypeToString(TokenType type); 