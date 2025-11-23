#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <algorithm>

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

int TypeCheckerVisitor::type(Program* program){
    fun_memoria.clear();
    for (auto i : program->fundecs) {
        i->accept(this);
    }
    return 0;
}
    
int TypeCheckerVisitor::visit(FunDec* fd) {
    int parametros = fd->ptypes.size();
    locales = 0;
    if (fd->body) {
        fd->body->accept(this);
    }
    fun_memoria[fd->name] = parametros + locales;
    return 0;
}


int TypeCheckerVisitor::visit(Body* body) {
    for(auto i:body->vardecs){
        i->accept(this);
    }
    for(auto i:body->stmts){
        i->accept(this);
    }
    return 0;
}

int TypeCheckerVisitor::visit(VarDec* vd) {
    locales += vd->vars.size();
    return 0;
}


int TypeCheckerVisitor::visit(WhileStm* stm) {
    stm->body->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(IfStm* stm) {
    int a = locales;
    stm->thenbody->accept(this);
    int b = locales;
    if (stm->elsebody) {
        stm->elsebody->accept(this);
    }
    int c = locales;
    locales = a + max(b-a,c-b);
    return 0;
}

int TypeCheckerVisitor::visit(ForStm* stm) {
    if (stm->init) stm->init->accept(this);
    if (stm->body) stm->body->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(FcallStm* stm) {
    return 0;
}

int TypeCheckerVisitor::visit(BinaryExp* exp) {
    return 0;
}
int TypeCheckerVisitor::visit(NumberExp* exp) {
    return 0;
}
int TypeCheckerVisitor::visit(FloatExp* exp)  { 
    return 0; 
}
int TypeCheckerVisitor::visit(IdExp* exp){
    return 0;
}
int TypeCheckerVisitor::visit(BoolExp* exp)   { 
    return 0; 
}
int TypeCheckerVisitor::visit(StringExp* exp) { 
    return 0; 
}
int TypeCheckerVisitor::visit(Program* p) {
    return 0;
}
int TypeCheckerVisitor::visit(PrintStm* stm) {
    return 0;
}
int TypeCheckerVisitor::visit(AssignStm* stm) {
    return 0;
}
int TypeCheckerVisitor::visit(FcallExp* fcall) {
    return 0;
}
int TypeCheckerVisitor::visit(StructDec* sd)  { 
    return 0; 
}
int TypeCheckerVisitor::visit(ReturnStm* r) {
    return 0;
}

int CodeGenerator::generar(Program* prog) {
    typeChecker.type(prog);
    fun_memoria = typeChecker.fun_memoria;
    prog->accept(this);
    return 0;
}


int CodeGenerator::visit(BinaryExp* exp) {
    int constantValue;
    if (exp->isConstant(constantValue)) {
        out << "    movq $" << constantValue << ", %rax\n";
        return 0;
    }

    // Evaluar izquierda y derecha
    exp->left->accept(this);          // left en %rax
    out << "    pushq %rax\n";
    exp->right->accept(this);         // right en %rax
    out << "    movq %rax, %rcx\n";
    out << "    popq %rax\n";         // left en %rax, right en %rcx

    switch (exp->op) {
        case PLUS_OP:
            out << "    addq %rcx, %rax\n";
            break;
        case MINUS_OP:
            out << "    subq %rcx, %rax\n";
            break;
        case MUL_OP:
            out << "    imulq %rcx, %rax\n";
            break;
        case DIV_OP:
            // %rax = left, %rcx = right
            out << "    cqto\n";          // extiende signo a %rdx
            out << "    idivq %rcx\n";    // cociente en %rax
            break;
        case LT_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    setl %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case LE_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    setle %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    setg %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case GE_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    setge %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    sete %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case NE_OP:
            out << "    cmpq %rcx, %rax\n";
            out << "    movl $0, %eax\n";
            out << "    setne %al\n";
            out << "    movzbq %al, %rax\n";
            break;
        case POW_OP: {
            int myLabel = labelCounter++;
            // base = %rax, exp = %rcx
            out << "    movq %rax, %rdx\n";      // base en %rdx
            out << "    movq %rcx, %rax\n";      // exp en %rax
            out << "    movq $1, %rcx\n";        // result en %rcx
            out << "pow_" << myLabel << ":\n";
            out << "    cmpq $0, %rax\n";
            out << "    je endpow_" << myLabel << "\n";
            out << "    imulq %rdx, %rcx\n";
            out << "    decq %rax\n";
            out << "    jmp pow_" << myLabel << "\n";
            out << "endpow_" << myLabel << ":\n";
            out << "    movq %rcx, %rax\n";
            break;
        }
        default:
            // no debería entrar aquí
            break;
    }
    return 0;
}

int CodeGenerator::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax\n";
    return 0;
}
int CodeGenerator::visit(FloatExp* exp) {
    long val = (long)exp->value; // truncamos
    out << "    movq $" << val << ", %rax\n";
    return 0;
}
int CodeGenerator::visit(IdExp* exp) {
    if (globalVars.count(exp->value)) {
        out << "    movq " << exp->value << "(%rip), %rax\n";
    } else {
        int off = localVars.lookup(exp->value);
        out << "    movq " << off << "(%rbp), %rax\n";
    }
    return 0;
}
int CodeGenerator::visit(BoolExp* exp) {
    out << "    movq $" << (exp->value ? 1 : 0) << ", %rax\n";
    return 0;
}

int CodeGenerator::visit(StringExp* exp) {
    string label = getStringLabel(exp->value);
    out << "    leaq " << label << "(%rip), %rax\n";
    return 0;
}

int CodeGenerator::visit(FcallExp* exp) {
    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->args.size();
    for (int i = 0; i < size && i < 6; i++) {
        exp->args[i]->accept(this); 
        out << "    movq %rax, " << argRegs[i] << "\n";
    }
    out << "call " << exp->fname  << endl;
    return 0;
}

int CodeGenerator::visit(VarDec* vd) {
    for (auto it = vd->vars.begin(); it != vd->vars.end(); ++it) {
        const string& name = *it;
        if (!inFunction) {
            globalVars[name] = true;
        } else {
            localVars.add_var(name, offset);
            offset -= 8;
        }
    }
    return 0;
}
int CodeGenerator::visit(StructDec* sd) { return 0; }

int CodeGenerator::visit(FunDec* fd) {
    inFunction      = true;
    currentFunction = fd->name;

    // Entorno de variables locales
    localVars.clear();
    localVars.add_level();
    offset = -8;

    // Prologue
    out << ".globl " << fd->name << "\n";
    out << fd->name << ":\n";
    out << "    pushq %rbp\n";
    out << "    movq %rsp, %rbp\n";

    int totalSlots = 0;
    if (fun_memoria.count(fd->name)) {
        totalSlots = fun_memoria[fd->name];
    }
    if (totalSlots > 0) {
        out << "    subq $" << (totalSlots * 8) << ", %rsp\n";
    }

    // Pasar parámetros a la pila local
    static const char* argRegs[] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    int size = (int)fd->pnames.size();
    for (int i = 0; i < size && i < 6; ++i) {
        const string& pname = fd->pnames[i];
        localVars.add_var(pname, offset);
        out << "    movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -= 8;
    }

    // Cuerpo
    if (fd->body) {
        fd->body->accept(this);
    }

    // etiqueta final para los returns
    out << ".end_" << fd->name << ":\n";
    out << "    leave\n";
    out << "    ret\n";

    localVars.remove_level();
    inFunction      = false;
    currentFunction.clear();
    return 0;
}
int CodeGenerator::visit(AssignStm* stm) {
    stm->rhs->accept(this);  // valor en %rax
    if (globalVars.count(stm->id)) {
        out << "    movq %rax, " << stm->id << "(%rip)\n";
    } else {
        int off = localVars.lookup(stm->id);
        out << "    movq %rax, " << off << "(%rbp)\n";
    }
    return 0;
}
int CodeGenerator::visit(PrintStm* stm) {
    for (auto it = stm->args.begin(); it != stm->args.end(); ++it) {
        // Ignorar argumentos que son StringExp (por ahora)
        if (dynamic_cast<StringExp*>(*it) != nullptr) {
            continue;
        }
        (*it)->accept(this);     // valor en %rax
        out << "    movq %rax, %rsi\n";
        out << "    leaq print_fmt(%rip), %rdi\n";
        out << "    movl $0, %eax\n";
        out << "    call printf@PLT\n";
    }
    return 0;
}
int CodeGenerator::visit(IfStm* stm) {
    int constVal;
    if (stm->condition->isConstant(constVal)) {
        if (constVal != 0) {
            if (stm->thenbody) stm->thenbody->accept(this);
        } else if (stm->elsebody) {
            stm->elsebody->accept(this);
        }
        return 0;
    }

    int label = labelCounter++;

    stm->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    je else_" << label << "\n";

    int savedOffset = offset;
    if (stm->thenbody) stm->thenbody->accept(this);

    out << "    jmp endif_" << label << "\n";
    out << "else_" << label << ":\n";

    offset = savedOffset;
    if (stm->elsebody) stm->elsebody->accept(this);

    out << "endif_" << label << ":\n";
    return 0;
}

int CodeGenerator::visit(WhileStm* stm) {
    int label = labelCounter++;
    out << "while_" << label << ":\n";
    stm->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    je endwhile_" << label << "\n";
    if (stm->body) stm->body->accept(this);
    out << "    jmp while_" << label << "\n";
    out << "endwhile_" << label << ":\n";
    return 0;
}

int CodeGenerator::visit(ForStm* stm) {
    int label = labelCounter++;

    if (stm->init) stm->init->accept(this);

    out << "for_" << label << ":\n";
    if (stm->condition) {
        stm->condition->accept(this);
        out << "    cmpq $0, %rax\n";
        out << "    je endfor_" << label << "\n";
    }

    if (stm->body) stm->body->accept(this);
    if (stm->update) stm->update->accept(this);

    out << "    jmp for_" << label << "\n";
    out << "endfor_" << label << ":\n";
    return 0;
}

int CodeGenerator::visit(ReturnStm* stm) {
    if (stm->expr) {
        stm->expr->accept(this);   // valor en %rax
    }
    out << "    jmp .end_" << currentFunction << "\n";
    return 0;
}

int CodeGenerator::visit(FcallStm* stm) {
    if (stm->fcall) stm->fcall->accept(this);
    return 0;
}

// ====== Estructuras compuestas ======

int CodeGenerator::visit(Body* body) {
    localVars.add_level();
    for (auto vd : body->vardecs) {
        vd->accept(this);
    }
    for (auto stm : body->stmts) {
        stm->accept(this);
    }
    localVars.remove_level();
    return 0;
}
int CodeGenerator::visit(Program* prog) {
    globalVars.clear();

    // Sección de datos
    out << ".data\n";
    out << "print_fmt: .string \"%ld \\n\"\n";

    // Variables globales
    inFunction = false;
    for (auto vd : prog->vardecs) {
        vd->accept(this);
    }

    for (auto it = globalVars.begin(); it != globalVars.end(); ++it) {
        if (it->second) {
            out << it->first << ": .quad 0\n";
        }
    }

    // (structs no generan nada por ahora)
    // Sección de código
    out << ".text\n";
    for (auto fd : prog->fundecs) {
        fd->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}