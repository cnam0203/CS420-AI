// Change the coordinate map
// Agent can go back (xA,yA) by the 150th step
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdlib.h> 
#include <thread>
#include <chrono>

#define RESET   "\033[0m"
#define RED     "\033[31m"      
#define GREEN   "\033[32m"      
#define YELLOW  "\033[33m" 

using namespace std;



class Position {
private:
    int x;
    int y;
public:
    Position() {
        this->x = 0;
        this->y = 0;
    }

    void setPos(int x, int y) {
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

class Stack {
private:
    Position arr[300];
    int top;

public:
    Stack() {
        top = -1;
    }

    void Push(Position item) {
        if (top == 300 - 1) {
            cout << endl << "Stack is full";
            return;
        }
        top++;
        arr[top] = item;
        return;
    }

    void Pop() {
        top--;
        return;
    }

    bool isEmpty() {
        if (top == -1)
            return true;
        return false;
    }

    Position getPop() {
        Position data = arr[top];
        return data;
    }

};

class Block {
private:
    int type;
    bool ok;
    bool pit;
    int visited;
    bool wump;
    bool predicted;
    bool realWumpus;
    bool realPit;
public:
    Block() {
        this->type = 0;
        this->ok = false;
        this->pit = false;
        this->visited = 0;
        this->wump = false;
        this->predicted = false;
    }
    void setBlock(int kind) {
        this->type = kind;
    }
    void setVisited() {
        this->visited +=1;
    }
    void setOk() {
        this->ok = true;
    }
    void setPit(bool pit) {
        this->pit = pit;
    }
    void setWump(bool wump) {
        this->wump = wump;
    }
    void setPredicted() {
        this->predicted = true;
    }
    void setRealPit() {
        this->realPit = true;
    }
    void setRealWump() {
        this->realWumpus = true;
    }
    int getType() {
        return this->type;
    }
    bool isPit() {
        return this->pit;
    }
    bool isOk() {
        return this->ok;
    }
    int isVisited() {
        return this->visited;
    }
    bool isPredicted() {
        return this->predicted;
    }
    bool isWump() {
        return this->wump;
    }
    bool isRealWump() {
        return this->realWumpus;
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

Position getInitPos(int **arr, int size) { // Trả về vị trí khởi tạo cho agent
    Position agent;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (arr[i][j] == 7) {
                cout << j << endl;
                agent.setPos(i, j);
                return agent;
            }
        }
    }
    return agent;
}

bool limitPos(int x, int y, int size) {  // Xét xem position có vượt khỏi map ko
    if (x < 0 || x>size-1)
        return false;
    if (y < 0 || y>size-1)
        return false;
    return true;
}

void checkBlock(Block &block, int kind, int x, int y, int size) {
       
        if (!block.isOk()) { // Nếu như block sắp predict đã chắc chắn an toàn trước đó thì khỏi predict
            switch (kind) {  // Xét block đang đứng type nào ( S, B , BS, G , A hay whitespace)
            case 1: // Nếu ô đang đứng là gold thì ô xung quanh an toàn + đã được predict
                block.setOk();
                block.setPredicted();
                break;
            case 2: // Nếu ô đang đứng là S
                if (block.isPredicted()) {  // Nếu block đã được predict
                        if (block.isPit()) { // Nếu block trước đó đã được predict là có pit
                            block.setPit(false);
                        }
                    }
                else { // Nếu ô đó chưa được predict
                        block.setWump(true);
                        block.setPredicted();
                }
                break;
            case 3: // Nếu ô đang đứng là B
                if (block.isPredicted()) {  // Nếu block đó đã được predict rồi
                    if (block.isWump()) { // Nếu block này trước đó được dự đoán là có wumpus
                        block.setWump(false);
                    }
                }
                else { // Nếu block đó chưa được predict
                    block.setPit(true);
                    block.setPredicted();
                }
                break;
            case 4: // Nếu ô đang đứng là SB
                    block.setWump(true);
                    block.setPit(true);
                    if (!block.isPredicted()) {
                        block.setPredicted();
                    }
                break;
            case 5:
                break;
            case 6:
                break;
            case 7: // Nếu như ô đang đứng là vị trí bắt đầu
                block.setOk();
                block.setPredicted();
                break;
            default: // Nếu như ô đang đứng là white-space
                block.setOk();
                block.setPredicted();
                break;
            }
        }
    return;
}

void predictBlock(Block **&arrBlock, int x, int y, int kind, int size) {
    if (limitPos(x, y-1, size) && arrBlock[x][y - 1].isVisited() == 0)
        checkBlock(arrBlock[x][y - 1], kind, x, y - 1, size); // Predict thằng bên trái
    if (limitPos(x, y+1, size) && arrBlock[x][y + 1].isVisited() == 0)
        checkBlock(arrBlock[x][y + 1], kind, x, y + 1, size); // Predict thằng bên phải
    if (limitPos(x-1, y, size) && arrBlock[x - 1][y].isVisited() == 0)
        checkBlock(arrBlock[x - 1][y], kind, x - 1, y, size); // Predict thằng phía trên
    if (limitPos(x+1, y, size) && arrBlock[x + 1][y].isVisited() == 0)
        checkBlock(arrBlock[x + 1][y], kind, x + 1, y, size); // Predict thằng phía dưới
}



bool compareStep(int x, int y, int oldX, int oldY, int size) {
    if (!limitPos(x, y, size)) // Nếu như step tiếp theo nằm ngoài map
        return false;
    if (x == oldX && y == oldY) // Check xem có lặp lại bước cũ
        return false;
    return true;
}

void setNextStep(Position &pos, Stack &stackPos, Block **block, int x, int y, bool &isUpdateStep,int &i, int size) {

    int oldX, oldY;

    if (stackPos.isEmpty()) {
        oldX = x;
        oldY = y;
    }

    else {
        Position posTop = stackPos.getPop();
        oldX = posTop.getX();
        oldY = posTop.getY();
    }
    if (compareStep(x, y + 1, oldX, oldY, size) && block[x][y + 1].isOk() && block[x][y + 1].isVisited() == 0) {
        pos.setPos(x, y + 1);
        Position newPos;
        newPos.setPos(x, y);
        stackPos.Push(newPos);
        i = i + 1;
        return;
    }
    if (compareStep(x, y - 1, oldX, oldY, size) && block[x][y - 1].isOk() && block[x][y - 1].isVisited() == 0) {
        pos.setPos(x, y - 1);
        Position newPos;
        newPos.setPos(x, y);
        stackPos.Push(newPos);
        i = i + 1;
        return;
    }

    if (compareStep(x + 1, y, oldX, oldY, size) && block[x + 1][y].isOk() && block[x + 1][y].isVisited()==0) {
        pos.setPos(x + 1, y);
        Position newPos;
        newPos.setPos(x, y);
        stackPos.Push(newPos);
        i = i + 1;
        return;
    }
    if (compareStep(x - 1, y, oldX, oldY, size) && block[x - 1][y].isOk() && block[x - 1][y].isVisited()==0) {
        pos.setPos(x - 1, y);
        Position newPos;
        newPos.setPos(x, y);
        stackPos.Push(newPos);
        i = i + 1;
        return;
    }
    
    

    if (!stackPos.isEmpty()) {
        stackPos.Pop();
        pos.setPos(oldX, oldY);
        i = i - 1;
    }
    else {
        isUpdateStep = false;
    }

    return;
}

void printMap(Block **arrBlock, int size, int x, int y) {
    for (int i = 0 ; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == x && j == y) {
                cout<<YELLOW<<"A"<<RESET<<" ";
            }
            else {
                if (arrBlock[i][j].isOk()) {
                    cout<<"."<<" ";
                }
                else if (arrBlock[i][j].isPit()) {
                    cout<<RED<<"P"<<RESET<<" ";
                }
                else if (arrBlock[i][j].isWump()) {
                    cout<<GREEN<<"W"<<RESET<<" ";
                }
                else {
                    cout<<"."<<" ";
                }
            }
        }
        cout<<endl;
    }
    cout<<endl;
}

