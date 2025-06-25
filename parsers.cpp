#include "parser.h"
#include <iostream>
#include <cctype>
#include <stdexcept>
using namespace std;

shared_ptr<ASTNode> optimizeAST(const shared_ptr<ASTNode>& node) {
    if (!node) return nullptr;

    if (auto bin = dynamic_pointer_cast<BinaryExpr>(node)) {
        auto left = optimizeAST(bin->left);
        auto right = optimizeAST(bin->right);

        if (auto lval = dynamic_pointer_cast<Literal>(left)) {
            if (auto rval = dynamic_pointer_cast<Literal>(right)) {
                int result = 0;
                if (bin->op == "+") result = lval->value + rval->value;
                else if (bin->op == "-") result = lval->value - rval->value;
                else if (bin->op == "*") result = lval->value * rval->value;
                else if (bin->op == "/") {
                    if (rval->value == 0) throw runtime_error("Division by zero in constant folding");
                    result = lval->value / rval->value;
                } else return make_shared<BinaryExpr>(bin->op, left, right);
                return make_shared<Literal>(result);
            }
        }
        return make_shared<BinaryExpr>(bin->op, left, right);
    }

    if (auto assign = dynamic_pointer_cast<Assignment>(node)) {
        auto newVal = optimizeAST(assign->value);
        return make_shared<Assignment>(assign->name, newVal);
    }

    if (auto ifstmt = dynamic_pointer_cast<IfStmt>(node)) {
        auto cond = optimizeAST(ifstmt->condition);
        auto thenB = optimizeAST(ifstmt->thenBranch);
        auto elseB = optimizeAST(ifstmt->elseBranch);
        if (auto condLit = dynamic_pointer_cast<Literal>(cond)) {
            return condLit->value ? thenB : elseB;
        }
        return make_shared<IfStmt>(cond, thenB, elseB);
    }

    if (auto wh = dynamic_pointer_cast<WhileStmt>(node)) {
        auto cond = optimizeAST(wh->condition);
        auto body = optimizeAST(wh->body);
        if (auto condLit = dynamic_pointer_cast<Literal>(cond)) {
            if (condLit->value == 0) return nullptr;
        }
        return make_shared<WhileStmt>(cond, body);
    }

    if (auto blk = dynamic_pointer_cast<Block>(node)) {
        vector<shared_ptr<ASTNode>> newStmts;
        for (auto& stmt : blk->statements) {
            auto opt = optimizeAST(stmt);
            if (opt) newStmts.push_back(opt);
        }
        return make_shared<Block>(newStmts);
    }

    if (auto print = dynamic_pointer_cast<PrintStmt>(node)) {
        return make_shared<PrintStmt>(optimizeAST(print->expr));
    }

    return node;
}


class Parser::ParserImpl {
public:
    ParserImpl(const string& in) : input(in), pos(0) {}

    shared_ptr<ASTNode> parse() {
        auto stmts = parseStatements();
        return make_shared<Block>(stmts);
    }

private:
    string input;
    size_t pos;

    void skipWhitespace() {
        while (pos < input.size() && isspace(input[pos])) pos++;
    }

    char peek() {
        skipWhitespace();
        if (pos < input.size()) return input[pos];
        return '\0';
    }

    char get() {
        skipWhitespace();
        if (pos < input.size()) return input[pos++];
        return '\0';
    }

    bool match(const string& kw) {
        skipWhitespace();
        size_t len = kw.size();
        if (input.substr(pos, len) == kw) {
            pos += len;
            return true;
        }
        return false;
    }

    vector<shared_ptr<ASTNode>> parseStatements() {
        vector<shared_ptr<ASTNode>> stmts;
        while (peek() && peek() != '}') {
            stmts.push_back(parseStatement());
        }
        return stmts;
    }

    shared_ptr<ASTNode> parseStatement() {
        if (match("if")) return parseIf();
        if (match("while")) return parseWhile();
        if (match("print")) return parsePrint();
        if (peek() == '{') return parseBlock();
        if (isalpha(peek()) || peek() == '_') {
            size_t save = pos;
            string name = parseIdentifier();
            skipWhitespace();
            if (peek() == '=') {
                pos = save;
                return parseAssignment();
            } else {
                pos = save;
                auto expr = parseExpression();
                expect(';');
                return expr;
            }
        }
        auto expr = parseExpression();
        expect(';');
        return expr;
    }

    shared_ptr<ASTNode> parseAssignment() {
        string name = parseIdentifier();
        expect('=');
        auto value = parseExpression();
        expect(';');
        return make_shared<Assignment>(name, value);
    }

    shared_ptr<ASTNode> parseIf() {
        expect('(');
        auto cond = parseExpression();
        expect(')');
        auto thenB = parseStatement();
        shared_ptr<ASTNode> elseB = nullptr;
        if (match("else")) {
            elseB = parseStatement();
        }
        return make_shared<IfStmt>(cond, thenB, elseB);
    }

    shared_ptr<ASTNode> parseWhile() {
        expect('(');
        auto cond = parseExpression();
        expect(')');
        auto body = parseStatement();
        return make_shared<WhileStmt>(cond, body);
    }

