#include "common.hpp"

using namespace std;

DFA::DFA(NFA& nfa) : alphabet(nfa.getAlphabet()) {
  int nAlphabet = alphabet.size();

  // populate this->alphabetIdToSym using alphabet
  // assm : in alphabet, each symbol has a unique id
  for (auto symIdPair : alphabet) {
    assert(alphabetIdToSym.find(symIdPair.second) == alphabetIdToSym.end());
    alphabetIdToSym.insert(make_pair(symIdPair.second, symIdPair.first));
  }

  numStates = 0;
  // NFA -> DFA
  // map each sub-set of Q used to an integer with an unordered_map

  // as C++ does not provide hash fn for set<int>
  // convert set<int> to a string and use it as key
  // set<int> = {x1, x2, ..., xn} -> string: "x1_x2_..._xn_"
  unordered_map<string, int> subsetToId;
  unordered_map<string, set<int>> strToSubset;

  // phi -> id = -1
  subsetToId.insert(make_pair("", -1));
  strToSubset.insert(make_pair("", set<int>()));

  transistionFn = vector<vector<int>>();
  vector<vector<set<int>>> nfaTrFn = nfa.getTransistionFn();
  set<int> nfaStartState = {nfa.getStartState()};
  queue<set<int>> pendingStatesQueue;
  pendingStatesQueue.push(nfaStartState);
  subsetToId.insert(make_pair(setToString(nfaStartState), numStates++));
  strToSubset.insert(make_pair(setToString(nfaStartState), nfaStartState));

  while (!pendingStatesQueue.empty()) {
    set<int> currSet = pendingStatesQueue.front();
    pendingStatesQueue.pop();

    vector<int> transistionFnRow(nAlphabet, -1);
    for (int sym = 0; sym < nAlphabet; ++sym) {
      set<int> tranSubset;
      for (int qi : currSet) {
        for (int qj : nfaTrFn[qi][sym]) {
          tranSubset.insert(qj);
        }
      }
      string tranSubsetStr = setToString(tranSubset);
      if (subsetToId.find(tranSubsetStr) == subsetToId.end()) {
        subsetToId.insert(make_pair(tranSubsetStr, numStates++));
        strToSubset.insert(make_pair(tranSubsetStr, tranSubset));
        pendingStatesQueue.push(tranSubset);
      }

      transistionFnRow[sym] = subsetToId[tranSubsetStr];
    }
    transistionFn.push_back(transistionFnRow);
  }

  startState = subsetToId[setToString(nfaStartState)];
  finalStates = set<int>();
  set<int> nfaFinalStates = nfa.getFinalStates();
  for (auto state : strToSubset) {
    for (int qf : nfaFinalStates) {
      if (state.second.find(qf) != state.second.end()) {
        finalStates.insert(subsetToId[state.first]);
        break;
      }
    }
  }
}

string DFA::setToString(set<int>& S) {
  string str = "";
  for (int s : S) {
    str += to_string(s) + "_";
  }
  return str;
}

