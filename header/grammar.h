#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class Grammar
{
private:
    set<string> terminals;
    set<string> variables;
    unordered_map<string, int> order; // Para marcar a ordem de cada vértice
    int next_order;                   // FIXME: temp?
    string startSymbol;
    map<string, set<vector<string>>> productions;

    // GREIBACH
    void renameVariables();
    bool respectGreibachOrder();
    void replace(string lhs, string vRhs);
    void replaceBackwards();
    void applyRuleOrderConstraint();
    void removeLeftRecursionGreibach(string lhs);

    /// @brief Remove todas as produções que intercalam terminais e variáveis, i.e. S -> aBa,
    /// adicionando estados para cada variável.
    void removeMixedProductions();
    void removeRecursionAtBeginning();
    set<string> findVoidableVariables();
    void removeLambdaProductions();
    void removeUnitProductions();
    void removeUselessSymbols();
    set<string> findVariableChain(string &A);
    set<string> findUnitProductionsVar(string &A);
    set<string> getTerm();
    set<string> getReach();

public:
    Grammar();
    Grammar(string startSymbol, set<string> terminals);

    // TODO: ao final mover tudo pra privado e expor somente print, clone e as normalizações pra cada formato.
    void addVariable(string A);
    void addProduction(string A, vector<string> rhs);
    void removeProduction(string A, vector<string> rhs);
    void clearProductions(string A);
    void print(ostream &out);
    bool isVariable(const string &symbol) const;
    void removeVariable(const string &A);
    Grammar clone() const;
    void toChomskyNormalForm();
    void toGreibachNormalForm();

    set<vector<string>> getProductions(string A);
    set<string> getTerminals() { return terminals; }
    string getStartSymbol() { return startSymbol; }
    set<string> getVariables() { return variables; }
    void setVariables(set<string> v) { this->variables = v; }
    void setStartSymbol(string A) { this->startSymbol = A; }
};

#endif