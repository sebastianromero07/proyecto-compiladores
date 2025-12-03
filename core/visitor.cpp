#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

int TypeCheckerVisitor::type(Program* program){
    fun_memoria.clear();
    structSizes.clear();
    for (auto sd : program->structdecs) {
        sd->accept(this);
    }
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
    if (vd->type->kind == TypeDecl::STRUCT_TYPE) {
        if (structSizes.count(vd->type->name)) {
            locales += structSizes[vd->type->name];
        } else {
            locales += 1; // Fallback? Or error.
        }
    } else {
        locales += vd->vars.size();
    }
    return 0;
}

int TypeCheckerVisitor::visit(StructDec* sd) {
    int size = 0;
    for (auto field : sd->fields) {
        size += field->vars.size(); // Assume each field var is 1 slot (8 bytes)
    }
    structSizes[sd->name] = size;
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

int TypeCheckerVisitor::visit(ReturnStm* r) {
    return 0;
}
int TypeCheckerVisitor::visit(TernaryExp* exp) {
    return 0;
}
int TypeCheckerVisitor::visit(StructAccessExp* exp) {
    return 0;
}

int CodeGenerator::generar(Program* prog) {
    typeChecker.type(prog);
    fun_memoria = typeChecker.fun_memoria;
    isFloat = false; 
    isUnsigned = false; 
    prog->accept(this);
    out << "\n.section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}

bool CodeGenerator::evalConstExpr(Exp* e, int& value) {
    if (auto num = dynamic_cast<NumberExp*>(e)) {
        value = num->value;
        return true;
    }

    if (auto b = dynamic_cast<BoolExp*>(e)) {
        value = b->value ? 1 : 0;
        return true;
    }

    if (auto id = dynamic_cast<IdExp*>(e)) {
        auto it = isConst.find(id->value);
        if (it != isConst.end() && it->second) {
            value = constVal[id->value];
            return true;
        }
        return false;
    }

    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        int lv, rv;
        if (!evalConstExpr(bin->left, lv))  return false;
        if (!evalConstExpr(bin->right, rv)) return false;

        switch (bin->op) {
            case PLUS_OP:  value = lv + rv;  return true;
            case MINUS_OP: value = lv - rv;  return true;
            case MUL_OP:   value = lv * rv;  return true;
            case DIV_OP:
                if (rv == 0) return false;
                value = lv / rv;
                return true;
            case LT_OP:  value = (lv <  rv) ? 1 : 0; return true;
            case LE_OP:  value = (lv <= rv) ? 1 : 0; return true;
            case GT_OP:  value = (lv >  rv) ? 1 : 0; return true;
            case GE_OP:  value = (lv >= rv) ? 1 : 0; return true;
            case EQ_OP:  value = (lv == rv) ? 1 : 0; return true;
            case NE_OP:  value = (lv != rv) ? 1 : 0; return true;
            default: return false;
        }
    }

    return false;
}


bool CodeGenerator::exprIsFloat(Exp* e) {
    if (dynamic_cast<FloatExp*>(e)) return true;

    if (auto id = dynamic_cast<IdExp*>(e)) {
        auto it = varTypes.find(id->value);
        if (it != varTypes.end()) {
            return it->second == TypeDecl::FLOAT_TYPE;
        }
        // Check struct members? Too complex without full type system.
        return false;
    }
    
    if (auto sa = dynamic_cast<StructAccessExp*>(e)) {
        // We need to know the type of the member.
        // This is hard without context.
        // But we can guess or rely on runtime (codegen time) checks.
        // For now return false, and let visit handle it.
        return false; 
    }

    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        return exprIsFloat(bin->left) || exprIsFloat(bin->right);
    }

    if (auto tern = dynamic_cast<TernaryExp*>(e)) {
        return exprIsFloat(tern->thenExp) || exprIsFloat(tern->elseExp);
    }
    return false;
}

