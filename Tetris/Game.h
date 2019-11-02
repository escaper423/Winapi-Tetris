#pragma once
#include <Windows.h>
#include "Block.h"
#include <ctime>
#include <time.h>
#include <string>

#define BLOCK_SIZE		32 // Block size : 32x32 is ideal
#define BLOCK_PADDING	2
#define BOARD_WIDTH		10
#define BOARD_HEIGHT	20
#define BOARD_MARGIN	50

#define WND_HEIGHT		768
#define WND_WIDTH		1024

enum Colors {
	RED = 1,
	BLUE,
	GREEN,
	YELLOW,
	CYAN,
	PURPLE,
	BROWN
};

enum GameStatus {
	MENU,
	INGAME
};

class Game {
public:
	Game() {}
	Game(HWND hw, HINSTANCE hi) : hWnd(hw), hInst(hi) {
		rc = { 0 };
		GetClientRect(hWnd, &rc);
	}
	void DrawBackground();
	void DrawBlock();
	void Update();
	void Render();
	void InitGame();
	void setHDC(HDC h) { hdc = h; }
	HDC getHDC() { return hdc; }
	bool CollisionCheck();
	void RotateBlock();
	void MoveBlock(int dx);
	void MoveDown();
	void GameOver();
	void LineCheck();
	void DrawStartPage();
	void MakeBlock();
	HBRUSH GetBlockColorRGB(int clr);
private:
	int boardLeft;
	int boardRight;
	int boardTop;
	int boardBottom;

	int nextLeft;
	int nextRight;
	int nextTop;
	int nextBottom;

	int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };
	
	HINSTANCE hInst;
	HWND hWnd;
	HFONT hFont, oldFont;
	HDC hdc, hdc2;
	HBITMAP newBitmap, oldBitmap;

	HBRUSH curColor, nextColor, bgColor, oldColor;
	RECT rc;

	enum { Up = 1, Left, Right, Down, Menu, Ingame };
	int combo = 0;
	int score = 0;
	int keyPressed;
	int delayTime = 400;
	int elapsedTime = 0;
	int moveDelay = 100, rotateDelay = 125;
	int curMoveDelay, curRotateDelay;
	int moveElapsed = 0, rotateElapsed = 0;
	int gameStatus = GameStatus::MENU;
	int blockShapes[7][4] =
	{
		{1,3,5,7},
		{2,4,5,7},
		{3,5,4,6},
		{3,5,4,7},
		{2,3,5,7},
		{3,5,7,6},
		{2,3,4,5}
	};

	int colorTable[7][4] = {
		{255,25,25},
		{150,75,0},
		{25,25,255},
		{0,191,230},
		{255,255,51},
		{25,255,25},
		{128,0,128}
	};

	Block curBlock;
	Point curPosition;

	int blockType;
	int nextBlockType;

};

