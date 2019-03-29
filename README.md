# Compiler-Labs

This is a collection of compiler course labs, all together it would leads to a compiler of simplified version of C language, here we call it cmm. All labs are implemented using C and other tools that helps build compilers, such as flex for lexical analysis and Detailed descriptions of each lab is as follows.

- Lab01 - Lexical and Syntax Analysis 
  - implemented basic lexical analysis of a cmm program in `lexical.l`, compile by `flex lexical.l` in terminal. Support integer (hex/oct/dec), float-point number (exponential), id and reserved words, basic illegal integer error handling.
  - implemented basic syntax analysis with bison, compile by `bison -d syntax.y` in terminal. Support basic syntax error handling and grammar tree output with non-terminals' locations. 

