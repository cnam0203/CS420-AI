
#include <iostream>
#include <fstream>
#include <string>

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
	string **arr = new string*[10];
	for (int i = 0; i < 10; i++)
		arr[i] = new string[10];

	myFile.open(fileName);
	if (myFile.is_open()) {
		int m = 9;
		int n = 0;
		getline(myFile, line);
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

int** convertInt(string **arrStr) {
	int **arrInt = new int*[10];
	for (int i = 0; i < 10; i++)
		arrInt[i] = new int[10];

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (arrStr[9-i][j].compare("G") == 0)
				arrInt[9-i][j] = 1;
			else if (arrStr[9-i][j].compare("S") == 0)
				arrInt[9-i][j] = 2;
			else if (arrStr[9-i][j].compare("B") == 0)
				arrInt[9-i][j] = 3;
			else if (arrStr[9-i][j].compare("SB") == 0)
				arrInt[9-i][j] = 4;
			else if (arrStr[9-i][j].compare("W") == 0)
				arrInt[9-i][j] = 5;
			else if (arrStr[9-i][j].compare("P") == 0)
				arrInt[9-i][j] = 6;
			else if (arrStr[9-i][j].compare("A") == 0)
				arrInt[9-i][j] = 7;
			else
				arrInt[9-i][j] = 0;
			cout << arrInt[9-i][j] << " ";
		}
		cout << endl;
	}

	return arrInt;
}

int getNumGold(int **arr) { // Trả về số lượng vàng
	int num = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (arr[i][j] == 1)
				num++;
		}
	}
	return num;
}

Position getInitPos(int **arr) { // Trả về vị trí khởi tạo cho agent
	Position agent;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (arr[i][j] == 7) {
				cout << j << endl;
				agent.setPos(i, j);
				return agent;
			}
		}
	}
	return agent;
}

bool limitPos(int x, int y) {  // Xét xem position có vượt khỏi map ko
	if (x < 0 || x>9)
		return false;
	if (y < 0 || y>9)
		return false;
	return true;
}

void checkBlock(Block &block, int kind, int x, int y, bool &isWumpDied, Position &wumPos, bool &isDetected, string direct) {
	if (limitPos(x, y)) {  // Nếu như block đó nằm ngoài map
		if (!block.isOk()) { // Nếu như block sắp predict đã chắc chắn an toàn trước đó thì khỏi predict
			switch (kind) {  // Xét block đang đứng type nào ( S, B , BS, G , A hay whitespace)
			case 1: // Nếu ô đang đứng là gold thì ô xung quanh an toàn + đã được predict
				block.setOk();
				block.setPredicted();
				break;
			case 2: // Nếu ô đang đứng là S
				if (block.isPredicted()) {  // Nếu block đã được predict
					if (!isWumpDied) { // Nếu có wumpus chưa die
						if (block.isWump()) { // Nếu block trước đó đã được predict là có wumpus => giết
							cout << "Wump died by at " << x << "," << y << " " << direct << endl;
							isWumpDied = true;
							isDetected = true;
							wumPos.setPos(x, y);
							block.setOk();
							block.setPredicted();
							return;
						}
						if (block.isPit()) { // Nếu block trước đó đã được predict là có pit
							block.setPit(false);
						}

					}
					else { // Nếu wumpus đã die rồi
						if (x != wumPos.getX() && y != wumPos.getY()) { // Nếu block đang predict nằm khác vị trí con wumpus thì block đó an toàn
							block.setOk();
						}
					}
				}
				else { // Nếu ô đó chưa được predict
					if (!isWumpDied) { // Nếu con wumpus chưa chết thì predict block đó có wumpus
						block.setWump(true);
						block.setPredicted();
					}
					else { // Nếu con wumpus đã chết rồi
						if (x != wumPos.getX() && y != wumPos.getY()) { // Nếu block đang predict nằm khác vị trí con wumpus thì block đó an toàn
							block.setOk();
							block.setPredicted();
						}
					}
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
				if (block.isPredicted()) { // Nếu chưa được predict
					if (!isWumpDied) { // Nếu như wumpus chưa die
						if (block.isWump()) { //  Nếu như block này đã được dự đoán có wumpus trước đó
							cout << "Wump died by at " << x << "," << y << " " << direct << endl;
							isWumpDied = true;
							isDetected = true;
							wumPos.setPos(x, y);
							block.setOk();
							block.setPredicted();
							return;
						}
					}
					else { // Nếu như con wumpus đã die
						if (x != wumPos.getX() && y != wumPos.getY()) { // Nếu như block này khác vị trí wumpus
							block.setWump(false);
						}
					}
				}
				else { // Nếu như chưa được predict
					block.setWump(true);
					block.setPit(true);
					block.setPredicted();
				}
				break;
			case 5:
				if (isWumpDied) {
					block.setOk();
					block.setPredicted();
				}
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
	}

	isDetected = false;
	return;
}

void predictBlock(Block **&arrBlock, int x, int y, int kind, bool &isWumpDied, Position &wumPos) {
	bool isDetected = false;
	checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isWumpDied, wumPos, isDetected, "<-"); // Predict thằng bên trái


	checkBlock(arrBlock[x][y + 1], kind, x, y + 1, isWumpDied, wumPos, isDetected, "->"); // Predict thằng bên phải
	if (isDetected) { // Nếu như thằng Wumpus đã detect được nằm trong block này, thì những block predicted trước đó được xem xét lại,
		checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isWumpDied, wumPos, isDetected, "<-");
	}


	checkBlock(arrBlock[x - 1][y], kind, x - 1, y, isWumpDied, wumPos, isDetected, "|^"); // Predict thằng phía trên
	if (isDetected) { // Tương tự
		checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isWumpDied, wumPos, isDetected, "<-");
		checkBlock(arrBlock[x][y + 1], kind, x, y + 1, isWumpDied, wumPos, isDetected, "->");
	}


	checkBlock(arrBlock[x + 1][y], kind, x + 1, y, isWumpDied, wumPos, isDetected, "|v"); // Predict thằng phía dưới
	if (isDetected) { // Tương tự
		checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isWumpDied, wumPos, isDetected, "<-");
		checkBlock(arrBlock[x][y + 1], kind, x, y + 1, isWumpDied, wumPos, isDetected, "->");
		checkBlock(arrBlock[x + 1][y], kind, x + 1, y, isWumpDied, wumPos, isDetected, "|v");
	}
}



