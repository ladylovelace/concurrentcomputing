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
#include <mpi.h>



#define TAM_CNT_WORD 100
std:: string randonWord(int tam);

using namespace std;

int cntword[TAM_CNT_WORD];
string upToLow(string word);
void initVector();
int countWord(map<string, int> freq);
void printVector(int max);

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
        if (len == 1)
            cout << iter->first << "\n";
        cntword[len]++;
        if (len > max)
            max = len;
    }
    
    return max;
}
void printVector(int max){
    int i;
    for (i=0;i<=max;i++)
        cout << cntword[i]++ << "\n";
}
int main (int argc, const char * argv[])
{
    /*mapa*/
    map<string, int> freq; 
    string word,neword;
    string auxword;
    map<string, int>::iterator iter2;
    
    int max,len,i,auxlen,j;
    time_t inicio,fim;
    /*arquivos*/
    ofstream fileout;
    ifstream filein;
    
    /*MPI*/
    //int qdeProc, myRank, nameLen;
    //char processorName[20];
    //MPI_Init ( &argc, &argv );
    
    //MPI_Comm_size ( MPI_COMM_WORLD, &qdeProc );
    //MPI_Comm_rank ( MPI_COMM_WORLD, &myRank );
    //MPI_Get_processor_name( processorName, &nameLen );
    
    initVector();
    filein.open(argv[1]);
    if(!filein) {
        cerr << "Error: arquivo nao pode ser aberto." << endl;
        exit(1);
    }
    cout << "Iniciando...\n";
    inicio = time(NULL);
    
    fileout.open("new.txt");
    while (filein >> word) {
        word = upToLow(word);
        len = (int)strlen(word.c_str());
        if (len<6)
            freq[word]++;
        else{
            i=0;
            auxlen=len;
            j=5;
            while(i<len){
                auxword.append(word.begin()+i,word.begin()+i+j);
                freq[auxword]++;
                //cout << auxword.c_str() << " :D\n";
                auxword.clear();
                i=i+5;
                auxlen=auxlen-5;
                if (auxlen<=0)
                    break;
                if (j+5 < auxlen)
                    j = j+5;
                else
                    j=auxlen;
            }
        }
    }
    filein.close();
    for (iter2=freq.begin(); iter2 != freq.end(); ++iter2) {
     fileout << iter2->first << " ";
    }
    max = countWord(freq);
    printVector(max);
    cout << "Tamanho maximo de palavra: "<< max << "\n";
    cout << "Removido palavras repetidas.\nProcessando...\n";
    srand((unsigned) time(NULL)); /*fundamental para a geracao de strings randomicas*/
    len = 2; /*Teste*/
    while(cntword[len]>0){
        neword = randonWord(len);
        if (freq.count(neword.c_str())> 0){
            iter2 = freq.find(neword.c_str());
            freq.erase(iter2);
            cout << "Eliminando " << neword.c_str() << "\n";
            cntword[len] --;
        }
    }
    cout << "FIM\n";
    fim = time(NULL);
    cout << "Tempo total de execução: "<< difftime(fim, inicio)<< "\n";
    fileout.close();
    return 0;
}

