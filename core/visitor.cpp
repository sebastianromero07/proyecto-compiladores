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

int CodeGenerator::generar(Program* prog) {
    typeChecker.type(prog);
    fun_memoria = typeChecker.fun_memoria;
    isFloat = false; // Reset state
    isUnsigned = false; // Reset state
    prog->accept(this);
    return 0;
}


int CodeGenerator::visit(BinaryExp* exp) {
    // Evaluar izquierda
    exp->left->accept(this);
    bool leftIsFloat = isFloat;
    
    if (leftIsFloat) {
        out << "    subq $8, %rsp\n";
        out << "    movsd %xmm0, (%rsp)\n"; // Guardar float en stack
    } else {
        out << "    pushq %rax\n"; // Guardar int en stack
    }
    bool leftIsUnsigned = isUnsigned;

    // Evaluar derecha
    exp->right->accept(this);
    bool rightIsFloat = isFloat;
    bool rightIsUnsigned = isUnsigned;
    
    // Si alguno es float, convertir el otro a float si es necesario y operar en XMM
    if (leftIsFloat || rightIsFloat) {
        if (!rightIsFloat) {
            // Convertir right (en %rax) a float en %xmm1
            out << "    cvtsi2sd %rax, %xmm1\n";
        } else {
            // right ya está en %xmm0, mover a %xmm1
            out << "    movsd %xmm0, %xmm1\n";
        }
        
        // Recuperar left
        if (leftIsFloat) {
            out << "    movsd (%rsp), %xmm0\n";
            out << "    addq $8, %rsp\n";
        } else {
            out << "    popq %rax\n";
            out << "    cvtsi2sd %rax, %xmm0\n";
        }
        
        // Operar %xmm0 (left) y %xmm1 (right)
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
                out << "    movl $0, %eax\n"; // Reset eax
                switch (exp->op) {
                    case LT_OP: out << "    setb %al\n"; break; // setb para unsigned/float <
                    case LE_OP: out << "    setbe %al\n"; break;
                    case GT_OP: out << "    seta %al\n"; break;
                    case GE_OP: out << "    setae %al\n"; break;
                    case EQ_OP: out << "    sete %al\n"; break; // sete funciona igual
                    case NE_OP: out << "    setne %al\n"; break;
                    default: break;
                }
                out << "    movzbq %al, %rax\n";
                isFloat = false; // Resultado de comparación es int (bool)
                return 0;
            default: break;
        }
        isFloat = true; // Resultado es float
    } else {
        // Enteros normales (signed o unsigned)
        out << "    movq %rax, %rcx\n"; // right en %rcx
        out << "    popq %rax\n";       // left en %rax
        
        bool useUnsigned = leftIsUnsigned || rightIsUnsigned;

        switch (exp->op) {
            case PLUS_OP: out << "    addq %rcx, %rax\n"; break;
            case MINUS_OP: out << "    subq %rcx, %rax\n"; break;
            case MUL_OP: out << "    imulq %rcx, %rax\n"; break; // imulq funciona para ambos (truncamiento igual)
            case DIV_OP:
                out << "    movq $0, %rdx\n"; // Limpiar rdx para div
                if (useUnsigned) {
                    out << "    divq %rcx\n"; // Unsigned div
                } else {
                    out << "    cqto\n";      // Sign extension rax->rdx:rax
                    out << "    idivq %rcx\n"; // Signed div
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
        isUnsigned = useUnsigned; // Propagar unsignedness
    }
    return 0;
}

int CodeGenerator::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax\n";
    isFloat = false;
    isUnsigned = false; // Literales numéricos son signed int por defecto
    return 0;
}
int CodeGenerator::visit(FloatExp* exp) {
    // Generar label para el float
    string label = ".FL" + to_string(labelCount++);
    out << ".data\n";
    // Usar .long para float (32-bit) o .quad para double (64-bit). Usaremos double.
    // Hack: escribir el valor double como hex o usar directiva .double si el ensamblador lo soporta
    // GAS soporta .double
    out << label << ": .double " << exp->value << "\n";
    out << ".text\n";
    out << "    movsd " << label << "(%rip), %xmm0\n";
    isFloat = true;
    return 0;
}
int CodeGenerator::visit(IdExp* exp) {
    if (localVars.check(exp->value)) {
        int offset = localVars.lookup(exp->value);
        // Check type
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
        // Asumir int por ahora para globales o necesitaríamos mapa de tipos globales
        out << "    movq " << exp->value << "(%rip), %rax\n";
        isFloat = false;
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

// En visitor.cpp, línea ~530, VERIFICAR que esté así:
int CodeGenerator::visit(FcallExp* exp) {
    if (exp->fname == "printf") {
        if (exp->args.empty()) return 0;
        
        // 1. Manejar el formato (primer argumento)
        auto it = exp->args.begin();
        Exp* formatExp = *it;
        StringExp* strExp = dynamic_cast<StringExp*>(formatExp);
        
        if (strExp) {
            string label = "str_" + to_string(labelCount++);
            out << ".data\n";
            out << label << ": .string " << strExp->value << "\n";
            out << ".text\n";
            out << "    leaq " << label << "(%rip), %rdi\n"; // 1er argumento (formato) en %rdi
            
            // Avanzar al siguiente argumento
            it++;
        } else {
            // Si el primer argumento no es string literal, evaluarlo y ponerlo en %rdi
            formatExp->accept(this);
            out << "    movq %rax, %rdi\n";
            it++;
        }

        // 2. Manejar argumentos restantes (variadic)
        int gp_reg_idx = 0; // Índice para registros de propósito general (rsi, rdx, rcx, r8, r9)
        int xmm_reg_idx = 0; // Índice para registros XMM (xmm0 - xmm7)
        vector<string> gp_regs = {"%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        
        for (; it != exp->args.end(); ++it) {
            (*it)->accept(this); // Evaluar argumento -> %rax (int) o %xmm0 (float)
            
            if (isFloat) {
                if (xmm_reg_idx < 8) {
                    if (xmm_reg_idx > 0) {
                        out << "    movsd %xmm0, %xmm" << xmm_reg_idx << "\n";
                    }
                    // Si xmm_reg_idx == 0, ya está en %xmm0
                    xmm_reg_idx++;
                } else {
                    // TODO: Manejar spill a stack si hay más de 8 floats
                }
            } else {
                if (gp_reg_idx < 5) {
                    out << "    movq %rax, " << gp_regs[gp_reg_idx++] << "\n";
                } else {
                    out << "    pushq %rax\n"; // Pasar en stack (reverse order needed technically)
                }
            }
        }
        
        out << "    movl $" << xmm_reg_idx << ", %eax\n"; // Número de registros vectoriales usados
        out << "    call printf@PLT\n";
        
    } else {
        // Otras funciones (convención estándar)
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
        out << "    movl $" << xmm_idx << ", %eax\n"; // Importante para funciones variadic o sin prototipo
        out << "    call " << exp->fname << "@PLT\n";
    }
    return 0;
}

int CodeGenerator::visit(VarDec* vd) {
    for (auto& var : vd->vars) {
        const string& name = var.name;
        
        // Registrar tipo
        if (vd->type->kind == TypeDecl::FLOAT_TYPE) {
            varTypes[name] = TypeDecl::FLOAT_TYPE;
        } else if (vd->type->kind == TypeDecl::UNSIGNED_TYPE) {
            varTypes[name] = TypeDecl::UNSIGNED_TYPE;
        } else {
            varTypes[name] = TypeDecl::INT_TYPE;
        }

        if (!inFunction) {
            // Variables globales
            globalVars[name] = true;
        } else {
            // ✅ Variables locales CON inicialización
            localVars.add_var(name, offset);
            
            if (var.init_value) {
                // Evaluar valor inicial
                var.init_value->accept(this);
                
                // Conversión implícita: si la variable es float pero el valor es int
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
                // Sin inicialización, poner 0
                out << "    movq $0, " << offset << "(%rbp)\n";
            }
            
            offset -= 8;  // ✅ CRÍTICO: Siguiente posición en el stack
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
    // ✅ Alineación de stack a 16 bytes
    if (totalSlots % 2 != 0) totalSlots++; 

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
    stm->rhs->accept(this);  // valor en %rax o %xmm0
    
    // Verificar tipo de variable destino
    bool destIsFloat = (varTypes.count(stm->id) && varTypes[stm->id] == TypeDecl::FLOAT_TYPE);
    
    if (globalVars.count(stm->id)) {
        out << "    movq %rax, " << stm->id << "(%rip)\n"; // TODO: Global floats
    } else {
        int off = localVars.lookup(stm->id);
        if (destIsFloat) {
            if (!isFloat) {
                // Convertir int a float
                out << "    cvtsi2sd %rax, %xmm0\n";
            }
            out << "    movsd %xmm0, " << off << "(%rbp)\n";
        } else {
            // Destino es int
            if (isFloat) {
                // Convertir float a int (truncar)
                out << "    cvttsd2si %xmm0, %rax\n";
            }
            out << "    movq %rax, " << off << "(%rbp)\n";
        }
    }
    return 0;
}

int CodeGenerator::visit(PrintStm* stm) {
    if (stm->args.empty()) return 0;
    
    // ✅ Usar iteradores para list
    auto it = stm->args.begin();
    Exp* formatExp = *it;
    
    StringExp* strExp = dynamic_cast<StringExp*>(formatExp);
    
    if (strExp) {
        string label = "str_" + to_string(labelCount++);
        
        // ✅ Generar string literal
        out << ".data\n";
        out << label << ": .string " << strExp->value << "\n";
        out << ".text\n";
        
        if (stm->args.size() == 1) {
            // printf("string literal")
            out << "    leaq " << label << "(%rip), %rdi\n";
            out << "    movl $0, %eax\n";
            out << "    call printf@PLT\n";
        } else {
            // printf("format %d", variable)
            ++it;  // Saltar formato, ir al argumento
            (*it)->accept(this);  // Evaluar variable -> %rax o %xmm0
            
            if (isFloat) {
                out << "    movsd %xmm0, %xmm0\n"; // Redundante pero claro
                out << "    movl $1, %eax\n"; // 1 vector register used
            } else {
                out << "    movq %rax, %rsi\n";  // Segundo parámetro
                out << "    movl $0, %eax\n";
            }
            
            out << "    leaq " << label << "(%rip), %rdi\n";
            out << "    call printf@PLT\n";
        }
    } else {
        // ✅ NUEVO: printf(variable) sin formato explícito
        // Imprimir cada argumento como entero o float
        for (auto arg : stm->args) {
            arg->accept(this); // Evaluar -> %rax o %xmm0
            
            if (isFloat) {
                out << "    leaq print_float_fmt(%rip), %rdi\n";
                out << "    movl $1, %eax\n"; // 1 float arg
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
    out << "    # DEBUG: FcallStm " << stm->fcall->fname << "\n";
    if (!stm->fcall) return 0;

    if (stm->fcall->fname == "printf") {
        if (stm->fcall->args.empty()) return 0;

        Exp* firstArg = stm->fcall->args[0];
        StringExp* formatExp = dynamic_cast<StringExp*>(firstArg);

        if (formatExp) {
            // Caso: printf("string") o printf("format %d", var)
            string label = "str_" + to_string(labelCount++);
            out << ".data\n";
            out << label << ": .string " << formatExp->value << "\n";
            out << ".text\n";

            if (stm->fcall->args.size() == 1) {
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    movl $0, %eax\n";
                out << "    call printf@PLT\n";
            } else {
                for (size_t i = 1; i < stm->fcall->args.size() && i < 6; i++) {
                    stm->fcall->args[i]->accept(this);
                    switch (i) {
                        case 1: out << "    movq %rax, %rsi\n"; break;
                        case 2: out << "    movq %rax, %rdx\n"; break;
                        case 3: out << "    movq %rax, %rcx\n"; break;
                        case 4: out << "    movq %rax, %r8\n"; break;
                        case 5: out << "    movq %rax, %r9\n"; break;
                    }
                }
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    movl $0, %eax\n";
                out << "    call printf@PLT\n";
            }
        } else {
            // Caso: printf(variable) o printf(expr)
            stm->fcall->args[0]->accept(this); // evalúa variable/expresión en %rax
            out << "    movq %rax, %rsi\n";
            out << "    leaq print_fmt(%rip), %rdi\n";
            out << "    movl $0, %eax\n";
            out << "    call printf@PLT\n";
        }
    } else {
        // Otras funciones
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
        out << "    # DEBUG: Body statement\n";
        FcallStm* fcallStm = dynamic_cast<FcallStm*>(stm);
        if (fcallStm && fcallStm->fcall->fname == "printf") {
            fcallStm->accept(this);
        } else {
            stm->accept(this);
        }
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
