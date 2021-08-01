#ifndef __DFA_HPP__
#define __DFA_HPP__

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "NFA.hpp"

using namespace std;

class DFA {
 public:
  DFA(NFA& nfa);
  void minimizeDFA();
  void printDFA();
  int getNumStates();
  unordered_map<string, int> getAlphabet();
  vector<vector<int>> getTransistionFn();
  int getStartState();
  set<int> getFinalStates();

 private:
  // Q -> {0, 1, ... numStates-1}
  int numStates;
  // Sigma
  unordered_map<string, int> alphabet;
  unordered_map<int, string> alphabetIdToSym;
  // Delta
  // transistionFn[q][a] = delta(q, a) where q is the state and a is symbol
  // delta(q, a) will be an int as it is a transition fn of an "DFA"
  vector<vector<int>> transistionFn;
  // q0
  int startState;
  // F
  set<int> finalStates;

  string setToString(set<int>& S);
};

#endif