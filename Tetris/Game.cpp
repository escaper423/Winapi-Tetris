#include "Game.h"
void Game::InitGame() {
	//Create Random Seed
	srand((unsigned int)time(NULL));

	blockType = (rand() % 7);
	nextBlockType = (rand() % 7);

	//Create Basic Block Datas
	std::vector<Point> tmp;
	tmp.resize(4);

	for (int i = 0; i < 4; i++) {
		tmp[i].x = (blockShapes[blockType][i] % 2) + 3;
		tmp[i].y = blockShapes[blockType][i] >> 1;
	}
	curBlock.setBlock(tmp);
	curBlock.setColor(blockType + 1);

	for (int y = 0; y < BOARD_HEIGHT; y++)
		for (int x = 0; x < BOARD_WIDTH; x++)
			board[y][x] = 0;

	curMoveDelay = GetTickCount();
	curRotateDelay = GetTickCount();
	elapsedTime = GetTickCount();

	score = 0;
	combo = 0;
}
void Game::RotateBlock() {
	std::vector<Point> block = curBlock.getBlock();
	std::vector<Point> backup = block;
	Point pivot = block[1];

	if (blockType != 6) {
		for (int i = 0; i < 4; i++) {
			int dx = block[i].y - pivot.y;
			int dy = block[i].x - pivot.x;
			block[i].x = pivot.x - dx;
			block[i].y = pivot.y + dy;
		}
		curBlock.setBlock(block);

		if (!CollisionCheck())
			curBlock.setBlock(backup);
	}
}

bool Game::CollisionCheck() {
	std::vector<Point> next = curBlock.getBlock();
	for (int i = 0; i < 4; i++) {
		if (next[i].x < 0 || next[i].x >= BOARD_WIDTH || next[i].y >= BOARD_HEIGHT)
			return 0;
		else if (board[next[i].y][next[i].x])
			return 0;
	}
	return 1;
}

void Game::GameOver() {
	MessageBox(NULL, "Game Over.", "Tetris", MB_OK);
	InitGame();
	gameStatus = GameStatus::MENU;
}

void Game::MakeBlock() {

	blockType = nextBlockType;
	nextBlockType = rand() % 7;

	std::vector<Point> tmp(4);
	for (int i = 0; i < 4; i++) {
		tmp[i].x = (blockShapes[blockType][i] % 2) + 3;
		tmp[i].y = blockShapes[blockType][i] >> 1;
	}

	curBlock.setBlock(tmp);
	curBlock.setColor(blockType + 1);

	if (!CollisionCheck())
		GameOver();
}
void Game::MoveDown() {
	std::vector<Point> next = curBlock.getBlock();
	std::vector<Point> backup = next;

	for (int i = 0; i < 4; i++)
		next[i].y += 1;

	curBlock.setBlock(next);

	if (!CollisionCheck()) {
		for (int i = 0; i < 4; i++)
			board[backup[i].y][backup[i].x] = curBlock.getColor();
		LineCheck();
		MakeBlock();
	}
}

void Game::MoveBlock(int dx) {
	std::vector<Point> next = curBlock.getBlock();
	std::vector<Point> backup = next;

	//Update block states
	for (int i = 0; i < 4; i++)
		next[i].x += dx;
	curBlock.setBlock(next);

	//Back to previous state if collision detected
	if (!CollisionCheck())
		curBlock.setBlock(backup);

}

