//
//  main.cpp
//  repeticaoArquivo
//
//  Created by Leticia Silva on 6/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <ctime>

#define TAM_CNT_WORD 100
std:: string randonWord(int tam);

using namespace std;

int cntword[TAM_CNT_WORD];
string upToLow(string word);
void initVector();
int countWord(map<string, int> freq);
void printVector();

string randonWord(int tam){
    string word;
    int coi;
    int i;
    coi = rand()%5;
    for (i = 0; i < tam; i++) {
        if(coi > 0)
            word[i] = (rand()%26) + 'a';
        else
            word[i] = rand()%9 + '0';
    }
    return word;
}

string upToLow(string word){
    int i;
    for(i=0;i<strlen(word.c_str());i++){
        word[i] = tolower(word[i]);
    }
    return word;
}

void initVector(){
    int i;
    for (i=0;i<TAM_CNT_WORD;i++)
        cntword[i] = 0;
}
int countWord(map<string, int> freq){
    int len;
    int max=0;
    map<string, int>::const_iterator iter;
    for (iter=freq.begin(); iter != freq.end(); ++iter) {
        len = (int)iter->first.size();
        if (len > max)
            max = len;
        cntword[len]++; 
    }
    return max;
}
void printVector(){
    int i;
    for (i=0;i<45;i++)
        cout << cntword[i]++ << "\n";
}
int main (int argc, const char * argv[])
{
    map<string, int> freq; 
    string word,neword;
    map<string, int>::iterator iter2;
    int max,len;
    initVector();
    ofstream fileout;
    ifstream filein;
    filein.open(argv[1]);
    if(!filein) {
        cerr << "Error: arquivo nao pode ser aberto." << endl;
        exit(1);
    }
    cout << "Iniciando...\n";
    while (filein >> word) {
        word = upToLow(word);
        freq[word]++;
    }
    max = countWord(freq);
    printVector();
    cout << "Tamanho maximo de palavra: "<< max << "\n";
    cout << "Removido palavras repetidas.\nProcessando...\n";
    srand((unsigned) time(NULL)); /*fundamental para a geracao de strings randomicas*/
    len = 1; /*Teste*/
    while(cntword[len]>1){
        neword = randonWord(len);
        if (freq.count(neword.c_str())> 0){
            iter2 = freq.find(neword.c_str());
            freq.erase(iter2);
            cout << "Eliminando " << neword.c_str() << "\n";
            cntword[len] --;
        }
    }
    /*for (iter=freq.begin(); iter != freq.end(); ++iter) {
        cout << iter->first << " " << iter->second << endl;
        file << iter->first << " ";
    }*/
    
    filein.close();
    return 0;
}

