#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
using namespace std;

inline vector<string> getSortedVariables(const unordered_map<string, int> &map)
{
    vector<string> sortedVariables(map.size());
    for (auto &[val, idx] : map)
    {
        sortedVariables[idx - 1] = val;
    }

    return sortedVariables;
}

inline bool productionContainsTerminal(const vector<string> &rhs, const set<string> &terminals)
{
    for (auto sym : rhs)
    {
        if (terminals.count(sym) == 1)
        {
            return true;
        }
    }
    return false;
}

inline vector<int> getNullablePositionsRHS(const vector<string> &rhs, set<string> voidableVariables)
{
    vector<int> nullablePositions;

    for (int i = 0; i < (int)rhs.size(); i++)
    {
        if (voidableVariables.count(rhs[i]))
        {
            nullablePositions.push_back(i);
        }
    }

    return nullablePositions;
}

inline vector<string> generateAuxiliaryRHS(vector<string> rhs, vector<int> nullablePositions, int mask)
{
    int k = nullablePositions.size();
    vector<string> newRhs = rhs;

    for (int bit = 0; bit < k; bit++)
    {
        if (((mask >> bit) & 1) == 0)
        {
            int pos = nullablePositions[bit];
            newRhs[pos] = "#REMOVE#";
        }
    }

    return newRhs;
}
#endif // !1