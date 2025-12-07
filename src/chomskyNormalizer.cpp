#include "../header/grammar.h"
#include "../header/chomskyNormalizer.h"
#include <string>

using namespace std;

ChomskyNormalizer::ChomskyNormalizer(const Grammar& g) {
  this->grammar = g;
}

Grammar ChomskyNormalizer::removeRecursionAtBeginning(){
  Grammar g = this->grammar.clone();
  string S = g.getStartSymbol();
  set<vector<string>> productionsS = g.getProductions(S);
  
  for(auto& prod : productionsS){
      for(auto& symbol : prod){
        if(symbol == S){
          g.setStartSymbol("S'");
          g.addProduction("S'", {"S"});
          break; 
        }
      }
  }

  return g;
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
    //cout << ">>> Componente da producao " << rhs[i] << endl;
    if(voidableVariables.count(rhs[i])){
      //cout << ">>>> Essa variavel eh anulavel! --> " << rhs[i] << endl;
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

vector<string> generateAuxiliaryRHS(vector<string> rhs, vector<int> nullablePositions, int mask){
  int k = nullablePositions.size();
  vector<string> newRhs = rhs;

  for (int bit = 0; bit < k; bit++) {
    //cout << "bit: " << bit << endl;
    //cout << ((mask >> bit) & 1) << endl;
    if (((mask >> bit) & 1) == 0) {
      int pos = nullablePositions[bit];
      //cout << "Pos: " << pos << " | nullablePositions[bit]: " << nullablePositions[bit] << " | bit: " << bit << endl;
      newRhs[pos] = "#REMOVE#";
    }
  }

  return newRhs;
}

Grammar ChomskyNormalizer::removeLambdaProductions(){
  Grammar g = this->grammar.clone();
  //cout << "Inicio" << endl;

  set<string> voidableVariables = findVoidableVariables();
  set<string> variables = g.getVariables();

  for(string A : g.getVariables()){
    //cout << "\n\nVariavel analisada:" << A << endl;
    set<vector<string>> productionsA = g.getProductions(A);

    for(const vector<string>& rhs : productionsA){
      // cout << "Producao " << A << " --> ";
      // for(string s : rhs){
      //    cout << s << " ";
      // }
      // cout << endl;

      vector<int> nullablePositions = getNullablePositionsRHS(rhs, voidableVariables);
      int k = nullablePositions.size();
      if(k == 0) continue;

      int total = 1 << k;
      //cout << "Total: " << total << endl;

      for(int mask = 0; mask < total; mask++){
        vector<string> newRhs = generateAuxiliaryRHS(rhs, nullablePositions, mask);
        vector<string> cleaned;

        for (auto& s : newRhs) {
            if (s != "#REMOVE#")
                cleaned.push_back(s);
        }

        if(cleaned.empty()) continue;
        g.addProduction(A, cleaned);
      }
    }  
  }

  for (string A : g.getVariables()) {
      vector<string> lambda = {"&"};
      g.removeProduction(A, lambda);
  }

  string S = g.getStartSymbol();
  if (voidableVariables.count(S)) {
      g.addProduction(S, {"&"});
  }

  return g;
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

Grammar ChomskyNormalizer::removeUnitProductions(){
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
    for (auto prod : toRemove)
        result.removeProduction(A, prod);
  }

  return result;
}

Grammar ChomskyNormalizer::removeLeftRecursion(){

  Grammar g = this->grammar.clone();

  for (string A : g.getVariables()){

    set<vector<string>> productionsA = g.getProductions(A);

    for(auto& prod : productionsA){

      for(auto& symbol : prod){

        if(symbol == A){
            g.removeProduction(A, prod);

            vector<std::string> aux(prod.begin() + 1, prod.end());
            if (aux.size() != 0) { 
              g.addProduction(A + "'", aux);
              vector<std::string> auxA = aux;
              auxA.push_back(A + "'");
              g.addProduction(A + "'", auxA);    
            }
     
        }  

        break;         

      }
    }
  }

  return g;

}