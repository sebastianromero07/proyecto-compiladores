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

bool Parser::isTypeStart() {
    if (check(Token::UNSIGNED) || check(Token::STRUCT)) {
        return true;
    }
    if (check(Token::ID)) {
        string id = current->text;
        return (id == "int" || id == "float");
    }
    return false;
}

bool Parser::isLocalDecl() {
    return isTypeStart();
}

Program* Parser::parseProgram() {
    Program* prog = new Program();

    while (!isAtEnd()) {
        if (check(Token::INCLUDE) || check(Token::PREPROCESSOR)) {
            advance();
            continue;
        }

        if (check(Token::STRUCT)) {
            prog->structdecs.push_back(parseStructDec());
        } else if (isTypeStart()) {
            parseGlobalDecl(prog);
        } else {
            advance();
        }
    }
    return prog;
}

void Parser::parseGlobalDecl(Program* prog) {
    TypeDecl* type = parseType();

    if (!match(Token::ID)) {
        delete type;
        return;
    }

    string name = previous->text;

    if (check(Token::LPAREN)) {
        prog->fundecs.push_back(parseFunDec(type, name));
    } else {
        VarDec* vd = new VarDec(type);

        Exp* init_value = nullptr;
        if (match(Token::ASSIGN)) {
            init_value = parseCE();
        }
        vd->addVar(name, init_value);

        while (match(Token::COMA)) {
            if (match(Token::ID)) {
                string nextVar = previous->text;
                Exp* nextInit = nullptr;
                if (match(Token::ASSIGN)) {
                    nextInit = parseCE();
                }
                vd->addVar(nextVar, nextInit);
            }
        }

        match(Token::SEMICOL);
        prog->vardecs.push_back(vd);
    }
}

TypeDecl* Parser::parseType() {
    if (match(Token::UNSIGNED)) {
        if (match(Token::ID)) {
            return new TypeDecl(TypeDecl::UNSIGNED_TYPE, previous->text);
        } else {
            return new TypeDecl(TypeDecl::UNSIGNED_TYPE, "int");
        }
    } else if (match(Token::STRUCT)) {
        if (match(Token::ID)) {
            return new TypeDecl(TypeDecl::STRUCT_TYPE, previous->text);
        } else {
            return nullptr;
        }
    } else if (match(Token::ID)) {
        string typeName = previous->text;
        if (typeName == "int") {
            return new TypeDecl(TypeDecl::INT_TYPE);
        } else if (typeName == "float") {
            return new TypeDecl(TypeDecl::FLOAT_TYPE);
        } else {
            return new TypeDecl(TypeDecl::ID_TYPE, typeName);
        }
    }
    return nullptr;
}

VarDec* Parser::parseVarDec() {
    TypeDecl* type = parseType();
    VarDec* vd = new VarDec(type);

    if (match(Token::ID)) {
        string varname = previous->text;
        Exp* init_value = nullptr;

        if (match(Token::ASSIGN)) {
            init_value = parseCE();
        }

        vd->addVar(varname, init_value);

        while (match(Token::COMA)) {
            if (match(Token::ID)) {
                string nextVar = previous->text;
                Exp* nextInit = nullptr;

                if (match(Token::ASSIGN)) {
                    nextInit = parseCE();
                }

                vd->addVar(nextVar, nextInit);
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
    return nullptr;
}

FunDec* Parser::parseFunDec(TypeDecl* rtype, string name) {
    vector<TypeDecl*> ptypes;
    vector<string> pnames;

    match(Token::LPAREN);

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

    Body* body = parseBody();

    return new FunDec(rtype, name, ptypes, pnames, body);
}

Body* Parser::parseBody() {
    Body* body = new Body();

    match(Token::LBRACE);

    while (isLocalDecl() && !isAtEnd()) {
        body->vardecs.push_back(parseVarDec());
    }

    while (!check(Token::RBRACE) && !isAtEnd()) {
        Stm* stm = parseStm();
        if (stm) {
            body->stmts.push_back(stm);
        }
    }

    match(Token::RBRACE);
    return body;
}

bool Parser::isStatement() {
    return check(Token::IF) || check(Token::WHILE) || check(Token::FOR) ||
           check(Token::RETURN) || check(Token::PRINTF) ||
           check(Token::ID);
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
        string id = previous->text;
        if (match(Token::ASSIGN)) {
            Exp* rhs = parseCE();
            match(Token::SEMICOL);
            return new AssignStm(id, rhs);
        } else if (check(Token::LPAREN)) {
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

    while (!check(Token::SEMICOL) && !isAtEnd()) {
        advance();
    }
    match(Token::SEMICOL);

    return nullptr;
}

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

    Stm* init = nullptr;

    if (isLocalDecl()) {
        init = parseVarDec();
    } else if (match(Token::ID)) {
        string id = previous->text;
        match(Token::ASSIGN);
        Exp* rhs = parseCE();
        init = new AssignStm(id, rhs);
        match(Token::SEMICOL);
    }

    Exp* condition = parseCE();
    match(Token::SEMICOL);

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
    Exp* cond = parseRelExp();

    if (match(Token::QUESTION)) {
        Exp* thenExp = parseCE();
        match(Token::COLON);
        Exp* elseExp = parseCE();
        return new TernaryExp(cond, thenExp, elseExp);
    }

    return cond;
}

Exp* Parser::parseRelExp() {
    Exp* left = parseE();

    if (match(Token::LT) || match(Token::LE) || match(Token::EQ) ||
        match(Token::GT) || match(Token::GE)) {

        BinaryOp op;
        if (previous->type == Token::LT)      op = LT_OP;
        else if (previous->type == Token::LE) op = LE_OP;
        else if (previous->type == Token::EQ) op = EQ_OP;
        else if (previous->type == Token::GT) op = GT_OP;
        else                                  op = GE_OP;

        Exp* right = parseE();
        left = new BinaryExp(left, right, op);
    }

    return left; 
}
Exp* Parser::parseBE() {
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
            FcallExp* fcall = new FcallExp(id);

            if (!check(Token::RPAREN)) {
                do {
                    fcall->args.push_back(parseCE());
                } while (match(Token::COMA));
            }

            match(Token::RPAREN);
            return fcall;
        } else {
            return new IdExp(id);
        }
    } else if (match(Token::LPAREN)) {
        Exp* exp = parseCE();
        match(Token::RPAREN);
        return exp;
    }

    return new NumberExp(0);
}