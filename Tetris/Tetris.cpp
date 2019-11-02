#include "windows.h"
#include "Game.h"
#include <iostream>	

#define WND_HEIGHT		768
#define WND_WIDTH		1024
#define WND_MARGIN		50

HINSTANCE globalInst;
const char* className = "Tetris111";

Game* game;

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPrev, LPSTR cmdLine, int nShowCmd) {

	UNREFERENCED_PARAMETER(hPrev);
	UNREFERENCED_PARAMETER(cmdLine);
	HWND hWnd;
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.cbSize = sizeof(wc);
	wc.hCursor = LoadCursor(hinst, IDC_ARROW);
	wc.hIcon = LoadIcon(hinst, IDI_APPLICATION);
	wc.hIconSm = nullptr;
	wc.hInstance = hinst;
	wc.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = className;
	wc.lpszMenuName = nullptr;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Error", "Couldnt Register.", MB_OK);
		return 123;
	}


	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, "Tetris", WS_CAPTION | WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
		NULL, NULL, hinst, NULL);

	game = new Game(hWnd, hinst);
	if (!hWnd) {
		MessageBox(NULL, "Error", "Couldnt Create Window.", MB_OK);
		return 124;
	}

	ShowWindow(hWnd, nShowCmd);
	globalInst = hinst;

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			game->Update();
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
		}
	}

	delete game;
	return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		game->setHDC(BeginPaint(hWnd, &ps));
		game->Render();
		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, game->getHDC());
		break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}