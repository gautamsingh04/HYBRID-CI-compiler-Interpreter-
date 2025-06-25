#include "lexer.h"
#include <sstream>

using namespace std;

string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "Keyword";
        case TokenType::IDENTIFIER: return "Identifier";
        case TokenType::NUMBER: return "Number";
        case TokenType::OPERATOR: return "Operator";
        case TokenType::SEPARATOR: return "Separator";
        case TokenType::STRING_LITERAL: return "String";
        case TokenType::COMMENT: return "Comment";
        default: return "Unknown";
    }
}

vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    istringstream stream(code);
    string line;
    int lineNumber = 0;

    regex keywordRegex("\\b(if|else|while|for|return|int|float|char|void|bool)\\b");
    regex identifierRegex("[a-zA-Z_][a-zA-Z0-9_]*");
    regex numberRegex("\\b\\d+(\\.\\d+)?\\b");
    regex operatorRegex("==|!=|<=|>=|[+\\-*/=<>]");
    regex separatorRegex("[\\(\\)\\{\\}\\[\\],;]");
    regex stringLiteralRegex("\"(\\\\.|[^\"])*\"");
    regex commentRegex("//.*");

    while (getline(stream, line)) {
        lineNumber++;
        string::const_iterator searchStart(line.cbegin());

        while (searchStart != line.cend()) {
            smatch match;

            if (regex_search(searchStart, line.cend(), match, commentRegex) && match.position() == 0) {
                tokens.push_back({TokenType::COMMENT, match[0], lineNumber});
                break;
            }
            else if (regex_search(searchStart, line.cend(), match, keywordRegex) && match.position() == 0) {
                tokens.push_back({TokenType::KEYWORD, match[0], lineNumber});
            }
            else if (regex_search(searchStart, line.cend(), match, identifierRegex) && match.position() == 0) {
                tokens.push_back({TokenType::IDENTIFIER, match[0], lineNumber});
            }
            else if (regex_search(searchStart, line.cend(), match, numberRegex) && match.position() == 0) {
                tokens.push_back({TokenType::NUMBER, match[0], lineNumber});
            }
            else if (regex_search(searchStart, line.cend(), match, operatorRegex) && match.position() == 0) {
                tokens.push_back({TokenType::OPERATOR, match[0], lineNumber});
            }
            else if (regex_search(searchStart, line.cend(), match, separatorRegex) && match.position() == 0) {
                tokens.push_back({TokenType::SEPARATOR, match[0], lineNumber});
            }
            else if (regex_search(searchStart, line.cend(), match, stringLiteralRegex) && match.position() == 0) {
                tokens.push_back({TokenType::STRING_LITERAL, match[0], lineNumber});
            }
            else if (isspace(*searchStart)) {
                ++searchStart;
                continue;
            }
            else {
                string unknown(1, *searchStart);
                tokens.push_back({TokenType::UNKNOWN, unknown, lineNumber});
                ++searchStart;
                continue;
            }

            searchStart += match.length();
        }
    }

    return tokens;
}
