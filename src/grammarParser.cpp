#include "../header/grammarParser.h"

const string RESET = "\033[0m";
const string RED = "\033[31m";

Grammar GrammarParser::parserFromFile(string filename) {
    Grammar grammar;
    ifstream file;
    file.open(filename);

    if (!file.is_open()) {
        cout << RED << "Erro: não foi possível abrir o arquivo!" << RESET << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t pos = line.find(" -> ");
        if (pos != string::npos) {
            grammar.addProduction(line.substr(0, pos), {line.substr(pos + 2)});
        }
    }
    return grammar;
}