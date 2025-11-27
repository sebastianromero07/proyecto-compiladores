#include "ast.h"
#include <iostream>
#include "visitor.h"

using namespace std;

// ------------------ TypeDecl ------------------
TypeDecl::TypeDecl(TypeKind k, string n) : kind(k), name(n) {}

TypeDecl::~TypeDecl() {}

string TypeDecl::toString() const {
    switch (kind) {
        case INT_TYPE: return "int";
        case UNSIGNED_TYPE: return "unsigned " + name;
        case FLOAT_TYPE: return "float";
        case STRUCT_TYPE: return "struct " + name;
        case ID_TYPE: return name;
        default: return "unknown";
    }
}

// ------------------ Exp ------------------
string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MUL_OP: return "*";
        case DIV_OP: return "/";
        case POW_OP: return "^";
        case LE_OP: return "<=";
        case LT_OP: return "<";
        case GE_OP: return ">=";
        case GT_OP: return ">";
        case EQ_OP: return "==";
        case NE_OP: return "!=";
        default: return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

bool BinaryExp::isConstant(int& value) const {
    int lv, rv;
    if (!left->isConstant(lv) || !right->isConstant(rv)) {
        return false;
    }

    switch (op) {
        case PLUS_OP:
            value = lv + rv;
            return true;
        case MINUS_OP:
            value = lv - rv;
            return true;
        case MUL_OP:
            value = lv * rv;
            return true;
        case DIV_OP:
            if (rv == 0) return false;
            value = lv / rv;
            return true;
        case LT_OP:
            value = (lv < rv) ? 1 : 0;
            return true;
        case LE_OP:
            value = (lv <= rv) ? 1 : 0;
            return true;
        case GT_OP:
            value = (lv > rv) ? 1 : 0;
            return true;
        case GE_OP:
            value = (lv >= rv) ? 1 : 0;
            return true;
        case EQ_OP:
            value = (lv == rv) ? 1 : 0;
            return true;
        case NE_OP:
            value = (lv != rv) ? 1 : 0;
            return true;

        default:
            return false;
    }
}


// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

bool NumberExp::isConstant(int& value) const {
    value = this->value;
    return true;
}

// ------------------ FloatExp ------------------
FloatExp::FloatExp(float v) : value(v) {}

FloatExp::~FloatExp() {}

int FloatExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ IdExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ BoolExp ------------------
BoolExp::BoolExp(bool v) : value(v) {}

BoolExp::~BoolExp() {}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

bool BoolExp::isConstant(int& value) const {
    value = this->value ? 1 : 0;
    return true;
}

// ------------------ StringExp ------------------
StringExp::StringExp(string v) : value(v) {}

StringExp::~StringExp() {}

int StringExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ FcallExp ------------------
FcallExp::FcallExp(string fname) : fname(fname) {}

FcallExp::~FcallExp() {
    for (Exp* arg : args) {
        delete arg;
    }
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ VarDec ------------------
VarDec::VarDec(TypeDecl* t) : type(t) {}

VarDec::~VarDec() {
    delete type;
    for (auto& var : vars) {
        if (var.init_value) {
            delete var.init_value;
        }
    }
}

int VarDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ StructDec ------------------
StructDec::StructDec(string n) : name(n) {}

StructDec::~StructDec() {
    for (auto field : fields) {
        delete field;
    }
}

int StructDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ Body ------------------
Body::Body() {}

Body::~Body() {
    for (auto vd : vardecs) {
        delete vd;
    }
    for (auto stm : stmts) {
        delete stm;
    }
}

int Body::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ AssignStm ------------------
AssignStm::AssignStm(string id, Exp* rhs) : id(id), rhs(rhs) {}

AssignStm::~AssignStm() {
    delete rhs;
}

int AssignStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ PrintStm ------------------
PrintStm::PrintStm() {}

PrintStm::~PrintStm() {
    for (auto arg : args) {
        delete arg;
    }
}

int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ IfStm ------------------
IfStm::IfStm(Exp* condition, Body* thenbody, Body* elsebody) 
    : condition(condition), thenbody(thenbody), elsebody(elsebody) {}

IfStm::~IfStm() {
    delete condition;
    delete thenbody;
    delete elsebody;
}

int IfStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ TernaryExp ------------------
TernaryExp::TernaryExp(Exp* condition, Exp* thenExp, Exp* elseExp)
    : condition(condition), thenExp(thenExp), elseExp(elseExp) {}

TernaryExp::~TernaryExp() {
    delete condition;
    delete thenExp;
    delete elseExp;
}

int TernaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ WhileStm ------------------
WhileStm::WhileStm(Exp* condition, Body* body) : condition(condition), body(body) {}

WhileStm::~WhileStm() {
    delete condition;
    delete body;
}

int WhileStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ ForStm ------------------
ForStm::ForStm(Stm* init, Exp* condition, AssignStm* update, Body* body)
    : init(init), condition(condition), update(update), body(body) {}

ForStm::~ForStm() {
    delete init;
    delete condition;
    delete update;
    delete body;
}

int ForStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ ReturnStm ------------------
ReturnStm::ReturnStm(Exp* expr) : expr(expr) {}

ReturnStm::~ReturnStm() {
    delete expr;
}

int ReturnStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ FcallStm ------------------
FcallStm::FcallStm(FcallExp* fcall) : fcall(fcall) {}

FcallStm::~FcallStm() {
    delete fcall;
}

int FcallStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ FunDec ------------------
FunDec::FunDec(TypeDecl* rtype, string name, vector<TypeDecl*> ptypes, 
               vector<string> pnames, Body* body)
    : rtype(rtype), name(name), ptypes(ptypes), pnames(pnames), body(body) {}

FunDec::~FunDec() {
    delete rtype;
    for (auto ptype : ptypes) {
        delete ptype;
    }
    delete body;
}

int FunDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// ------------------ Program ------------------
Program::Program() {}

Program::~Program() {
    for (auto vd : vardecs) {
        delete vd;
    }
    for (auto sd : structdecs) {
        delete sd;
    }
    for (auto fd : fundecs) {
        delete fd;
    }
}

int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}