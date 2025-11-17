#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "token.h"

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
    Scanner* scanner = new Scanner(content.c_str());
    
    // Ejecutar scanner
    int result = ejecutar_scanner(scanner, inputFile);
    
    delete scanner;
    return result;
}