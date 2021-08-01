Use build.sh script file to compile the program and create an executable (./build/RegExToNFA)

Features
	- Any alphabet is accepted (symbol of the alphabet must not contain white-spaces)
	- Regular expression is converted to epsilon-NFA, NFA, DFA, and minimized DFA
	- Supports parenthesis('(' and ')'), Kleene star ('*'), Union ('+'), Concatenation ('.')

Files
	- NFA.cpp 
		- creates an NFA for a symbol
		- handles various operations (Kleene star, Concatenation, Union)
		- has function to convert epsilon-NFA to NFA
	- DFA.cpp
		- constructor creates DFA from an NFA given as argument
		- has a function to minimize the DFA using the single pass table filling algorithm
	- RegExToNFA
		- contains the main() method
		- reads RegEx and evalutes it (post-fix evalution)
		- it also creates an epsilon-NFA while evaluating RegEx
		- then it converts it to NFA, DFA, and minimized DFA using utilities in NFA.cpp and DFA.cpp
	- NFA.hpp and DFA.hpp contains class definitions for NFA and DFA classes 
	with include guards
	- common.hpp has include statements used across all .cpp files
	- build.sh
		- it compiles and links all the files to create ./build/RegExToNFA executable
	- run.sh
		- builds and executes the code
	- files in input folder are some test inputs
	
Note:
	- Code uses unicode escape characters to print greek symbols
	(if the escape characters are not supported, output will not be proper)
	- '.' must be used to denote concatenation 
	(AB is treated as a symbol, A.B is treated as (A concatenation B))
