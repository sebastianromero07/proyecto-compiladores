#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include "environment.h"
#include <string>

using namespace std;

// Forward declarations para todas las clases del AST
class BinaryExp;
class NumberExp;
class FloatExp;
class IdExp;
class BoolExp;
class StringExp;
class FcallExp;
class Program;
class VarDec;
class StructDec;
class Body;
class AssignStm;
class PrintStm;
class IfStm;
class WhileStm;
class ForStm;
class ReturnStm;
class FcallStm;
class FunDec;

// Clase abstracta Visitor
class Visitor {
public:
    virtual ~Visitor() {}
    
    // Métodos virtuales puros para expresiones
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(FloatExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(StringExp* exp) = 0;
    virtual int visit(FcallExp* exp) = 0;
    
    // Métodos virtuales puros para declaraciones
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(StructDec* sd) = 0;
    virtual int visit(FunDec* fd) = 0;
    
    // Métodos virtuales puros para statements
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(ForStm* stm) = 0;
    virtual int visit(ReturnStm* stm) = 0;
    virtual int visit(FcallStm* stm) = 0;
    
    // Métodos virtuales puros para estructuras compuestas
    virtual int visit(Body* body) = 0;
    virtual int visit(Program* prog) = 0;
};

// Visitor para interpretar el código
class Interpreter : public Visitor {
private:
    Environment<int> env;
    unordered_map<string, FunDec*> functions;
    unordered_map<string, StructDec*> structs;
    int retValue;
    bool returning;
    
public:
    Interpreter();
    
    // Implementaciones para expresiones
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(FcallExp* exp) override;
    
    // Implementaciones para declaraciones
    int visit(VarDec* vd) override;
    int visit(StructDec* sd) override;
    int visit(FunDec* fd) override;
    
    // Implementaciones para statements
    int visit(AssignStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(FcallStm* stm) override;
    
    // Implementaciones para estructuras compuestas
    int visit(Body* body) override;
    int visit(Program* prog) override;
    
    // Método para ejecutar el programa
    void interpretar(Program* prog);
};

// Visitor para generar código assembly x86 AT&T
class CodeGenerator : public Visitor {
private:
    string code;
    int labelCounter;
    int stackOffset;
    Environment<int> localVars;
    unordered_map<string, FunDec*> functions;
    unordered_map<string, StructDec*> structs;
    
    string newLabel();
    void emit(const string& instruction);
    
public:
    CodeGenerator();
    
    // Implementaciones para expresiones
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(FcallExp* exp) override;
    
    // Implementaciones para declaraciones
    int visit(VarDec* vd) override;
    int visit(StructDec* sd) override;
    int visit(FunDec* fd) override;
    
    // Implementaciones para statements
    int visit(AssignStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(FcallStm* stm) override;
    
    // Implementaciones para estructuras compuestas
    int visit(Body* body) override;
    int visit(Program* prog) override;
    
    // Método para generar código
    string generateCode(Program* prog);
};

// Visitor para imprimir el AST (útil para debug)
class PrintVisitor : public Visitor {
private:
    int indent;
    string output;
    
    void printIndent();
    
public:
    PrintVisitor();
    
    // Implementaciones para expresiones
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(FcallExp* exp) override;
    
    // Implementaciones para declaraciones
    int visit(VarDec* vd) override;
    int visit(StructDec* sd) override;
    int visit(FunDec* fd) override;
    
    // Implementaciones para statements
    int visit(AssignStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(FcallStm* stm) override;
    
    // Implementaciones para estructuras compuestas
    int visit(Body* body) override;
    int visit(Program* prog) override;
    
    // Método para obtener la representación del AST
    string print(Program* prog);
};

#endif // VISITOR_H