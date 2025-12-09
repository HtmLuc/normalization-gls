#include <fstream>
#include <iostream>
#include "../header/grammar.h"
#include "../header/logger.h"
#include "../header/grammarParser.h"

int main()
{
    string file;
    string method;
    cout << "Nome do arquivo com a gramática: ";
    cin >> file;
    string path = "data/" + file;

    GrammarParser parser;
    Grammar grammar = parser.parserFromFile(path);

    static ofstream stream("result.txt");
    Logger::setStream(&stream);

    while (true)
    {
        cout << "Método de normalização: Greibach ou Chomsky? ";
        cin >> method;

        if (method.compare("Greibach") == 0 || method.compare("greibach") == 0 || method.compare("g") == 0)
        {
            grammar.toGreibachNormalForm();
            break;
        }
        else if (method.compare("Chomksy") == 0 || method.compare("chomksy") == 0 || method.compare("c") == 0)
        {
            grammar.toChomskyNormalForm();
            break;
        }
        else
        {
            cout << "Normalização não disponível, tente novamente. \n";
        }
    }

    return 0;
}
