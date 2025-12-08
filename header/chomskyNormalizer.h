#ifndef CHOMSKY_HPP
#define CHOMSKY_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "grammar.h"

class ChomskyNormalizer
{
private:
    Grammar grammar;

public:
    ChomskyNormalizer(const Grammar &g);
    Grammar getGrammar(){return this->grammar;}
    void updateGrammar(const Grammar &g){this->grammar = g;}

    void removeRecursionAtBeginning();
    void removeLambdaProductions();
    set<string> findVoidableVariables();
    void removeUnitProductions();
    void removeUselessSymbols();
    Grammar toChomskyNormalForm();
    set<string> findVariableChain(string& A);
    set<string> findUnitProductionsVar(string& A);
    set<string> getTerm();
    set<string> getReach();

    /*
    na main:
        ChomskyNormalizer normalizer(G);
        Grammar fnc = normalizer.toChomskyNormalForm();
    */

    Grammar getGrammar() const;
};

#endif