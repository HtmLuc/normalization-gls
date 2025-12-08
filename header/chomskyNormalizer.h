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
    void removeUnitProductions();
    void removeUselessSymbols();
    void toChomskyNormalForm();

    //auxiliary:
    set<string> findVoidableVariables();
    set<string> findVariableChain(string& A);
    set<string> findUnitProductionsVar(string& A);
    set<string> getTerm();
    set<string> getReach();

    /*
    na main:
        ChomskyNormalizer normalizer(G);
        Grammar fnc = normalizer.toChomskyNormalForm();
    */
};

#endif