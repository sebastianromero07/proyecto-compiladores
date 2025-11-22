#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// Implementación completa del Interpreter
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
        case GT_OP: return left_val > right_val;   
        case GE_OP: return left_val >= right_val; 
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
    // ✅ CORREGIR: Procesar string de format para printf
    string format = exp->value;
    // Remover comillas
    if (format.length() >= 2 && format[0] == '"' && format.back() == '"') {
        format = format.substr(1, format.length() - 2);
    }
    
    // Procesar secuencias de escape básicas
    for (size_t i = 0; i < format.length(); i++) {
        if (format[i] == '\\' && i + 1 < format.length()) {
            switch (format[i + 1]) {
                case 'n':
                    cout << '\n';
                    i++; // Saltar el siguiente carácter
                    break;
                case 't':
                    cout << '\t';
                    i++;
                    break;
                case '\\':
                    cout << '\\';
                    i++;
                    break;
                default:
                    cout << format[i];
                    break;
            }
        } else if (format[i] == '%' && i + 1 < format.length()) {
            // Por simplicidad, solo imprimir el placeholder
            cout << format[i] << format[i + 1];
            i++;
        } else {
            cout << format[i];
        }
    }
    return 0;
}


int Interpreter::visit(FcallExp* exp) {
    if (exp->fname == "printf") {
        auto it = exp->args.begin();
        if (it != exp->args.end()) {
            (*it)->accept(this);
            it++;
            while (it != exp->args.end()) {
                cout << " = ";
                int value = (*it)->accept(this);
                cout << value;
                it++;
            }
        }
        cout << endl;
    } else if (functions.count(exp->fname)) {  
        FunDec* func = functions[exp->fname];
        
        // Crear nuevo scope para la función
        env.add_level();
        
        // Asignar parámetros
        auto argIt = exp->args.begin();
        auto paramIt = func->pnames.begin();
        
        while (argIt != exp->args.end() && paramIt != func->pnames.end()) {
            int argValue = (*argIt)->accept(this);
            env.add_var(*paramIt, argValue);
            argIt++;
            paramIt++;
        }
        
        // Ejecutar cuerpo de la función
        bool wasReturning = returning;
        int oldRetValue = retValue;
        returning = false;
        
        func->body->accept(this);
        
        int result = retValue;
        
        // Restaurar estado
        returning = wasReturning;
        retValue = oldRetValue;
        env.remove_level();
        
        return result;
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

CodeGenerator::CodeGenerator() : labelCounter(0), stackOffset(0) {}

string CodeGenerator::newLabel() {
    return "L" + to_string(labelCounter++);
}

void CodeGenerator::emit(const string& instruction) {
    code += "    " + instruction + "\n";
}

// Implementar todos los métodos visit para CodeGenerator
int CodeGenerator::visit(BinaryExp* exp) { return 0; }
int CodeGenerator::visit(NumberExp* exp) { return 0; }
int CodeGenerator::visit(FloatExp* exp) { return 0; }
int CodeGenerator::visit(IdExp* exp) { return 0; }
int CodeGenerator::visit(BoolExp* exp) { return 0; }
int CodeGenerator::visit(StringExp* exp) { return 0; }
int CodeGenerator::visit(FcallExp* exp) { return 0; }
int CodeGenerator::visit(VarDec* vd) { return 0; }
int CodeGenerator::visit(StructDec* sd) { return 0; }
int CodeGenerator::visit(FunDec* fd) { return 0; }
int CodeGenerator::visit(AssignStm* stm) { return 0; }
int CodeGenerator::visit(PrintStm* stm) { return 0; }
int CodeGenerator::visit(IfStm* stm) { return 0; }
int CodeGenerator::visit(WhileStm* stm) { return 0; }
int CodeGenerator::visit(ForStm* stm) { return 0; }
int CodeGenerator::visit(ReturnStm* stm) { return 0; }
int CodeGenerator::visit(FcallStm* stm) { return 0; }
int CodeGenerator::visit(Body* body) { return 0; }
int CodeGenerator::visit(Program* prog) { return 0; }

string CodeGenerator::generateCode(Program* prog) {
    prog->accept(this);
    return code;
}

PrintVisitor::PrintVisitor() : indent(0) {}

void PrintVisitor::printIndent() {
    for (int i = 0; i < indent; i++) {
        output += "  ";
    }
}

// Implementar todos los métodos visit para PrintVisitor
int PrintVisitor::visit(BinaryExp* exp) { return 0; }
int PrintVisitor::visit(NumberExp* exp) { return 0; }
int PrintVisitor::visit(FloatExp* exp) { return 0; }
int PrintVisitor::visit(IdExp* exp) { return 0; }
int PrintVisitor::visit(BoolExp* exp) { return 0; }
int PrintVisitor::visit(StringExp* exp) { return 0; }
int PrintVisitor::visit(FcallExp* exp) { return 0; }
int PrintVisitor::visit(VarDec* vd) { return 0; }
int PrintVisitor::visit(StructDec* sd) { return 0; }
int PrintVisitor::visit(FunDec* fd) { return 0; }
int PrintVisitor::visit(AssignStm* stm) { return 0; }
int PrintVisitor::visit(PrintStm* stm) { return 0; }
int PrintVisitor::visit(IfStm* stm) { return 0; }
int PrintVisitor::visit(WhileStm* stm) { return 0; }
int PrintVisitor::visit(ForStm* stm) { return 0; }
int PrintVisitor::visit(ReturnStm* stm) { return 0; }
int PrintVisitor::visit(FcallStm* stm) { return 0; }
int PrintVisitor::visit(Body* body) { return 0; }
int PrintVisitor::visit(Program* prog) { return 0; }

string PrintVisitor::print(Program* prog) {
    prog->accept(this);
    return output;
}