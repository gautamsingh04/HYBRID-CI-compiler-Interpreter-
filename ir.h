#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "parser.h"
using namespace std;

// Simple IR instruction set
enum class OpCode {
    PUSH,   // PUSH value
    LOAD,   // LOAD var
    STORE,  // STORE var
    ADD,    // ADD
    SUB,    // SUB
    MUL,    // MUL
    DIV,    // DIV
    GT,     // Greater than
    LT,     // Less than
    EQ,     // Equal
    JZ,     // Jump if zero
    JMP,    // Unconditional jump
    LABEL,  // Label
    NOP,    // No operation
    PRINT   // Print top of stack
};

struct IRInstr {
    OpCode op;
    string arg; // For PUSH (value), LOAD/STORE (var), LABEL (label), JZ/JMP (label)
    IRInstr(OpCode o, const string& a = "") : op(o), arg(a) {}
};

struct IRProgram {
    vector<IRInstr> instructions;
};

inline void printIR(const IRProgram& prog) {
    for (size_t i = 0; i < prog.instructions.size(); ++i) {
        const auto& instr = prog.instructions[i];
        cout << i << ": ";
        switch (instr.op) {
            case OpCode::PUSH: cout << "PUSH " << instr.arg; break;
            case OpCode::LOAD: cout << "LOAD " << instr.arg; break;
            case OpCode::STORE: cout << "STORE " << instr.arg; break;
            case OpCode::ADD: cout << "ADD"; break;
            case OpCode::SUB: cout << "SUB"; break;
            case OpCode::MUL: cout << "MUL"; break;
            case OpCode::DIV: cout << "DIV"; break;
            case OpCode::GT: cout << "GT"; break;
            case OpCode::LT: cout << "LT"; break;
            case OpCode::EQ: cout << "EQ"; break;
            case OpCode::JZ: cout << "JZ " << instr.arg; break;
            case OpCode::JMP: cout << "JMP " << instr.arg; break;
            case OpCode::LABEL: cout << "LABEL " << instr.arg; break;
            case OpCode::NOP: cout << "NOP"; break;
            case OpCode::PRINT: cout << "PRINT"; break;
        }
        cout << endl;
    }
}

inline int toInt(const string& s) {
    return stoi(s);
}

// Simple stack-based VM to execute IR
class IRVM {
public:
    void run(const IRProgram& prog);
};

inline void IRVM::run(const IRProgram& prog) {
    unordered_map<string, size_t> labels;
    for (size_t i = 0; i < prog.instructions.size(); ++i) {
        if (prog.instructions[i].op == OpCode::LABEL) {
            labels[prog.instructions[i].arg] = i;
        }
    }
    vector<int> stack;
    unordered_map<string, int> vars;
    for (size_t ip = 0; ip < prog.instructions.size(); ++ip) {
        const auto& instr = prog.instructions[ip];
        cout << "[VM] Executing: ";
        switch (instr.op) {
            case OpCode::PUSH: cout << "PUSH " << instr.arg; break;
            case OpCode::LOAD: cout << "LOAD " << instr.arg; break;
            case OpCode::STORE: cout << "STORE " << instr.arg; break;
            case OpCode::ADD: cout << "ADD"; break;
            case OpCode::SUB: cout << "SUB"; break;
            case OpCode::MUL: cout << "MUL"; break;
            case OpCode::DIV: cout << "DIV"; break;
            case OpCode::GT: cout << "GT"; break;
            case OpCode::LT: cout << "LT"; break;
            case OpCode::EQ: cout << "EQ"; break;
            case OpCode::JZ: cout << "JZ " << instr.arg; break;
            case OpCode::JMP: cout << "JMP " << instr.arg; break;
            case OpCode::LABEL: cout << "LABEL " << instr.arg; break;
            case OpCode::NOP: cout << "NOP"; break;
            case OpCode::PRINT: cout << "PRINT"; break;
        }
        cout << endl;
        switch (instr.op) {
            case OpCode::PUSH:
                stack.push_back(toInt(instr.arg));
                break;
            case OpCode::LOAD:
                stack.push_back(vars[instr.arg]);
                break;
            case OpCode::STORE: {
                int val = stack.back(); stack.pop_back();
                vars[instr.arg] = val;
                break;
            }
            case OpCode::ADD: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a + b);
                break;
            }
            case OpCode::SUB: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a - b);
                break;
            }
            case OpCode::MUL: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a * b);
                break;
            }
            case OpCode::DIV: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a / b);
                break;
            }
            case OpCode::GT: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a > b ? 1 : 0);
                break;
            }
            case OpCode::LT: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a < b ? 1 : 0);
                break;
            }
            case OpCode::EQ: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a == b ? 1 : 0);
                break;
            }
            case OpCode::JZ: {
                int cond = stack.back(); stack.pop_back();
                if (cond == 0) {
                    ip = labels[instr.arg];
                }
                break;
            }
            case OpCode::JMP:
                ip = labels[instr.arg];
                break;
            case OpCode::LABEL:
            case OpCode::NOP:
                break;
            case OpCode::PRINT: {
                int val = stack.back(); stack.pop_back();
                std::cout << "print: " << val << std::endl;
                break;
            }
        }
        cout << "[VM] Stack: ";
        for (auto v : stack) cout << v << " ";
        cout << "| Vars: ";
        for (const auto& [k, v] : vars) cout << k << "=" << v << " ";
        cout << endl;
    }
    cout << "\n=== VM Variable State ===\n";
    for (const auto& [k, v] : vars) {
        cout << k << " = " << v << endl;
    }
}

