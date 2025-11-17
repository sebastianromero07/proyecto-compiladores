#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
}

Parser::~Parser() {
    delete previous;
    delete current;
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        delete previous;
        previous = current;
        current = scanner->nextToken();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return current->type == Token::END;
}

Program* Parser::parseProgram() {
    Program* prog = new Program();
    
    // Parsear declaraciones globales y funciones
    while (!isAtEnd()) {
        if (check(Token::STRUCT)) {
            prog->structdecs.push_back(parseStructDec());
        } else {
            // Determinar si es variable o función
            TypeDecl* type = parseType();
            if (match(Token::ID)) {
                string name = previous->text;
                if (check(Token::LPAREN)) {
                    // Es una función
                    FunDec* fd = parseFunDec(type, name);
                    prog->fundecs.push_back(fd);
                } else {
                    // Es una variable global con posible inicialización
                    VarDec* vd = new VarDec(type);
                    vd->vars.push_back(name);
                    
                    // Manejar inicialización (opcional)
                    if (match(Token::ASSIGN)) {
                        // Saltar la expresión de inicialización por ahora
                        while (!check(Token::SEMICOL) && !check(Token::COMA) && !isAtEnd()) {
                            advance();
                        }
                    }
                    
                    // Manejar múltiples variables
                    while (match(Token::COMA)) {
                        if (match(Token::ID)) {
                            vd->vars.push_back(previous->text);
                            // Manejar inicialización de esta variable también
                            if (match(Token::ASSIGN)) {
                                while (!check(Token::SEMICOL) && !check(Token::COMA) && !isAtEnd()) {
                                    advance();
                                }
                            }
                        }
                    }
                    match(Token::SEMICOL);
                    prog->vardecs.push_back(vd);
                }
            }
        }
    }
    
    return prog;
}

TypeDecl* Parser::parseType() {
    if (match(Token::INT)) {
        return new TypeDecl(TypeDecl::INT_TYPE);
    } else if (match(Token::UNSIGNED)) {
        if (match(Token::INT)) {
            return new TypeDecl(TypeDecl::UNSIGNED_INT_TYPE);
        } else {
            // Si solo dice "unsigned", asumir "unsigned int"
            return new TypeDecl(TypeDecl::UNSIGNED_INT_TYPE);
        }
    } else if (match(Token::FLOAT)) {
        return new TypeDecl(TypeDecl::FLOAT_TYPE);
    } else if (match(Token::STRUCT)) {
        if (match(Token::ID)) {
            return new TypeDecl(TypeDecl::STRUCT_TYPE, previous->text);
        }
    } else if (match(Token::ID)) {
        return new TypeDecl(TypeDecl::ID_TYPE, previous->text);
    }
    
    return new TypeDecl(TypeDecl::INT_TYPE); // Default
}

VarDec* Parser::parseVarDec() {
    TypeDecl* type = parseType();
    VarDec* vd = new VarDec(type);
    
    if (match(Token::ID)) {
        vd->vars.push_back(previous->text);
        
        while (match(Token::COMA)) {
            if (match(Token::ID)) {
                vd->vars.push_back(previous->text);
            }
        }
    }
    
    match(Token::SEMICOL);
    return vd;
}

StructDec* Parser::parseStructDec() {
    match(Token::STRUCT);
    StructDec* sd = nullptr;
    
    if (match(Token::ID)) {
        sd = new StructDec(previous->text);
        match(Token::LBRACE);
        
        while (!check(Token::RBRACE) && !isAtEnd()) {
            sd->fields.push_back(parseVarDec());
        }
        
        match(Token::RBRACE);
        match(Token::SEMICOL);
    }
    
    return sd;
}

FunDec* Parser::parseFunDec() {
    // Esta es la versión básica llamada desde parseProgram
    return nullptr;
}

// Versión sobrecargada para parsear función con tipo y nombre ya conocidos
FunDec* Parser::parseFunDec(TypeDecl* rtype, string name) {
    vector<TypeDecl*> ptypes;
    vector<string> pnames;
    
    match(Token::LPAREN);
    
    // Parsear parámetros si existen
    if (!check(Token::RPAREN)) {
        do {
            TypeDecl* ptype = parseType();
            if (match(Token::ID)) {
                ptypes.push_back(ptype);
                pnames.push_back(previous->text);
            }
        } while (match(Token::COMA));
    }
    
    match(Token::RPAREN);
    
    // Parsear cuerpo de función
    Body* body = parseBody();
    
    return new FunDec(rtype, name, ptypes, pnames, body);
}

Body* Parser::parseBody() {
    Body* body = new Body();
    
    match(Token::LBRACE);
    
    // Parsear declaraciones de variables locales
    while (check(Token::INT) || check(Token::UNSIGNED) || check(Token::FLOAT) || 
           check(Token::STRUCT) || (check(Token::ID) && !isStatement())) {
        body->vardecs.push_back(parseVarDec());
    }
    
    // Parsear statements
    while (!check(Token::RBRACE) && !isAtEnd()) {
        Stm* stm = parseStm();
        if (stm) {
            body->stmts.push_back(stm);
        }
    }
    
    match(Token::RBRACE);
    return body;
}

// Función auxiliar para determinar si el token actual inicia un statement
bool Parser::isStatement() {
    return check(Token::IF) || check(Token::WHILE) || check(Token::FOR) || 
           check(Token::RETURN) || check(Token::PRINTF) || check(Token::LBRACE) ||
           (check(Token::ID) && current + 1 && (current + 1)->type == Token::ASSIGN);
}

