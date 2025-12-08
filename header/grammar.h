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
    set<string> variables;            // TODO: trocar pra vector mesmo pra manter uma ordenação personalizada
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

public:
    Grammar();
    Grammar(string startSymbol, set<string> terminals);

    void addVariable(string A);
    void addProduction(string A, vector<string> rhs);
    void removeProduction(string A, vector<string> rhs);
    void clearProductions(string A);
    void print(ostream &out);
    bool isVariable(const string &symbol) const;
    void removeVariable(const string &A);
    Grammar clone() const;
    // CHOMKSY - TEMPORÁRIO, depois que fizer o método toChomksyNormalForm aí tem que mover cada um desses métodos
    // para o bloco privado.
    Grammar removeRecursionAtBeginning();
    Grammar removeLambdaProductions();
    set<string> findVoidableVariables();
    Grammar removeUnitProductions();
    Grammar removeUselessSymbols();
    set<string> findVariableChain(string &A);
    set<string> findUnitProductionsVar(string &A);
    set<string> getTerm();
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