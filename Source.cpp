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
	bool wumpded;
	bool realPit;
	bool shotted; // bắn rồi không bắn nữa
public:
	Block() {
		this->type = 0;
		this->ok = false;
		this->pit = false;
		this->visited = 0;
		this->wump = false;
		this->wumpded = false;
		this->predicted = false;
		this->shotted = false;
	}
	void setBlock(int kind) {
		this->type = kind;
	}
	void setVisited() {
		this->visited += 1;
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
	void setRealDead() {
		this->wumpded = true;
	}
	void setShot() {
		this->shotted = true;
	}
	int getType() {
		return this->type;
	}
	bool isPit() {
		return this->pit;
	}
	bool isShot() {
		return this->shotted;
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
	bool isWumpDead() {
		return this->wumpded;
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
			if (arrStr[size - 1 - i][j].compare("G") == 0)
				arrInt[size - 1 - i][j] = 1;
			else if (arrStr[size - 1 - i][j].compare("S") == 0)
				arrInt[size - 1 - i][j] = 2;
			else if (arrStr[size - 1 - i][j].compare("B") == 0)
				arrInt[size - 1 - i][j] = 3;
			else if (arrStr[size - 1 - i][j].compare("SB") == 0|| arrStr[size - 1 - i][j].compare("BS") == 0)
				arrInt[size - 1 - i][j] = 4;
			else if (arrStr[size - 1 - i][j].compare("W") == 0)
				arrInt[size - 1 - i][j] = 5;
			else if (arrStr[size - 1 - i][j].compare("P") == 0)
				arrInt[size - 1 - i][j] = 6;
			else if (arrStr[size - 1 - i][j].compare("A") == 0)
				arrInt[size - 1 - i][j] = 7;
			else
				arrInt[size - 1 - i][j] = 0;
			cout << arrInt[size - 1 - i][j] << " ";
		}
		cout << endl;
	}

	return arrInt;
}
int** wumpos(int** map, int size)
{
	int **wump = new int*[size];
	for (int i = 0; i < size; i++)
		wump[i] = new int[size];
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (map[i][j] == 5) wump[i][j] = 1;
			else wump[i][j] = 0;
		}
	}
	return wump;

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
				agent.setPos(i, j);
				return agent;
			}
		}
	}
	return agent;
}

