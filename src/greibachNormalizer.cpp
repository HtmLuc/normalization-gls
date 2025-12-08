#include "../header/greibachNormalizer.h"
#include "../header/chomskyNormalizer.h"
#include <hash_map>
#include <algorithm>

vector<string> getSortedVariables(unordered_map<string, int> map)
{
    vector<string> sortedVariables(map.size());
    for (auto [val, idx] : map)
    {
        sortedVariables[idx - 1] = val;
    }

    return sortedVariables;
}

void GreibachNormalizer::renameVariables()
{
    this->next_order = 2;
    auto start = this->grammar.getStartSymbol();
    this->order[start] = 1;

    for (auto v : this->grammar.getVariables())
    {
        if (v != start)
        {
            this->order[v] = this->next_order++;
        }
    }

    auto sortedVariables = getSortedVariables(this->order);
    this->sortedVarList = sortedVariables;
}

bool GreibachNormalizer::hasOrder()
{
    for (auto k : this->sortedVarList)
    {
        for (vector<string> p : this->grammar.getProductions(k))
        {
            string first_var = p.front();

            if (this->grammar.isVariable(first_var))
            {
                if (order[first_var] <= order[k])
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void GreibachNormalizer::removeLeftRecursionGreibach(string lhs)
{
    set<vector<string>> productionsA = this->grammar.getProductions(lhs);
    vector<vector<string>> recursive;
    vector<vector<string>> non_recursive;
    string new_lhs = lhs + "'";

    // Divide as regras entre recursivas e não recursivas
    for (auto &prod : productionsA)
    {
        auto symbol = prod.front();
        if (symbol == lhs)
        {
            vector<string> beta(prod.begin() + 1, prod.end());
            recursive.push_back(beta);
        }
        else
        {
            non_recursive.push_back(prod);
        }
    }

    if (!recursive.empty())
    {
        // Remove todas as regras pra depois adicionar novamente, evitando mexer
        this->grammar.clearProductions(lhs);
        this->grammar.addVariable(new_lhs);
        this->order[new_lhs] = this->next_order++;
        this->sortedVarList.push_back(new_lhs);

        // Criar agora as regras recursivas
        for (auto beta : recursive)
        {
            this->grammar.addProduction(new_lhs, beta); // A' -> B
            vector<string> betaB = beta;
            betaB.push_back(new_lhs);
            this->grammar.addProduction(new_lhs, betaB); // A' -> B | BA'
        }

        // Adicionar as regras não recursivas
        for (auto og : non_recursive)
        {
            this->grammar.addProduction(lhs, og); // A -> a

            vector<string> ogRec = og;
            ogRec.push_back(new_lhs);
            this->grammar.addProduction(lhs, ogRec); // A -> a | aA'
        }
    }
}

void GreibachNormalizer::applyRuleOrderConstraint()
{
    while (!this->hasOrder())
    {
        // Analisaremos cada regra de cada variável para ver se está no padrão.
        for (auto k : this->sortedVarList)
        {
            for (auto p : this->grammar.getProductions(k))
            {
                string j = p.front();
                // Se começa com terminal ou se a regra Ak -> Aj tem j > k,
                // já está seguindo a ordem.
                if (!this->grammar.isVariable(j) || order[j] > order[k])
                {
                    continue;
                }
                else if (order[j] < order[k])
                {
                    this->replace(k, j);
                }
                else
                { // Aplica remoção de recursão à esquerda
                    this->removeLeftRecursionGreibach(k);
                }
            }
        }
    }
}

void GreibachNormalizer::replace(string lhs, string vRhs)
{
    set<vector<string>> to_remove;
    set<vector<string>> to_add;

    auto current_productions = this->grammar.getProductions(lhs);
    auto replacer_productions = this->grammar.getProductions(vRhs);

    for (const auto &p : current_productions)
    {
        if (!p.empty() && p.front() == vRhs)
        {
            to_remove.insert(p);

            // extrai o resto da regra para além da 1a variábel
            vector<string> beta;
            if (p.size() > 1)
            {
                beta.assign(p.begin() + 1, p.end());
            }

            // para cada regra de A_j, adicionamos em A_k seguido do beta
            for (const auto &replacer_rhs : replacer_productions)
            {
                vector<string> new_rhs = replacer_rhs;
                new_rhs.insert(new_rhs.end(), beta.begin(), beta.end());
                to_add.insert(new_rhs);
            }
        }
    }

    // aplica as mudanças
    for (const auto &prod : to_remove)
        this->grammar.removeProduction(lhs, prod);
    for (const auto &prod : to_add)
        this->grammar.addProduction(lhs, prod);
}

void GreibachNormalizer::replaceBackwards()
{
    auto variables = this->sortedVarList;
    for (auto v = variables.rbegin(); v != variables.rend(); v++)
    {
        for (auto p : this->grammar.getProductions(*v))
        {
            string j = p.front();
            string k = *v;

            if (!this->grammar.isVariable(j))
            {
                continue;
            }
            else if (order[j] > order[k])
            {
                this->replace(k, j); // Forçar com que todas as regras de k que iniciam com j iniciem com terminais
            }
        }
    }
}

Grammar *GreibachNormalizer::normalize()
{
    this->renameVariables();
    this->applyRuleOrderConstraint();
    this->replaceBackwards();
    // Step 4 não parece mais necessário agora...

    grammar.print(cout);

    return &grammar;
}