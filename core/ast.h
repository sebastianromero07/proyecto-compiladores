#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;

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

class TypeDecl {
public:
    enum TypeKind {
        INT_TYPE,
        UNSIGNED_TYPE,
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


class Exp {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Exp() {}
    static string binopToChar(BinaryOp op);
    virtual bool isConstant(int& value) const {return false;}
};


class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    
    BinaryExp(Exp* l, Exp* r, BinaryOp o);
    int accept(Visitor* visitor);
    ~BinaryExp();
    bool isConstant(int& value) const override;
};

class NumberExp : public Exp {
public:
    int value;
    
    NumberExp(int v);
    int accept(Visitor* visitor);
    ~NumberExp();

    bool isConstant(int& value) const override;
};

class FloatExp : public Exp {
public:
    float value;
    
    FloatExp(float v);
    int accept(Visitor* visitor);
    ~FloatExp();
};

class IdExp : public Exp {
public:
    string value;
    
    IdExp(string v);
    int accept(Visitor* visitor);
    ~IdExp();
};

class BoolExp : public Exp {
public:
    bool value;
    
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ~BoolExp();

    bool isConstant(int& value) const override;
};

class StringExp : public Exp {
public:
    string value;
    
    StringExp(string v);
    int accept(Visitor* visitor);
    ~StringExp();
};

class FcallExp : public Exp {
public:
    string fname;
    vector<Exp*> args;
    
    FcallExp(string fname);
    int accept(Visitor* visitor);
    ~FcallExp();
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() {}
};

class VarDec : public Stm {
public:
    TypeDecl* type;

    struct VarInit {
        string name;
        Exp* init_value; 
        
        VarInit(string n, Exp* init = nullptr) : name(n), init_value(init) {}
    };
    
    list<VarInit> vars;
    
    VarDec(TypeDecl* t);
    int accept(Visitor* visitor);
    ~VarDec();

    void addVar(string name, Exp* init_value = nullptr) {
        vars.emplace_back(name, init_value);
    }
};


class StructDec {
public:
    string name;
    list<VarDec*> fields;
    
    StructDec(string n);
    int accept(Visitor* visitor);
    ~StructDec();
};

class Body {
public:
    list<VarDec*> vardecs;
    list<Stm*> stmts;
    
    Body();
    int accept(Visitor* visitor);
    ~Body();
};

class AssignStm : public Stm {
public:
    string id;
    Exp* rhs;
    
    AssignStm(string id, Exp* rhs);
    int accept(Visitor* visitor);
    ~AssignStm();
};

class PrintStm : public Stm {
public:
    list<Exp*> args;
    
    PrintStm();
    int accept(Visitor* visitor);
    ~PrintStm();
};

class IfStm : public Stm {
public:
    Exp* condition;
    Body* thenbody;
    Body* elsebody;
    
    IfStm(Exp* condition, Body* thenbody, Body* elsebody = nullptr);
    int accept(Visitor* visitor);
    ~IfStm();
};

class TernaryExp : public Exp {
public:
    Exp* condition;
    Exp* thenExp;
    Exp* elseExp;

    TernaryExp(Exp* condition, Exp* thenExp, Exp* elseExp);
    int accept(Visitor* visitor);
    ~TernaryExp();
};

class WhileStm : public Stm {
public:
    Exp* condition;
    Body* body;
    
    WhileStm(Exp* condition, Body* body);
    int accept(Visitor* visitor);
    ~WhileStm();
};

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

class ReturnStm : public Stm {
public:
    Exp* expr;
    
    ReturnStm(Exp* expr = nullptr);
    int accept(Visitor* visitor);
    ~ReturnStm();
};

class FcallStm : public Stm {
public:
    FcallExp* fcall;
    
    FcallStm(FcallExp* fcall);
    int accept(Visitor* visitor);
    ~FcallStm();
};

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