    shared_ptr<ASTNode> parseBlock() {
        expect('{');
        auto stmts = parseStatements();
        expect('}');
        return make_shared<Block>(stmts);
    }

    shared_ptr<ASTNode> parseExpression() {
        return parseEquality();
    }

    shared_ptr<ASTNode> parseEquality() {
        auto node = parseRelational();
        while (true) {
            if (match("==")) node = make_shared<BinaryExpr>("==", node, parseRelational());
            else if (match("!=")) node = make_shared<BinaryExpr>("!=", node, parseRelational());
            else return node;
        }
    }

    shared_ptr<ASTNode> parseRelational() {
        auto node = parseAdditive();
        while (true) {
            if (match("<")) node = make_shared<BinaryExpr>("<", node, parseAdditive());
            else if (match(">")) node = make_shared<BinaryExpr>(">", node, parseAdditive());
            else if (match("<=")) node = make_shared<BinaryExpr>("<=", node, parseAdditive());
            else if (match(">=")) node = make_shared<BinaryExpr>(">=", node, parseAdditive());
            else return node;
        }
    }

    shared_ptr<ASTNode> parseAdditive() {
        auto node = parseTerm();
        while (true) {
            if (match("+")) node = make_shared<BinaryExpr>("+", node, parseTerm());
            else if (match("-")) node = make_shared<BinaryExpr>("-", node, parseTerm());
            else return node;
        }
    }

    shared_ptr<ASTNode> parseTerm() {
        auto node = parseFactor();
        while (true) {
            if (match("*")) node = make_shared<BinaryExpr>("*", node, parseFactor());
            else if (match("/")) node = make_shared<BinaryExpr>("/", node, parseFactor());
            else return node;
        }
    }

    shared_ptr<ASTNode> parseFactor() {
        if (isdigit(peek())) {
            int val = 0;
            while (isdigit(peek())) val = val * 10 + (get() - '0');
            return make_shared<Literal>(val);
        } else if (peek() == '(') {
            get();
            auto node = parseExpression();
            expect(')');
            return node;
        } else if (isalpha(peek()) || peek() == '_') {
            string name = parseIdentifier();
            return make_shared<Identifier>(name);
        } else {
            throw runtime_error("Unexpected character in factor");
        }
    }

    string parseIdentifier() {
        skipWhitespace();
        string name;
        if (isalpha(peek()) || peek() == '_') {
            name += get();
            while (isalnum(peek()) || peek() == '_') name += get();
        } else {
            throw runtime_error("Expected identifier");
        }
        return name;
    }

    void expect(char c) {
        skipWhitespace();
        if (get() != c) throw runtime_error(string("Expected '") + c + "'");
    }

    shared_ptr<ASTNode> parsePrint() {
        auto expr = parseExpression();
        expect(';');
        return make_shared<PrintStmt>(expr);
    }
};

Parser::Parser(const string& in) : impl(new ParserImpl(in)) {}

Parser::~Parser() { delete impl; }

shared_ptr<ASTNode> Parser::parse() { return impl->parse(); }

void printTree(const shared_ptr<ASTNode>& node, int depth) {
    if (!node) return;
    string indent(depth * 4, ' ');
    if (auto lit = dynamic_pointer_cast<Literal>(node)) {
        cout << indent << "Literal: " << lit->value << "\n";
    } else if (auto id = dynamic_pointer_cast<Identifier>(node)) {
        cout << indent << "Identifier: " << id->name << "\n";
    } else if (auto bin = dynamic_pointer_cast<BinaryExpr>(node)) {
        cout << indent << "BinaryExpr: " << bin->op << "\n";
        printTree(bin->left, depth + 1);
        printTree(bin->right, depth + 1);
    } else if (auto assign = dynamic_pointer_cast<Assignment>(node)) {
        cout << indent << "Assignment: " << assign->name << "\n";
        printTree(assign->value, depth + 1);
    } else if (auto iff = dynamic_pointer_cast<IfStmt>(node)) {
        cout << indent << "IfStmt\n";
        cout << indent << "  Condition:\n";
        printTree(iff->condition, depth + 2);
        cout << indent << "  Then:\n";
        printTree(iff->thenBranch, depth + 2);
        if (iff->elseBranch) {
            cout << indent << "  Else:\n";
            printTree(iff->elseBranch, depth + 2);
        }
    } else if (auto wh = dynamic_pointer_cast<WhileStmt>(node)) {
        cout << indent << "WhileStmt\n";
        cout << indent << "  Condition:\n";
        printTree(wh->condition, depth + 2);
        cout << indent << "  Body:\n";
        printTree(wh->body, depth + 2);
    } else if (auto block = dynamic_pointer_cast<Block>(node)) {
        cout << indent << "Block\n";
        for (auto& stmt : block->statements) printTree(stmt, depth + 1);
    } else if (auto print = dynamic_pointer_cast<PrintStmt>(node)) {
        cout << indent << "PrintStmt\n";
        printTree(print->expr, depth + 1);
    } else {
        cout << indent << "Unknown node\n";
    }
}
