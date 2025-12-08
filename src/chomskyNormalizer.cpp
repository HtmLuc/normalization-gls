#include "../header/grammar.h"
#include "../header/chomskyNormalizer.h"
#include <string>
#include <queue>

using namespace std;

ChomskyNormalizer::ChomskyNormalizer(const Grammar& g) {
  this->grammar = g;
}

void ChomskyNormalizer::removeRecursionAtBeginning(){
  string S = (this->grammar).getStartSymbol();
  set<vector<string>> productionsS = (this->grammar).getProductions(S);
  
  for(auto& prod : productionsS){
      for(auto& symbol : prod){
        if(symbol == S){
          (this->grammar).setStartSymbol("S'");
          (this->grammar).addProduction("S'", {"S"});
          break; 
        }
      }
  }
}

set<string> ChomskyNormalizer::findVoidableVariables(){
  Grammar g = this->grammar.clone();
  set<string> voidableVariables;
  bool changed = true;

  for(string variable : g.getVariables()){
    for(vector<string> production : g.getProductions(variable)){
      if(production.size() == 1){
        if(production[0] == "&"){
          voidableVariables.insert(variable);
          break;
        }
      }
    }
  }

  while (changed) {
    changed = false;

    for (const string& A : g.getVariables()) {
      if (voidableVariables.count(A)) continue; // já é anulável

      for (const vector<string>& rhs : g.getProductions(A)) {
        bool allNullable = true;

        for (const string& symbol : rhs) {
          if ( (g.getVariables().count(symbol) == 0) && symbol != "&") {
            allNullable = false;
            break;
          }
          if (!voidableVariables.count(symbol)) {
            allNullable = false;
            break;
          }
        }

        if (allNullable) {
          voidableVariables.insert(A);
          changed = true;
          break;
        }
      }
    }
}

  return voidableVariables;
}

vector<int> getNullablePositionsRHS(const vector<string>& rhs, set<string> voidableVariables){
  vector<int> nullablePositions;
  
  for(int i = 0; i < (int)rhs.size(); i++){
    if(voidableVariables.count(rhs[i])){
      nullablePositions.push_back(i);
    }
  }

  return nullablePositions;
}

vector<string> generateAuxiliaryRHS(vector<string> rhs, vector<int> nullablePositions, int mask){
  int k = nullablePositions.size();
  vector<string> newRhs = rhs;

  for (int bit = 0; bit < k; bit++) {
    if (((mask >> bit) & 1) == 0) {
      int pos = nullablePositions[bit];
      newRhs[pos] = "#REMOVE#";
    }
  }

  return newRhs;
}

void ChomskyNormalizer::removeLambdaProductions(){
  set<string> voidableVariables = findVoidableVariables();
  set<string> variables = (this->grammar).getVariables();

  for(string A : (this->grammar).getVariables()){
    cout << "\n\nVariavel analisada:" << A << endl;
    set<vector<string>> productionsA = (this->grammar).getProductions(A);

    for(const vector<string>& rhs : productionsA){
      vector<int> nullablePositions = getNullablePositionsRHS(rhs, voidableVariables);
      int k = nullablePositions.size();
      if(k == 0) continue;

      int total = 1 << k;

      for(int mask = 0; mask < total; mask++){
        vector<string> newRhs = generateAuxiliaryRHS(rhs, nullablePositions, mask);
        vector<string> cleaned;

        for (auto& s : newRhs) {
            if (s != "#REMOVE#")
                cleaned.push_back(s);
        }

        if(cleaned.empty()) continue;
        (this->grammar).addProduction(A, cleaned);
      }
    }  
  }

  for (string A : (this->grammar).getVariables()) {
      vector<string> lambda = {"&"};
      (this->grammar).removeProduction(A, lambda);
  }

  string S = (this->grammar).getStartSymbol();
  if (voidableVariables.count(S)) {
      (this->grammar).addProduction(S, {"&"});
  }
}

set<string> ChomskyNormalizer::findUnitProductionsVar(string& A){
  Grammar g = this->grammar.clone();
  set<vector<string>> productionsA = g.getProductions(A);
  set<string> unitProductionsA;

  for(vector<string> rhs : productionsA){
    if((int)rhs.size() == 1){
      set<string> variables = g.getVariables();
      if(variables.count(rhs[0])){
        unitProductionsA.insert(rhs[0]);
        //cout << "Producao unitaria de " << A << " : " << rhs[0] << endl;
      }
    }
  }

  return unitProductionsA;
}