bool compareStep(int x, int y, int oldX, int oldY) {
	if (!limitPos(x, y)) // Nếu như step tiếp theo nằm ngoài map
		return false;
	if (x == oldX && y == oldY) // Check xem có lặp lại bước cũ
		return false;
	return true;
}

void setNextStep(Position &pos, Stack &stackPos, Block **block, int x, int y, bool &isUpdateStep,int &i) {

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
	if (compareStep(x, y + 1, oldX, oldY) && block[x][y + 1].isOk() && block[x][y + 1].isVisited() == 0) {
		pos.setPos(x, y + 1);
		Position newPos;
		newPos.setPos(x, y);
		stackPos.Push(newPos);
		i = i + 1;
		return;
	}
	if (compareStep(x, y - 1, oldX, oldY) && block[x][y - 1].isOk() && block[x][y - 1].isVisited() == 0) {
		pos.setPos(x, y - 1);
		Position newPos;
		newPos.setPos(x, y);
		stackPos.Push(newPos);
		i = i + 1;
		return;
	}

	if (compareStep(x + 1, y, oldX, oldY) && block[x + 1][y].isOk() && block[x + 1][y].isVisited()==0) {
		pos.setPos(x + 1, y);
		Position newPos;
		newPos.setPos(x, y);
		stackPos.Push(newPos);
		i = i + 1;
		return;
	}
	if (compareStep(x - 1, y, oldX, oldY) && block[x - 1][y].isOk() && block[x - 1][y].isVisited()==0) {
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

int main(int argc, const char * argv[]) {
	
	string **inputArr = readFile("map.txt");

	if (inputArr) {
		int **map = convertInt(inputArr);

		Block **arrBlock = new Block*[10];
		for (int i = 0; i < 10; i++)
		{
			arrBlock[i] = new Block[10];
			
		}
		bool isAgentDied = false;
		bool isWumpDied = false;
		int numGold = getNumGold(map);
		int gold = 0;
		int step = 0;
		bool isUpdateStep = true;
		Stack stackPos;


		Position pos = getInitPos(map);
		int i = 0;
		stackPos.Push(pos);
		Position wumPos;

		while ((step+i)!= 150 && !isAgentDied) {
			int x = pos.getX();
			int y = pos.getY();

			if (map[x][y] == 6) { // Agent in wumpus or pit position
				isAgentDied = true;
			}

			else if (map[x][y] == 5 && !isWumpDied) {
				isAgentDied = true;
			}

			else {

				if (arrBlock[x][y].isVisited()==0) {

					if (map[x][y] == 1)
						gold+=100;

					arrBlock[x][y].setVisited();
					arrBlock[x][y].setOk();
				}

				predictBlock(arrBlock, x, y, map[x][y], isWumpDied, wumPos);
				setNextStep(pos, stackPos, arrBlock, x, y, isUpdateStep,i);
				if (!isUpdateStep)
					break;
				cout << "At step " << step + 1 << ": (" << pos.getY()+1 << ", " << pos.getX()+1 << "), score: " << gold << endl;
			}
			step++;
		}
		if (!isUpdateStep)
			cout << "Unable to move" << endl;
		else {
			
			while (i>0)
			{
				cout << "At step " << 150-i+1 << ": (" << stackPos.getPop().getY() << ", " << stackPos.getPop().getX() << "), score: " << gold << endl;
				if (i == 1)
				{
					pos = stackPos.getPop();
					cout << pos.getY() << "AND " << pos.getX() << endl;
 				}
				stackPos.Pop();
				i--;
				
			}
		}
		if (map[pos.getX()][pos.getY()]!=7)
			cout << "The agent is trapped and die" << endl;
		else cout << "The agent surpass the level" << endl;
		cout << step << ", score: " << gold << "/" << numGold << endl;
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