int CodeGenerator::visit(BinaryExp* exp) {
    int constVal;
    if (exp->isConstant(constVal)) {
        out << "    movq $" << constVal << ", %rax\n";
        isFloat = false;      
        isUnsigned = false;   
        return 0;
    }

    exp->left->accept(this);
    bool leftIsFloat = isFloat;
    
    if (leftIsFloat) {
        out << "    subq $8, %rsp\n";
        out << "    movsd %xmm0, (%rsp)\n"; 
    } else {
        out << "    pushq %rax\n"; 
    }
    bool leftIsUnsigned = isUnsigned;

    exp->right->accept(this);
    bool rightIsFloat = isFloat;
    bool rightIsUnsigned = isUnsigned;
    
    if (leftIsFloat || rightIsFloat) {
        if (!rightIsFloat) {
            out << "    cvtsi2sd %rax, %xmm1\n";
        } else {
            out << "    movsd %xmm0, %xmm1\n";
        }
        
        if (leftIsFloat) {
            out << "    movsd (%rsp), %xmm0\n";
            out << "    addq $8, %rsp\n";
        } else {
            out << "    popq %rax\n";
            out << "    cvtsi2sd %rax, %xmm0\n";
        }
        
        switch (exp->op) {
            case PLUS_OP: out << "    addsd %xmm1, %xmm0\n"; break;
            case MINUS_OP: out << "    subsd %xmm1, %xmm0\n"; break;
            case MUL_OP: out << "    mulsd %xmm1, %xmm0\n"; break;
            case DIV_OP: out << "    divsd %xmm1, %xmm0\n"; break;
            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
            case EQ_OP:
            case NE_OP:
                out << "    ucomisd %xmm1, %xmm0\n";
                out << "    movl $0, %eax\n"; 
                switch (exp->op) {
                    case LT_OP: out << "    setb %al\n"; break;
                    case LE_OP: out << "    setbe %al\n"; break;
                    case GT_OP: out << "    seta %al\n"; break;
                    case GE_OP: out << "    setae %al\n"; break;
                    case EQ_OP: out << "    sete %al\n"; break;
                    case NE_OP: out << "    setne %al\n"; break;
                    default: break;
                }
                out << "    movzbq %al, %rax\n";
                isFloat = false; 
                return 0;
            default: break;
        }
        isFloat = true;
    } else {

        out << "    movq %rax, %rcx\n"; // right en %rcx
        out << "    popq %rax\n";       // left en %rax
        
        bool useUnsigned = leftIsUnsigned || rightIsUnsigned;

        switch (exp->op) {
            case PLUS_OP: out << "    addq %rcx, %rax\n"; break;
            case MINUS_OP: out << "    subq %rcx, %rax\n"; break;
            case MUL_OP: out << "    imulq %rcx, %rax\n"; break;
            case DIV_OP:
                out << "    movq $0, %rdx\n";
                if (useUnsigned) {
                    out << "    divq %rcx\n";
                } else {
                    out << "    cqto\n";
                    out << "    idivq %rcx\n";
                }
                break;
            case LT_OP:
                out << "    cmpq %rcx, %rax\n";
                out << "    movl $0, %eax\n";
                if (useUnsigned) out << "    setb %al\n"; else out << "    setl %al\n";
                out << "    movzbq %al, %rax\n";
                break;
            case LE_OP:
                out << "    cmpq %rcx, %rax\n";
                out << "    movl $0, %eax\n";
                if (useUnsigned) out << "    setbe %al\n"; else out << "    setle %al\n";
                out << "    movzbq %al, %rax\n";
                break;
            case GT_OP:
                out << "    cmpq %rcx, %rax\n";
                out << "    movl $0, %eax\n";
                if (useUnsigned) out << "    seta %al\n"; else out << "    setg %al\n";
                out << "    movzbq %al, %rax\n";
                break;
            case GE_OP:
                out << "    cmpq %rcx, %rax\n";
                out << "    movl $0, %eax\n";
                if (useUnsigned) out << "    setae %al\n"; else out << "    setge %al\n";
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
            default: break;
        }
        isFloat = false;
        isUnsigned = useUnsigned;
    }
    return 0;
}


int CodeGenerator::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax\n";
    isFloat = false;
    isUnsigned = false; 
    return 0;
}
int CodeGenerator::visit(FloatExp* exp) {
    string label = ".FL" + to_string(labelCount++);
    out << ".data\n";
    out << label << ": .double " << exp->value << "\n";
    out << ".text\n";
    out << "    movsd " << label << "(%rip), %xmm0\n";
    isFloat = true;
    return 0;
}

// Updated visit(IdExp) to handle address generation and struct types
string lastStructType; // Helper global for this file (or member)

