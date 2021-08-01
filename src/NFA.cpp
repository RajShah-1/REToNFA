#include "common.hpp"

NFA::NFA(string symbol, unordered_map<string, int>& alphabet)
    : alphabet(alphabet) {
  numStates = 0;
  int nAlphabet = alphabet.size();
  int state1 = numStates++;
  int state2 = numStates++;
  // eps is assumed to be appended to the alphabet
  this->transistionFn = vector<vector<set<int>>>(
      numStates, vector<set<int>>(nAlphabet + 1, set<int>()));

  // symbol must be from the alphabet
  assert(alphabet.find(symbol) != alphabet.end());

  // populate this->alphabetIdToSym using alphabet
  // assm : in alphabet, each symbol has a unique id
  for (auto symIdPair : alphabet) {
    assert(alphabetIdToSym.find(symIdPair.second) == alphabetIdToSym.end());
    alphabetIdToSym.insert(make_pair(symIdPair.second, symIdPair.first));
  }
  // id of epsilon = nAlphabet
  alphabetIdToSym.insert(make_pair(nAlphabet, "\u03B5"));

  int symIndex = alphabet[symbol];
  this->transistionFn[state1][symIndex].insert(state2);
  this->startState = state1;
  this->finalStates = {state2};
}

void NFA::convertEpsNFAToNFA() {
  unordered_map<int, set<int>> eps_closure;
  
}

void NFA::applyKleeneStar() {
  int nAlphabet = alphabet.size();
  // add a new state (qk)
  int qk = numStates;
  numStates++;
  transistionFn.push_back(vector<set<int>>(nAlphabet + 1, set<int>()));
  assert(numStates == transistionFn.size());
  // add eps transition from qk to the curr start state
  transistionFn[qk][nAlphabet].insert(startState);
  // add eps transition from all the final states to qk
  for (int qF : finalStates) {
    transistionFn[qF][nAlphabet].insert(qk);
  }
  // qk is new start state
  startState = qk;
  // qk is also a final state
  finalStates.insert(qk);
}

// assm: nfa2 has the same this->alphabet as nfa1(the calling nfa)
void NFA::applyUnionWith(NFA& nfa2) {
  // shift nfa2's states by this->states.size()
  // and add them in nfa1(calling nfa)
  int nAlphabet = alphabet.size();
  int nfa1_nStates = numStates;
  // assm: nfa2 has the same this->alphabet as nfa1
  numStates += nfa2.getNumStates();
  vector<vector<set<int>>> nfa2_transitionFn = nfa2.getTransistionFn();
  for (int tfRow = 0; tfRow < nfa2_transitionFn.size(); ++tfRow) {
    vector<set<int>> tranFnRow(nAlphabet + 1, set<int>());
    for (int tr = 0; tr < nfa2_transitionFn[tfRow].size(); ++tr) {
      assert(nfa2_transitionFn[tfRow].size() == nAlphabet + 1);
      for (int trState : nfa2_transitionFn[tfRow][tr]) {
        tranFnRow[tr].insert(trState + nfa1_nStates);
      }
    }
    transistionFn.push_back(tranFnRow);
  }
  // add a new state: qk
  int qk = numStates;
  numStates++;
  transistionFn.push_back(vector<set<int>>(nAlphabet + 1, set<int>()));
  assert(numStates == transistionFn.size());
  // eps transition from qk to nfa1.startState and nfa2.startState
  transistionFn[qk][nAlphabet].insert(this->startState);
  transistionFn[qk][nAlphabet].insert(nfa2.getStartState() + nfa1_nStates);
  // finalStates  = nfa1.finalStates union nfa2.finalStates
  for (int qF : nfa2.getFinalStates()) {
    finalStates.insert(qF + nfa1_nStates);
  }
  // startState = qk
  startState = qk;
}

// assm: nfa2 has the same this->alphabet as nfa1(the calling nfa)
void NFA::applyConcatenationWith(NFA& nfa2) {
  // shift nfa2's states by this->states.size()
  // and add them in nfa1(calling nfa)
  int nAlphabet = alphabet.size();
  int nfa1_nStates = numStates;
  // assm: nfa2 has the same this->alphabet as nfa1
  numStates += nfa2.getNumStates();
  vector<vector<set<int>>> nfa2_transitionFn = nfa2.getTransistionFn();
  for (int tfRow = 0; tfRow < nfa2_transitionFn.size(); ++tfRow) {
    vector<set<int>> tranFnRow(nAlphabet + 1, set<int>());
    for (int tr = 0; tr < nfa2_transitionFn[tfRow].size(); ++tr) {
      assert(nfa2_transitionFn[tfRow].size() == nAlphabet + 1);
      for (int trState : nfa2_transitionFn[tfRow][tr]) {
        tranFnRow[tr].insert(trState + nfa1_nStates);
      }
    }
    transistionFn.push_back(tranFnRow);
  }
  // eps transitions from nfa1.finalStates to nfa2.startState
  for (int qF : this->finalStates) {
    transistionFn[qF][nAlphabet].insert(nfa2.getStartState() + nfa1_nStates);
  }
  // finalStates  = nfa2.finalStates
  finalStates.clear();
  for (int qF : nfa2.getFinalStates()) {
    finalStates.insert(qF + nfa1_nStates);
  }
}

void NFA::printNFA() {
  cout << "NFA: \n";
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
      if (transistionFn[stateTr][sym].size() == 0) continue;
      cout << "\u03B4(q" << stateTr << ", " << alphabetIdToSym[sym] << ") = ";
      for (int trState : transistionFn[stateTr][sym]) {
        cout << "q" << trState << ", ";
      }
      cout << "\n";
    }
    cout << "\n";
  }
  cout << "===\n";
}

int NFA::getNumStates() { return this->numStates; };

unordered_map<string, int> NFA::getAlphabet() { return this->alphabet; };

vector<vector<set<int>>> NFA::getTransistionFn() { return this->transistionFn; }

int NFA::getStartState() { return this->startState; }

set<int> NFA::getFinalStates() { return this->finalStates; }