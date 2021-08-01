#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct State {
  int id;                       // state id in the input dfa
  int reducedID;                // state id in the optimized dfa
  vector<State*> transistions;  // defines transitions from the current state
  bool isFinish;                // true if current state is the final state
  State(int id, int nS) {
    this->id = id;
    this->reducedID = id;
    this->transistions = vector<State*>(nS);
    this->isFinish = false;
  }
};

struct StatePairs {
  vector<pair<State*, State*>> list;  // dependency list for the given pair
  bool d;  // denotes whether both the states are distinct or not
};

void getInput(vector<State*>& S, State*& startState,
              vector<State*>& finishStates);
void printTransistions(vector<State*>& S);
void createDependencyList(vector<State*>& S, vector<vector<StatePairs>>& list);
void printList(vector<vector<StatePairs>>& L);
void TableFillingAlgo(vector<State*>& S, vector<vector<StatePairs>>& list);
void getOptTransitionFunc(vector<State*>& S, vector<vector<StatePairs>>& list);

int main() {
  // startState and finishStates variables are not used in the algorithm
  vector<State*> S;             // All the states with their transitions
  vector<State*> finishStates;  // Set of finish states
  State* startState;
  vector<vector<StatePairs>> List;  // 2D array of dependency lists

  // Read input from the user
  getInput(S, startState, finishStates);

  // Create dependency list from the transition function
  createDependencyList(S, List);

  // Debug
  // printTransistions(S);
  // Debug
  // printList(List);

  // Run the table filling algo
  TableFillingAlgo(S, List);

  // Create tranisition table from the o/p of the table filling algo
  getOptTransitionFunc(S, List);


  for(int i = 0; i < S.size(); ++i){
    delete S[i];
  }
  return 0;
}

// O(N^2)
void getOptTransitionFunc(vector<State*>& S, vector<vector<StatePairs>>& list) {
  int nState = S.size();
  int nSymbols = S[0]->transistions.size();
  for (int i = 0; i < nState; ++i) {
    for (int j = i + 1; j < nState; ++j) {
      if (!list[i][j].d && S[j]->reducedID == S[j]->id) {
        // S[i] and S[j] are equivalent
        S[j]->reducedID = i;
      }
    }
  }

  cout << endl << endl;
  cout << "Reduced transition table: " << endl;
  for (int i = 0; i < nSymbols; ++i) cout << "==";
  cout << "==" << endl;
  cout << "S: ";
  for (int i = 0; i < nSymbols; ++i) cout << i << " ";
  cout << endl;
  for (int i = 0; i < nSymbols; ++i) cout << "==";
  cout << "==" << endl;

  // Print the prev transition fun after combining non distingushable states
  // After combining all the non distinguishable states it becomes the
  // transition funtion of the optimized DFA
  for (int i = 0; i < nState; ++i) {
    if (S[i]->id != S[i]->reducedID) continue;
    cout << i << ": ";
    for (int j = 0; j < nSymbols; ++j) {
      cout << S[i]->transistions[j]->reducedID << " ";
    }
    cout << endl;
  }
}

void TableFillingAlgo(vector<State*>& S, vector<vector<StatePairs>>& list) {
  queue<pair<State*, State*>> Q;
  int nStates = S.size();

  // Initialize: O(N^2)
  // Can be optimized to O(|F|*N)
  // where F = set of final states
  for (int i = 0; i < nStates; ++i) {
    for (int j = i + 1; j < nStates; ++j) {
      if (S[i]->isFinish != S[j]->isFinish) {
        Q.push({S[i], S[j]});
        list[i][j].d = true;
      }
    }
  }
  pair<State*, State*> X, Y;
  StatePairs U;
  while (!Q.empty()) {
    X = Q.front();
    Q.pop();
    U = list[X.first->id][X.second->id];

    for (int i = 0; i < U.list.size(); ++i) {
      Y = U.list[i];
      if (!list[Y.first->id][Y.second->id].d) {
        list[Y.first->id][Y.second->id].d = true;
        Q.push({Y.first, Y.second});
      }
    }
  }

  // Print the table
  cout << endl << endl;
  cout << "Table (After running the table filling algorithm): " << endl;
  for (int i = 0; i < nStates; ++i) {
    for (int j = 0; j < nStates; ++j) {
      cout << list[i][j].d << " ";
    }
    cout << endl;
  }
}

