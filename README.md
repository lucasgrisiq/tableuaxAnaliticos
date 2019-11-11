
# tableuaxAnaliticos
SAT problem using the Analitycs Tableuax. 

## Input
inout/Entrada.in : File to test satisfiability with the right formatting as follows

    1
    (((B & C) & (A > A)) v ((B > A) & (D v D))) e refutavel?


- First line is the number of problems to be solved and the subsequent ones are the problems with the correct number of parentheses.

## Compiling

- compile : g++ a.cpp -o a -std=c++14
- run : ./a
- output in :  inout/Saida.out
