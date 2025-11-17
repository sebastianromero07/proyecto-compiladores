#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;

// Operadores binarios soportados
enum BinaryOp {
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    POW_OP,
    LE_OP,
    LT_OP,
    GE_OP,
    GT_OP,
    EQ_OP,
    NE_OP
};

// Estructura de tipo
class TypeDecl {
public:
    enum TypeKind {
        INT_TYPE,
        UNSIGNED_INT_TYPE,
        FLOAT_TYPE,
        STRUCT_TYPE,
        ID_TYPE
    };
    
    TypeKind kind;
    string name;
    
    TypeDecl(TypeKind k, string n = "");
    string toString() const;
    ~TypeDecl();
};

// Clase abstracta Exp
class Exp {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Exp() {}
    static string binopToChar(BinaryOp op);
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    
    BinaryExp(Exp* l, Exp* r, BinaryOp o);
    int accept(Visitor* visitor);
    ~BinaryExp();
};

// Expresión numérica entera
class NumberExp : public Exp {
public:
    int value;
    
    NumberExp(int v);
    int accept(Visitor* visitor);
    ~NumberExp();
};

// Expresión numérica flotante
class FloatExp : public Exp {
public:
    float value;
    
    FloatExp(float v);
    int accept(Visitor* visitor);
    ~FloatExp();
};

// Expresión de identificador
class IdExp : public Exp {
public:
    string value;
    
    IdExp(string v);
    int accept(Visitor* visitor);
    ~IdExp();
};

// Expresión booleana
class BoolExp : public Exp {
public:
    bool value;
    
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ~BoolExp();
};

// Expresión de string
class StringExp : public Exp {
public:
    string value;
    
    StringExp(string v);
    int accept(Visitor* visitor);
    ~StringExp();
};

// Expresión de llamada a función
class FcallExp : public Exp {
public:
    string fname;
    list<Exp*> args;
    
    FcallExp(string fname);
    int accept(Visitor* visitor);
    ~FcallExp();
};

// Clase abstracta Stm (Statement)
class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() {}
};

// Declaración de variables - HEREDA DE Stm
class VarDec : public Stm {
public:
    TypeDecl* type;
    list<string> vars;
    
    VarDec(TypeDecl* t);
    int accept(Visitor* visitor);
    ~VarDec();
};

// Declaración de estructura
class StructDec {
public:
    string name;
    list<VarDec*> fields;
    
    StructDec(string n);
    int accept(Visitor* visitor);
    ~StructDec();
};

// Cuerpo de función o bloque
class Body {
public:
    list<VarDec*> vardecs;
    list<Stm*> stmts;
    
    Body();
    int accept(Visitor* visitor);
    ~Body();
};

// Statement de asignación
class AssignStm : public Stm {
public:
    string id;
    Exp* rhs;
    
    AssignStm(string id, Exp* rhs);
    int accept(Visitor* visitor);
    ~AssignStm();
};

// Statement de printf
class PrintStm : public Stm {
public:
    list<Exp*> args;
    
    PrintStm();
    int accept(Visitor* visitor);
    ~PrintStm();
};

// Statement de if
class IfStm : public Stm {
public:
    Exp* condition;
    Body* thenbody;
    Body* elsebody;
    
    IfStm(Exp* condition, Body* thenbody, Body* elsebody = nullptr);
    int accept(Visitor* visitor);
    ~IfStm();
};

// Statement de while
class WhileStm : public Stm {
public:
    Exp* condition;
    Body* body;
    
    WhileStm(Exp* condition, Body* body);
    int accept(Visitor* visitor);
    ~WhileStm();
};

// Statement de for
class ForStm : public Stm {
public:
    Stm* init;
    Exp* condition;
    AssignStm* update;
    Body* body;
    
    ForStm(Stm* init, Exp* condition, AssignStm* update, Body* body);
    int accept(Visitor* visitor);
    ~ForStm();
};

// Statement de return
class ReturnStm : public Stm {
public:
    Exp* expr;
    
    ReturnStm(Exp* expr = nullptr);
    int accept(Visitor* visitor);
    ~ReturnStm();
};

// Statement de llamada a función
class FcallStm : public Stm {
public:
    FcallExp* fcall;
    
    FcallStm(FcallExp* fcall);
    int accept(Visitor* visitor);
    ~FcallStm();
};

// Declaración de función
class FunDec {
public:
    TypeDecl* rtype;
    string name;
    vector<TypeDecl*> ptypes;
    vector<string> pnames;
    Body* body;
    
    FunDec(TypeDecl* rtype, string name, vector<TypeDecl*> ptypes, 
           vector<string> pnames, Body* body);
    int accept(Visitor* visitor);
    ~FunDec();
};

// Programa principal
class Program {
public:
    list<VarDec*> vardecs;
    list<StructDec*> structdecs;
    list<FunDec*> fundecs;
    
    Program();
    int accept(Visitor* visitor);
    ~Program();
};

#endif // AST_H