inline void compileAST(const shared_ptr<ASTNode>& node, IRProgram& ir, int& labelCount) {
    if (!node) return;
    if (auto block = dynamic_pointer_cast<Block>(node)) {
        for (auto& stmt : block->statements) compileAST(stmt, ir, labelCount);
    } else if (auto assign = dynamic_pointer_cast<Assignment>(node)) {
        compileAST(assign->value, ir, labelCount);
        ir.instructions.emplace_back(OpCode::STORE, assign->name);
        cout << "[Compiler] STORE " << assign->name << endl;
    } else if (auto bin = dynamic_pointer_cast<BinaryExpr>(node)) {
        compileAST(bin->left, ir, labelCount);
        compileAST(bin->right, ir, labelCount);
        if (bin->op == "+") { ir.instructions.emplace_back(OpCode::ADD); cout << "[Compiler] ADD" << endl; }
        else if (bin->op == "-") { ir.instructions.emplace_back(OpCode::SUB); cout << "[Compiler] SUB" << endl; }
        else if (bin->op == "*") { ir.instructions.emplace_back(OpCode::MUL); cout << "[Compiler] MUL" << endl; }
        else if (bin->op == "/") { ir.instructions.emplace_back(OpCode::DIV); cout << "[Compiler] DIV" << endl; }
        else if (bin->op == ">") { ir.instructions.emplace_back(OpCode::GT); cout << "[Compiler] GT" << endl; }
        else if (bin->op == "<") { ir.instructions.emplace_back(OpCode::LT); cout << "[Compiler] LT" << endl; }
        else if (bin->op == "==") { ir.instructions.emplace_back(OpCode::EQ); cout << "[Compiler] EQ" << endl; }
    } else if (auto lit = dynamic_pointer_cast<Literal>(node)) {
        ir.instructions.emplace_back(OpCode::PUSH, to_string(lit->value));
        cout << "[Compiler] PUSH " << lit->value << endl;
    } else if (auto id = dynamic_pointer_cast<Identifier>(node)) {
        ir.instructions.emplace_back(OpCode::LOAD, id->name);
        cout << "[Compiler] LOAD " << id->name << endl;
    } else if (auto iff = dynamic_pointer_cast<IfStmt>(node)) {
        string elseLabel = "L_else_" + to_string(labelCount++);
        string endLabel = "L_end_" + to_string(labelCount++);
        compileAST(iff->condition, ir, labelCount);
        ir.instructions.emplace_back(OpCode::JZ, elseLabel);
        cout << "[Compiler] JZ " << elseLabel << endl;
        compileAST(iff->thenBranch, ir, labelCount);
        ir.instructions.emplace_back(OpCode::JMP, endLabel);
        cout << "[Compiler] JMP " << endLabel << endl;
        ir.instructions.emplace_back(OpCode::LABEL, elseLabel);
        cout << "[Compiler] LABEL " << elseLabel << endl;
        if (iff->elseBranch) compileAST(iff->elseBranch, ir, labelCount);
        ir.instructions.emplace_back(OpCode::LABEL, endLabel);
        cout << "[Compiler] LABEL " << endLabel << endl;
    } else if (auto wh = dynamic_pointer_cast<WhileStmt>(node)) {
        string startLabel = "L_start_" + to_string(labelCount++);
        string endLabel = "L_end_" + to_string(labelCount++);
        ir.instructions.emplace_back(OpCode::LABEL, startLabel);
        cout << "[Compiler] LABEL " << startLabel << endl;
        compileAST(wh->condition, ir, labelCount);
        ir.instructions.emplace_back(OpCode::JZ, endLabel);
        cout << "[Compiler] JZ " << endLabel << endl;
        compileAST(wh->body, ir, labelCount);
        ir.instructions.emplace_back(OpCode::JMP, startLabel);
        cout << "[Compiler] JMP " << startLabel << endl;
        ir.instructions.emplace_back(OpCode::LABEL, endLabel);
    } else if (auto print = dynamic_pointer_cast<PrintStmt>(node)) {
        compileAST(print->expr, ir, labelCount);
        ir.instructions.emplace_back(OpCode::PRINT);
        cout << "[Compiler] PRINT" << endl;
    }
} 