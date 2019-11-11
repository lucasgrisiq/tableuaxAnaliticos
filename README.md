# tableuaxAnaliticos
SAT problem using the Analitycs Tableuax. 

inout/Entrada.in : File to test satisfiability should be in the inout/ directory, with the formatation as follows
1
(((~C) v (A > D)) v (~(A v C))) e consequencia logica de {G, (G > (~(~C))), (((A > A) v (D > A)) v (~(B & D))), (((B > D) & (C v D)) v ((C & B) & (B > D))), L}?

- First line is the number of problems to be solved and the subsequent ones are the problems with the correct number of parentheses.


compile : g++ a.cpp -o a -std=c++14
output :  inout/Saida.out
