#include "../header/grammar.h"
#include "../header/chomskyNormalizer.h"
#include <string>
#include <map>
#include <queue>

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

Grammar ChomskyNormalizer::removeUselessSymbols() {
    Grammar g = this->grammar.clone();
    set<string> generating;
    bool changed = true;

    // Encontrando variáveis geradoras
    while(changed) {
        changed = false;
        for(const string& var : g.getVariables()) {
            if(generating.count(var)) continue;

            for(const auto& rhs : g.getProductions(var)) {
                bool allGen = true;
                for(const string& sym : rhs) {
                    if(g.isVariable(sym) && !generating.count(sym)) {
                        allGen = false;
                        break;
                    }
                }
                if(allGen) {
                    generating.insert(var);
                    changed = true;
                    break;
                }
            }
        }
    }

    // Encontrando variáveis alcançáveis (apenas entre as geradoras)
    set<string> reachable;
    string startSymbol = g.getStartSymbol();
    
    if(generating.count(startSymbol)) {
        reachable.insert(startSymbol);
        queue<string> q;
        q.push(startSymbol);

        while(!q.empty()) {
            string curr = q.front();
            q.pop();

            for(const auto& rhs : g.getProductions(curr)) {
                bool activeProd = true;
                for(const string& sym : rhs) {
                    if(g.isVariable(sym) && !generating.count(sym)) {
                        activeProd = false; break;
                    }
                }

                if(activeProd) {
                    for(const string& sym : rhs) {
                        if(g.isVariable(sym) && !reachable.count(sym)) {
                            reachable.insert(sym);
                            q.push(sym);
                        }
                    }
                }
            }
        }
    }

    // Reconstruido gramática limpa
    Grammar cleanG;
    cleanG.setStartSymbol(startSymbol);

    for(const string& var : reachable) {
        for(const auto& rhs : g.getProductions(var)) {
            bool valid = true;
            for(const string& sym : rhs) {
                if(g.isVariable(sym) && (!generating.count(sym) || !reachable.count(sym))) {
                    valid = false; break;
                }
            }
            if(valid) cleanG.addProduction(var, rhs);
        }
    }
    return cleanG;
}

string ChomskyNormalizer::generateNewVarName(const Grammar& context) {
    string name;
    do {
        name = "X" + to_string(newVarCounter++);
    } while(context.isVariable(name));
    return name;
}

Grammar ChomskyNormalizer::replaceTerminalsInLongBodies(const Grammar& gInput) {
    Grammar g = gInput.clone();
    map<string, string> termToVar;
    set<string> vars = g.getVariables();

    for(const string& var : vars) {
        set<vector<string>> prods = g.getProductions(var);
        for(const auto& rhs : prods) {
            if(rhs.size() < 2) continue;

            vector<string> newRhs;
            bool changed = false;

            for(const string& sym : rhs) {
                if(!g.isVariable(sym)) {
                    string tVar;
                    if(termToVar.count(sym)) {
                        tVar = termToVar[sym];
                    } else {
                        tVar = "T_" + sym;
                        int suffix = 0;
                        while(g.isVariable(tVar)) tVar = "T_" + sym + to_string(suffix++);
                        termToVar[sym] = tVar;
                        g.addProduction(tVar, {sym});
                    }
                    newRhs.push_back(tVar);
                    changed = true;
                } else {
                    newRhs.push_back(sym);
                }
            }

            if(changed) {
                g.removeProduction(var, rhs);
                g.addProduction(var, newRhs);
            }
        }
    }
    return g;
}

Grammar ChomskyNormalizer::breakLongBodies(const Grammar& gInput) {
    Grammar g = gInput.clone();
    set<string> vars = g.getVariables();

    for(const string& var : vars) {
        set<vector<string>> prods = g.getProductions(var);
        for(const auto& rhs : prods) {
            if(rhs.size() <= 2) continue;

            g.removeProduction(var, rhs);
            string currVar = var;
            
            for(size_t i = 0; i < rhs.size() - 2; ++i) {
                string nextVar = generateNewVarName(g);
                g.addProduction(currVar, {rhs[i], nextVar});
                currVar = nextVar;
            }
            g.addProduction(currVar, {rhs[rhs.size()-2], rhs[rhs.size()-1]});
        }
    }
    return g;
}

Grammar ChomskyNormalizer::fixLongProductions() {
    Grammar g1 = replaceTerminalsInLongBodies(this->grammar);
    ChomskyNormalizer temp(g1);
    return temp.breakLongBodies(g1);
}

Grammar ChomskyNormalizer::toChomskyNormalForm() {
    Grammar g = this->grammar.clone();
    
    g = ChomskyNormalizer(g).removeRecursionAtBeginning();
    g = ChomskyNormalizer(g).removeLambdaProductions();
    g = ChomskyNormalizer(g).removeUnitProductions();
    g = ChomskyNormalizer(g).removeUselessSymbols();
    g = ChomskyNormalizer(g).fixLongProductions();

    return g;
}

Grammar ChomskyNormalizer::getGrammar() const {
    return this->grammar;
}