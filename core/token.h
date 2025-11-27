#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    enum Type {
        LPAREN, RPAREN, LBRACE, RBRACE, SEMICOL, COMA,
        PLUS, MINUS, MUL, DIV, ASSIGN, LT, LE, GT, GE, EQ, NE,
        NUM, FLOAT_NUM, ID, STRING,
        IF, ELSE, WHILE, FOR, RETURN, PRINTF, TRUE, FALSE,
        UNSIGNED, STRUCT, QUESTION, COLON, INCLUDE, PREPROCESSOR,
        ERR, END
    };

    Type type;
    string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);
    
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H