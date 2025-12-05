#include <fstream>
#include <iostream>
#include "../header/grammar.h"
#include "../header/chomskyNormalizer.h"

void testes_lambda(Grammar& g){
    cout << "---------------------------------------" << endl;
    ChomskyNormalizer normalizer(g);

    Grammar v2 = normalizer.removeLambdaProductions();

    cout << "\n\n---------------------------------------" << endl;

    cout << "\nGramática sem regras-&:\n";

    v2.print(cout);

    cout << "---------------------------------------" << endl;
}

int main() {
    ofstream file("grammar.txt");

    Grammar g("S", { "a", "b", "&"});

    g.addProduction("S", {"a", "S", "b"});
    g.addProduction("S", {"a", "b"});
    g.addProduction("S", {"b"});
    g.addProduction("A", {"b", "B"});
    g.addProduction("B", {"&"});
    g.addProduction("B", {"C", "b", "a"});
    g.addProduction("C", {"&"});
    g.addProduction("C", {"C"});

    cout << "Gramática original:\n";
    g.print(cout);

    cout << "\nProduções de \"S\" obtidas por getProductions:\n";
    auto prodsS = g.getProductions("S");
    for (const auto& rhs : prodsS) {
        cout << "  S ->";
        for (const auto& sym : rhs) {
            cout << " \"" << sym << "\"";
        }
        cout << endl;
    }

    // cout << "\nProduções de \"X\" (não existe na gramática):\n";
    // auto prodsX = g.getProductions("X");
    // cout << "  quantidade de produções de X: " << prodsX.size() << endl;

    // cout << "\nGramática original (para conferir que não mudou):\n";
    // g.print(file);

    ChomskyNormalizer normalizer(g);
    Grammar v2 = normalizer.removeRecursionAtBeginning();
    //v2.print(cout);

    testes_lambda(v2);

    return 0;
}
