#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
using namespace std;

class Grammar
{
private:
    set<string> terminals;
    set<string> variables; // TODO: trocar pra vector mesmo pra manter uma ordenação personalizada
    string startSymbol;
    map<string, set<vector<string>>> productions;

public:
    Grammar();
    Grammar(string startSymbol, set<string> terminals);

    void addVariable(string A);
    void addProduction(string A, vector<string> rhs);
    void removeProduction(string A, vector<string> rhs);
    void clearProductions(string A);
    void print(ostream &out);
    bool isVariable(const string &symbol) const;
    Grammar clone() const;

    set<vector<string>> getProductions(string A);
    set<string> getTerminals() { return terminals; }
    string getStartSymbol() { return startSymbol; }
    set<string> getVariables() { return variables; }
    void setVariables(set<string> v) { this->variables = v; }

    void setStartSymbol(string A) { this->startSymbol = A; }
};

#endif