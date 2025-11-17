#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"
#include <vector>

using namespace std;


class Scanner {
private:
    string input;
    int first;
    int current;

public:
    Scanner(const char* s);
    ~Scanner();
    Token* nextToken();
    Token* peek(); // ✅ NUEVO: para lookahead
    vector<Token*>& getTokens(); // ✅ NUEVO: para backtracking
};

// Ejecutar scanner
int ejecutar_scanner(Scanner* scanner,const string& InputFile);

#endif // SCANNER_H