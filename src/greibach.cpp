#include "../header/grammar.h"
#include "../header/logger.h"
#include "../header/utils.h"

void Grammar::orderVariables()
{
    this->nextOrder = 2;
    auto start = this->getStartSymbol();
    this->order[start] = 1;

    for (auto v : this->getVariables())
    {
        if (v != start)
        {
            this->order[v] = this->nextOrder++;
        }
    }

    Logger::log() << "===============================================\n";
    Logger::log() << "Numerando em ordem as variáveis da gramática. \n";
    auto sorted = getSortedVariables(this->order);

    for (auto v : sorted)
    {
        Logger::log() << v << ": " << order[v] << "\n";
    }
}

string Grammar::removeLeftRecursion(string lhs)
{
    auto productions = this->getProductions(lhs);

    vector<vector<string>> recursive;
    vector<vector<string>> non_recursive;
    string new_lhs = lhs + "'";

    for (const auto &prod : productions)
    {
        if (!prod.empty() && prod.front() == lhs)
        {
            vector<string> alpha(prod.begin() + 1, prod.end());
            recursive.push_back(alpha);
        }
        else
        {
            non_recursive.push_back(prod);
        }
    }

    if (!recursive.empty())
    {
        this->clearProductions(lhs);
        this->addVariable(new_lhs);

        for (const auto &alpha : recursive)
        {
            this->addProduction(new_lhs, alpha);

            vector<string> newAlpha = alpha;
            newAlpha.push_back(new_lhs);
            this->addProduction(new_lhs, newAlpha);
        }

        for (const auto &beta : non_recursive)
        {
            this->addProduction(lhs, beta);

            vector<string> newBeta = beta;
            newBeta.push_back(new_lhs);
            this->addProduction(lhs, newBeta);
        }

        return new_lhs;
    }

    return "";
}

void Grammar::applyRuleOrderConstraint()
{
    auto sorted = getSortedVariables(order);

    Logger::log() << "==================================================\n";
    Logger::log() << "Iniciando a ordenação das produções da gramática. \n";

    for (int k_idx = 0; k_idx < sorted.size(); k_idx++)
    {
        string k = sorted[k_idx];
        bool changed = true;

        while (changed)
        {
            changed = false;
            auto kProds = this->getProductions(k);

            for (auto p : kProds)
            {
                if (p.empty())
                {
                    continue;
                }
                string j = p.front();

                if (this->isVariable(j))
                {
                    if (order[j] < order[k])
                    {
                        this->replace(k, j);
                        changed = true;
                        break;
                    }
                    else if (order[j] == order[k])
                    {
                        string recursiveVar = this->removeLeftRecursion(k);

                        this->order[recursiveVar] = this->nextOrder++;
                        this->variables.insert(recursiveVar);

                        changed = false;
                        break;
                    }
                }
            }
        }
    }

    this->print(Logger::log());
}

void Grammar::replace(string lhs, string vRhs)
{
    Logger::log() << "Faremos a substituição de " << vRhs << " em " << lhs << ".\n";
    set<vector<string>> to_remove;
    set<vector<string>> to_add;

    auto lhsProds = this->getProductions(lhs);
    auto rhsProds = this->getProductions(vRhs);

    for (const auto &p : lhsProds)
    {
        if (!p.empty() && p.front() == vRhs)
        {
            to_remove.insert(p);

            vector<string> beta;
            if (p.size() > 1)
            {
                beta.assign(p.begin() + 1, p.end());
            }

            for (const auto &rhsProd : rhsProds)
            {
                vector<string> new_rhs;
                new_rhs.reserve(rhsProd.size() + beta.size());

                new_rhs.insert(new_rhs.end(), rhsProd.begin(), rhsProd.end());
                new_rhs.insert(new_rhs.end(), beta.begin(), beta.end());

                to_add.insert(new_rhs);
            }
        }
    }

    for (const auto &prod : to_remove)
    {
        this->removeProduction(lhs, prod);
    }
    for (const auto &prod : to_add)
    {
        this->addProduction(lhs, prod);
    }
}

void Grammar::replaceBackwards()
{
    Logger::log() << "================================================\n";
    Logger::log() << "Iniciando a substituição reversa das produções. \n";

    auto sorted = getSortedVariables(order);

    for (int i = sorted.size() - 2; i >= 0; --i)
    {
        string k = sorted[i];
        bool changed = true;

        while (changed)
        {
            changed = false;
            auto prodK = this->getProductions(k);

            for (const auto &p : prodK)
            {
                if (p.empty())
                {
                    continue;
                }

                string j = p.front();

                if (this->isVariable(j) && order[j] > order[k])
                {
                    this->replace(k, j);
                    changed = true;
                    break;
                }
            }
        }
    }
    this->print(Logger::log());
}

void Grammar::replaceRecursiveVariables()
{
    auto sorted = getSortedVariables(order);
    vector<string> recursiveVars;

    for (auto v : this->getVariables())
    {
        if (v[v.length() - 1] == '\'')
        {
            recursiveVars.push_back(v);
        }
    }

    if (!recursiveVars.empty())
    {
        Logger::log() << "===================================================\n";
        Logger::log() << "Iniciando a substituição nas variáveis recursivas. \n";

        for (auto rv : recursiveVars)
        {
            bool changed = true;
            while (changed)
            {
                changed = false;

                auto rvProd = this->getProductions(rv);
                for (auto rvProdVec : rvProd)
                {
                    string k = rvProdVec.front();

                    if (this->isVariable(k))
                    {
                        this->replace(rv, k);
                        changed = true;
                        break;
                    }
                }
            }
        }

        this->print(Logger::log());
    }
}

void Grammar::toGreibachNormalForm()
{
    this->toChomskyNormalForm();
    this->print(Logger::log());

    Logger::log() << "Iniciando a normalização da Gramática para a Forma Normal de Greibach!\n";
    this->orderVariables();
    this->applyRuleOrderConstraint();
    this->replaceBackwards();
    this->replaceRecursiveVariables();
    Logger::log() << "A Gramática agora está na Forma Normal de Greibach!\n";
}