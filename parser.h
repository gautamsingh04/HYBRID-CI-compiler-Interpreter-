#pragma once
#include <memory>
#include <string>
#include <vector>
using namespace std;

// Base AST node
struct ASTNode {
    virtual ~ASTNode() = default;
};

// Expression nodes
struct Literal : ASTNode {
    int value;
    Literal(int v) : value(v) {}
};

struct Identifier : ASTNode {
    string name;
    Identifier(const string& n) : name(n) {}
};

struct BinaryExpr : ASTNode {
    string op;
    shared_ptr<ASTNode> left, right;
    BinaryExpr(const string& o, shared_ptr<ASTNode> l, shared_ptr<ASTNode> r)
        : op(o), left(l), right(r) {}
};

// Statement nodes
struct Assignment : ASTNode {
    string name;
    shared_ptr<ASTNode> value;
    Assignment(const string& n, shared_ptr<ASTNode> v) : name(n), value(v) {}
};

struct IfStmt : ASTNode {
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> thenBranch;
    shared_ptr<ASTNode> elseBranch;
    IfStmt(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> thenB, shared_ptr<ASTNode> elseB = nullptr)
        : condition(cond), thenBranch(thenB), elseBranch(elseB) {}
};

struct WhileStmt : ASTNode {
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> body;
    WhileStmt(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> b)
        : condition(cond), body(b) {}
};

struct Block : ASTNode {
    vector<shared_ptr<ASTNode>> statements;
    Block(const vector<shared_ptr<ASTNode>>& stmts) : statements(stmts) {}
};

struct PrintStmt : ASTNode {
    shared_ptr<ASTNode> expr;
    PrintStmt(shared_ptr<ASTNode> e) : expr(e) {}
};

// Parser interface
class Parser {
public:
    Parser(const string& input);
    ~Parser();
    shared_ptr<ASTNode> parse(); // Parse the whole program/file
private:
    class ParserImpl; // Forward declaration
    ParserImpl* impl;
};

void printTree(const shared_ptr<ASTNode>& node, int depth = 0); 