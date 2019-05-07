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
string* getSubExpr(string expr);
string getExpr2(string linha);

int main() {
    ifstream entrada;
    ofstream saida;
    int n, i, prob;
    string linha, expr;
    vector <Node *> appNodes, leafs;
    Node *arvore;

    saida.open("Saida.out");
    entrada.open("Entrada.in");
    entrada >> n;
    getline(entrada, linha);
    i = n;

    while(i > 0) {
        getline(entrada, linha);
        prob = getProblem(linha);

        if(prob == 0 || prob == 3) {
            expr = getExpr(linha);
            Node node(expr, true);
            arvore = &node;
        }
        else if(prob == 1 || prob == 2) {
            expr = getExpr(linha);
            Node node(expr, false);
            arvore = &node;
        }
        else if(prob == 4) {
            int j;
            bool stop=true;
            string lnova;
            expr = getExpr(linha);
            Node node(expr, false);
            arvore = &node;
            for(j=0; stop; j++) {
                if(linha[j] == '{') {
                    stop=false;
                }
            }
            while(linha[j] != '}') {
                lnova += linha[j];
                j++;
            }
            while(lnova.length() > 0) {
                expr = getExpr(lnova);
                arvore->insertFront(expr, true);
                lnova = getExpr2(lnova);
            }
            
        }
        while(!arvore->isClosed() && !arvore->getAppliableNodes().empty()) {
            appNodes = sortNodes(arvore->getAppliableNodes());
            for(int k=0; k<appNodes.size(); k++) {
                expr = appNodes[k]->getExpression();
                bool v = appNodes[k]->getTruthValue();
                char op = getOperator(expr);
                if(op == '~') {
                    string neg = getNegacao(expr);
                    leafs = appNodes[k]->insertFront(neg, !v);
                }
                else if(op == '&') {
                    string *subs;
                    subs = new string[2];
                    subs = getSubExpr(expr);

                    if(v) leafs = appNodes[k]->insertFront(subs[0], v, subs[1], v);
                    else leafs = appNodes[k]->insertSides(subs[0], v, subs[1], v);
                }
                else if(op == 'v') {
                    string *subs;
                    subs = new string[2];
                    subs = getSubExpr(expr);
                    
                    if(!v) leafs = appNodes[k]->insertFront(subs[0], v, subs[1], v);
                    else leafs = appNodes[k]->insertSides(subs[0], v, subs[1], v);
                }
                else if(op == '>') {
                    string *subs;
                    subs = new string[2];
                    subs = getSubExpr(expr);
                    if(!v) leafs = appNodes[k]->insertFront(subs[0], !v, subs[1], v);
                    else leafs = appNodes[k]->insertSides(subs[0], !v, subs[1], v);
                }
                for(int a=0; a<leafs.size(); a++) {
                    if(leafs[a]->checkContradiction()){
                        leafs[a]->markContradiction();
                    }    
                }
                appNodes[k]->markApplied();
            }
        }
        arvore->printTree();

        saida << "Problema #" << n-i+1 << endl;

        if(prob==0) {
            if(arvore->isClosed()) saida << "Nao, nao e satisfativel." << endl;
            else saida << "Sim, e satisfativel." << endl;
        }
        else if(prob==1) {
            if(arvore->isClosed()) saida << "Nao, nao e refutavel." << endl;
            else saida << "Sim, e refutavel." << endl;
        }
        else if(prob==2) {
            if(arvore->isClosed()) saida << "Sim, e tautologia." << endl;
            else saida << "Nao, nao e tautologia." << endl;
        }
        else if(prob==3) {
            if(arvore->isClosed()) saida << "Sim, e insatisfativel." << endl;
            else saida << "Nao, nao e insatisfativel." << endl;
        }
        else {
            if(arvore->isClosed()) saida << "Sim, e consequencia logica." << endl;
            else saida << "Nao, nao e consequencia logica." << endl;
        }
        i--;
        saida << endl;
    }
}

string getNegacao(string expr) {
    string sub;
    int i=2, pe=0, pd=0;
    if(expr[i]=='(') {
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

    if(linha[0] > 40 && linha[0] < 91) {
        expr = linha[0];
        return expr;
    }
    else {
        for(i=0; stop; i++) {
            if(linha[i] == '(') {
                expr = linha[i];
                pe++;
                stop=false;
            }
        }
        while(pe > pd) {
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
    if(!stop) return expr;
    else return "";
}

string getExpr2(string linha) {
    int i=0;
    bool stop=true;
    string expr;

    while(stop) {
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
    if(expr[i] != '~') {
        while(stop) {
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
    char op;
    bool value;
    for(int i=0; i<nodes.size(); i++) {
        op = getOperator(nodes[i]->getExpression());
        value = nodes[i]->getTruthValue();
        if((op=='v' && value==true) || (op=='&' && value==false) || (op=='>' && value==false) || (op=='~')){
            nos.insert(nos.begin(), nodes[i]);
        }
        else nos.push_back(nodes[i]);
    }
    return nos;
}

string* getSubExpr(string expr) {
    int i=1, pe=0, pd=0;
    string *sub;
    sub = new string[2];

    if(expr[i] != '(') {
        while((i<expr.size()) && (expr[i]!='&' && expr[i]!='v' && expr[i]!='>')) {
            sub[0] += expr[i];
            i++;
        }
        i+=2;
    }
    else {
        pe++;
        sub[0] += expr[i];
        i++;
        while(pe!=pd) {
            if(expr[i]=='(') pe++;
            else if(expr[i]==')') pd++;
            sub[0] += expr[i];
            i++;
        }
        i+=3;
    }
    if(sub[0][0]>40 && sub[0][0]<91) sub[0]=sub[0][0];
    while(i < expr.size()-1) {
        sub[1] += expr[i];
        i++;
    }
    if(sub[1][0]>40 && sub[1][0]<91) sub[1]=sub[1][0];

    return sub;
}