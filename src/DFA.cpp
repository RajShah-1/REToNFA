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

void DFA::minimizeDFA() {}

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

int DFA::getNumStates() { return this->numStates; };

unordered_map<string, int> DFA::getAlphabet() { return this->alphabet; };

vector<vector<int>> DFA::getTransistionFn() { return this->transistionFn; }

int DFA::getStartState() { return this->startState; }

set<int> DFA::getFinalStates() { return this->finalStates; }