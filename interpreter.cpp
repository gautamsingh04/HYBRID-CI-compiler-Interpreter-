#include "interpreter.h"
#include <iostream>
#include <iomanip>
using namespace std;

Interpreter::Interpreter() {}

Value Interpreter::eval(shared_ptr<ASTNode> node) {
    if (auto bin = dynamic_cast<BinaryExpr*>(node.get())) return evalBinaryExpr(bin);
    if (auto lit = dynamic_cast<Literal*>(node.get())) return evalLiteral(lit);
    if (auto id = dynamic_cast<Identifier*>(node.get())) return evalIdentifier(id);
    if (auto assign = dynamic_cast<Assignment*>(node.get())) return evalAssignment(assign);
    if (auto ifStmt = dynamic_cast<IfStmt*>(node.get())) return evalIfStmt(ifStmt);
    if (auto block = dynamic_cast<Block*>(node.get())) return evalBlock(block);
    if (auto whileStmt = dynamic_cast<WhileStmt*>(node.get())) return evalWhileStmt(whileStmt);
    if (auto printStmt = dynamic_cast<PrintStmt*>(node.get())) return evalPrintStmt(printStmt);
    throw runtime_error("Unknown AST node");
}

Value Interpreter::evalBinaryExpr(BinaryExpr* expr) {
    Value left = eval(expr->left);
    Value right = eval(expr->right);

    if (expr->op == "+") return Value{left.asInt() + right.asInt()};
    if (expr->op == "-") return Value{left.asInt() - right.asInt()};
    if (expr->op == "*") return Value{left.asInt() * right.asInt()};
    if (expr->op == "/") return Value{left.asInt() / right.asInt()};
    if (expr->op == "==") return Value{left.asInt() == right.asInt()};
    if (expr->op == "<") return Value{left.asInt() < right.asInt()};
    if (expr->op == ">") return Value{left.asInt() > right.asInt()};
    throw runtime_error("Unknown binary operator: " + expr->op);
}

Value Interpreter::evalLiteral(Literal* expr) {
    return Value{expr->value};
}

Value Interpreter::evalIdentifier(Identifier* expr) {
    if (variables.find(expr->name) == variables.end())
        throw runtime_error("Undefined variable: " + expr->name);
    return variables[expr->name];
}

Value Interpreter::evalAssignment(Assignment* expr) {
    cout << "[Interpreter] Assignment: " << expr->name << " = ...\n";
    Value val = eval(expr->value);
    variables[expr->name] = val;
    cout << "[Interpreter] Assigned " << expr->name << " = " << val.asInt() << "\n";
    return val;
}

Value Interpreter::evalIfStmt(IfStmt* stmt) {
    cout << "[Interpreter] If condition...\n";
    Value cond = eval(stmt->condition);
    if (cond.asBool()) {
        cout << "[Interpreter] Condition true, executing then-branch\n";
        return eval(stmt->thenBranch);
    }
    if (stmt->elseBranch) {
        cout << "[Interpreter] Condition false, executing else-branch\n";
        return eval(stmt->elseBranch);
    }
    cout << "[Interpreter] Condition false, no else-branch\n";
    return Value{0};
}

Value Interpreter::evalBlock(Block* stmt) {
    Value last;
    cout << "\n[Interpreter] Entering block with " << stmt->statements.size() << " statement(s)\n";
    for (auto& s : stmt->statements) {
        cout << "[Interpreter] Evaluating statement...\n";
        last = eval(s);
        cout << "[Interpreter] Variable state: ";
        for (const auto& [k, v] : variables) {
            cout << k << "=" << v.asInt() << " ";
        }
        cout << "\n";
    }
    cout << "[Interpreter] Exiting block\n";
    return last;
}

Value Interpreter::evalWhileStmt(WhileStmt* stmt) {
    cout << "[Interpreter] Entering while loop\n";
    Value last;
    while (eval(stmt->condition).asBool()) {
        last = eval(stmt->body);
        cout << "[Interpreter] Variable state (in while): ";
        for (const auto& [k, v] : variables) {
            cout << k << "=" << v.asInt() << " ";
        }
        cout << "\n";
    }
    cout << "[Interpreter] Exiting while loop\n";
    return last;
}

Value Interpreter::evalPrintStmt(PrintStmt* stmt) {
    Value val = eval(stmt->expr);
    std::cout << "print: " << val.asInt() << std::endl;
    return val;
}
