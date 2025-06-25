#pragma once
#include "parser.h"
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <string>
#include <memory>
using namespace std;

struct Value {
    variant<int, bool> data;

    int asInt() const { return get<int>(data); }
    bool asBool() const { return get<bool>(data); }
};

class Interpreter {
public:
    Interpreter();
    Value eval(shared_ptr<ASTNode> node);

private:
    unordered_map<string, Value> variables;

    Value evalBinaryExpr(BinaryExpr* expr);
    Value evalLiteral(Literal* expr);
    Value evalIdentifier(Identifier* expr);
    Value evalAssignment(Assignment* expr);
    Value evalIfStmt(IfStmt* stmt);
    Value evalBlock(Block* stmt);
    Value evalWhileStmt(WhileStmt* stmt);
    Value evalPrintStmt(PrintStmt* stmt);
};
