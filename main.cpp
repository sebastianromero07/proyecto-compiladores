#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <archivo_entrada>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    ifstream file(inputFile);
    
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << inputFile << endl;
        return 1;
    }

    // Leer todo el contenido del archivo
    string content((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());
    file.close();

    // Crear scanner
    Scanner scanner_scan(content.c_str());
    ejecutar_scanner(&scanner_scan, inputFile);
    
    Scanner scanner_parser(content.c_str());
    Parser parser(&scanner_parser);

    Program* program = parser.parseProgram();     
        string baseName = inputFile;
        size_t dotPos = baseName.find_last_of('.');
        if (dotPos != string::npos) {
            baseName = baseName.substr(0, dotPos);
        }
        string outputFilename = baseName + ".s";
        ofstream outfile(outputFilename);
        if (!outfile.is_open()) {
            cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
            return 1;
        }

    cout << "Generando codigo ensamblador en " << outputFilename << endl;

    CodeGenerator codigo(outfile);
    codigo.generar(program);
    outfile.close();
    delete program;
    return 0;
}