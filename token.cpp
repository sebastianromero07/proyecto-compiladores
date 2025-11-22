#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type) 
    : type(type), text("") { }

Token::Token(Type type, char c) 
    : type(type), text(string(1, c)) { }

Token::Token(Type type, const string& source, int first, int last) 
    : type(type), text(source.substr(first, last)) { }

ostream& operator<<(ostream& outs, const Token& tok) {
    switch (tok.type) {
        case Token::PLUS:      outs << "TOKEN(PLUS, \""      << tok.text << "\")"; break;
        case Token::MINUS:     outs << "TOKEN(MINUS, \""     << tok.text << "\")"; break;
        case Token::MUL:       outs << "TOKEN(MUL, \""       << tok.text << "\")"; break;
        case Token::DIV:       outs << "TOKEN(DIV, \""       << tok.text << "\")"; break;
        case Token::LPAREN:    outs << "TOKEN(LPAREN, \""    << tok.text << "\")"; break;
        case Token::RPAREN:    outs << "TOKEN(RPAREN, \""    << tok.text << "\")"; break;
        case Token::LBRACE:    outs << "TOKEN(LBRACE, \""    << tok.text << "\")"; break;
        case Token::RBRACE:    outs << "TOKEN(RBRACE, \""    << tok.text << "\")"; break;
        case Token::LT:        outs << "TOKEN(LT, \""        << tok.text << "\")"; break;
        case Token::LE:        outs << "TOKEN(LE, \""        << tok.text << "\")"; break;
        case Token::GT:        outs << "TOKEN(GT, \""        << tok.text << "\")"; break; 
        case Token::GE:        outs << "TOKEN(GE, \""        << tok.text << "\")"; break; 
        case Token::EQ:        outs << "TOKEN(EQ, \""        << tok.text << "\")"; break;
        case Token::NE:        outs << "TOKEN(NE, \""        << tok.text << "\")"; break; 
        case Token::ASSIGN:    outs << "TOKEN(ASSIGN, \""    << tok.text << "\")"; break;
        case Token::SEMICOL:   outs << "TOKEN(SEMICOL, \""   << tok.text << "\")"; break;
        case Token::COMA:      outs << "TOKEN(COMA, \""      << tok.text << "\")"; break;
        case Token::NUM:       outs << "TOKEN(NUM, \""       << tok.text << "\")"; break;
        case Token::FLOAT_NUM: outs << "TOKEN(FLOAT_NUM, \"" << tok.text << "\")"; break;
        case Token::ID:        outs << "TOKEN(ID, \""        << tok.text << "\")"; break;
        case Token::STRING:    outs << "TOKEN(STRING, \""    << tok.text << "\")"; break;
        case Token::TRUE:      outs << "TOKEN(TRUE, \""      << tok.text << "\")"; break;
        case Token::FALSE:     outs << "TOKEN(FALSE, \""     << tok.text << "\")"; break;
        case Token::IF:        outs << "TOKEN(IF, \""        << tok.text << "\")"; break;
        case Token::ELSE:      outs << "TOKEN(ELSE, \""      << tok.text << "\")"; break;
        case Token::WHILE:     outs << "TOKEN(WHILE, \""     << tok.text << "\")"; break;
        case Token::FOR:       outs << "TOKEN(FOR, \""       << tok.text << "\")"; break;
        case Token::RETURN:    outs << "TOKEN(RETURN, \""    << tok.text << "\")"; break;
        case Token::PRINTF:    outs << "TOKEN(PRINTF, \""    << tok.text << "\")"; break;
        case Token::UNSIGNED:  outs << "TOKEN(UNSIGNED, \""  << tok.text << "\")"; break;
        case Token::STRUCT:    outs << "TOKEN(STRUCT, \""    << tok.text << "\")"; break;
        case Token::INCLUDE:     outs << "TOKEN(INCLUDE, \""     << tok.text << "\")"; break;
        case Token::PREPROCESSOR: outs << "TOKEN(PREPROCESSOR, \"" << tok.text << "\")"; break;
        
        case Token::END:       outs << "TOKEN(END)"; break;
        case Token::ERR:       outs << "TOKEN(ERR, \""       << tok.text << "\")"; break;
        default:               outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;
}