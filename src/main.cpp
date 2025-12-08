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

void callFuncions(Grammar g)
{
    g.print(cout);

    g.toChomskyNormalForm();
    cout << "\nFNCl: " << endl;
    g.print(cout);
}

int main()
{
    ofstream file("grammar.txt");

    Grammar g("S", {"a", "b", "c"});

    g.addProduction("S", {"S"});
    g.addProduction("S", {"A", "B", "C"});
    g.addProduction("A", {"a", "A"});
    g.addProduction("A", {"&"});
    g.addProduction("B", {"b", "B"});
    g.addProduction("B", {"&"});
    g.addProduction("C", {"c", "C"});
    g.addProduction("C", {"&"});

    callFuncions(g);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g2("S", {"a", "b", "c"});

    g2.addProduction("S", {"A", "a", "A"});
    g2.addProduction("A", {"A", "B", "C"});
    g2.addProduction("B", {"c", "C"});
    g2.addProduction("B", {"C", "C"});
    g2.addProduction("C", {"a", "b", "a"});
    g2.addProduction("C", {"&"});

    callFuncions(g2);

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

    callFuncions(g3);

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

    callFuncions(g4);

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

    callFuncions(g5);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g6("S", {"a", "b", "c", "d"});

    g6.addProduction("S", {"A"});
    g6.addProduction("A", {"B"});
    g6.addProduction("B", {"C"});
    g6.addProduction("C", {"D"});
    g6.addProduction("D", {"d"});

    callFuncions(g6);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g7("S", {"a", "b", "c"});

    g7.addProduction("S", {"A", "C"});
    g7.addProduction("S", {"B", "S"});
    g7.addProduction("S", {"B"});
    g7.addProduction("A", {"a", "A"});
    g7.addProduction("A", {"a", "F"});
    g7.addProduction("B", {"C", "F"});
    g7.addProduction("B", {"b"});
    g7.addProduction("C", {"c", "C"});
    g7.addProduction("C", {"D"});
    g7.addProduction("D", {"a", "D"});
    g7.addProduction("D", {"B", "D"});
    g7.addProduction("D", {"C"});
    g7.addProduction("E", {"a", "A"});
    g7.addProduction("E", {"B", "S", "A"});
    g7.addProduction("F", {"b", "B"});
    g7.addProduction("F", {"b"});

    callFuncions(g7);
    return 0;
}