set<string> ChomskyNormalizer::findVariableChain(string& A){
  set<string> chainA = {A};
  set<string> prev;

  do{
    set<string> newVars;
    for(string v : chainA){
      if(!prev.count(v)){
        newVars.insert(v);
      }
    }
    prev = chainA;
    for(string B : newVars){
      set<string> unitProductionsB = findUnitProductionsVar(B);
      for(string unitProd : unitProductionsB){
        chainA.insert(unitProd);
      }
    }
  } while (chainA != prev);

  return chainA;
}

void ChomskyNormalizer::removeUnitProductions(){
  Grammar g = this->grammar.clone();
  Grammar result = this->grammar.clone();
  set<string> variables = g.getVariables();

  for(string A : variables){
    set<string> unitProductionsA = findUnitProductionsVar(A);
    if((int)unitProductionsA.size() == 0){
      continue;
    }
    // add new productions
    set<string> chainA = findVariableChain(A);

    for(string B : chainA){
      for(vector<string> production : g.getProductions(B)){
        if (!(production.size() == 1 && g.isVariable(production[0]))) {
            result.addProduction(A, production);
        }
      }
    }
  }
  // remove unit productions
  for(string A : variables){
    vector<vector<string>> toRemove;
    for (auto prod : result.getProductions(A)) {
        if (prod.size() == 1 && result.isVariable(prod[0])) {
            toRemove.push_back(prod);
        }
    }
    for (auto prod : toRemove){
        result.removeProduction(A, prod);
    }
  }

  updateGrammar(result);
}

set<string> ChomskyNormalizer::getTerm() {
    Grammar g = this->grammar.clone();
    set<string> term;

    for (string A : g.getVariables()) {
        for (auto prod : g.getProductions(A)) {
            bool allTerminals = true;
            for (string s : prod) {
                if (g.isVariable(s)) {
                    allTerminals = false;
                    break;
                }
            }

            if(allTerminals){
                term.insert(A);
                break;
            }
        }
    }

    bool changed = true;

    while (changed) {
        changed = false;
        for (string A : g.getVariables()) {
            if (term.count(A)) continue; 
            for (auto prod : g.getProductions(A)) {
                bool allInTerm = true;
                for (string s : prod) {
                    if (g.isVariable(s) && !term.count(s)) {
                        allInTerm = false;
                        break;
                    }
                }
                if (allInTerm) {
                    term.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }

    return term;
}

set<string> ChomskyNormalizer::getReach() {
    Grammar g = this->grammar.clone();
    string start = g.getStartSymbol();

    set<string> reach;
    queue<string> q;             

    reach.insert(start);
    q.push(start);

    while (!q.empty()) {
      string A = q.front();
      q.pop();

      for (const auto& prod : g.getProductions(A)) {
        for (const string& symbol : prod) {
          if (g.isVariable(symbol) && !reach.count(symbol)) {
            reach.insert(symbol);
            q.push(symbol);
          }
        }
      }
    }
    cout << "reach:\n";
    for (const string& v : reach)
        cout << v << endl;

    return reach;
}

void ChomskyNormalizer::removeUselessSymbols(){
    set<string> variables = (this->grammar).getVariables();
    set<string> term = getTerm();

    cout << "Conjunto Term: " << (int)term.size() << endl;
    for(string s : term){
        cout << s << " ";
    }
    cout << endl;

    for(string A : variables){
        if(!term.count(A)){
            cout << "removendo variavel: " << A << endl;
            (this->grammar).removeVariable(A);
        }
    }

    set<string> reach = getReach();

    cout << "Conjunto Reach: " << (int)reach.size() << endl;
    for(string s : reach){
        cout << s << " ";
    }
    cout << endl;

    set<string> new_variables = (this->grammar).getVariables();
    for(string A : new_variables){
        if(!reach.count(A)){
            cout << "removendo variavel: " << A << endl;
            (this->grammar).removeVariable(A);
        }
    }

    (this->grammar).print(cout);
}

void ChomskyNormalizer::toChomskyNormalForm(){
  removeRecursionAtBeginning();
  removeLambdaProductions();
  removeUnitProductions();
  removeUselessSymbols();
  //fixLongProductions();
}