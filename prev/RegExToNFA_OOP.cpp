#include <iostream>
#include <string>
#include <vector>

using namespace std;

class RegExpression {
 public:
  RegExpression(const string& str, const vector<string>& alphabet);
  static RegExpression readRegExFromStdIn();
  RegExpression getRegExStr();

 private:
  string regExStr;
  vector<string> alphabet;
};

class NFA {
 private:
  // Q
  vector<string> states;
  // Sigma
  vector<string> alphabet;
  // Delta
  // transistionFn[q][a] = delta(q, a) where q is the state and a is char
  vector<vector<string>> transistionFn;
  // q0
  string startState;
  // F
  vector<string> finalStates;
};

int main() { return 0; }