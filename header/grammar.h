#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
using namespace std;

class Grammar{
private:
    set<string> terminals;
    set<string> variables;
    string startSymbol;
    map<string, set<vector<string>>> productions;
public:
    Grammar();
    Grammar(string startSymbol, set<string> terminals);

    void addProduction(string A, vector<string> rhs);
    void removeProduction(string A, vector<string> rhs);
    void print(ostream &out);
    bool isVariable(const string& symbol) const;
    Grammar clone() const;

    set<vector<string>> getProductions(string A);
    string getStartSymbol(){return startSymbol;}
    set<string> getVariables(){return variables;}

    void setStartSymbol(string A){this->startSymbol = A;}
};

#endif