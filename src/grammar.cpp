#include "../header/grammar.h"
#include "../header/logger.h"
#include "../header/utils.h"
#include <queue>

Grammar::Grammar()
{
    this->terminals = {};
    this->startSymbol = "";
    this->productions = {};
}

Grammar::Grammar(string startSymbol, set<string> terminals)
{
    this->startSymbol = startSymbol;
    this->terminals = terminals;
}

void Grammar::removeVariable(const string &A)
{
    variables.erase(A);
    productions.erase(A);

    for (auto &pair : productions)
    {
        auto &prodSet = pair.second;
        set<vector<string>> toRemove;
        set<vector<string>> toAdd;

        for (const auto &rhs : prodSet)
        {
            vector<string> newRhs;
            bool changed = false;

            for (const string &sym : rhs)
            {
                if (sym == A)
                {
                    changed = true;
                }
                else
                {
                    newRhs.push_back(sym);
                }
            }

            if (changed)
            {
                toRemove.insert(rhs);
                if (!newRhs.empty())
                {
                    toAdd.insert(newRhs);
                }
            }
        }

        for (const auto &r : toRemove)
            prodSet.erase(r);
        for (const auto &r : toAdd)
            prodSet.insert(r);
    }
}

void Grammar::addProduction(string A, vector<string> rhs)
{
    if (this->variables.count(A) == 0)
    {
        this->addVariable(A);
    }
    this->productions[A].insert(rhs);
}

void Grammar::addVariable(string A)
{
    variables.insert(A);
}

void Grammar::removeProduction(string A, vector<string> rhs)
{
    productions[A].erase(rhs);
    // if (productions[A].empty())
    // {
    //     productions.erase(A);
    // }
}

void Grammar::clearProductions(string A)
{
    productions.erase(A);
}

set<vector<string>> Grammar::getProductions(string A)
{
    auto it = productions.find(A);
    if (it != productions.end())
    {
        return it->second;
    }
    return set<vector<string>>();
}

void Grammar::print(ostream &out)
{
    bool aux = true;
    out << "Gramática atual... \n";

    out << "Inicial: " << this->getStartSymbol() << "\n";
    out << "Terminais: <";
    for (auto t : terminals)
    {
        if (!aux)
        {
            out << ", ";
        }
        aux = false;
        out << t;
    }
    out << ">\n";

    auto startGrammar = productions.find(startSymbol);
    if (startGrammar != productions.end())
    {
        out << startGrammar->first << " -> ";
        aux = true;
        for (auto t : startGrammar->second)
        {
            if (!aux)
            {
                out << " | ";
            }
            aux = false;
            for (auto p : t)
            {
                out << p;
            }
        }
        out << "\n";
    }
    for (auto p : productions)
    {
        if (p.first == startSymbol)
        {
            continue;
        }
        out << p.first << " -> ";
        aux = true;
        for (auto ps : p.second)
        {
            if (!aux)
            {
                out << " | ";
            }
            aux = false;
            for (auto pss : ps)
            {
                out << pss;
            }
        }
        out << "\n";
    }
    out << "\n";
}

Grammar Grammar::clone() const
{
    Grammar g2;
    g2.terminals = this->terminals;
    g2.startSymbol = this->startSymbol;
    g2.productions = this->productions;
    g2.variables = this->variables;
    return g2;
}

bool Grammar::isVariable(const string &symbol) const
{
    return variables.count(symbol) > 0;
}