void Game::DrawBackground() {
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP nBitmap = CreateCompatibleBitmap(hdc, WND_WIDTH, WND_HEIGHT);
	oldBitmap = (HBITMAP)SelectObject(memDC, nBitmap);

	HBRUSH hbr;
	RECT boardRECT, nextRECT;

	//Text option
	SetTextAlign(memDC, TA_CENTER);
	SetTextColor(memDC, RGB(80, 50, 30));
	SetBkMode(memDC, TRANSPARENT);

	//Background Color
	bgColor = CreateSolidBrush(RGB(38, 177, 90));
	oldColor = (HBRUSH)SelectObject(memDC, bgColor);
	FillRect(memDC, &rc, bgColor);
	DeleteObject(bgColor);


	//Game Board  
	boardLeft = rc.left + BOARD_MARGIN;
	boardTop = rc.top + BOARD_MARGIN;
	boardRight = rc.left + (BLOCK_SIZE * BOARD_WIDTH) + BOARD_MARGIN;
	boardBottom = boardTop + (BOARD_HEIGHT * BLOCK_SIZE);
	boardRECT = { boardLeft, boardTop, boardRight, boardBottom };

	hbr = CreateSolidBrush(RGB(0, 0, 0));
	oldColor = (HBRUSH)SelectObject(memDC, hbr);
	FillRect(memDC, &boardRECT, hbr);

	//Score Block
	oldFont = (HFONT)SelectObject(memDC, hFont);
	int scoreLeftTop = boardRight + 120;
	int scoreY = 20;
	SelectObject(memDC, hFont);

	std::string scoreStr = std::to_string(score);
	TextOut(memDC, scoreLeftTop, scoreY, "Score", 5);
	TextOut(memDC, scoreLeftTop, scoreY + 40, scoreStr.c_str(), scoreStr.length());

	//Next Block
	nextLeft = boardRight + BOARD_MARGIN;
	nextTop = boardBottom - 128;
	nextRight = nextLeft + 128;
	nextBottom = boardBottom;
	TextOut(memDC, scoreLeftTop, nextTop - 50, "Next", 4);

	nextRECT = { nextLeft, nextTop, nextRight, nextBottom };
	FillRect(memDC, &nextRECT, hbr);
	BitBlt(hdc2, 0, 0, WND_WIDTH, WND_HEIGHT, memDC, 0, 0, SRCPAINT);


	SelectObject(memDC, oldColor);
	DeleteObject(hbr);
	SelectObject(memDC, oldFont);
	DeleteObject(hFont);
	SelectObject(memDC, oldBitmap);
	DeleteObject(nBitmap);

	DeleteDC(memDC);
}

HBRUSH Game::GetBlockColorRGB(int clr) {
	int r = colorTable[clr - 1][0];
	int g = colorTable[clr - 1][1];
	int b = colorTable[clr - 1][2];
	HBRUSH hbr = CreateSolidBrush(RGB(r, g, b));
	return hbr;
}

void Game::LineCheck() {
	int tmp = BOARD_HEIGHT - 1;
	bool isFilled = true;
	for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			if (!board[y][x]) {
				isFilled = false;
			}
			board[tmp][x] = board[y][x];
		}
		if (!isFilled) {
			tmp--;
			if (combo) {
				score += (combo * combo) * 10;
				combo = 0;
			}
		}
		else {
			combo++;
		}
		isFilled = true;
	}
}
void Game::DrawBlock() {
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP nBitmap = CreateCompatibleBitmap(hdc, WND_WIDTH, WND_HEIGHT);
	oldBitmap = (HBITMAP)SelectObject(memDC, nBitmap);

	curColor = GetBlockColorRGB(curBlock.getColor());
	nextColor = GetBlockColorRGB(nextBlockType + 1);

	RECT rect;
	//Prnit Field Blocks
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			if (board[y][x]) {
				HBRUSH boardColor = GetBlockColorRGB(board[y][x]);
				rect = { boardLeft + (x * BLOCK_SIZE) + BLOCK_PADDING,
						boardTop + (y * BLOCK_SIZE) + BLOCK_PADDING,
						boardLeft + ((x + 1) * BLOCK_SIZE) - BLOCK_PADDING,
						boardTop + ((y + 1) * BLOCK_SIZE) - BLOCK_PADDING };
				FillRect(memDC, &rect, boardColor);
				DeleteObject(boardColor);
			}
		}
	}
	//Print Current Block
	std::vector<Point> cur = curBlock.getBlock();
	for (int i = 0; i < 4; i++) {
		rect = { boardLeft + (cur[i].x * BLOCK_SIZE) + BLOCK_PADDING,
				boardTop + (cur[i].y * BLOCK_SIZE) + BLOCK_PADDING,
				boardLeft + ((cur[i].x + 1) * BLOCK_SIZE) - BLOCK_PADDING,
				boardTop + ((cur[i].y + 1) * BLOCK_SIZE) - BLOCK_PADDING };
		FillRect(memDC, &rect, curColor);
	}
	SelectObject(memDC, oldColor);
	DeleteObject(curColor);
	//Print Next Block
	int nextCenterX = nextLeft + ((nextRight - nextLeft) >> 2);
	int nextCenterY = nextTop + ((nextBottom - nextTop) >> 8);

	for (int i = 0; i < 4; i++) {
		rect = { nextCenterX + ((blockShapes[nextBlockType][i] % 2) * BLOCK_SIZE) + BLOCK_PADDING,
				nextCenterY + ((blockShapes[nextBlockType][i] >> 1) * BLOCK_SIZE) + BLOCK_PADDING,
				nextCenterX + (((blockShapes[nextBlockType][i] % 2) + 1) * BLOCK_SIZE) - BLOCK_PADDING,
				nextCenterY + (((blockShapes[nextBlockType][i] >> 1) + 1) * BLOCK_SIZE) - BLOCK_PADDING };
		FillRect(memDC, &rect, nextColor);
	}

	BitBlt(hdc2, 0, 0, WND_WIDTH, WND_HEIGHT, memDC, 0, 0, SRCPAINT);

	SelectObject(memDC, oldBitmap);
	DeleteObject(nBitmap);

	SelectObject(memDC, oldColor);
	DeleteObject(nextColor);

	DeleteDC(memDC);
}

