//
//  main.cpp
//  q-learning
//
//  Created by Nguyen Chan Nam on 12/15/19.
//  Copyright © 2019 Nam. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>


using namespace std;

class Position {
private:
    int x, y = 0;
public:
    Position() {
        this->x = 0;
        this->y = 0;
    }
    void setValue(int x, int y) {
        this->x = x;
        this->y = y;
    }
    int getX() {
        return this->x;
    }
    int getY() {
        return this->y;
    }
};


bool checkAdjacent(int x1, int y1, int x2, int y2) {
    if (x1 == x2 + 1 && y1 == y2)
        return true;
    if (x1 == x2 - 1 && y1 == y2)
        return true;
    if (x1 == x2 && y1 == y2 + 1)
        return true;
    if (x1 == x2 && y1 == y2 - 1)
        return true;
    return false;
}

class Block {
private:
    Position pos1, pos2;
    float reward = -1;
public:
    void setBlock(int x1, int y1, int x2, int y2, float value) {
        this->pos1.setValue(x1, y1);
        this->pos2.setValue(x2, y2);
        
        if (checkAdjacent(x1, y1, x2, y2)) {
            if (value == 5 || value == 6)
                this->reward = -100;
            else if (value == 1)
                this->reward = 100;
            else
                this->reward = 0;
        }
    }
    
    void copyBlock(Block block) {
        this->pos1.setValue(block.getPos1().getX(), block.getPos1().getY());
        this->pos2.setValue(block.getPos2().getX(), block.getPos2().getY());
        this->reward = block.getReward();
    }
    
    float getReward() {
        return this->reward;
    }
    
    void setReward(int value) {
        this->reward = value;
    }
    
    Position getPos1() {
        return this->pos1;
    }
    
    Position getPos2() {
        return this->pos2;
    }
};

string** readFile(string fileName) {
    string line;
    ifstream myFile;

    myFile.open(fileName);
    if (myFile.is_open()) {
        getline(myFile, line);
        int size = stoi(line);
        int m = size - 1;
        int n = 0;
        string **arr = new string*[size];
        for (int i = 0; i < size; i++)
            arr[i] = new string[size];
        while (getline(myFile, line)) {
            for (int i = 0; i < line.length(); i++) {
                if (line[i] != '.')
                    arr[m][n] += line[i];
                else
                    n++;
            }
            n = 0;
            m--;
        }
        myFile.close();
        return arr;
    }
    else {
        cout << "Unable to open file" << endl;
        return NULL;
    }
}

int readSize(string fileName) {
    string line;
    ifstream myFile;

    myFile.open(fileName);
    if (myFile.is_open()) {
        getline(myFile, line);
        int size = stoi(line);
        myFile.close();
        return size;
    }
    else {
        cout << "Unable to open file" << endl;
        return NULL;
    }
}

int** convertInt(string **arrStr, int size) {
    int **arrInt = new int*[size];
    for (int i = 0; i < size; i++)
        arrInt[i] = new int[size];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (arrStr[size-1-i][j].compare("G") == 0)
                arrInt[size-1-i][j] = 1;
            else if (arrStr[size-1-i][j].compare("S") == 0)
                arrInt[size-1-i][j] = 2;
            else if (arrStr[size-1-i][j].compare("B") == 0)
                arrInt[size-1-i][j] = 3;
            else if (arrStr[size-1-i][j].compare("SB") == 0)
                arrInt[size-1-i][j] = 4;
            else if (arrStr[size-1-i][j].compare("W") == 0)
                arrInt[size-1-i][j] = 5;
            else if (arrStr[size-1-i][j].compare("P") == 0)
                arrInt[size-1-i][j] = 6;
            else if (arrStr[size-1-i][j].compare("A") == 0)
                arrInt[size-1-i][j] = 7;
            else
                arrInt[size-1-i][j] = 0;
            cout << arrInt[size-1-i][j] << " ";
        }
        cout << endl;
    }

    return arrInt;
}

Position getInitPos(int **arr, int size) { // Trả về vị trí khởi tạo cho agent
    Position agent;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (arr[i][j] == 7) {
                agent.setValue(i, j);
                return agent;
            }
        }
    }
    return agent;
}

