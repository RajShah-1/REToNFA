#ifndef __NFA_HPP__
#define __NFA_HPP__

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class NFA {
 public:
  NFA(string symbol, unordered_map<string, int>& alphabet);
  void convertEpsNFAToNFA();
  void applyKleeneStar();
  void applyConcatenationWith(NFA& nfa);
  void applyUnionWith(NFA& nfa);
  void printNFA();
  int getNumStates();
  unordered_map<string, int> getAlphabet();
  vector<vector<set<int>>> getTransistionFn();
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
  // delta(q, a) will be a set<int> as it is a transition fn of an "NFA"
  vector<vector<set<int>>> transistionFn;
  // q0
  int startState;
  // F
  set<int> finalStates;

  set<int> closureOfSet(set<int>& states,
                        unordered_map<int, set<int>>& eps_closure);
};

#endif