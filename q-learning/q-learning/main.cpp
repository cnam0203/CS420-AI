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
#include <ctime>
#include <cstring>

#include <thread>
#include <chrono>

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"


using namespace std;

class Position {
private:
    int x, y = 0;
    bool visited = false;
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
    bool isVisted() {
        return this->visited;
    }
    void setVisited() {
        this->visited = true;
    }
 };

void printMap(int size, int x, int y) {
    for (int i = 0 ; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == x && j == y) {
                cout<<YELLOW<<"A"<<RESET<<" ";
            }
            else
                cout<<"."<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}


bool checkAdjacent(int x2, int y2, int size) {
    if (x2 < 0 || y2 < 0 || x2 >= size || y2 >= size)
        return false;
    return true;
}

class Block {
private:
    Position pos1, pos2;
    float reward = -5;
public:
    void setBlock(int x1, int y1, int x2, int y2, int **map, int size) {
        this->pos1.setValue(x1, y1);
        this->pos2.setValue(x2, y2);
        
        if (checkAdjacent(x2, y2, size)) {
            
            int value = map[x2][y2];
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
            else if (arrStr[size-1-i][j].compare("BS") == 0)
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

void createRewardTable(int **map, Block **arrInitBlock, int size) {
    int count1 = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            arrInitBlock[count1] = new Block [4];
            int count2 = 0;
            arrInitBlock[count1][count2++].setBlock(i, j, i+1, j, map, size);
            arrInitBlock[count1][count2++].setBlock(i, j, i-1, j, map, size);
            arrInitBlock[count1][count2++].setBlock(i, j, i, j+1, map, size);
            arrInitBlock[count1][count2++].setBlock(i, j, i, j-1, map, size);
            count1++;
        }
    }
}

void createQTable(int **predict, int size) {
    for (int i = 0; i < size*size; i++) {
        predict[i] = new int [4];
        for (int j = 0; j < 4; j++) {
            predict[i][j] = 0;
        }
    }
}

int getRow(Block **arrBlock, int size, Position init) {
    for (int i = 0; i < size*size; i++) {
        for (int j = 0; j < 4; j++) {
            int x = arrBlock[i][j].getPos1().getX();
            int y = arrBlock[i][j].getPos1().getY();
            if (x == init.getX() && y == init.getY()) {
                return i;          // Lấy vị trí hàng chứa initial state
            }
        }
    }
    return 0;
}

int getRandomState(Block **arrBlock, int size, int row) {
    int randCount  = 0;
    for (int i = 0; i < 4; i++) {
        
        if (arrBlock[row][i].getReward() >= 0 )  // Xét những ô có thể đi tiếp
            randCount++;
    }

    // Chọn random state tiếp theo
    int* arrRand = new int [randCount];
    int updateCount = 0;
    for (int i = 0; i < 4; i++) {
        if (arrBlock[row][i].getReward() >= 0) {
            arrRand[updateCount] = i;
            updateCount++;
        }
    }
    
//    srand ((unsigned int)time(NULL));
    int randCol =  rand() % randCount;
    return arrRand[randCol];
}

int getMaxReward(Block **arrBlock, int **predict, int size, int row, Position newState, int max) {
    for (int i = 0; i < size*size; i++) {
        for (int j = 0; j < 4; j++) {
            int x = arrBlock[i][j].getPos1().getX();
            int y = arrBlock[i][j].getPos1().getY();
            int reward = arrBlock[i][j].getReward();
            if (x == newState.getX() && y == newState.getY()) {
                if (predict[i][j] > max) {
                    max = predict[i][j];
                }
            }
        }
    }
    return max;
}

void updateQTable(Block **arrBlock, int size, int &count, Position newState, int randIndex, int row) {
    if (arrBlock[row][randIndex].getReward() == 100) {
        count--;
        for (int i = 0; i < size*size; i++) {
            for (int j = 0; j < 4; j++) {
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
}

Position* findGoldPos(int **map, int size, int goal) {
    Position* arrGold = new Position[goal];
    int count = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (map[i][j] == 1) {
                arrGold[count++].setValue(i, j);
            }
        }
    }
    return arrGold;
}

void printPredict(int **predict, int size, ofstream &fout) {
    fout<<"Predict table:"<<endl;
    fout<<"      Down  Up    Right Left"<<endl;
    for (int i = 0; i < size*size; i++) {
        int row = i/size;
        int col = i%size;
        fout<<row+1<<","<<col+1<<"   ";
        for (int j = 0 ; j < 4; j++) {
            fout<<predict[i][j];
            string stringPre = to_string(predict[i][j]);
            int lengthStr = stringPre.length();
            for (int j = 6 - lengthStr; j > 0; j--) {
                fout<<" ";
            }
        }
        fout<<endl;
    }
}

void findPath(Position initPos, int** predict, int size, Position* arrGold, int numGold, ofstream &fout) {
    int step = 1;
    int score = 0;
    int goal = numGold;
    Position currentPos;
    currentPos.setValue(initPos.getX(), initPos.getY());
    while (step <= 150 && goal > 0) {
        printMap(size, currentPos.getX(), currentPos.getY());
        this_thread::sleep_for(chrono::milliseconds(500));
        for (int i = 0 ; i < numGold; i++) {
            if (!arrGold[i].isVisted()) {
                int x1 = currentPos.getX();
                int y1 = currentPos.getY();
                int x2 = arrGold[i].getX();
                int y2 = arrGold[i].getY();
                if ((x1 == x2) && (y1 == y2)) {
                    goal--;
                    score+= 100;
                    arrGold[i].setVisited();
                }
            }
        }
        int state = currentPos.getX()*size + currentPos.getY();
        int max = -2000;
        int countMax = 0;
        for (int i = 0; i < 4; i++) {
            if (predict[state][i] >= max) {
                max = predict[state][i];
                countMax = 1;
            }
            else if (predict[state][i] == max) {
                countMax++;
            }
        }
        int* posMax = new int[countMax];
        int countPos = 0;
        for (int i = 0; i < 4; i++) {
            if (predict[state][i] == max) {
                posMax[countPos] = i;
            }
        }
        int randMax = rand() % countMax;
        int nextIndex = posMax[randMax];
        if (nextIndex == 0)
            currentPos.setValue(currentPos.getX()+1, currentPos.getY());
        else if (nextIndex == 1)
            currentPos.setValue(currentPos.getX()-1, currentPos.getY());
        else if (nextIndex == 2)
            currentPos.setValue(currentPos.getX(), currentPos.getY()+1);
        else
            currentPos.setValue(currentPos.getX(), currentPos.getY()-1);
        fout<<"Step "<<step<<": next step("<<currentPos.getX()+1<<","<<currentPos.getY()+1<<"),  "<<score<<endl;
        step++;
    }
    fout<<"Score: "<<score<<"/"<<numGold*100<<endl;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    int size = readSize("map3.txt");
    string **inputArr = readFile("map3.txt");
    srand((unsigned)(time(0)));
    ofstream fout;
    fout.open("output.txt");
    
    if (inputArr) {
        int **map = convertInt(inputArr, size);
         int **predict = new int*[size*size];
        createQTable(predict, size);
        
       
        int epsilon = 1000;
       
        while (epsilon > 0) {
            epsilon--;
            Block **arrBlock = new Block*[size*size];
            createRewardTable(map, arrBlock, size);
            
            int count = getNumGold(map, size);    // Lấy số lượng vàng
            int step = 0;
            
            int rand1 = rand()%size;
            int rand2 = rand()%size;
            
            Position init; // Lấy vị trí ban đầu
            init.setValue(rand1, rand2);
            int row = getRow(arrBlock, size, init);
            step = 1;
            
            while(!(count == 0 || step > 150)) {  // Khi chưa đạt đến goal state hoặc đi nhiều hơn 150 steps
                
                int randIndex = getRandomState(arrBlock, size, row);
                
                // Set position cho new Random State
                Position newState;
                newState.setValue(arrBlock[row][randIndex].getPos2().getX(), arrBlock[row][randIndex].getPos2().getY());
                
                
                // Chọn ra max Q
                int max = getMaxReward(arrBlock, predict, size, row, newState, -10000);
                
                // Cập nhật Q
                predict[row][randIndex] = arrBlock[row][randIndex].getReward() + 0.9*max;
                
                // Nếu như ô tiếp theo là ô được chọn là goal => reward = 0
                updateQTable(arrBlock, size, count, newState, randIndex, row);
                
                // Chọn ra hàng chứa newState
                row = getRow(arrBlock, size, newState);
                step++;
            }
        }
        printPredict(predict, size, fout);
        Position pos = getInitPos(map, size);
        int numGoal = getNumGold(map, size);
        Position* goldPos;
        goldPos = findGoldPos(map, size, numGoal);
        findPath(pos, predict, size, goldPos, numGoal, fout);
        fout.close();
    }
    return 0;
}