void Game::DrawStartPage() {
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP nBitmap = CreateCompatibleBitmap(hdc, WND_WIDTH, WND_HEIGHT);
	oldBitmap = (HBITMAP)SelectObject(memDC, nBitmap);

	bgColor = CreateSolidBrush(RGB(138, 77, 150));

	//Text option
	SetTextAlign(memDC, TA_CENTER);
	SetTextColor(memDC, RGB(80, 50, 30));
	SetBkMode(memDC, TRANSPARENT);

	//Background Color
	FillRect(memDC, &rc, bgColor);

	oldFont = (HFONT)SelectObject(memDC, hFont);
	TextOut(memDC, WND_WIDTH >> 1, WND_HEIGHT >> 2, "Tetris v1.0", strlen("Tetris v1.0"));
	TextOut(memDC, WND_WIDTH >> 1, (WND_HEIGHT >> 2) + 50, "Press Space to Start.", strlen("Press Space to Start."));

	BitBlt(hdc2, 0, 0, WND_WIDTH, WND_HEIGHT, memDC, 0, 0, SRCPAINT);

	SelectObject(memDC, oldFont);
	DeleteObject(hFont);
	SelectObject(memDC, oldBitmap);
	DeleteObject(nBitmap);
	SelectObject(memDC, oldColor);
	DeleteObject(bgColor);
	DeleteDC(memDC);

}
void Game::Render() {
	hdc2 = CreateCompatibleDC(hdc);
	newBitmap = CreateCompatibleBitmap(hdc, WND_WIDTH, WND_HEIGHT);
	oldBitmap = (HBITMAP)SelectObject(hdc2, newBitmap);

	hFont = CreateFont(40, 18, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));

	switch (gameStatus) {
	case GameStatus::INGAME:
		DrawBackground();
		DrawBlock();
		break;
	case GameStatus::MENU:
		DrawStartPage();
		break;
	}

	BitBlt(hdc, 0, 0, WND_WIDTH, WND_HEIGHT, hdc2, 0, 0, SRCCOPY);
	SelectObject(hdc2, oldFont);
	DeleteObject(hFont);

	SelectObject(hdc2, oldBitmap);
	DeleteObject(newBitmap);
	DeleteDC(hdc2);
}


void Game::Update() {
	//Processing Input

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		keyPressed = Left;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		keyPressed = Right;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
		keyPressed = Up;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		keyPressed = Down;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		keyPressed = Menu;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		keyPressed = Ingame;


	int curMoveTime = GetTickCount();
	int curRotateTime = GetTickCount();

	switch (keyPressed) {
	case Left:
		//Move Left
		if (gameStatus == GameStatus::INGAME && curMoveTime - moveElapsed > moveDelay) {
			moveElapsed = curMoveTime;
			MoveBlock(-1);
		}
		break;
	case Right:
		//Move Right
		if (gameStatus == GameStatus::INGAME && curMoveTime - moveDelay > moveElapsed) {
			moveElapsed = curMoveTime;
			MoveBlock(1);
		}
		break;
	case Up:
		//Rotate
		if (gameStatus == GameStatus::INGAME && curRotateTime - rotateElapsed > rotateDelay) {
			rotateElapsed = curRotateTime;
			RotateBlock();
		}
		break;
	case Down:
		//Move Down
		if (gameStatus == GameStatus::INGAME)
			MoveDown();
		break;
	case Menu:
		if (gameStatus == GameStatus::INGAME)
			gameStatus = GameStatus::MENU;
		break;
	case Ingame:
		if (gameStatus == GameStatus::MENU) {
			gameStatus = GameStatus::INGAME;
			InitGame();
		}
		break;
	}
	int curElapsedTime = GetTickCount();
	if (curElapsedTime - elapsedTime > delayTime && gameStatus == GameStatus::INGAME)
	{
		elapsedTime = curElapsedTime;
		MoveDown();
	}
	keyPressed = 0;
}