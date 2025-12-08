#include "../header/grammarParser.h"

Grammar GrammarParser::parserFromFile(string filename)
{
    Grammar grammar;
    vector<string> v;
    bool complete = false;
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "\033[0m" << "Erro: não foi possível abrir o arquivo!" << "\033[31m" << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string ladoEsquerdo, seta;
        v.clear();

        ss >> ladoEsquerdo;
        ss >> seta;

        if (ladoEsquerdo == "Inicial")
        {
            char c;
            while (ss.get(c))
            {
                if (c != ' ' && c != '=')
                {
                    string st(1, c);
                    grammar.setStartSymbol(st);
                }
            }
            complete = true;
        }

        if (ladoEsquerdo == "Alfabeto")
        {
            char c;
            while (ss.get(c))
            {
                if (!isspace(c) && c != '=' && c != '&' && c != '}' && c != '{' && c != ',' && c != '\n')
                {
                    string st(1, c);
                    grammar.addTerminal(st);
                }
            }
            complete = true;
        }

        if (seta == "->")
        {
            char c;
            while (ss.get(c))
            {
                if (c != ' ')
                {
                    if (!complete)
                    {
                        grammar.setStartSymbol(ladoEsquerdo);
                        complete = true;
                    }
                    v.push_back(string(1, c));
                }
            }
            grammar.addProduction(ladoEsquerdo, v);
        }
    }
    return grammar;
}