int CodeGenerator::visit(IdExp* exp) {
    if (localVars.check(exp->value)) {
        int offset = localVars.lookup(exp->value);
        
        if (wantAddress) {
            out << "    leaq " << offset << "(%rbp), %rax\n";
            if (varStructTypes.count(exp->value)) {
                lastStructType = varStructTypes[exp->value];
            }
            return 0;
        }

        if (varTypes.count(exp->value) && varTypes[exp->value] == TypeDecl::FLOAT_TYPE) {
            out << "    movsd " << offset << "(%rbp), %xmm0\n";
            isFloat = true;
            isUnsigned = false;
        } else {
            out << "    movq " << offset << "(%rbp), %rax\n";
            isFloat = false;
            if (varTypes.count(exp->value) && varTypes[exp->value] == TypeDecl::UNSIGNED_TYPE) {
                isUnsigned = true;
            } else {
                isUnsigned = false;
            }
        }
    } else if (globalVars.count(exp->value)) {
        if (wantAddress) {
            out << "    leaq " << exp->value << "(%rip), %rax\n";
            if (varStructTypes.count(exp->value)) {
                lastStructType = varStructTypes[exp->value];
            }
            return 0;
        }

        out << "    movq " << exp->value << "(%rip), %rax\n";
        isFloat = false;
        if (varTypes.count(exp->value) && varTypes[exp->value] == TypeDecl::UNSIGNED_TYPE) {
            isUnsigned = true;
        } else {
            isUnsigned = false;
        }
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
    if (exp->fname == "printf") {
        if (exp->args.empty()) return 0;
        
        auto it = exp->args.begin();
        Exp* formatExp = *it;
        StringExp* strExp = dynamic_cast<StringExp*>(formatExp);
        
        if (strExp) {
            string label = getStringLabel(strExp->value);
            
            // Procesar argumentos adicionales
            ++it;
            int argIndex = 0;
            int xmmIndex = 0;
            
            while (it != exp->args.end()) {
                Exp* arg = *it;
                arg->accept(this);
                
                if (isFloat) {
                    // Para floats, ya están en %xmm0
                    if (xmmIndex > 0) {
                        out << "    movsd %xmm0, %xmm" << xmmIndex << "\n";
                    }
                    xmmIndex++;
                } else {
                    // Para enteros
                    switch (argIndex) {
                        case 0: out << "    movq %rax, %rsi\n"; break;
                        case 1: out << "    movq %rax, %rdx\n"; break;
                        case 2: out << "    movq %rax, %rcx\n"; break;
                        case 3: out << "    movq %rax, %r8\n"; break;
                        case 4: out << "    movq %rax, %r9\n"; break;
                        default: break;
                    }
                    argIndex++;
                }
                ++it;
            }
            
            out << "    movl $" << xmmIndex << ", %eax\n";
            out << "    leaq " << label << "(%rip), %rdi\n";
            out << "    call printf@PLT\n";
        }
        
    } else {
        // Otras funciones (no printf)
        vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        int xmm_idx = 0;
        int gp_idx = 0;
        
        for (size_t i = 0; i < exp->args.size() && i < 6; i++) {
            exp->args[i]->accept(this);
            
            if (isFloat) {
                if (xmm_idx > 0) {
                    out << "    movsd %xmm0, %xmm" << xmm_idx << "\n";
                }
                xmm_idx++;
            } else {
                out << "    movq %rax, " << argRegs[gp_idx] << "\n";
                gp_idx++;
            }
        }
        
        out << "    movl $" << xmm_idx << ", %eax\n";
        out << "    call " << exp->fname << "@PLT\n";
    }
    return 0;
}

int CodeGenerator::visit(VarDec* vd) {
    for (auto& var : vd->vars) {
        const string& name = var.name;
        
        if (vd->type->kind == TypeDecl::FLOAT_TYPE) {
            varTypes[name] = TypeDecl::FLOAT_TYPE;
        } else if (vd->type->kind == TypeDecl::UNSIGNED_TYPE) {
            varTypes[name] = TypeDecl::UNSIGNED_TYPE;
        } else if (vd->type->kind == TypeDecl::STRUCT_TYPE) {
            varTypes[name] = TypeDecl::STRUCT_TYPE;
            varStructTypes[name] = vd->type->name;
        } else {
            varTypes[name] = TypeDecl::INT_TYPE;
        }

        if (!inFunction) {
            globalVars[name] = true;
            // Global struct init not supported yet
            if (var.init_value) {
                int value;
                if (var.init_value->isConstant(value)) {
                    globalInitializers[name] = value;
                }
            }
        } else {
            if (vd->type->kind == TypeDecl::STRUCT_TYPE) {
                int size = structDefs[vd->type->name].size;
                offset = offset - size + 8; // Allocate space, packing downwards
                localVars.add_var(name, offset);
                offset -= 8;
                // Struct init not supported
            } else {
                localVars.add_var(name, offset);
                
                if (var.init_value) {
                    var.init_value->accept(this);
                    
                    if (vd->type->kind == TypeDecl::FLOAT_TYPE && !isFloat) {
                        out << "    cvtsi2sd %rax, %xmm0\n";
                        isFloat = true;
                    }
                    
                    if (isFloat) {
                        out << "    movsd %xmm0, " << offset << "(%rbp)\n";
                    } else {
                        out << "    movq %rax, " << offset << "(%rbp)\n";
                    }
                } else {
                    out << "    movq $0, " << offset << "(%rbp)\n";
                }
                
                offset -= 8;
            }
        }
    }
    return 0;
}

int CodeGenerator::visit(StructDec* sd) {
    StructInfo info;
    int currentOffset = 0;
    for (auto field : sd->fields) {
        for (auto& var : field->vars) {
            info.offsets[var.name] = currentOffset;
            info.memberTypes[var.name] = field->type->kind;
            currentOffset += 8; // Assume 8 bytes for everything
        }
    }
    info.size = currentOffset;
    structDefs[sd->name] = info;
    return 0;
}

int CodeGenerator::visit(StructAccessExp* exp) {
    bool oldWantAddress = wantAddress;
    wantAddress = true;
    exp->left->accept(this); // %rax has base address
    wantAddress = oldWantAddress;
    
    // lastStructType should be set by visit(IdExp) or recursive visit(StructAccessExp)
    string structName = lastStructType;
    if (structDefs.count(structName) == 0) {
        // Error or unknown struct
        return 0;
    }
    
    int memberOffset = structDefs[structName].offsets[exp->id];
    TypeDecl::TypeKind memberType = structDefs[structName].memberTypes[exp->id];
    
    out << "    addq $" << memberOffset << ", %rax\n";
    
    // Update lastStructType for next access if needed
    // (If member is also a struct, we need to know its type name. 
    // But our StructInfo only stores TypeKind. We might need to store type name too.
    // For now, assume flat structs or no nested struct access in this test case.)
    
    if (wantAddress) {
        return 0; // %rax has address
    }
    
    if (memberType == TypeDecl::FLOAT_TYPE) {
        out << "    movsd (%rax), %xmm0\n";
        isFloat = true;
    } else {
        out << "    movq (%rax), %rax\n";
        isFloat = false;
    }
    
    return 0;
}

int CodeGenerator::visit(TernaryExp* exp) {
    bool wantFloat = exprIsFloat(exp->thenExp) || exprIsFloat(exp->elseExp);

    int lbl = labelCounter++;

    exp->condition->accept(this);  
    if (isFloat) {
        out << "    cvttsd2si %xmm0, %rax\n";
        isFloat = false;
    }

    out << "    cmpq $0, %rax\n";
    out << "    je tern_else_" << lbl << "\n";

    exp->thenExp->accept(this); 

    if (wantFloat) {
        if (!isFloat) {
            out << "    cvtsi2sd %rax, %xmm0\n";
            isFloat = true;
        }
    } else {
        if (isFloat) {
            out << "    cvttsd2si %xmm0, %rax\n";
            isFloat = false;
        }
    }

    out << "    jmp tern_end_" << lbl << "\n";

    out << "tern_else_" << lbl << ":\n";

    exp->elseExp->accept(this);

    if (wantFloat) {
        if (!isFloat) {
            out << "    cvtsi2sd %rax, %xmm0\n";
            isFloat = true;
        }
    } else {
        if (isFloat) {
            out << "    cvttsd2si %xmm0, %rax\n";
            isFloat = false;
        }
    }

    out << "tern_end_" << lbl << ":\n";

    if (!wantFloat) {
        isUnsigned = false;
    }
    return 0;
}


int CodeGenerator::visit(FunDec* fd) {
    inFunction      = true;
    currentFunction = fd->name;
    isConst.clear();
    constVal.clear();
    localVars.clear();
    localVars.add_level();
    offset = -8;

    out << ".globl " << fd->name << "\n";
    out << fd->name << ":\n";
    out << "    pushq %rbp\n";
    out << "    movq %rsp, %rbp\n";

    int totalSlots = 0;
    if (fun_memoria.count(fd->name)) {
        totalSlots = fun_memoria[fd->name];
    }
    if (totalSlots % 2 != 0) totalSlots++; 

    if (totalSlots > 0) {
        out << "    subq $" << (totalSlots * 8) << ", %rsp\n";
    }

    vector<string> gpArgRegs = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    int gp_idx  = 0;  
    int xmm_idx = 0;   

    int nparams = (int)fd->pnames.size();
    for (int i = 0; i < nparams; ++i) {
        const string& pname = fd->pnames[i];
        TypeDecl* ptype = (i < (int)fd->ptypes.size()) ? fd->ptypes[i] : nullptr;

        if (ptype) {
            if (ptype->kind == TypeDecl::FLOAT_TYPE) {
                varTypes[pname] = TypeDecl::FLOAT_TYPE;
            } else if (ptype->kind == TypeDecl::UNSIGNED_TYPE) {
                varTypes[pname] = TypeDecl::UNSIGNED_TYPE;
            } else if (ptype->kind == TypeDecl::STRUCT_TYPE) {
                varTypes[pname] = TypeDecl::STRUCT_TYPE;
                varStructTypes[pname] = ptype->name;
            } else {
                varTypes[pname] = TypeDecl::INT_TYPE;
            }
        } else {
            varTypes[pname] = TypeDecl::INT_TYPE;
        }

        localVars.add_var(pname, offset);

        if (ptype && ptype->kind == TypeDecl::FLOAT_TYPE) {

            out << "    movsd %xmm" << xmm_idx << ", " << offset << "(%rbp)\n";
            xmm_idx++;
        } else {

            out << "    movq " << gpArgRegs[gp_idx] << ", " << offset << "(%rbp)\n";
            gp_idx++;
        }

        offset -= 8;
    }

    if (fd->body) {
        fd->body->accept(this);
    }

    out << ".end_" << fd->name << ":\n";
    out << "    leave\n";
    out << "    ret\n";

    localVars.remove_level();
    inFunction      = false;
    currentFunction.clear();
    return 0;
}

int CodeGenerator::visit(AssignStm* stm) {
    // Optimización: Si el LHS es un IdExp simple, generar código más eficiente
    IdExp* idLhs = dynamic_cast<IdExp*>(stm->lhs);
    
    if (idLhs && localVars.check(idLhs->value)) {
        // Caso simple: variable local
        int lhsOffset = localVars.lookup(idLhs->value);
        
        // Evaluar RHS
        stm->rhs->accept(this);
        
        // Almacenar directamente
        if (isFloat) {
            out << "    movsd %xmm0, " << lhsOffset << "(%rbp)\n";
        } else {
            out << "    movq %rax, " << lhsOffset << "(%rbp)\n";
        }
        
        // Propagación de constantes (opcional)
        int cv;
        if (stm->rhs->isConstant(cv)) {
            isConst[idLhs->value] = true;
            constVal[idLhs->value] = cv;
        } else {
            isConst[idLhs->value] = false;
        }
        
        return 0;
    }
    
    // Caso general: usar leaq para LHS complejos (structs, arrays, etc.)
    wantAddress = true;
    stm->lhs->accept(this);
    wantAddress = false;
    
    out << "    pushq %rax\n";
    
    stm->rhs->accept(this);
    
    out << "    popq %rcx\n";
    
    if (isFloat) {
        out << "    movsd %xmm0, (%rcx)\n";
    } else {
        out << "    movq %rax, (%rcx)\n";
    }
    
    return 0;
}

int CodeGenerator::visit(PrintStm* stm) {
    if (stm->args.empty()) return 0;
    
    auto it = stm->args.begin();
    Exp* formatExp = *it;
    
    StringExp* strExp = dynamic_cast<StringExp*>(formatExp);
    
    if (strExp) {
        string label = getStringLabel(strExp->value);
        
        // Procesar argumentos adicionales (después del formato)
        ++it;
        int argIndex = 0;
        int xmmIndex = 0;
        
        while (it != stm->args.end()) {
            Exp* arg = *it;
            arg->accept(this);
            
            if (isFloat) {
                // Para floats, ya están en %xmm0, moverlos al registro XMM correspondiente
                if (xmmIndex > 0) {
                    out << "    movsd %xmm0, %xmm" << xmmIndex << "\n";
                }
                xmmIndex++;
            } else {
                // Para enteros, mover de %rax al registro correspondiente
                // printf usa: %rsi, %rdx, %rcx, %r8, %r9 para args después del formato
                switch (argIndex) {
                    case 0: out << "    movq %rax, %rsi\n"; break;
                    case 1: out << "    movq %rax, %rdx\n"; break;
                    case 2: out << "    movq %rax, %rcx\n"; break;
                    case 3: out << "    movq %rax, %r8\n"; break;
                    case 4: out << "    movq %rax, %r9\n"; break;
                    default: break; // Más de 5 argumentos irían al stack
                }
                argIndex++;
            }
            ++it;
        }
        
        // Configurar el número de registros XMM usados
        out << "    movl $" << xmmIndex << ", %eax\n";
        
        // Cargar la dirección del formato en %rdi
        out << "    leaq " << label << "(%rip), %rdi\n";
        
        // Llamar a printf
        out << "    call printf@PLT\n";
    }
    
    return 0;
}

int CodeGenerator::visit(IfStm* stm) {
    int constVal;
    if (evalConstExpr(stm->condition, constVal)) {
        if (constVal != 0) {
            if (stm->thenbody) stm->thenbody->accept(this);
        } else {
            if (stm->elsebody) stm->elsebody->accept(this);
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
        stm->expr->accept(this); 
    }
    out << "    jmp .end_" << currentFunction << "\n";
    return 0;
}

int CodeGenerator::visit(FcallStm* stm) {
    if (!stm->fcall) return 0;

    if (stm->fcall->fname == "printf") {
        if (stm->fcall->args.empty()) return 0;

        Exp* firstArg = stm->fcall->args[0];
        StringExp* formatExp = dynamic_cast<StringExp*>(firstArg);

        if (formatExp) {
            string label = "str_" + to_string(labelCount++);
            out << ".data\n";
            out << label << ": .string " << formatExp->value << "\n";
            out << ".text\n";

            // Detectar formato
            // (Simplified printf logic)
             out << "    leaq " << label << "(%rip), %rdi\n";
             
             int xmm_idx = 0;
             vector<string> gp_regs = {"%rsi", "%rdx", "%rcx", "%r8", "%r9"};
             int gp_idx = 0;
             
             for (size_t i = 1; i < stm->fcall->args.size(); ++i) {
                 stm->fcall->args[i]->accept(this);
                 if (isFloat) {
                     if (xmm_idx < 8) out << "    movsd %xmm0, %xmm" << xmm_idx++ << "\n";
                 } else {
                     if (gp_idx < 5) out << "    movq %rax, " << gp_regs[gp_idx++] << "\n";
                 }
             }
             out << "    movl $" << xmm_idx << ", %eax\n";
             out << "    call printf@PLT\n";
        }
    } else {
        // Normal function call statement
        stm->fcall->accept(this);
    }
    return 0;
}

int CodeGenerator::visit(Body* body) {
    for (auto vd : body->vardecs) {
        vd->accept(this);
    }
    for (auto stm : body->stmts) {
        stm->accept(this);
    }
    return 0;
}

int CodeGenerator::visit(Program* prog) {
    for (auto sd : prog->structdecs) {
        sd->accept(this);
    }
    for (auto vd : prog->vardecs) {
        vd->accept(this);
    }
    for (auto fd : prog->fundecs) {
        fd->accept(this);
    }
    return 0;
}

string CodeGenerator::getStringLabel(const string& s) {
    if (stringLabels.count(s)) return stringLabels[s];
    
    string label = "str_" + to_string(stringCounter++);
    stringLabels[s] = label;
    
    // Emitir la definición del string
    out << ".data\n";
    out << label << ": .string " << s << "\n";
    out << ".text\n";
    
    return label;
}