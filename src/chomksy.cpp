
#include "../header/grammar.h"
#include "../header/utils.h"
#include <queue>

using namespace std;

void Grammar::fixLongProductions() {
    vector<pair<string, vector<string>>> toRemove;
    vector<pair<string, vector<string>>> toAdd;

    int idx = 1;

    for (auto A : this->getVariables())
    {
        for (auto prod : this->getProductions(A))
        {
            if (prod.size() <= 2) continue;

            toRemove.push_back({A, prod});

            string new_lhs = "M_";
            string suffix = to_string(idx);
            new_lhs.append(suffix);
            idx++;

            vector<string> rhs = { prod[0], new_lhs };
            toAdd.push_back({A, rhs});

            for (int i = 0; i < prod.size() - 2; i++ )
            {
                string next_lhs = "M_";
                suffix = to_string(idx);
                next_lhs.append(suffix);
                idx++;

                rhs = { prod[i + 1], next_lhs };
                toAdd.push_back({next_lhs, rhs});
            }

            rhs = { prod[prod.size() - 2], prod.back() };
            toAdd.push_back({new_lhs, rhs});
        }
    }
    for (auto& rm : toRemove)
    {
        this->removeProduction(rm.first, rm.second);
    }

    for (auto& add : toAdd)
    {
        this->addProduction(add.first, add.second);
    }
}

void Grammar::removeMixedProductions()
{
    map<string, string> newVariables;
    set<pair<string, vector<string>>> toRemove;
    set<pair<string, vector<string>>> toAdd;

    int idx = 1;
    for (auto sym : this->getTerminals())
    {
        string new_lhs = "T_";
        string suffix = to_string(idx);

        new_lhs.append(suffix);
        idx++;

        this->addProduction(new_lhs, {sym});
        newVariables[sym] = new_lhs;
    }

    for (auto var : this->getVariables())
    {
        for (auto prod : this->getProductions(var))
        {
            if (prod.size() > 1 && productionContainsTerminal(prod, this->getTerminals()))
            {
                toRemove.insert({var, prod});

                vector<string> newProd = prod;
                for (auto &sym : newProd)
                {
                    if (!this->isVariable(sym))
                    {
                        sym = newVariables[sym]; // Substitui a por Ta, por ex.
                    }
                }
                toAdd.insert({var, newProd});
            }
        }
    }

    for (auto rm : toRemove)
    {
        this->removeProduction(rm.first, rm.second);
    }
    for (auto add : toAdd)
    {
        this->addProduction(add.first, add.second);
    }
}

void Grammar::removeRecursionAtBeginning()
{
    string S = this->getStartSymbol();
    set<vector<string>> productionsS = this->getProductions(S);

    for (auto &prod : productionsS)
    {
        for (auto &symbol : prod)
        {
            if (symbol == S)
            {
                this->setStartSymbol("S'");
                this->addProduction("S'", {"S"});
                break;
            }
        }
    }
}

