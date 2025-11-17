#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// ELIMINAR TODAS LAS IMPLEMENTACIONES DE accept() - Ya están en ast.cpp
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Implementación básica del Interpreter
///////////////////////////////////////////////////////////////////////////////////

Interpreter::Interpreter() : retValue(0), returning(false) {}

int Interpreter::visit(BinaryExp* exp) {
    int left_val = exp->left->accept(this);
    int right_val = exp->right->accept(this);
    
    switch (exp->op) {
        case PLUS_OP: return left_val + right_val;
        case MINUS_OP: return left_val - right_val;
        case MUL_OP: return left_val * right_val;
        case DIV_OP: return left_val / right_val;
        case LT_OP: return left_val < right_val;
        case LE_OP: return left_val <= right_val;
        case EQ_OP: return left_val == right_val;
        default: return 0;
    }
}

int Interpreter::visit(NumberExp* exp) {
    return exp->value;
}

int Interpreter::visit(FloatExp* exp) {
    return (int)exp->value;
}

int Interpreter::visit(IdExp* exp) {
    if (env.check(exp->value)) {
        return env.lookup(exp->value);
    }
    return 0;
}

int Interpreter::visit(BoolExp* exp) {
    return exp->value ? 1 : 0;
}

int Interpreter::visit(StringExp* exp) {
    cout << exp->value;
    return 0;
}

int Interpreter::visit(FcallExp* exp) {
    if (exp->fname == "printf") {
        for (auto arg : exp->args) {
            arg->accept(this);
        }
        cout << endl;
    }
    return 0;
}

int Interpreter::visit(VarDec* vd) {
    for (auto var : vd->vars) {
        env.add_var(var, 0);
    }
    return 0;
}

int Interpreter::visit(StructDec* sd) {
    structs[sd->name] = sd;
    return 0;
}

int Interpreter::visit(FunDec* fd) {
    functions[fd->name] = fd;
    return 0;
}

int Interpreter::visit(AssignStm* stm) {
    int val = stm->rhs->accept(this);
    env.update(stm->id, val);
    return 0;
}

int Interpreter::visit(PrintStm* stm) {
    for (auto arg : stm->args) {
        arg->accept(this);
        cout << " ";
    }
    cout << endl;
    return 0;
}

int Interpreter::visit(IfStm* stm) {
    int condition = stm->condition->accept(this);
    if (condition) {
        stm->thenbody->accept(this);
    } else if (stm->elsebody) {
        stm->elsebody->accept(this);
    }
    return 0;
}

int Interpreter::visit(WhileStm* stm) {
    while (stm->condition->accept(this) && !returning) {
        stm->body->accept(this);
    }
    return 0;
}

int Interpreter::visit(ForStm* stm) {
    env.add_level();
    stm->init->accept(this);
    
    while (stm->condition->accept(this) && !returning) {
        stm->body->accept(this);
        stm->update->accept(this);
    }
    
    env.remove_level();
    return 0;
}

int Interpreter::visit(ReturnStm* stm) {
    if (stm->expr) {
        retValue = stm->expr->accept(this);
    }
    returning = true;
    return retValue;
}

int Interpreter::visit(FcallStm* stm) {
    return stm->fcall->accept(this);
}

int Interpreter::visit(Body* body) {
    env.add_level();
    
    for (auto vd : body->vardecs) {
        vd->accept(this);
    }
    
    for (auto stm : body->stmts) {
        stm->accept(this);
        if (returning) break;
    }
    
    env.remove_level();
    return 0;
}

int Interpreter::visit(Program* prog) {
    for (auto vd : prog->vardecs) {
        vd->accept(this);
    }
    
    for (auto sd : prog->structdecs) {
        sd->accept(this);
    }
    
    for (auto fd : prog->fundecs) {
        fd->accept(this);
    }
    
    if (functions.count("main")) {
        functions["main"]->body->accept(this);
    }
    
    return 0;
}

void Interpreter::interpretar(Program* prog) {
    prog->accept(this);
}

///////////////////////////////////////////////////////////////////////////////////
// Implementaciones básicas para CodeGenerator y PrintVisitor
///////////////////////////////////////////////////////////////////////////////////

// ... (resto de implementaciones sin los métodos accept duplicados)