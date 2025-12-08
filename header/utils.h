#include <string>
#include <vector>
#include <set>
#include <unordered_map>
using namespace std;

vector<string> getSortedVariables(const unordered_map<string, int> &map)
{
    vector<string> sortedVariables(map.size());
    for (auto [val, idx] : map)
    {
        sortedVariables[idx - 1] = val;
    }

    return sortedVariables;
}

vector<int> getNullablePositionsRHS(const vector<string> &rhs, set<string> voidableVariables)
{
    vector<int> nullablePositions;

    for (int i = 0; i < (int)rhs.size(); i++)
    {
        // cout << ">>> Componente da producao " << rhs[i] << endl;
        if (voidableVariables.count(rhs[i]))
        {
            // cout << ">>>> Essa variavel eh anulavel! --> " << rhs[i] << endl;
            nullablePositions.push_back(i);
        }
    }
    // if(nullablePositions.size() > 0){
    //   //cout << "\nPosicoes anulaveis da producao: " << endl;
    // }
    // for(int pos : nullablePositions){
    //   cout << pos << endl;
    // }

    return nullablePositions;
}

vector<string> generateAuxiliaryRHS(vector<string> rhs, vector<int> nullablePositions, int mask)
{
    int k = nullablePositions.size();
    vector<string> newRhs = rhs;

    for (int bit = 0; bit < k; bit++)
    {
        // cout << "bit: " << bit << endl;
        // cout << ((mask >> bit) & 1) << endl;
        if (((mask >> bit) & 1) == 0)
        {
            int pos = nullablePositions[bit];
            // cout << "Pos: " << pos << " | nullablePositions[bit]: " << nullablePositions[bit] << " | bit: " << bit << endl;
            newRhs[pos] = "#REMOVE#";
        }
    }

    return newRhs;
}