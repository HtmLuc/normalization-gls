#ifndef NORMALIZATION_GLS_GRAMMARPARSER_H
#define NORMALIZATION_GLS_GRAMMARPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "grammar.h"

using namespace std;

class GrammarParser
{
public:
    /// @brief Realiza a leitura de um arquivo contendo a descrição de uma GLC e retorna a GLC já construída.
    /// @param filename Nome do arquivo. Este arquivo deve estar presente na pasta data/
    /// @return Uma GLC formada através das informações definidas no arquivo.
    Grammar parserFromFile(string filename);
};

#endif