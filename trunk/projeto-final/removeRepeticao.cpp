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
using namespace std;

int main (int argc, const char * argv[])
{
    map<string, int> freq; 
    string word;
    map<string, int>::const_iterator iter;
    
    while (cin >> word) {
        freq[word]++;
    }
    
    for (iter=freq.begin(); iter != freq.end(); ++iter) {
        cout << iter->first << " " << iter->second << endl;
    }
    
    
    return 0;
}