set<string> Grammar::findVoidableVariables()
{
    set<string> voidableVariables;
    bool changed = true;

    for (string variable : this->getVariables())
    {
        for (vector<string> production : this->getProductions(variable))
        {
            if (production.size() == 1)
            {
                if (production[0] == "&")
                {
                    voidableVariables.insert(variable);
                    break;
                }
            }
        }
    }

    while (changed)
    {
        changed = false;

        for (const string &A : this->getVariables())
        {
            if (voidableVariables.count(A))
                continue; // já é anulável

            for (const vector<string> &rhs : this->getProductions(A))
            {
                bool allNullable = true;

                for (const string &symbol : rhs)
                {
                    if ((this->getVariables().count(symbol) == 0) && symbol != "&")
                    {
                        allNullable = false;
                        break;
                    }
                    if (!voidableVariables.count(symbol))
                    {
                        allNullable = false;
                        break;
                    }
                }

                if (allNullable)
                {
                    voidableVariables.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }

    return voidableVariables;
}

void Grammar::removeLambdaProductions()
{
    set<string> voidableVariables = findVoidableVariables();
    set<string> variables = this->getVariables();

    for (string A : this->getVariables())
    {
        set<vector<string>> productionsA = this->getProductions(A);

        for (const vector<string> &rhs : productionsA)
        {
            vector<int> nullablePositions = getNullablePositionsRHS(rhs, voidableVariables);
            int k = nullablePositions.size();
            if (k == 0)
                continue;

            int total = 1 << k;

            for (int mask = 0; mask < total; mask++)
            {
                vector<string> newRhs = generateAuxiliaryRHS(rhs, nullablePositions, mask);
                vector<string> cleaned;

                for (auto &s : newRhs)
                {
                    if (s != "#REMOVE#")
                        cleaned.push_back(s);
                }

                if (cleaned.empty())
                    continue;
                this->addProduction(A, cleaned);
            }
        }
    }

    for (string A : this->getVariables())
    {
        vector<string> lambda = {"&"};
        this->removeProduction(A, lambda);
    }

    string S = this->getStartSymbol();
    if (voidableVariables.count(S))
    {
        this->addProduction(S, {"&"});
    }
}

set<string> Grammar::findUnitProductionsVar(string &A)
{
    // TODO: remove this
    Grammar g = this->clone();
    set<vector<string>> productionsA = g.getProductions(A);
    set<string> unitProductionsA;

    for (vector<string> rhs : productionsA)
    {
        if ((int)rhs.size() == 1)
        {
            set<string> variables = g.getVariables();
            if (variables.count(rhs[0]))
            {
                unitProductionsA.insert(rhs[0]);
            }
        }
    }

    return unitProductionsA;
}

set<string> Grammar::findVariableChain(string &A)
{
    set<string> chainA = {A};
    set<string> prev;

    do
    {
        set<string> newVars;
        for (string v : chainA)
        {
            if (!prev.count(v))
            {
                newVars.insert(v);
            }
        }
        prev = chainA;
        for (string B : newVars)
        {
            set<string> unitProductionsB = findUnitProductionsVar(B);
            for (string unitProd : unitProductionsB)
            {
                chainA.insert(unitProd);
            }
        }
    } while (chainA != prev);

    return chainA;
}

void Grammar::removeUnitProductions()
{
    for (string A : this->getVariables())
    {
        set<string> unitProductionsA = findUnitProductionsVar(A);
        if ((int)unitProductionsA.size() == 0)
        {
            continue;
        }
        // add new productions
        set<string> chainA = findVariableChain(A);

        for (string B : chainA)
        {
            for (vector<string> production : this->getProductions(B))
            {
                if (!(production.size() == 1 && this->isVariable(production[0])))
                {
                    this->addProduction(A, production);
                }
            }
        }
    }
    // remove unit productions
    for (string A : variables)
    {
        vector<vector<string>> toRemove;
        for (auto prod : this->getProductions(A))
        {
            if (prod.size() == 1 && this->isVariable(prod[0]))
            {
                toRemove.push_back(prod);
            }
        }
        for (auto prod : toRemove)
        {
            this->removeProduction(A, prod);
        }
    }
}

set<string> Grammar::getTerm()
{
    set<string> term;

    for (string A : this->getVariables())
    {
        for (auto prod : this->getProductions(A))
        {
            bool allTerminals = true;
            for (string s : prod)
            {
                if (this->isVariable(s))
                {
                    allTerminals = false;
                    break;
                }
            }

            if (allTerminals)
            {
                term.insert(A);
                break;
            }
        }
    }

    bool changed = true;

    while (changed)
    {
        changed = false;
        for (string A : this->getVariables())
        {
            if (term.count(A))
                continue;
            for (auto prod : this->getProductions(A))
            {
                bool allInTerm = true;
                for (string s : prod)
                {
                    if (this->isVariable(s) && !term.count(s))
                    {
                        allInTerm = false;
                        break;
                    }
                }
                if (allInTerm)
                {
                    term.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }

    return term;
}

set<string> Grammar::getReach()
{
    string start = this->getStartSymbol();

    set<string> reach;
    queue<string> q;

    reach.insert(start);
    q.push(start);

    while (!q.empty())
    {
        string A = q.front();
        q.pop();

        for (const auto &prod : this->getProductions(A))
        {
            for (const string &symbol : prod)
            {
                if (this->isVariable(symbol) && !reach.count(symbol))
                {
                    reach.insert(symbol);
                    q.push(symbol);
                }
            }
        }
    }

    return reach;
}

void Grammar::removeUselessSymbols()
{
    set<string> variables = this->getVariables();
    set<string> term = getTerm();

    for (string A : variables)
    {
        if (!term.count(A))
        {
            this->removeVariable(A);
        }
    }

    set<string> reach = getReach();

    for (string A : this->getVariables())
    {
        if (!reach.count(A))
        {
            this->removeVariable(A);
        }
    }
}

void Grammar::toChomskyNormalForm()
{
    removeRecursionAtBeginning();
    removeLambdaProductions();
    removeMixedProductions();
    removeUselessSymbols();
    removeUnitProductions();
    fixLongProductions();
}
