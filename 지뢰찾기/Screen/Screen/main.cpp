// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "Utils.h"
#include <time.h>

using namespace std;

class Screen {
	int width;
	int height;
	char* canvas;

	static Screen* instance;
	Screen(int width = 30, int height = 15)
		: width(width), height(height),
		canvas(new char[(width + 1)*height])

	{
		Borland::initialize();
	}
public:
	static Screen& getInstance() {
		if (instance == nullptr) {
			instance = new Screen();
		}
		return *instance;
	}

	~Screen() {
		if (instance) {
			delete[] canvas;
			instance = nullptr;
		}
	}

	void inGameDraw(const char* shape, int w, int h, const Position& pos)
	{
		if (!shape) return;
		strncpy(&canvas[pos.x + (pos.y)*(width + 1)], &shape[pos.x + (pos.y)*(width + 1)], 1);
	}

	void FinishDraw(const char* shape)
	{
		if (!shape) return;
		strcpy(canvas, shape);
	}

	void render()
	{
		for (int i = 0; i < height; i++)
			canvas[width + i * (width + 1)] = '\n';
		canvas[width + (height - 1) * (width + 1)] = '\0';
		Borland::gotoxy(0, 0);
		cout << canvas;
	}

	void clear()
	{

		memset(canvas, 'X', (width + 1)*height);
		canvas[width + (height - 1)*(width + 1)] = '\0';
	}

	int getWidth()
	{
		return width;
	}

	int getHeight()
	{
		return height;
	}
};

Screen* Screen::instance = nullptr;

class Mine {

	static Mine* instance;

	int* mine;
	int mineSize;

	int width;
	Screen& screen;

	Mine()
		: screen(Screen::getInstance())
	{
		mineSize = screen.getHeight()*screen.getWidth() / 10;
		mine = new int[mineSize];

		width = screen.getWidth();

		srand((unsigned int)time(NULL));

		for (int i = 0; i < mineSize; i++)
		{
			mine[i] = rand() % (screen.getHeight() * (screen.getWidth() + 1));
			if (width <= mine[i] && ((mine[i] + 1) % (width + 1)) == 0)
			{
				i--;
				continue;
			}
			for (int j = 0; j < i; j++)
			{

				if (mine[i] < 0 || mine[i] == mine[j])
				{
					mine[i] = rand() % (screen.getHeight() * (screen.getWidth() + 1));
					j = -1;
				}
			}
		}
	}
public:
	static Mine& getInstance() {
		if (instance == nullptr) {
			instance = new Mine();
		}
		return *instance;
	}
	const int getMine(int num) {
		return mine[num];
	}

	const int getMineSize() {
		return mineSize;
	}
	~Mine() {
		if (instance) {
			delete[] mine;
			instance = nullptr;
		}
	}


};

Mine* Mine::instance = nullptr;

class GameObject {
	char* shape;
	int width;
	int height;
	Position pos;
	int direction;
	Screen& screen;
	Mine& mine;

	bool *dominoCache;

	bool life;

	void shapeSetting() {

		for (int i = 0; i < strlen(shape); i++)
		{
			if (shape[i] == '*') continue;
			if ((i + 1) % (width + 1) == 0 && i != 0) { shape[i] = '\n'; continue; }

			int mineCount = 0;

			if (i < width)
			{
				if ((shape[i - 1]) == '*' && i > 0) mineCount++;
				if ((shape[i + 1]) == '*' && i < width - 1) mineCount++;
				if ((shape[i + (width + 1) - 1]) == '*' && i > 0) mineCount++;
				if (shape[i + (width + 1)] == '*') mineCount++;
				if ((shape[i + (width + 1) + 1]) == '*' && i < width - 1) mineCount++;
			}
			else
			{
				if ((shape[i - (width + 1) - 1]) == '*' && (i % (width + 1)) != 0) mineCount++;
				if ((shape[i - (width + 1)]) == '*') mineCount++;
				if ((shape[i - (width + 1) + 1]) == '*' && ((i - width + 1) % (width + 1)) != 0) mineCount++;
				if ((shape[i - 1]) == '*' && (i % (width + 1)) != 0) mineCount++;
				if ((shape[i + 1]) == '*' && ((i - width + 1) % (width + 1)) != 0) mineCount++;

				if (i < (width + 1)*height - (width + 1))
				{
					if ((shape[i + (width + 1) - 1]) == '*' && (i % (width + 1)) != 0) mineCount++;
					if (shape[i + (width + 1)] == '*') mineCount++;
					if ((shape[i + (width + 1) + 1]) == '*' && ((i - width + 1) % (width + 1)) != 0) mineCount++;
				}
			}
			shape[i] = mineCount + '0';
		}
	}

