#ifndef GREIBACH_HPP
#define GREIBACH_HPP

#include "grammar.h"
#include <unordered_map>

class GreibachNormalizer
{
private:
    Grammar grammar;
    unordered_map<string, int> order; // Para marcar a ordem de cada vértice
    vector<string> sortedVarList;     // TODO: depois unir com o atributo do grammar
    int next_order;

    bool hasOrder();
    void renameVariables();
    void replace(string lhs, string vRhs);
    void replaceBackwards();
    void applyRuleOrderConstraint();
    void removeLeftRecursionGreibach(string lhs);

public:
    GreibachNormalizer(Grammar g)
    {
        this->grammar = g.clone();
    }
    Grammar *normalize();
};

#endif // !GREIBACH_HPP
