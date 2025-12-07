#include <fstream>
#include <iostream>
#include "../header/grammar.h"
#include "../header/chomskyNormalizer.h"

void printProducions(Grammar& g){
    cout << "\nProduções de \"S\" obtidas por getProductions:\n";
    auto prodsS = g.getProductions("S");
    for (const auto& rhs : prodsS) {
        cout << "  S ->";
        for (const auto& sym : rhs) {
            cout << " \"" << sym << "\"";
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    ofstream file("grammar.txt");

    Grammar g("S", { "a", "b"});

    g.addProduction("S", {"A", "B", "C"});
    g.addProduction("A", {"a", "A"});
    g.addProduction("A", {"&"});
    g.addProduction("B", {"b", "B"});
    g.addProduction("B", {"&"});
    g.addProduction("C", {"c", "C"});
    g.addProduction("C", {"&"});

    cout << "Gramática original:\n";
    g.print(cout);

    ChomskyNormalizer normalizer1(g);
    Grammar v2 = normalizer1.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    v2.print(cout);

    Grammar v3 = normalizer1.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    v3.print(cout);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g2("S", { "a", "b", "c"});

    g2.addProduction("S", {"A", "a", "A"});
    g2.addProduction("A", {"A", "B", "C"});
    g2.addProduction("B", {"c", "C"});
    g2.addProduction("B", {"C", "C"});
    g2.addProduction("C", {"a", "b", "a"});
    g2.addProduction("C", {"&"});

    cout << "Gramática original:\n";
    g2.print(cout);

    ChomskyNormalizer normalizer2(g2);
    Grammar g2v2 = normalizer2.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g2v2.print(cout);

    ChomskyNormalizer normalizer2_1(g2v2);
    Grammar g2v3 = normalizer2_1.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g2v3.print(cout);

    ChomskyNormalizer normalizer2_2(g2v3);
    Grammar g2v4 = normalizer2_2.removeLeftRecursion();
    cout << "\nRemovendo recursao à esquerda: " << endl;
    g2v4.print(cout);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g3("S", { "a", "b", "c"});

    g3.addProduction("S", {"a", "B", "C"});
    g3.addProduction("S", {"B", "a"});
    g3.addProduction("A", {"a", "A"});
    g3.addProduction("A", {"&"});
    g3.addProduction("B", {"A", "C"});
    g3.addProduction("C", {"c", "A"});
    g3.addProduction("C", {"b"});
    g3.addProduction("C", {"A"});

    cout << "Gramática original:\n";
    g3.print(cout);

    ChomskyNormalizer normalizer3(g3);
    Grammar g3v2 = normalizer3.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g3v2.print(cout);

    Grammar g3v3 = normalizer3.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g3v3.print(cout);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g4("S", { "a", "b", "c"});

    g4.addProduction("S", {"a", "A"});
    g4.addProduction("S", {"b", "A", "B"});
    g4.addProduction("A", {"a", "b", "c"});
    g4.addProduction("A", {"&"});
    g4.addProduction("B", {"A", "c"});
    g4.addProduction("B", {"A", "A"});
    g4.addProduction("C", {"a", "B", "C"});
    g4.addProduction("C", {"a", "B"});

    cout << "Gramática original:\n";
    g4.print(cout);

    ChomskyNormalizer normalizer4(g4);
    Grammar g4v2 = normalizer4.removeRecursionAtBeginning();
    cout << "\nRemovendo recursao inicial: " << endl;
    g4v2.print(cout);

    Grammar g4v3 = normalizer4.removeLambdaProductions();
    cout << "\nRemovendo regras-lambda: " << endl;
    g4v3.print(cout);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g5("S", { "a", "b", "c", "d"});

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
    g5.print(cout);

    ChomskyNormalizer normalizer5(g5);
    Grammar g5v2 = normalizer5.removeUnitProductions();
    cout << "\nRemovendo regras unitarias: " << endl;
    g5v2.print(cout);

    cout << "-------------------------------------------------------------" << endl;

    Grammar g6("S", { "a", "b", "c", "d"});

    g6.addProduction("S", {"A"});
    g6.addProduction("A", {"B"});
    g6.addProduction("B", {"C"});
    g6.addProduction("C", {"D"});
    g6.addProduction("D", {"d"});

    cout << "Gramática original:\n";
    g6.print(cout);

    ChomskyNormalizer normalizer6(g6);
    Grammar g6v2 = normalizer5.removeUnitProductions();
    cout << "\nRemovendo regras unitarias: " << endl;
    g6v2.print(cout);


    return 0;
}
