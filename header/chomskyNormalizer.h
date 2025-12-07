#ifndef CHOMSKY_HPP
#define CHOMSKY_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "grammar.h"

class ChomskyNormalizer {
private:
    Grammar grammar;

public:
    ChomskyNormalizer(const Grammar& g);

    Grammar removeRecursionAtBeginning();
    Grammar removeLambdaProductions();
    set<string> findVoidableVariables();
    Grammar removeUnitProductions();
    Grammar removeUselessSymbols();
    Grammar toChomskyNormalForm();
    set<string> findVariableChain(string& A);
    set<string> findUnitProductionsVar(string& A);
    Grammar removeLeftRecursion();


    /*
    na main:
        ChomskyNormalizer normalizer(G);
        Grammar fnc = normalizer.toChomskyNormalForm();
    */

    Grammar getGrammar() const;
};

#endif