	void domino(Position pos) {

		Position m_pos = pos;

		if (dominoCache == nullptr)
		{
			dominoCache = new bool[(width + 1)*height];

			for (int i = 0; i < (width + 1)*height; i++)
			{
				dominoCache[i] = false;
			}
		}

		if (dominoCache && dominoCache[pos.x + pos.y*(width + 1)])
		{
			inGameDraw(pos);
			return;
		}

		dominoCache[pos.x + pos.y*(width + 1)] = true;

		if (((pos.x + pos.y*(width + 1) + 1) % (width + 1) == 0 && pos.x != 0) || pos.y < 0 || pos.y > height) return;
		else if (shape[pos.x + pos.y*(width + 1)] != '0')
		{
			inGameDraw(pos);
			return;
		}
		else
		{
			m_pos.x -= 1; m_pos.y -= 1;
			domino(m_pos);
			m_pos.x += 1;
			domino(m_pos);
			m_pos.x += 1;
			domino(m_pos);
			m_pos.y += 1;
			domino(m_pos);
			m_pos.y += 1;
			domino(m_pos);
			m_pos.x -= 1;
			domino(m_pos);
			m_pos.x -= 1;
			domino(m_pos);
			m_pos.y -= 1;
			domino(m_pos);
		}

		inGameDraw(pos);


	}
public:
	GameObject()
		: screen(Screen::getInstance()), mine(Mine::getInstance()), life(true), dominoCache(nullptr)
	{
		this->width = screen.getWidth();
		this->height = screen.getHeight();

		this->shape = new char[(width + 1)*height];
		for (int i = 0; i < mine.getMineSize(); i++)
		{
			this->shape[mine.getMine(i)] = '*';
		}

		shapeSetting();

	}
	virtual ~GameObject() {
		if (shape) { delete[] shape; }
		width = 0, height = 0;

		if (dominoCache)
		{
			delete[] dominoCache;
			dominoCache = nullptr;
		}
	}



	void setPos(int x, int y) { this->pos.x = x; this->pos.y = y; }

	void inGameDraw(const Position pos) {
		screen.inGameDraw(shape, width, height, pos);
	}

	void FinishDraw() {
		screen.FinishDraw(shape);
	}

	virtual void update(const Position& pos) {
		setPos(pos.x, pos.y);
		inGameDraw(pos);

		domino(pos);

		if (shape[pos.x + pos.y*(width + 1)] == '*')
		{
			Borland::gotoxy(75, 10);
			printf("!!!!!!!!! ＠o＠ !!!!!!!!!");
			Sleep(2000);

			life = false;
		}


	}

	void information() {
		Borland::gotoxy(75, 5);
		printf("지뢰밭 크기 : %d * %d", screen.getWidth(), screen.getHeight());
		Borland::gotoxy(75, 6);
		printf("지뢰 개수   : %d", mine.getMineSize());
		Borland::gotoxy(0, 0);
	}

	bool lifeCheck() {
		return life;
	}
};

int main()
{
	GameObject mineGame;

	Screen&	 screen = Screen::getInstance();
	INPUT_RECORD InputRecord;
	DWORD Events;

	bool mindCheck = false;

	mineGame.information();
	screen.clear(); screen.render();
	while (true)
	{
		screen.render();
		Sleep(30);
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &InputRecord, 1, &Events);
		if (InputRecord.EventType == MOUSE_EVENT) {
			if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
				COORD coord;

				coord.X = InputRecord.Event.MouseEvent.dwMousePosition.X;
				coord.Y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
				Position pos;
				pos.x = InputRecord.Event.MouseEvent.dwMousePosition.X;
				pos.y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
				if (screen.getWidth() > pos.x)
				{
					mineGame.update(pos);

					if (!mineGame.lifeCheck())
						break;
				}
			}
		}

	}

	Borland::gotoxy(0, 0);
	mineGame.FinishDraw();
	screen.render();

	Borland::gotoxy(75, 11);
	printf("GAME OVER\n");
	Sleep(10000);

	return 0;
}