int main(int argc, const char * argv[]) {
    
    int size = readSize("map5.txt");
    string **inputArr = readFile("map5.txt");
    ofstream fout;
    fout.open("output.txt");

    if (inputArr) {
        int **map = convertInt(inputArr, size);

        Block **arrBlock = new Block*[size];
        for (int i = 0; i < size; i++)
        {
            arrBlock[i] = new Block[size];
            
        }
        bool isAgentDied = false;
        int numGold = getNumGold(map, size);
        int gold = 0;
        int step = 0;
        bool isUpdateStep = true;
        Stack stackPos;


        Position pos = getInitPos(map, size);
        int i = 0;

        while ((step+i)!= 150 && !isAgentDied) {
            int x = pos.getX();
            int y = pos.getY();

            if (map[x][y] == 6) { // Agent in wumpus or pit position
                isAgentDied = true;
            }

            else if (map[x][y] == 5) {
                isAgentDied = true;
            }

            else {
                if (arrBlock[x][y].isVisited() == 0) {

                    if (map[x][y] == 1)
                        gold+=100;

                    arrBlock[x][y].setVisited();
                    arrBlock[x][y].setOk();
                }

                predictBlock(arrBlock, x, y, map[x][y], size);
                setNextStep(pos, stackPos, arrBlock, x, y, isUpdateStep,i, size);
                if (!isUpdateStep)
                    break;
                fout << "At step " << step + 1 << ": (" << pos.getX()+1 << ", " << pos.getY()+1 << "), score: " << gold << endl;
                printMap(arrBlock, size, x, y);
                this_thread::sleep_for(chrono::milliseconds(500));
            }
            step++;
        }

        if (!isUpdateStep)
            cout << "Unable to move" << endl;
        else {
            while (i > 0) {
                fout << "At step " << 150-i+1 << ": (" << stackPos.getPop().getX() + 1<< ", " << stackPos.getPop().getY() + 1<< "), score: " << gold << endl;
                printMap(arrBlock, size, stackPos.getPop().getX(), stackPos.getPop().getY());
                this_thread::sleep_for(chrono::milliseconds(500));
                if (i == 1)
                {
                    pos = stackPos.getPop();
                    cout << pos.getX() + 1<< " AND " << pos.getY() + 1<< endl;
                 }
                stackPos.Pop();
                i--;
            }
        }

        if (map[pos.getX()][pos.getY()] != 7)
            cout << "The agent is trapped and die" << endl;
        else 
            cout << "The agent surpass the level" << endl;
        cout << step << ", score: " << gold << "/" << numGold*100 << endl;

        fout<<"Score: "<<gold<<"/"<<numGold*100<<endl;
        fout.close();

        system("pause");
        return 0;
    }
    else
    {
        cout << "error";
        system("pause");
        return 0;
    }
}
