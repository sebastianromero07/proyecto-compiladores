#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    enum Type {
        // Operadores aritméticos
        PLUS, MINUS, MUL, DIV,
        
        // Paréntesis y delimitadores
        LPAREN, RPAREN, LBRACE, RBRACE,
        
        // Operadores de comparación
        LT, LE, EQ,
        
        // Asignación y otros símbolos
        ASSIGN, SEMICOL, COMA,
        
        // Literales
        NUM, ID, STRING, TRUE, FALSE, FLOAT_NUM,
        
        // Palabras clave
        IF, ELSE, WHILE, FOR, RETURN, PRINTF,
        
        // Tipos de datos
        INT, UNSIGNED, FLOAT, STRUCT,
        
        // Tokens especiales
        END, ERR
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