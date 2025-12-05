#ifndef NORMALIZATION_GLS_GRAMMARPARSER_H
#define NORMALIZATION_GLS_GRAMMARPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "grammar.h"

using namespace std;

class GrammarParser {
public:
    Grammar parserFromFile(string filename);
};

#endif