void DFA::minimizeDFA() {
  int nAlphabet = alphabet.size();

  // table filling algo

  // Create dependency lists
  // dependencyLists[qr][qs] -> dependency-list for state-pair (qr, qs)
  vector<vector<vector<pair<int, int>>>> dependencyLists(
      numStates, vector<vector<pair<int, int>>>(numStates));
  // populate values in the dependency-lists
  for (int qi = 0; qi < numStates; ++qi) {
    for (int qj = qi + 1; qj < numStates; ++qj) {
      for (int sym = 0; sym < nAlphabet; ++sym) {
        int tmp1 = transistionFn[qi][sym];
        int tmp2 = transistionFn[qj][sym];
        if (tmp1 == -1 || tmp2 == -1) continue;
        if (tmp1 == tmp2) continue;
        int qr = min(tmp1, tmp2);
        int qs = max(tmp1, tmp2);
        dependencyLists[qr][qs].push_back(make_pair(qi, qj));
      }
    }
  }

  // cout << "D-lists\n";
  // for (int i = 0; i < numStates; ++i) {
  //   for (int j = 0; j < numStates; ++j) {
  //     cout << "( " << i << ", " << j << "): ";
  //     for (auto k : dependencyLists[i][j]) {
  //       cout << "( " << k.first << ", " << k.second << "), ";
  //     }
  //     cout << "\n";
  //   }
  // }

  queue<pair<int, int>> processingQueue;

  // Table
  vector<vector<bool>> areDistinguishable(numStates,
                                          vector<bool>(numStates, false));

  // Initialize the table
  for (int qi = 0; qi < numStates; ++qi) {
    for (int qj = qi + 1; qj < numStates; ++qj) {
      // if exactly one of qi and qj is a final state,
      // the pair (qi, qj) is distinguishable
      if ((finalStates.find(qi) == finalStates.end()) !=
          (finalStates.find(qj) == finalStates.end())) {
        areDistinguishable[qi][qj] = true;
        processingQueue.push(make_pair(qi, qj));
      }
    }
  }

  // Process the queue
  while (!processingQueue.empty()) {
    pair<int, int> u = processingQueue.front();
    processingQueue.pop();
    if (u.first == u.second) continue;

    vector<pair<int, int>> dependentPairs = dependencyLists[u.first][u.second];
    for (auto dependentPair : dependentPairs) {
      if (!areDistinguishable[dependentPair.first][dependentPair.second]) {
        areDistinguishable[dependentPair.first][dependentPair.second] = true;
        processingQueue.push(
            make_pair(dependentPair.first, dependentPair.second));
      }
    }
  }

  // cout << "Table: \n";
  // for (int i = 0; i < numStates; ++i) {
  //   for (int j = 0; j < numStates; ++j) {
  //     cout << areDistinguishable[i][j] << " ";
  //   }
  //   cout << "\n";
  // }

  // relabel non-distinguishable states
  equivalentState.clear();
  for (int qi = 0; qi < numStates; ++qi) {
    for (int qj = qi + 1; qj < numStates; ++qj) {
      if (!areDistinguishable[qi][qj]) {
        int qiId = qi;
        if (equivalentState.find(qiId) != equivalentState.end()) {
          qiId = equivalentState[qiId];
        }

        if (equivalentState.find(qj) == equivalentState.end()) {
          equivalentState[qj] = qiId;
        } else {
          equivalentState[qj] = min(qiId, equivalentState[qj]);
        }
      }
    }
  }

  // check if some state(s) are equivalent to the empty state
  for (int qi = 0; qi < numStates; ++qi) {
    bool isPhi = true;
    for (int sym = 0; sym < nAlphabet; ++sym) {
      if (transistionFn[qi][sym] != -1) {
        isPhi = false;
        break;
      }
    }
    if (isPhi) {
      equivalentState[qi] = -1;
    }
  }
}

void DFA::printDFA() {
  cout << "===\n";
  cout << "Start State: q" << startState << "\n";
  cout << "Final States: ";
  for (int qF : finalStates) {
    cout << "q" << qF << ", ";
  }
  cout << "\n";
  cout << "Transitions: \n";
  for (int stateTr = 0; stateTr < transistionFn.size(); ++stateTr) {
    cout << "State-q" << stateTr << "\n";
    for (int sym = 0; sym < transistionFn[stateTr].size(); ++sym) {
      int trSym = transistionFn[stateTr][sym];
      cout << "\u03B4(q" << stateTr << ", " << alphabetIdToSym[sym]
           << ") = " << (trSym >= 0 ? ("q" + to_string(trSym)) : "\u03C6")
           << "\n";
    }
    cout << "\n";
  }
  cout << "===\n";
}

void DFA::printMinDFA() {
  cout << "===\n";

  cout << "Start State: q"
       << (equivalentState.find(startState) != equivalentState.end()
               ? equivalentState[startState]
               : startState)
       << "\n";
  cout << "Final States: ";
  for (int qF : finalStates) {
    if (equivalentState.find(qF) != equivalentState.end()) continue;
    cout << "q" << qF << ", ";
  }
  cout << "\n";

  cout << "Transitions: \n";
  for (int stateTr = 0; stateTr < transistionFn.size(); ++stateTr) {
    if (equivalentState.find(stateTr) != equivalentState.end()) {
      continue;
    }
    cout << "State-q" << stateTr << "\n";
    for (int sym = 0; sym < transistionFn[stateTr].size(); ++sym) {
      int trSym = transistionFn[stateTr][sym];
      if (equivalentState.find(trSym) != equivalentState.end()) {
        trSym = equivalentState[trSym];
      }
      cout << "\u03B4(q" << stateTr << ", " << alphabetIdToSym[sym]
           << ") = " << (trSym >= 0 ? ("q" + to_string(trSym)) : "\u03C6")
           << "\n";
    }
    cout << "\n";
  }
  cout << "===\n";
}

int DFA::getNumStates() { return this->numStates; };

unordered_map<string, int> DFA::getAlphabet() { return this->alphabet; };

vector<vector<int>> DFA::getTransistionFn() { return this->transistionFn; }

int DFA::getStartState() { return this->startState; }

set<int> DFA::getFinalStates() { return this->finalStates; }