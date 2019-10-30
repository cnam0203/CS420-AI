//
//  main.cpp
//  position
//
//  Created by Nguyen Chan Nam on 10/29/19.
//  Copyright © 2019 Nam. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


string** readFile(string fileName) {
    string line;
    ifstream myFile;
    string **arr = new string*[10];
    for (int i = 0; i < 10; i++)
        arr[i] = new string[10];
    
    myFile.open(fileName);
    if (myFile.is_open()) {
        int m = 0;
        int n = 0;
        while(getline(myFile, line)) {
            for (int i = 0; i < line.length(); i++) {
                if (line[i] != '.')
                    arr[m][n] += line[i];
                else
                    n++;
            }
            n = 0;
            m++;
        }
        myFile.close();
        return arr;
    }
    else {
        cout<<"Unable to open file"<<endl;
        return NULL;
    }
}

int** convertInt(string **arrStr) {
    int **arrInt = new int*[10];
    for (int i = 0; i < 10; i++)
        arrInt[i] = new int[10];
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (arrStr[i][j].compare("G") == 0)
                arrInt[i][j] = 1;
            else if (arrStr[i][j].compare("S") == 0)
                arrInt[i][j] = 2;
            else if (arrStr[i][j].compare("B") == 0)
                arrInt[i][j] = 3;
            else if (arrStr[i][j].compare("SB") == 0)
                arrInt[i][j] = 4;
            else if (arrStr[i][j].compare("W") == 0)
                arrInt[i][j] = 5;
            else if (arrStr[i][j].compare("P") == 0)
                arrInt[i][j] = 6;
            else if (arrStr[i][j].compare("A") == 0)
                arrInt[i][j] = 7;
            else
                arrInt[i][j] = 0;
        }
    }
    
    return arrInt;
}


int main(int argc, const char * argv[]) {
    
    string **inputArr = readFile("map2.txt");
    
    if (inputArr) {
        int **map = convertInt(inputArr);
    }
    else
        return 0;
}
