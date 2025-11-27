#ifndef PARSER_H       
#define PARSER_H

#include "scanner.h"
#include "ast.h"


class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    
    bool isTypeStart();
    bool isLocalDecl();
    bool isStatement();
    void parseGlobalDecl(Program* prog); 

public:
    Parser(Scanner* sc);
    ~Parser();
    
    Program* parseProgram();
    FunDec* parseFunDec();
    FunDec* parseFunDec(TypeDecl* rtype, string name);
    Body* parseBody();
    VarDec* parseVarDec();
    StructDec* parseStructDec();
    TypeDecl* parseType();
    
    // Métodos para statements
    Stm* parseStm();
    Stm* parseIfStm();
    Stm* parseWhileStm();
    Stm* parseForStm();
    Stm* parseReturnStm();
    Stm* parsePrintStm();
    
    // Métodos para expresiones
    Exp* parseRelExp();
    Exp* parseCE();
    Exp* parseBE();
    Exp* parseE();
    Exp* parseT();
    Exp* parseF();
};

#endif // PARSER_H