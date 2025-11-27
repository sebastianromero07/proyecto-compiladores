#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

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

int TypeCheckerVisitor::visit(TernaryExp* exp) {
    return 0;
}

int CodeGenerator::generar(Program* prog) {
    typeChecker.type(prog);
    fun_memoria = typeChecker.fun_memoria;
    isFloat = false; 
    isUnsigned = false; 
    prog->accept(this);
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
int CodeGenerator::visit(IdExp* exp) {
    if (localVars.check(exp->value)) {
        int offset = localVars.lookup(exp->value);
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
            string label = "str_" + to_string(labelCount++);
            out << ".data\n";
            out << label << ": .string " << strExp->value << "\n";
            out << ".text\n";
            out << "    leaq " << label << "(%rip), %rdi\n"; 
            
            it++;
        } else {
            formatExp->accept(this);
            out << "    movq %rax, %rdi\n";
            it++;
        }

        int gp_reg_idx = 0; 
        int xmm_reg_idx = 0; 
        vector<string> gp_regs = {"%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        
        for (; it != exp->args.end(); ++it) {
            (*it)->accept(this); 
            
            if (isFloat) {
                if (xmm_reg_idx < 8) {
                    if (xmm_reg_idx > 0) {
                        out << "    movsd %xmm0, %xmm" << xmm_reg_idx << "\n";
                    }
                    xmm_reg_idx++;
                } else {
             
                }
            } else {
                if (gp_reg_idx < 5) {
                    out << "    movq %rax, " << gp_regs[gp_reg_idx++] << "\n";
                } else {
                    out << "    pushq %rax\n";
                }
            }
        }
        
        out << "    movl $" << xmm_reg_idx << ", %eax\n"; 
        out << "    call printf@PLT\n";
        
    } else {
        vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        int xmm_idx = 0;
        
        for (size_t i = 0; i < exp->args.size() && i < 6; i++) {
            exp->args[i]->accept(this);
            if (isFloat) {
                 out << "    movsd %xmm0, %xmm" << xmm_idx++ << "\n";
            } else {
                 out << "    movq %rax, " << argRegs[i] << "\n";
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
        } else {
            varTypes[name] = TypeDecl::INT_TYPE;
        }

        if (!inFunction) {
            globalVars[name] = true;
            
            if (var.init_value) {
                int value;
                if (var.init_value->isConstant(value)) {
                    globalInitializers[name] = value;
                }
            }
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
    return 0;
}

int CodeGenerator::visit(StructDec* sd) { return 0; }

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
    stm->rhs->accept(this); 

    bool destIsFloat = (varTypes.count(stm->id) && varTypes[stm->id] == TypeDecl::FLOAT_TYPE);

    if (globalVars.count(stm->id)) {
        if (destIsFloat) {
            if (!isFloat) {
                out << "    cvtsi2sd %rax, %xmm0\n";
            }
            out << "    movsd %xmm0, " << stm->id << "(%rip)\n";
        } else {
            if (isFloat) {
                out << "    cvttsd2si %xmm0, %rax\n";
            }
            out << "    movq %rax, " << stm->id << "(%rip)\n";
        }
    } else {
        int off = localVars.lookup(stm->id);
        if (destIsFloat) {
            if (!isFloat) {
                out << "    cvtsi2sd %rax, %xmm0\n";
            }
            out << "    movsd %xmm0, " << off << "(%rbp)\n";
        } else {
            if (isFloat) {
                out << "    cvttsd2si %xmm0, %rax\n";
            }
            out << "    movq %rax, " << off << "(%rbp)\n";
        }
    }

    int v;
    if (evalConstExpr(stm->rhs, v)) {
        isConst[stm->id]  = true;
        constVal[stm->id] = v;
    } else {
        isConst[stm->id] = false;
    }

    return 0;
}
int CodeGenerator::visit(PrintStm* stm) {
    if (stm->args.empty()) return 0;
    
    auto it = stm->args.begin();
    Exp* formatExp = *it;
    
    StringExp* strExp = dynamic_cast<StringExp*>(formatExp);
    
    if (strExp) {
        string label = "str_" + to_string(labelCount++);
        out << ".data\n";
        out << label << ": .string " << strExp->value << "\n";
        out << ".text\n";
        
        if (stm->args.size() == 1) {
            out << "    leaq " << label << "(%rip), %rdi\n";
            out << "    movl $0, %eax\n";
            out << "    call printf@PLT\n";
        } else {
            ++it;  
            (*it)->accept(this);  
            
            if (isFloat) {
                out << "    movsd %xmm0, %xmm0\n"; 
                out << "    movl $1, %eax\n"; 
            } else {
                out << "    movq %rax, %rsi\n";  
                out << "    movl $0, %eax\n";
            }
            
            out << "    leaq " << label << "(%rip), %rdi\n";
            out << "    call printf@PLT\n";
        }
    } else {
        for (auto arg : stm->args) {
            arg->accept(this); 
            
            if (isFloat) {
                out << "    leaq print_float_fmt(%rip), %rdi\n";
                out << "    movl $1, %eax\n"; 
            } else {
                out << "    movq %rax, %rsi\n";
                out << "    leaq print_fmt(%rip), %rdi\n";
                out << "    movl $0, %eax\n";
            }
            out << "    call printf@PLT\n";
        }
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
            bool expectFloat = formatExp->value.find("%f") != string::npos;

            if (stm->fcall->args.size() == 1) {
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    movl $0, %eax\n";
                out << "    call printf@PLT\n";
            } else {
                Exp* arg = stm->fcall->args[1];
                arg->accept(this);

                if (expectFloat && !isFloat) {
                    out << "    cvtsi2sd %rax, %xmm0\n";
                    isFloat = true;
                }
                if (expectFloat) {
                    out << "    movsd %xmm0, %xmm0\n";
                    out << "    movl $1, %eax\n";
                } else {
                    out << "    movq %rax, %rsi\n";
                    out << "    movl $0, %eax\n";
                }
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    call printf@PLT\n";
            }
        }
    } else {
        stm->fcall->accept(this);
    }
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
    out << "print_float_fmt: .string \"%f \\n\"\n";

    // Variables globales
    inFunction = false;
    for (auto vd : prog->vardecs) {
        vd->accept(this);
    }

    for (auto it = globalVars.begin(); it != globalVars.end(); ++it) {
        if (it->second) {
            int initValue = 0;
            if (globalInitializers.count(it->first)) {
                initValue = globalInitializers[it->first];
            }
            out << it->first << ": .quad " << initValue << "\n";
        }
    }
    // Sección de código
    out << ".text\n";
    for (auto fd : prog->fundecs) {
        fd->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}
