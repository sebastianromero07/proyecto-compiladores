#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s): input(s), first(0), current(0) { }

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    char c;
    while (current < input.length() && is_white_space(input[current])) {
        current++;
    }

    if (current >= input.length()) {
        return new Token(Token::END);
    }

    first = current;
    c = input[current];

    // Manejar directivas de preprocesador (#include, #define, etc.)
    if (c == '#') {
        current++;
        while (current < input.length() && 
               input[current] != '\n' && input[current] != '\r') {
            current++;
        }
        
        string directive = input.substr(first, current - first);
        
        if (directive.find("#include") == 0) {
            return new Token(Token::INCLUDE, input, first, current - first);
        } else {
            return new Token(Token::PREPROCESSOR, input, first, current - first);
        }
    }
    else if (isdigit(c)) {
        while (current < input.length() && isdigit(input[current])) {
            current++;
        }
        
        // Verificar si es un número flotante
        if (current < input.length() && input[current] == '.') {
            current++; 
            if (current < input.length() && isdigit(input[current])) {
                while (current < input.length() && isdigit(input[current])) {
                    current++;
                }
                token = new Token(Token::FLOAT_NUM, input, first, current - first);
            } else {
                // Punto sin dígitos después - error
                return new Token(Token::ERR, input, first, current - first);
            }
        } else {
            token = new Token(Token::NUM, input, first, current - first);
        }
    }
    else if (c == '"') {
        current++;
        while (current < input.length() && input[current] != '"') {
            current++;
        }
        if (current >= input.length()) {
            return new Token(Token::ERR, input, first, current - first);
        }
        current++;
        token = new Token(Token::STRING, input, first, current - first);
    }

    else if (isalpha(c) || c == '_') {
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_' || input[current] == '.')) {
            current++;
        }
        string lexema = input.substr(first, current - first);
        

        if (lexema == "if") return new Token(Token::IF, input, first, current - first);
        else if (lexema == "else") return new Token(Token::ELSE, input, first, current - first);
        else if (lexema == "while") return new Token(Token::WHILE, input, first, current - first);
        else if (lexema == "for") return new Token(Token::FOR, input, first, current - first);
        else if (lexema == "return") return new Token(Token::RETURN, input, first, current - first);
        else if (lexema == "printf") return new Token(Token::PRINTF, input, first, current - first);
        else if (lexema == "true") return new Token(Token::TRUE, input, first, current - first);
        else if (lexema == "false") return new Token(Token::FALSE, input, first, current - first);
        else if (lexema == "unsigned") return new Token(Token::UNSIGNED, input, first, current - first);
        else if (lexema == "struct") return new Token(Token::STRUCT, input, first, current - first);
        else return new Token(Token::ID, input, first, current - first);
    }
    // Otros operadores y símbolos
    else if (strchr("+/-*(){};,?:=<>", c)) {
        switch (c) {
            case '+': token = new Token(Token::PLUS, c); break;
            case '-': token = new Token(Token::MINUS, c); break;
            case '=': 
            if (current + 1 < input.length() && input[current+1]=='=')
            {
                current++;
                token = new Token(Token::EQ, input, first, current + 1 - first);
            }
            else{
                token = new Token(Token::ASSIGN,   c);
            }
            break;
            case '*': token = new Token(Token::MUL, c); break;
            case '<':
            if (current + 1 < input.length() && input[current + 1] == '=') {
                current++;
                token = new Token(Token::LE, input, first, current + 1 - first);
            } else {
                token = new Token(Token::LT, input, first, 1);
            }
            break;

        case '>':
            if (current + 1 < input.length() && input[current + 1] == '=') {
                current++;
                token = new Token(Token::GE, input, first, current + 1 - first); 
            } else {
                token = new Token(Token::GT, input, first, 1);
            }
            break;
            case '/': token = new Token(Token::DIV, c); break;
            case '(': token = new Token(Token::LPAREN, c); break;
            case ')': token = new Token(Token::RPAREN, c); break;
            case '{': token = new Token(Token::LBRACE, c); break;
            case '}': token = new Token(Token::RBRACE, c); break;
            case ';': token = new Token(Token::SEMICOL, c); break;
            case ',': token = new Token(Token::COMA, c); break;
            case '?': token = new Token(Token::QUESTION, c); break;
            case ':': token = new Token(Token::COLON, c); break;
            default: token = new Token(Token::ERR, c); break;
        }
        current++;
    }
    else {
        token = new Token(Token::ERR, c);
        current++;
    }

    return token;
}

Scanner::~Scanner() { }


int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    // Crear nombre para archivo de salida
    string OutputFileName = InputFile;
    size_t pos = OutputFileName.find_last_of(".");
    if (pos != string::npos) {
        OutputFileName = OutputFileName.substr(0, pos);
    }
    OutputFileName += "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return 0;
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido" << endl << endl;
            outFile << "Scanner no exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