Stm* Parser::parseStm() {
    if (match(Token::IF)) {
        return parseIfStm();
    } else if (match(Token::WHILE)) {
        return parseWhileStm();
    } else if (match(Token::FOR)) {
        return parseForStm();
    } else if (match(Token::RETURN)) {
        return parseReturnStm();
    } else if (match(Token::PRINTF)) {
        return parsePrintStm();
    } else if (match(Token::ID)) {
        // Puede ser asignación o llamada a función
        string id = previous->text;
        if (match(Token::ASSIGN)) {
            // Es asignación
            Exp* rhs = parseCE();
            match(Token::SEMICOL);
            return new AssignStm(id, rhs);
        } else if (check(Token::LPAREN)) {
            // Es llamada a función
            FcallExp* fcall = new FcallExp(id);
            match(Token::LPAREN);
            
            if (!check(Token::RPAREN)) {
                do {
                    fcall->args.push_back(parseCE());
                } while (match(Token::COMA));
            }
            
            match(Token::RPAREN);
            match(Token::SEMICOL);
            return new FcallStm(fcall);
        }
    }
    
    // Si no es ningún statement reconocido, saltar hasta el siguiente ;
    while (!check(Token::SEMICOL) && !isAtEnd()) {
        advance();
    }
    match(Token::SEMICOL);
    
    return nullptr;
}

// Métodos auxiliares para parsear diferentes tipos de statements
Stm* Parser::parseIfStm() {
    match(Token::LPAREN);
    Exp* condition = parseCE();
    match(Token::RPAREN);
    Body* thenbody = parseBody();
    Body* elsebody = nullptr;
    
    if (match(Token::ELSE)) {
        elsebody = parseBody();
    }
    
    return new IfStm(condition, thenbody, elsebody);
}

Stm* Parser::parseWhileStm() {
    match(Token::LPAREN);
    Exp* condition = parseCE();
    match(Token::RPAREN);
    Body* body = parseBody();
    
    return new WhileStm(condition, body);
}

Stm* Parser::parseForStm() {
    match(Token::LPAREN);
    
    // Inicialización (puede ser declaración o asignación)
    Stm* init = nullptr;
    if (check(Token::INT) || check(Token::UNSIGNED) || check(Token::FLOAT)) {
        init = parseVarDec();
    } else if (match(Token::ID)) {
        string id = previous->text;
        match(Token::ASSIGN);
        Exp* rhs = parseCE();
        init = new AssignStm(id, rhs);
        match(Token::SEMICOL);
    }
    
    // Condición
    Exp* condition = parseCE();
    match(Token::SEMICOL);
    
    // Actualización
    AssignStm* update = nullptr;
    if (match(Token::ID)) {
        string id = previous->text;
        match(Token::ASSIGN);
        Exp* rhs = parseCE();
        update = new AssignStm(id, rhs);
    }
    
    match(Token::RPAREN);
    Body* body = parseBody();
    
    return new ForStm(init, condition, update, body);
}

Stm* Parser::parseReturnStm() {
    Exp* expr = nullptr;
    if (!check(Token::SEMICOL)) {
        expr = parseCE();
    }
    match(Token::SEMICOL);
    return new ReturnStm(expr);
}

Stm* Parser::parsePrintStm() {
    PrintStm* pstm = new PrintStm();
    match(Token::LPAREN);
    
    if (!check(Token::RPAREN)) {
        do {
            pstm->args.push_back(parseCE());
        } while (match(Token::COMA));
    }
    
    match(Token::RPAREN);
    match(Token::SEMICOL);
    return pstm;
}

Exp* Parser::parseCE() {
    Exp* left = parseE();
    
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ)) {
        BinaryOp op;
        if (previous->type == Token::LT) op = LT_OP;
        else if (previous->type == Token::LE) op = LE_OP;
        else op = EQ_OP;
        
        Exp* right = parseE();
        return new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseBE() {
    // Por ahora, delegamos a parseE
    return parseE();
}

Exp* Parser::parseE() {
    Exp* left = parseT();
    
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
        Exp* right = parseT();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseT() {
    Exp* left = parseF();
    
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op = (previous->type == Token::MUL) ? MUL_OP : DIV_OP;
        Exp* right = parseF();
        left = new BinaryExp(left, right, op);
    }
    
    return left;
}

Exp* Parser::parseF() {
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    } else if (match(Token::FLOAT_NUM)) {
        return new FloatExp(stof(previous->text));
    } else if (match(Token::STRING)) {
        return new StringExp(previous->text);
    } else if (match(Token::TRUE)) {
        return new BoolExp(true);
    } else if (match(Token::FALSE)) {
        return new BoolExp(false);
    } else if (match(Token::ID)) {
        string id = previous->text;
        if (match(Token::LPAREN)) {
            // Llamada a función
            FcallExp* fcall = new FcallExp(id);
            
            if (!check(Token::RPAREN)) {
                do {
                    fcall->args.push_back(parseCE());
                } while (match(Token::COMA));
            }
            
            match(Token::RPAREN);
            return fcall;
        } else {
            // Variable
            return new IdExp(id);
        }
    } else if (match(Token::LPAREN)) {
        Exp* exp = parseCE();
        match(Token::RPAREN);
        return exp;
    }
    
    return new NumberExp(0); // Default
}