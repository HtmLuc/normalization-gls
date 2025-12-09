#include "../header/grammar.h"
#include "../header/logger.h"
#include "../header/utils.h"

// TODO: mudar nome desse método
void Grammar::renameVariables()
{
    this->next_order = 2;
    auto start = this->getStartSymbol();
    this->order[start] = 1;

    for (auto v : this->getVariables())
    {
        if (v != start)
        {
            this->order[v] = this->next_order++;
        }
    }

    Logger::log() << "Numerando em ordem as variáveis da gramática. \n";
    auto sorted = getSortedVariables(this->order);

    for (auto v : sorted)
    {
        Logger::log() << v << ": " << order[v] << "\n";
    }
}

bool Grammar::respectGreibachOrder()
{
    for (auto k : this->variables)
    {
        for (vector<string> p : this->getProductions(k))
        {
            string first_var = p.front();

            if (this->isVariable(first_var))
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

string Grammar::removeLeftRecursion(string lhs)
{
    // 1. Obter uma CÓPIA das produções atuais para evitar erro de iterador
    set<vector<string>> current_productions = this->getProductions(lhs);

    vector<vector<string>> recursive_alphas;
    vector<vector<string>> non_recursive_betas;
    string new_lhs = lhs + "'";

    // 2. Classificação das Regras (Bufferizar)
    for (const auto &prod : current_productions)
    {
        if (!prod.empty() && prod.front() == lhs)
        {
            // A -> A alpha
            vector<string> alpha(prod.begin() + 1, prod.end());
            recursive_alphas.push_back(alpha);
        }
        else
        {
            // A -> beta
            non_recursive_betas.push_back(prod);
        }
    }

    // Só transformamos se houver recursão à esquerda detectada
    if (!recursive_alphas.empty())
    {
        // 3. Modificação da Gramática Original (Limpando as regras antigas)
        this->clearProductions(lhs);

        // 4. Registrar a nova variável no set interno (Garante que isVariable funcione)
        this->addVariable(new_lhs);

        // 5. Criar regras para a nova variável: A' -> alpha | alpha A'
        for (const auto &alpha : recursive_alphas)
        {
            this->addProduction(new_lhs, alpha); // A' -> alpha

            vector<string> alpha_with_prime = alpha;
            alpha_with_prime.push_back(new_lhs);
            this->addProduction(new_lhs, alpha_with_prime); // A' -> alpha A'
        }

        // 6. Atualizar as regras da variável original: A -> beta | beta A'
        for (const auto &beta : non_recursive_betas)
        {
            this->addProduction(lhs, beta); // A -> beta (mantém a original limpa)

            vector<string> beta_with_prime = beta;
            beta_with_prime.push_back(new_lhs);
            this->addProduction(lhs, beta_with_prime); // A -> beta A'
        }

        return new_lhs; // Retorna para que o chamador gerencie a ordem (Greibach)
    }

    return ""; // Nenhuma variável nova criada
}

void Grammar::applyRuleOrderConstraint()
{
    // 1. Fixar a ordem: Criamos um vetor estável.
    // O algoritmo de Greibach exige processar de V1 até Vn na ordem numérica.
    auto sorted = getSortedVariables(order);

    // 2. Loop de Recursão à Esquerda e Substituição Direta (Passo 1 a n)
    for (size_t k_idx = 0; k_idx < sorted.size(); ++k_idx)
    {
        string k = sorted[k_idx];
        bool changed = true;

        // Reanalisamos a variável 'k' até que ela esteja limpa
        while (changed)
        {
            changed = false;
            // COPIA LOCAL: Essencial para evitar Segmentation Fault
            auto productionsK = this->getProductions(k);

            for (auto p : productionsK)
            {
                if (p.empty())
                    continue;
                string j = p.front();

                if (this->isVariable(j))
                {
                    int oJ = order[j];
                    int oK = order[k];

                    if (oJ < oK)
                    {
                        // j < k: Substituição direta
                        this->replace(k, j);
                        changed = true;
                        break; // Sai do for para re-iterar com as novas produções de k
                    }
                    else if (oJ == oK)
                    {
                        // j == k: Recursão Direta
                        string newVar = this->removeLeftRecursion(k);

                        // Atualiza as estruturas de ordem com a nova variável
                        this->order[newVar] = this->next_order++;
                        this->variables.insert(newVar);

                        // Recursão direta resolvida para k, sai para a próxima variável
                        changed = false;
                        break;
                    }
                }
            }
        }
    }
}

void Grammar::replace(string lhs, string vRhs)
{
    Logger::log() << "Faremos a substituição de " << vRhs << " em " << lhs << ".\n";
    set<vector<string>> to_remove;
    set<vector<string>> to_add;

    auto current_productions = this->getProductions(lhs);
    auto replacer_productions = this->getProductions(vRhs);

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
                vector<string> new_rhs;
                new_rhs.reserve(replacer_rhs.size() + beta.size());

                new_rhs.insert(new_rhs.end(), replacer_rhs.begin(), replacer_rhs.end());
                new_rhs.insert(new_rhs.end(), beta.begin(), beta.end());

                to_add.insert(new_rhs);
            }
        }
    }

    // aplica as mudanças
    for (const auto &prod : to_remove)
        this->removeProduction(lhs, prod);
    for (const auto &prod : to_add)
        this->addProduction(lhs, prod);

    this->print(Logger::log());
}

// void Grammar::replaceBackwards()
// {
//     Logger::log() << "Faremos a substituição nas produções que não estão normalizadas.";
//     auto variables = getSortedVariables(order);

//     for (auto v = variables.rbegin(); v != variables.rend(); v++)
//     {
//         for (auto p : this->getProductions(*v))
//         {
//             string j = p.front();
//             string k = *v;

//             if (!this->isVariable(j))
//             {
//                 continue;
//             }
//             else if (order[j] > order[k])
//             {
//                 this->replace(k, j); // Forçar com que todas as regras de k que iniciam com j iniciem com terminais
//             }
//         }
//     }
//     this->print(Logger::log());
// }
void Grammar::replaceBackwards()
{
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
    Logger::log() << "Iniciando a substituição nas variáveis recursivas. \n";

    auto sorted = getSortedVariables(order);
    vector<string> recursiveVars;

    for (auto v : this->getVariables())
    {
        if (v[v.length() - 1] == '\'')
        {
            recursiveVars.push_back(v);
        }
    }

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

void Grammar::toGreibachNormalForm()
{
    this->toChomskyNormalForm();
    this->print(Logger::log());

    Logger::log() << "Iniciando a normalização para Forma Normal de Greibach!\n";
    this->renameVariables();
    this->applyRuleOrderConstraint();
    this->replaceBackwards();
    this->replaceRecursiveVariables();
    Logger::log() << "Gramática agora está na Forma Normal de Greibach!\n";
}