void createDependencyList(vector<State*>& S, vector<vector<StatePairs>>& list) {
  int nState = S.size();
  if (nState < 1) {
    cout << "Error: nState > 0" << endl;
    exit(EXIT_FAILURE);
  }
  int nSyms = S[0]->transistions.size();
  if (nSyms < 1) {
    cout << "Error: nSymbols > 0" << endl;
    exit(EXIT_FAILURE);
  }

  list = vector<vector<StatePairs>>(nState,
                                    vector<StatePairs>(nState, StatePairs()));

  State *u, *v;
  int i1, i2;
  for (int i = 0; i < nState; ++i) {
    for (int j = i + 1; j < nState; ++j) {
      for (int k = 0; k < nSyms; ++k) {
        u = S[i]->transistions[k];
        v = S[j]->transistions[k];
        if (u->id != v->id) {
          i1 = min(u->id, v->id);
          i2 = max(u->id, v->id);
          list[i1][i2].list.push_back({S[i], S[j]});
        }
      }
    }
  }
}

void getInput(vector<State*>& S, State*& startState,
              vector<State*>& finishStates) {
  int nStates, nSymbols;
  cout << "Enter total number of states: ";
  cin >> nStates;
  cout << "Enter total number of symbols: ";
  cin >> nSymbols;

  cout << "=== Note ===" << endl;
  cout << "States are represented by positive integers." << endl;
  cout << "If there are n states, they are represented by 0, 1 ... n-1" << endl;
  cout << "Symbols are represented by positive integers." << endl;
  cout << "If there are n symbols, use 0, 1 ... n-1 to represent them." << endl;
  cout << "===" << endl;

  cout << "Enter the transition function: " << endl;
  cout << "(Enter the initial state followed by n states that would be reached "
       << "after reading symbols 0, 1 ... n-1 respectively." << endl;

  cout << "(Enter transitions from the printed state on reading symbols 0, 1 "
          "... n-1 in the"
       << "same order)" << endl;

  for (int i = 0; i < nSymbols; ++i) cout << "==";
  cout << "==" << endl;
  cout << "S ";
  for (int i = 0; i < nSymbols; ++i) cout << i << " ";
  cout << endl;
  for (int i = 0; i < nSymbols; ++i) cout << "==";
  cout << "==" << endl;

  int tmp;
  S.resize(nStates);
  for (int i = 0; i < nStates; ++i) {
    S[i] = new State(i, nSymbols);
  }

  for (int i = 0; i < nStates; ++i) {
    cin >> tmp;
    S[i]->id = tmp;
    for (int j = 0; j < nSymbols; ++j) {
      cin >> tmp;
      S[i]->transistions[j] = S[tmp];
    }
  }
  cout << "Enter start state: ";
  cin >> tmp;
  startState = S[tmp];
  cout << "Enter total number of finish states: ";
  int nFinish;
  cin >> nFinish;
  finishStates.resize(nFinish);
  for (int i = 0; i < nFinish; ++i) {
    cin >> tmp;
    finishStates[i] = S[tmp];
    S[tmp]->isFinish = true;
  }
}

void printTransistions(vector<State*>& S) {
  cout << "Transition Table: " << endl;
  for (int i = 0; i < S.size(); ++i) {
    cout << S[i]->id << ": ";
    for (int j = 0; j < S[i]->transistions.size(); ++j) {
      cout << S[i]->transistions[j]->id << " ";
    }
    cout << endl;
  }
}

void printList(vector<vector<StatePairs>>& L) {
  cout << "Dependency List: " << endl;
  for (int i = 0; i < L.size(); ++i) {
    for (int j = 0; j < L[i].size(); ++j) {
      vector<pair<State*, State*>>& tmp = L[i][j].list;
      if (tmp.size() == 0) continue;
      cout << "[" << i << ", " << j << "] : ";
      for (int k = 0; k < tmp.size(); ++k) {
        cout << "[" << tmp[k].first->id << ", " << tmp[k].second->id << "], ";
      }
      cout << endl;
    }
  }
}