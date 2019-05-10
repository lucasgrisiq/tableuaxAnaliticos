#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include "AnalyticTableaux.cpp"

#define endl '\n'

using namespace std;

/*

PROBLEMAS

0 - satisfativel
1 - refutavel
2 - tautologia
3 - insatisfativel
4 - consequencia logica

-------------------------

OPERADORES

~ negacao
& conjuncao
v disjuncao
> implicacao

*/

string getExpr(string linha);
int getProblem(string linha);
char getOperator(string expr);
vector <Node *> sortNodes(vector <Node *> nodes);
string getNegacao(string expr);
void getSubExpr(string expr, string *sub1, string *sub2);
string getExpr2(string linha);

int main() {
    // arquivos
    ifstream entrada;
    ofstream saida;

    // variaveis
    int n, i, prob, j, k, a;
    string linha, expr, ep;
    string sub1, sub2, neg;
    Node arvore("", false);
    vector <Node *> appNodes, leafs;
    bool v, stop;
    char op;


    saida.open("./inout/Saida.out");
    entrada.open("./inout/Entrada.in");

    entrada >> n;
    i = n;

    // ignora '\n'
    getline(entrada, linha);

    while(i > 0) {
        // salva linha n-i+1 em linha
        getline(entrada, linha);
        prob = getProblem(linha);
        if(prob == 0 || prob == 3) {
            expr = getExpr(linha);
            Node node(expr, true);
            arvore = node;
        }
        else if(prob == 1 || prob == 2) {
            expr = getExpr(linha);
            Node node(expr, false);
            arvore = node;
        }
        else if(prob == 4) {
            stop=true;
            string lnova;
            expr = getExpr(linha);
            ep = expr;
            Node node(expr, false);
            arvore = node;

            // procura pelo conjunto de expressoes
            for(j=0; stop && j<linha.length(); j++) {
                if(linha[j] == '{') {
                    stop=false;
                }
            }
            // salva em nova string o conjunto de expressoes
            while(linha[j] != '}' && j<linha.length()) {
                lnova += linha[j];
                j++;
            }
            vector <Node *> insert;
            while(lnova.length() > 0 && !arvore.isClosed()) {
                expr = getExpr(lnova);
                insert = arvore.insertFront(expr, true);
                lnova = getExpr2(lnova);
                if(insert[0]->checkContradiction()) {
                    insert[0]->markContradiction();
                }
                insert.clear();
            }
        }


        while(!arvore.isClosed() && !arvore.getAppliableNodes().empty()) {
            appNodes.clear();
            leafs.clear();
            
            // ordena nos aplicaveis pondo os que bifurcam por ultimo
            appNodes = sortNodes(arvore.getAppliableNodes());

            for(k=0; k<appNodes.size(); k++) {

                expr = appNodes[k]->getExpression();
                v = appNodes[k]->getTruthValue();
                op = getOperator(expr);

                if(op == '~') {
                    neg="";
                    neg = getNegacao(expr);
                    leafs = appNodes[k]->insertFront(neg, !v);
                }
                else if(op == '&') {
                    sub1="";
                    sub2="";
                    getSubExpr(expr, &sub1, &sub2);

                    if(v) leafs = appNodes[k]->insertFront(sub1, v, sub2, v);
                    else leafs = appNodes[k]->insertSides(sub1, v, sub2, v);
                }
                else if(op == 'v') {
                    sub1="";
                    sub2="";
                    getSubExpr(expr, &sub1, &sub2);
                    
                    if(!v) leafs = appNodes[k]->insertFront(sub1, v, sub2, v);
                    else leafs = appNodes[k]->insertSides(sub1, v, sub2, v);
                }
                else if(op == '>') {
                    sub1="";
                    sub2="";
                    getSubExpr(expr, &sub1, &sub2);
                    if(!v) leafs = appNodes[k]->insertFront(sub1, !v, sub2, v);
                    else leafs = appNodes[k]->insertSides(sub1, !v, sub2, v);
                }
                for(a=0; a<leafs.size(); a++) {
                    if(leafs[a]->checkContradiction()){
                        leafs[a]->markContradiction();
                    }    
                }
                appNodes[k]->markApplied();
            }
        }
        
        appNodes.clear();
        leafs.clear();
        
        saida << "Problema #" << n-i+1 << endl;

        if(prob==0) {
            if(arvore.isClosed()) saida << "Nao, nao e satisfativel." << endl;
            else saida << "Sim, e satisfativel." << endl;
        }
        else if(prob==1) {
            if(arvore.isClosed()) saida << "Nao, nao e refutavel." << endl;
            else saida << "Sim, e refutavel." << endl;
        }
        else if(prob==2) {
            if(arvore.isClosed()) saida << "Sim, e tautologia." << endl;
            else saida << "Nao, nao e tautologia." << endl;
        }
        else if(prob==3) {
            if(arvore.isClosed()) saida << "Sim, e insatisfativel." << endl;
            else saida << "Nao, nao e insatisfativel." << endl;
        }
        else {
            if(arvore.isClosed()) saida << "Sim, e consequencia logica." << endl;
            else saida << "Nao, nao e consequencia logica." << endl;
        }

        i--;
        if(i > 0) saida << endl;
    }

    saida.close();
    entrada.close();

    return 0;
}