bool limitPos(int x, int y, int size) {  // Xét xem position có vượt khỏi map ko
	if (x < 0 || x>size - 1)
		return false;
	if (y < 0 || y>size - 1)
		return false;
	return true;
}
bool cheating(int x, int y, int shot_x, int shot_y) //check if the agent is cheating
{
	if (x == shot_x)
	{
		if (y - shot_y == 1 || y - shot_y == -1)
			return false;
		else return true;
	}
	else if (x - shot_x == 1 || x - shot_x == -1)
	{
		if (y == shot_y) return false;
		else return true;
	}
}
void checkBlock(Block &block, int kind, int x, int y, string direct, int size, int &score, int& shot_x, int& shot_y) {
	if (limitPos(x, y, size)) {  // Nếu như block đó nằm ngoài map
		if (!block.isOk()) { // Nếu như block sắp predict đã chắc chắn an toàn trước đó thì khỏi predict
			switch (kind) {  // Xét block đang đứng type nào ( S, B , BS, G , A hay whitespace)
			case 1: // Nếu ô đang đứng là gold thì ô xung quanh an toàn + đã được predict
				block.setOk();
				block.setPredicted();
				break;
			case 2: // Nếu ô đang đứng là S
				if (block.isPredicted()) {  // Nếu block đã được predict
					if (!block.isWumpDead()) { // Nếu có wumpus chưa die
						if (block.isWump()) { // Nếu block trước đó đã được predict là có wumpus => giết
							cout << "Shoot a wump at " << y+1 << "," << x+1 << " " << direct << endl;
							score = score - 100;
							shot_x = x;
							shot_y = y;
							block.setShot();
							block.setOk();
							block.setPredicted();
							return;
						}
						if (block.isPit()) { // Nếu block trước đó đã được predict là có pit
							block.setPit(false);
						}

					}
					else {
						block.setOk();
						block.setPredicted();
						return;
					
					}
				}
				else { // Nếu ô đó chưa được predict
					if (!block.isWumpDead()) { // Nếu con wumpus chưa chết thì predict block đó có wumpus
						block.setWump(true);
						block.setPredicted();
					}
					else { // Nếu con wumpus đã chết rồi
				//		if (x != wumPos.getX() && y != wumPos.getY()) { // Nếu block đang predict nằm khác vị trí con wumpus thì block đó an toàn
							block.setOk();
							block.setPredicted();
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
				if (block.isPredicted()) { // Nếu đã predict
					if (!block.isWumpDead()) { // Nếu như wumpus chưa die
						if (block.isWump()&&!block.isShot()) { //  Nếu như block này đã được dự đoán có wumpus trước đó và chưa bắn
							cout << "Shoot a wump at " << y+1 << "," << x+1 << " " << direct << endl;
							score = score - 100;
							shot_x = x;
							shot_y = y;
							block.setShot();
							block.setPit(true);
							block.setPredicted();
							return;
						}
					}
					else { // Nếu như con wumpus đã die
						block.setPit(false); // Nếu Wumpus nghẻo tại đây => block này không thể có pit
						block.setOk();
						block.setPredicted();
					}
				}
				else { // Nếu như chưa được predict
					block.setWump(true);
					block.setPit(true);
					block.setPredicted();
				}
				break;
			case 5:
			/*	if (isWumpDied) {
					block.setOk();
					block.setPredicted();
				}*/
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
	return;
}

void predictBlock(Block **&arrBlock, int x, int y, int kind, int size, int &score, int &shot_x, int & shot_y) {

	checkBlock(arrBlock[x][y - 1], kind, x, y - 1, "<-", size, score, shot_x, shot_y ); // Predict thằng bên trái
	if (shot_x == -1 && shot_y == -1)
	{
		checkBlock(arrBlock[x][y + 1], kind, x, y + 1, "->", size, score, shot_x, shot_y); // Predict thằng bên phải
		//if (isDetected) { // Nếu như thằng Wumpus đã detect được nằm trong block này, thì những block predicted trước đó được xem xét lại,
		//	checkBlock(arrBlock[x][y - 1], kind, x, y - 1, "<-", size, score, shot_x, shot_y);
		//}
		if (shot_x == -1 && shot_y == -1)
		{

			checkBlock(arrBlock[x - 1][y], kind, x - 1, y, "|v", size, score, shot_x, shot_y); // Predict thằng phía trên
			//if (isDetected) { // Tương tự
			//	checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isDetected, "<-", size);
			//	checkBlock(arrBlock[x][y + 1], kind, x, y + 1, isDetected, "->", size);
			//}
			if (shot_x == -1 && shot_y == -1)
			{

				checkBlock(arrBlock[x + 1][y], kind, x + 1, y, "|^", size, score, shot_x, shot_y); // Predict thằng phía dưới
				//if (isDetected) { // Tương tự
				//	checkBlock(arrBlock[x][y - 1], kind, x, y - 1, isDetected, "<-", size);
				//	checkBlock(arrBlock[x][y + 1], kind, x, y + 1, isWumpDied, isDetected, "->", size);
				//	checkBlock(arrBlock[x + 1][y], kind, x + 1, y, isWumpDied, isDetected, "|v", size);
				//}
			}
		}
	}
}



bool compareStep(int x, int y, int oldX, int oldY, int size) {
	if (!limitPos(x, y, size)) // Nếu như step tiếp theo nằm ngoài map
		return false;
	if (x == oldX && y == oldY) // Check xem có lặp lại bước cũ
		return false;
	return true;
}

void setNextStep(Position &pos, Stack &stackPos, Block **block, int x, int y, bool &isUpdateStep, int &i, int size) {

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
	int priority=0;
	Position prio;
	if (compareStep(x, y + 1, oldX, oldY, size) && block[x][y + 1].isOk())
	{
		if (block[x][y + 1].isVisited() == 0) {
			pos.setPos(x, y + 1);
			Position newPos;
			newPos.setPos(x, y);
			stackPos.Push(newPos);
			i = i + 1;
			return;
		}
		else
		{
			priority = block[x][y + 1].isVisited();
			prio.setPos(x, y + 1);
		}
	}
	if (compareStep(x, y - 1, oldX, oldY, size) && block[x][y - 1].isOk()) {
		if (block[x][y - 1].isVisited() == 0) {
			pos.setPos(x, y - 1);
			Position newPos;
			newPos.setPos(x, y);
			stackPos.Push(newPos);
			i = i + 1;
			return;
		}
		else if (block[x][y - 1].isVisited() < priority)
		{
			priority = block[x][y - 1].isVisited();
			prio.setPos(x, y - 1);
		}
	}

	if (compareStep(x + 1, y, oldX, oldY, size) && block[x + 1][y].isOk())
	{
		if (block[x + 1][y].isVisited() == 0) {
			pos.setPos(x + 1, y);
			Position newPos;
			newPos.setPos(x, y);
			stackPos.Push(newPos);
			i = i + 1;
			return;
		}
		else if (block[x + 1][y].isVisited() < priority)
		{
			priority = block[x + 1][y].isVisited();
			prio.setPos(x + 1, y);
		}
	}
	if (compareStep(x - 1, y, oldX, oldY, size) && block[x - 1][y].isOk()){
		if (block[x - 1][y].isVisited() == 0) {
			pos.setPos(x - 1, y);
			Position newPos;
			newPos.setPos(x, y);
			stackPos.Push(newPos);
			i = i + 1;
			return;
		}
		else if (block[x-1][y].isVisited() < priority)
		{
			priority = block[x-1][y].isVisited();
			prio.setPos(x-1, y );
		}
	}

	if (priority != 0)
	{
		pos.setPos(prio.getX(), prio.getY());
		Position newPos;
		newPos.setPos(x, y);
		stackPos.Push(newPos);
		i = i + 1;
		return;
	}
	else if (!stackPos.isEmpty()&&(oldX!=x||oldY!=y)) {
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

	int size = readSize("map3.txt");
	string **inputArr = readFile("map3.txt");

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
		stackPos.Push(pos);
		int shot_x = -1;
		int shot_y = -1;
		while ((step + i) != 150 && !isAgentDied) {
			int x = pos.getX();
			int y = pos.getY();

			if (map[x][y] == 6) { // Agent in wumpus or pit position
				isAgentDied = true;
				gold = gold - 10000;
			}

			else if (map[x][y] == 5) {
				isAgentDied = true;
				gold = gold - 10000;
			}

			else {

				if (arrBlock[x][y].isVisited() == 0) {

					if (map[x][y] == 1)
						gold += 100;

					
				}
				arrBlock[x][y].setVisited();
				arrBlock[x][y].setOk();
				predictBlock(arrBlock, x, y, map[x][y], size, gold, shot_x, shot_y);
				while (shot_x != -1 && shot_y != -1) //agent shot in checkblock function
				{
					if (!cheating(x, y, shot_x, shot_y))
					{
						if (map[shot_x][shot_y] == 5)
						{
							cout << "Nice shot! " <<endl;
							arrBlock[shot_x][shot_y].setRealDead();
							map[shot_x][shot_y] = 0;
						}
						else
						{
							cout << "Wumpus is not here" <<endl;
						}
						shot_x = shot_y = -1;
						predictBlock(arrBlock, x, y, map[x][y], size, gold, shot_x, shot_y);
					}
					else
					{
						shot_x = shot_y = -1;
					}

				}
				setNextStep(pos, stackPos, arrBlock, x, y, isUpdateStep, i, size);
				
			
				if (isUpdateStep)
					cout << "At step " << step + 1 << ": (" << pos.getY() + 1 << ", " << pos.getX() + 1 << "), score: " << gold << " and " <<map[pos.getX()][pos.getY()]<< endl;
				else
					break;
				
			}
			step++;
		}
		
		if (!isAgentDied) {
			if (!isUpdateStep)
			{
				cout << "Unable to move" << endl;
			}
			else {

				while (i > 0)
				{
					cout << "At step " << 150 - i + 1 << ": (" << stackPos.getPop().getX() + 1 << ", " << stackPos.getPop().getY() + 1 << "), score: " << gold << endl;
					if (i == 1)
					{
						pos = stackPos.getPop();
					}
					stackPos.Pop();
					i--;

				}
			}
		}
			if (isAgentDied)
				cout << "Agent is gone " << endl;
			else if (map[pos.getX()][pos.getY()] != 7||!isUpdateStep)
				cout << "The agent is trapped and die" << endl;
			else cout << "The agent surpass the level" << endl;
		cout  << "Total score: " << gold << endl;
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