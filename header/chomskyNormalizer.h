#ifndef CHOMSKY_HPP
#define CHOMSKY_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include "grammar.h"

using namespace std;

class ChomskyNormalizer {
private:
    Grammar grammar;
    int newVarCounter = 0;

    string generateNewVarName(const Grammar& context);
    Grammar replaceTerminalsInLongBodies(const Grammar& g);
    Grammar breakLongBodies(const Grammar& g);

public:
    ChomskyNormalizer(const Grammar& g);

    Grammar removeRecursionAtBeginning();
    Grammar removeLambdaProductions();
    Grammar removeUnitProductions();
    Grammar removeLeftRecursion();
    
    Grammar removeUselessSymbols();
    Grammar fixLongProductions();
    Grammar toChomskyNormalForm();

    set<string> findVoidableVariables();
    set<string> findUnitProductionsVar(string& A);
    set<string> findVariableChain(string& A);

    Grammar getGrammar() const;
};

#endif