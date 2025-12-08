#include <fstream>
#include <iostream>
#include "../header/grammar.h"
#include "../header/logger.h"

void printProducions(Grammar &g)
{
    cout << "\nProduções de \"S\" obtidas por getProductions:\n";
    auto prodsS = g.getProductions("S");
    for (const auto &rhs : prodsS)
    {
        cout << "  S ->";
        for (const auto &sym : rhs)
        {
            cout << " \"" << sym << "\"";
        }
        cout << endl;
    }
    cout << endl;
}

int main()
{
    ofstream file("grammar.txt");

    Logger::setStream(&cout); // temp, depois passa file

    Grammar g("S", {"a", "b"});
    g.addProduction("S", {"A", "B", "C"});
    g.addProduction("A", {"a", "A"});
    g.addProduction("A", {"&"});
    g.addProduction("B", {"b", "B"});
    g.addProduction("B", {"&"});
    g.addProduction("C", {"c", "C"});
    g.addProduction("C", {"&"});

    cout << "Gramática original:\n";
    g.print(Logger::log());

    g.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g.print(Logger::log());

    g.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g.print(Logger::log());

    cout << "-------------------------------------------------------------" << endl;

    Grammar g2("S", {"a", "b", "c"});

    g2.addProduction("S", {"A", "a", "A"});
    g2.addProduction("A", {"A", "B", "C"});
    g2.addProduction("B", {"c", "C"});
    g2.addProduction("B", {"C", "C"});
    g2.addProduction("C", {"a", "b", "a"});
    g2.addProduction("C", {"&"});

    cout << "Gramática original:\n";
    g2.print(Logger::log());

    g2.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g2.print(Logger::log());

    g2.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g2.print(Logger::log());

    cout << "-------------------------------------------------------------" << endl;

    Grammar g3("S", {"a", "b", "c"});

    g3.addProduction("S", {"a", "B", "C"});
    g3.addProduction("S", {"B", "a"});
    g3.addProduction("A", {"a", "A"});
    g3.addProduction("A", {"&"});
    g3.addProduction("B", {"A", "C"});
    g3.addProduction("C", {"c", "A"});
    g3.addProduction("C", {"b"});
    g3.addProduction("C", {"A"});

    cout << "Gramática original:\n";
    g3.print(Logger::log());

    g3.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g3.print(Logger::log());

    g3.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g3.print(Logger::log());

    cout << "-------------------------------------------------------------" << endl;

    Grammar g4("S", {"a", "b", "c"});

    g4.addProduction("S", {"a", "A"});
    g4.addProduction("S", {"b", "A", "B"});
    g4.addProduction("A", {"a", "b", "c"});
    g4.addProduction("A", {"&"});
    g4.addProduction("B", {"A", "c"});
    g4.addProduction("B", {"A", "A"});
    g4.addProduction("C", {"a", "B", "C"});
    g4.addProduction("C", {"a", "B"});

    cout << "Gramática original:\n";
    g4.print(Logger::log());

    g4.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g4.print(Logger::log());

    g4.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g4.print(Logger::log());

    cout << "-------------------------------------------------------------" << endl;

    Grammar g5("S", {"a", "b", "c", "d"});

    g5.addProduction("S", {"a", "S"});
    g5.addProduction("S", {"b"});
    g5.addProduction("S", {"A"});
    g5.addProduction("A", {"a", "A"});
    g5.addProduction("A", {"a"});
    g5.addProduction("A", {"C"});
    g5.addProduction("B", {"a"});
    g5.addProduction("B", {"b"});
    g5.addProduction("C", {"c"});
    g5.addProduction("C", {"B"});
    g5.addProduction("D", {"d", "D"});
    g5.addProduction("D", {"B"});

    cout << "Gramática original:\n";
    g5.print(Logger::log());

    g5.removeUnitProductions();
    cout << "\nRemovendo regras unitarias: " << endl;
    g5.print(Logger::log());

    cout << "-------------------------------------------------------------" << endl;

    Grammar g6("S", {"a", "b", "c", "d"});

    g6.addProduction("S", {"A"});
    g6.addProduction("A", {"B"});
    g6.addProduction("B", {"C"});
    g6.addProduction("C", {"D"});
    g6.addProduction("D", {"d"});

    cout << "Gramática original:\n";
    g6.print(Logger::log());

    g6.removeUnitProductions();
    cout << "\nRemovendo regras unitarias: " << endl;
    g6.print(Logger::log());

    // g.addProduction("S", {"A", "A"});
    // g.addProduction("S", {"a"});
    // g.addProduction("A", {"S", "A"});
    // g.addProduction("A", {"b"});
    // g.toGreibachNormalForm();

    return 0;
}
