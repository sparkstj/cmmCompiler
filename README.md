# Compiler-Labs

This is a collection of compiler course labs, all together it would leads to a compiler of simplified version of C language, here we call it cmm. All labs are implemented using C and other tools that helps build compilers, such as flex for lexical analysis and Detailed descriptions of each lab is as follows.

- Lab01 - Lexical and Syntax Analysis 

  - implemented basic lexical analysis of a cmm program in `lexical.l`, compile by command `flex lexical.l` . Support integer (hex/oct/dec), float-point number (exponential), id and reserved words, basic illegal integer error handling.

  - implemented basic syntax analysis with bison, compile by command `bison -d syntax.y`. Support basic syntax error handling and grammar tree output with non-terminals' locations. 

  - some tests are in `/tests/...` and `/optional-tests/...`

  - `main.c` contains some integration code. This overall lab can be compiled by makefile or use the following command lines.

    -  ``` bison -d syntax.y 
      bison -d syntax.y 
      flex lexical.l
      gcc main.c syntax.tab.c -ll -ly -o parser (for mac os)
      ./parser [test url]
       ```

      

