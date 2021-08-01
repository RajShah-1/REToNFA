#include "common.hpp"

void applyOperation(stack<string>& args, stack<NFA*>& argsNFA, string operation,
                    unordered_map<string, int>& alphabetMap) {
  // op = * -> pop one operand -> append * to it -> push it back
  if (operation == "*") {
    if (args.size() < 1 && argsNFA.size() < 1) {
      throw INVALID_REGEX_EXCEPTION;
    }
    string val = "[" + args.top() + " *]";
    args.pop();
    argsNFA.top()->applyKleeneStar();
    args.push(val);

  }
  // op = (. or +) -> pop two ops append them one after another -> append
  // (. or +) -> push it back
  else if (operation == "+" || operation == ".") {
    if (args.size() < 2 && argsNFA.size() < 2) {
      throw INVALID_REGEX_EXCEPTION;
    }
    string arg1 = args.top();
    NFA* nfa1 = argsNFA.top();
    args.pop();
    argsNFA.pop();

    string arg2 = args.top();
    args.pop();
    args.push("[" + arg2 + " " + arg1 + " " + operation + "]");

    if (operation == ".")
      argsNFA.top()->applyConcatenationWith(*nfa1);
    else
      argsNFA.top()->applyUnionWith(*nfa1);
    delete nfa1;
  }
}

int main() {
  string regExStr;
  int lenAlphabet;
  cout << "Enter number of symbols (characters) in the alphabet: " << endl;
  cin >> lenAlphabet;

  vector<string> alphabet(lenAlphabet);
  unordered_map<string, int> alphabetMap;

  cout << "Enter " << lenAlphabet
       << " symbols seperated by a space (symbol must contain characters from "
          "A-Z, "
          "a-z or 0-9 only): "
       << endl;
  for (int i = 0; i < lenAlphabet; ++i) {
    cin >> alphabet[i];
    if (alphabetMap.find(alphabet[i]) != alphabetMap.end()) {
      cout << "Symbols must be unique\n";
      return 0;
    }
    alphabetMap.insert(make_pair(alphabet[i], i));
  }

  cout << "Enter regular-expression: " << endl;
  cin >> regExStr;

  // evaluate reg-exp
  stack<string> operations, args;
  stack<NFA*> argsNFA;
  string currOperand = "";
  for (int i = 0; i < regExStr.size(); ++i) {
    // if an op -> process
    switch (regExStr[i]) {
      case '(': {
        if (currOperand != "") {
          args.push(currOperand);
          argsNFA.push(new NFA(currOperand, alphabetMap));
          currOperand = "";
        }
        operations.push("(");
        break;
      }
      case ')': {
        if (currOperand != "") {
          args.push(currOperand);
          argsNFA.push(new NFA(currOperand, alphabetMap));
          currOperand = "";
        }
        // keep popping till '(' is found
        while (!operations.empty() && operations.top() != "(") {
          applyOperation(args, argsNFA, operations.top(), alphabetMap);
          operations.pop();
        }
        if (operations.empty()) {
          throw INVALID_REGEX_EXCEPTION;
        }
        operations.pop();
        break;
      }
      case '*': {
        if (currOperand != "") {
          args.push(currOperand);
          argsNFA.push(new NFA(currOperand, alphabetMap));
          currOperand = "";
        }
        // push it to the operand
        applyOperation(args, argsNFA, "*", alphabetMap);
        break;
      }
      case '+': {
        if (currOperand != "") {
          args.push(currOperand);
          argsNFA.push(new NFA(currOperand, alphabetMap));
          currOperand = "";
        }
        // if operand-stack has . on top -> process and pop it
        while (!operations.empty() && operations.top() == ".") {
          applyOperation(args, argsNFA, operations.top(), alphabetMap);
          operations.pop();
        }
        operations.push("+");
        break;
      }
      case '.': {
        if (currOperand != "") {
          args.push(currOperand);
          argsNFA.push(new NFA(currOperand, alphabetMap));
          currOperand = "";
        }
        operations.push(".");
        break;
      }
      default: {
        currOperand += regExStr[i];
      }
    }
  }
  if (currOperand != "") {
    args.push(currOperand);
    argsNFA.push(new NFA(currOperand, alphabetMap));
    currOperand = "";
  }
  while (!operations.empty()) {
    applyOperation(args, argsNFA, operations.top(), alphabetMap);
    operations.pop();
  }
  if (args.size() != 1 && argsNFA.size() != 1) {
    throw INVALID_REGEX_EXCEPTION;
  }
  cout << "Post-fix: " << args.top() << endl;
  cout << "Epsilon-NFA: \n";
  argsNFA.top()->printNFA();
  
  argsNFA.top()->convertEpsNFAToNFA();
  cout << "===\nNFA: \n";
  argsNFA.top()->printNFA();
  DFA dfa(*argsNFA.top());
  cout << "DFA: \n";
  dfa.printDFA();
  dfa.minimizeDFA();
  cout << "MinimizedDFA: \n";
  dfa.printMinDFA();
  cout << endl;
  return 0;
}