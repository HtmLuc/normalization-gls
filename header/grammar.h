#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

/// @brief Classe que representa uma Gramática Livre de Contexto (GLC).
/// Ela expõe métodos úteis para sua representação e conversão entre formas normais.
/// Internaemnte, possui diversos métodos de simplificação de si própria que são organizados
/// nos métodos de conversão para as formas normais.
class Grammar
{
private:
    /// @brief Conjunto de símbolos terminais (alfabeto) da gramática.
    set<string> terminals;
    /// @brief Conjunto de variáveis da gramática.
    set<string> variables;

    /// @brief A variável que representa o símbolo inicial da gramática.
    string startSymbol;

    /// @brief Dicionário de produções, mapeia uma variável a um conjunto não-repetido de vetores de strings.
    /// @details Cada produção é um vetor de string pois isso permite armazenar não só 1 único caractere
    /// (A, a), mas também símbolos especiais e, portanto, regras com nomes mais específicos
    /// (D_1, A').
    map<string, set<vector<string>>> productions;

    /// @brief Dicionário que mapeia uma variável a um inteiro.
    /// @details Parte importante do algoritmo de conversão em Greibach, onde,
    /// para evitar reenomear todas as variáveis, basta associar um índice numérico a cada.
    unordered_map<string, int> order;

    /// @brief Inteiro que guarda o próximo índice numérico disponível.
    /// @details Deve ser usado somente no mesmo contexto do order.
    int nextOrder;

    /// @brief Função que realiza a substituição de todas as ocorrências de `rhs` em `lhs`
    /// @param lhs Variável à esquerda
    /// @param vRhs Variável à direita

    void replace(string lhs, string vRhs);
    /// @brief Função que remove a recursão à esquerda das produções de `lhs` por meio da adição de uma nova variável.
    /// @param lhs A variável que contém produções recursivas.
    /// @return A nova variável criada, usualmente "lhs'".

    string removeLeftRecursion(string lhs);
    /// @brief Função que associa uma variável a um número, montando uma ordem arbitrária.
    /// @details Fundamental para a conversão de Greibach.
    void orderVariables();

    /// @brief Função que varre todas as produções, de A1 até An, buscando ordenar cada produção para que atenda à
    /// regra Am -> An... ou Am -> a, onde m < n.
    /// Configura-se como o 2º passo do algoritmo para a Forma Normal de Greibach.
    /// @details Para realizar essa ordenação, os métodos da substituição (`replace`) e da remoção da recursão
    /// (`removeLeftRecursion`) são usados quando m > n e m = n, respectivamente.
    void applyRuleOrderConstraint();

    /// @brief Função que vai de An até A_1 substituindo as variáveis que vêm após An.
    /// Configura-se como o 3º passo do algoritmo para a Forma Normal de Greibach.
    /// Esta função funciona pois, dado um Ak arbitrário, Aj tal que k < j já está na Forma Normal de Greibach.
    /// @details Deve ser executada após `applyRuleOrderConstraint`, pois espera que todas as produções estejam
    /// seguindo a ordem definida em `order`
    void replaceBackwards();

    /// @brief Função que busca pelas variáveis recursivas e que ainda contém produções que não atendem à Forma Normal de Greibach,
    /// aplicando o método da substituição para normalizar.
    /// Confiugra-se como o 4º e último passo do algoritmo para a Forma Normal de Greibach.
    /// @details A identificação de uma variável recursiva é feita buscando por variáveis que terminam com '
    void replaceRecursiveVariables();

    set<string> findVariableChain(string &A);
    set<string> findUnitProductionsVar(string &A);
    set<string> getTerm();
    set<string> getReach();

    /// @brief Recupera as variáveis que têm produções lambda, sejam produções diretas ou derivadas.
    /// @return O conjunto de variáveis anuláveis.
    set<string> findVoidableVariables();

    /// @brief Função que remove a recursão na variável inicial caso exista. Configura-se como o 1º passo
    /// do algoritmo para a Forma Normal de Chomksy.
    void removeRecursionAtBeginning();

    /// @brief Função que remove todas as produções que têm lambda, gerando permutações da produção original.
    /// Configura-se como o 2º passo do algoritmo para a Forma Normal de Chomsky.
    void removeLambdaProductions();

    /// @brief Função que remove todas as produções unitárias, i.e. S -> A | B | Z, adicionando
    /// as produções da respectiva variável nas produções da variável à esquerda.
    /// Configura-se como o 3º passo do algoritmo para a Forma Normal de Chomsky.
    void removeUnitProductions();

    /// @brief Função que remove variáveis inúteis (inalcançáveis ou não-geradoras) da gramática.
    /// Configura-se como o 4º passo do algoritmo para a Forma Normal de Chomksy.
    void removeUselessVariables();

    /// @brief Função que remove todas as produções que intercalam terminais e variáveis, i.e. S -> aBa,
    /// adicionando uma nova produção composta somente por variáveis.
    /// Configura-se como o 5º passo do algoritmo para a Forma Normal de Chomksy.
    void removeMixedProductions();

    /// @brief Função que, dadas todas as regras compostas somente de variáveis ou 1 símbolo,
    /// separa regras com tamanho maior que 2 em novas variáveis para a gramática.
    /// Configura-se como o 6º e último passo do algoritmo para a Forma Normal de Chomsky.
    void fixLongProductions();

public:
    Grammar();
    Grammar(string startSymbol, set<string> terminals);

    void addTerminal(string a) { this->terminals.insert(a); }
    void addVariable(string A);
    void addProduction(string A, vector<string> rhs);
    void removeProduction(string A, vector<string> rhs);
    void clearProductions(string A);
    bool isVariable(const string &symbol) const;
    void removeVariable(const string &A);
    Grammar clone() const;
    set<vector<string>> getProductions(string A);
    set<string> getTerminals() { return terminals; }
    string getStartSymbol() { return startSymbol; }
    set<string> getVariables() { return variables; }
    void setVariables(set<string> v) { this->variables = v; }
    void setStartSymbol(string A) { this->startSymbol = A; }

    /// @brief Método que converte a gramática atual de forma permanente à Forma Normal de Chomksy.
    void toChomskyNormalForm();
    /// @brief Método que converte a gramática atual de forma permanente à Forma Normal de Greibach.
    /// @details Internamente, este método requer que a gramática esteja na Forma Normal de Chomksy, logo,
    /// primeiro há esta normalização para em seguida haver a normalização para Greibach.
    void toGreibachNormalForm();
    /// @brief Imprime as informações formatadas da gramática.
    /// @param out Canal para onde a informação será exibida.
    void print(ostream &out);
};

#endif