int getNumGold(int **arr, int size) { // Trả về số lượng vàng
    int num = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (arr[i][j] == 1)
                num++;
        }
    }
    return num;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    int size = readSize("map1.txt");
    string **inputArr = readFile("map1.txt");
    
    if (inputArr) {
        int **map = convertInt(inputArr, size);
        Block **arrInitBlock = new Block*[size*size];
        int count1 = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                arrInitBlock[count1] = new Block [size*size];
                int x1 = i;
                int y1 = j;
                int count2 = 0;
                for (int z = 0; z < size; z++) {
                    for (int t = 0; t < size; t++) {
                        int x2 = z;
                        int y2 = t;
                        arrInitBlock[count1][count2].setBlock(x1, y1, x2, y2, map[x2][y2]);
                        count2++;
                    }
                }
                count1++;
            }
        }
        
        for (int i = 0; i < size*size; i++) {
            for (int j = 0; j < size*size; j++)
                cout<<arrInitBlock[i][j].getReward()<<" ";
            cout<<endl;
        }
        
        int **predict = new int*[size*size];
        for (int i = 0; i < size*size; i++) {
            predict[i] = new int [size*size];
            for (int j = 0; j < size*size; j++) {
                predict[i][j] = 0;
            }
        }
        
        int epsilon = 1000;
       
        while (epsilon >= 0) {
            epsilon--;
            
            Block **arrBlock = new Block*[size*size];
            for (int i = 0; i < size*size; i++) {
                arrBlock[i] = new Block[size*size];
                for (int j = 0; j < size*size; j++) {
                    arrBlock[i][j].copyBlock(arrInitBlock[i][j]);
                }
            }
            
            int count = getNumGold(map, size);    // Lấy số lượng vàng
            int step = 0;
            int row = 0;
            Position init = getInitPos(map, size); // Lấy vị trí ban đầu
            Block currentBlock;
            for (int i = 0; i < size*size; i++) {
                for (int j = 0; j < size*size; j++) {
                    int x = arrBlock[i][j].getPos1().getX();
                    int y = arrBlock[i][j].getPos1().getY();
                    if (x == init.getX() && y == init.getY()) {
                        row = i;                  // Lấy vị trí hàng chứa initial state
                        break;
                    }
                }
            }
            
            while(!(count == 0 || step > 150)) {  // Khi chưa đạt đến goal state hoặc đi nhiều hơn 150 steps
                int randCount  = 0;
                for (int i = 0; i < size*size; i++) {
                    if (arrBlock[row][i].getReward() >= 0)  // Xét những ô có thể đi tiếp
                        randCount++;
                }
                
                // Chọn random state tiếp theo
                int* arrRand = new int [randCount];
                int updateCount = 0;
                for (int i = 0; i < size*size; i++) {
                    if (arrBlock[row][i].getReward() >= 0) {
                        arrRand[updateCount] = i;
                        updateCount++;
                    }
                }
                int randCol = rand() % randCount;
                int randIndex = arrRand[randCol];
                if (epsilon == 0) {
                    cout<<"("<<arrBlock[row][randIndex].getPos2().getX()<<","<<arrBlock[row][randIndex].getPos2().getY()<<")"<<endl;
                }
                // Set position cho new Random State
                Position newState;
                newState.setValue(arrBlock[row][randIndex].getPos2().getX(), arrBlock[row][randIndex].getPos2().getY());
                int max = -10000;
                
                // Chọn ra max Q
                for (int i = 0; i < size*size; i++) {
                    for (int j = 0; j < size*size; j++) {
                        int x = arrBlock[i][j].getPos1().getX();
                        int y = arrBlock[i][j].getPos1().getY();
                        int reward = arrBlock[i][j].getReward();
                        if (x == newState.getX() && y == newState.getY() && reward >= 0) {
                            if (predict[i][j] >= max) {
                                max = predict[i][j];
                            }
                        }
                    }
                }
                
                // Cập nhật Q
                predict[row][randIndex] = arrBlock[row][randIndex].getReward() + 0.6*max;
                
                // Nếu như ô tiếp theo là ô được chọn là goal => reward = 0
                if (arrBlock[row][randIndex].getReward() == 100) {
                    count--;
                    for (int i = 0; i < size*size; i++) {
                        for (int j = 0; j < size*size; j++) {
                            int x2 = arrBlock[i][j].getPos2().getX();
                            int y2 = arrBlock[i][j].getPos2().getY();
                            int choosenX = newState.getX();
                            int choosenY = newState.getY();
                            int value = arrBlock[i][j].getReward();
                            if (x2 == choosenX && y2 == choosenY && value == 100) {
                                arrBlock[i][j].setReward(0);
                            }
                        }
                    }
                }
                // Chọn ra hàng chứa newState
                for (int i = 0; i < size*size; i++) {
                    for (int j = 0; j < size*size; j++) {
                        int x = arrBlock[i][j].getPos1().getX();
                        int y = arrBlock[i][j].getPos1().getY();
                        int choosenX = newState.getX();
                        int choosenY = newState.getY();
                        if (x == choosenX && y == choosenY) {
                            row = i;
                            break;
                        }
                    }
                }
                step++;
            }
            if (epsilon == 0) {
                for (int i = 0; i < size*size; i++) {
                           for (int j = 0; j < size*size; j++)
                               cout<<predict[i][j]<<" ";
                           cout<<endl;
                }
                cout<<step<<endl;
            }
        }
    }
    return 0;
}
