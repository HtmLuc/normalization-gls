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
    Grammar removeLambdaProductions2();
    set<string> findVoidableVariables();
    Grammar removeUselessSymbols();
    Grammar toChomskyNormalForm();

    /*
    na main:
        ChomskyNormalizer normalizer(G);
        Grammar fnc = normalizer.toChomskyNormalForm();
    */

    Grammar getGrammar() const;
};

#endif