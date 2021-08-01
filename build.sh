g++ -o ./build/NFA -c ./src/NFA.cpp
g++ -o ./build/DFA -c ./src/DFA.cpp
g++ -o ./build/RegExToNFA_Main -c ./src/RegExToNFA.cpp
g++ -o ./build/RegExToNFA ./build/RegExToNFA_Main ./build/NFA ./build/DFA