string getNegacao(string expr) {
    string sub;
    int i=2, pe=0, pd=0;

    // pula caractere '~' e comeca pela pos 2 da string
    if(expr[i]=='(' && i < expr.length()) {
        pe++;
        sub += expr[i];
        i++;
        while(i < expr.length() && pe!=pd) {
            if(expr[i]=='(') pe++;
            else if(expr[i]==')') pd++;
            sub += expr[i];
            i++;
        }
    }
    // caso em que expr eh atomica
    else {
        while(i < expr.length() && expr[i]!=')') {
            sub += expr[i];
            i++;
        }
    }
    return sub;
}

string getExpr(string linha) {
    int pe=0, pd=0, i;
    bool stop=true;
    string expr;

    // verifica se eh expressao atomica
    if(linha[0] > 40 && linha[0] < 91) {
        expr = linha[0];
        return expr;
    }
    else {
        // salva em nova string ate que os parenteses a esquerda se igualem aos da direita
        for(i=0; stop && i<linha.length(); i++) {
            if(linha[i] == '(') {
                expr = linha[i];
                pe++;
                stop=false;
            }
        }
        while(pe > pd && i<linha.length()) {
            if(linha[i] == '(') {
                expr += linha[i];
                pe++;
            }
            else if(linha[i] == ')') {
                expr += linha[i];
                pd++;
            }
            else expr += linha[i];
            i++;
        }
    }
    return expr;
}

// funcao apaga expressoes antes da virgula e retorna string apos a virgula
string getExpr2(string linha) {
    int i=0;
    bool stop=true;
    string expr;
    
    while(stop && i<linha.length()) {
        if(linha[i]==',') stop = false;
        i++;
    }
    i++;
    while(i<linha.length()) {
        expr += linha[i];
        i++;
    }
    return expr;
}

int getProblem(string linha) {
    int prob;
    size_t taut, refut, sat, insat, cl;

    taut = linha.find("tautologia");
    refut = linha.find("refutavel");
    sat = linha.find("satisfativel");
    insat = linha.find("insatisfativel");
    cl = linha.find("consequencia logica");

    if(taut != string::npos) prob = 2;
    else if(refut != string::npos) prob = 1;
    else if(sat != string::npos && insat == string::npos) prob = 0;
    else if(insat != string::npos) prob = 3;
    else if(cl != string::npos) prob = 4;

    return prob;
}

char getOperator(string expr) {
    bool stop = true;
    int i=1, pe=0, pd=0;
    char ret;
    // caso seja ~ , nao tera o parenteses a esquerda no inicio
    if(expr[i] != '~') {
        while(stop && i<expr.length()) {
            if(expr[i] == '(') pe++;
            else if(expr[i] == ')') pd++;
            if(pd == pe) stop=false;
            i++;
        }
        ret = expr[i+1];
    }
    else ret = '~';

    return ret;
}

vector <Node *> sortNodes(vector <Node *> nodes) {
    vector <Node *> nos;
    int i;
    char op;
    bool value;
    for(i=0; i<nodes.size(); i++) {
        op = getOperator(nodes[i]->getExpression());
        value = nodes[i]->getTruthValue();

        // regras que nao bifurcam
        if((op=='&' && value==true) || (op=='v' && value==false) || (op=='>' && value==false) || (op=='~')){
            nos.insert(nos.begin(), nodes[i]);
        }
        else nos.push_back(nodes[i]);
    }
    return nos;
}

void getSubExpr(string expr, string *sub1, string *sub2) {
    int i=1, pe=0, pd=0;

    // eh atomico
    if(expr[i] > 40 && expr[i] < 91) {
        *sub1 = expr[i];
        i+=4;
    }

    // expr do tipo (A & B)
    else {
        pe++;
        *sub1 += expr[i];
        i++;
        while(pe!=pd && i<expr.length()) {
            if(expr[i]=='(') pe++;
            else if(expr[i]==')') pd++;
            *sub1 += expr[i];
            i++;
        }
        i+=3;
    }

    // pula 3 posicoes e copia o resto da expr para sub2 - o parenteses final
    while(i < expr.size()-1) {
        *sub2 += expr[i];
        i++;
    }

}