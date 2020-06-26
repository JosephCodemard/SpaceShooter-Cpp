#pragma once
#include <iostream>
#include <list>
#include <thread>
using namespace std;

#include <Windows.h>
#include <thread>

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, 
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class GameEngine {
public:
	GameEngine() {
		//hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleActiveScreenBuffer(hConsole);
		dwBytesWritten = 0;

	}
	int ConstructConsole(int width, int height, int fonth, int fontw, bool cursorFlag, std::wstring appName) {
		nScreenHeight = height;
		nScreenWidth = width;

		AppName = appName;

		//screen = new wchar_t[nScreenWidth * nScreenHeight];
		screen = new CHAR_INFO[nScreenWidth * nScreenHeight];

		// Set Physical Console Window Size
		rectWindow = { 0, 0, (short)nScreenWidth - 1, (short)nScreenHeight - 1 };
		SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

		//// Set the size of the screen buffer
		COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
		SetConsoleScreenBufferSize(hConsole, coord);
		SetConsoleActiveScreenBuffer(hConsole);


		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		SetCurrentConsoleFontEx(hConsole, false, &cfi);

		wcscpy_s(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(hConsole, false, &cfi);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
	
		SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		CONSOLE_CURSOR_INFO     cursorInfo;
		GetConsoleCursorInfo(hConsole, &cursorInfo);
		cursorInfo.bVisible = cursorFlag; // set the cursor visibility
		SetConsoleCursorInfo(hConsole, &cursorInfo);

		memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);


		// Fill Blank
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i].Char.UnicodeChar = PIXEL_SOLID;
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i].Attributes = FG_BLACK;

		return 1;
	}
	void Start()
	{
		active = true;
		std::thread t = std::thread(&GameEngine::GameLoop, this);
		t.join();
	}
	int ScreenHeight() {
		return nScreenHeight;
	}
	int ScreenWidth() {
		return nScreenWidth;
	}
	void DrawTextSprite(int x, int y, std::wstring c[], int clength, short col = FG_WHITE) {
		for (signed int i = 0; i < clength; i++)
		{
			for (size_t j = 0; j < c[i].size(); j++)
			{
				screen[(y + i) * nScreenWidth + x + j].Char.UnicodeChar = c[i][j];
				screen[(y + i)* nScreenWidth + x + j].Attributes = col;
			}
		}
	}
	void DrawString(int x, int y, std::wstring c, short col = FG_WHITE) {
		for (size_t i = 0; i < c.length(); i++)
		{
			screen[y * nScreenWidth + x + i].Char.UnicodeChar = c[i];
			screen[y * nScreenWidth + x + i].Attributes = col;
		}
	}
	virtual void Draw(int x, int y, short c = PIXEL_SOLID, short col = FG_WHITE) {

		if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
		{
			screen[y * nScreenWidth + x].Char.UnicodeChar = c;
			screen[y * nScreenWidth + x].Attributes = col;
		}
	}
	void Fill(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = FG_BLACK)
	{
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}
	void DrawLine(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}
			Draw(x, y, c, col);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}
			Draw(x, y, c, col);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
	}
	void DrawRect(int x, int y, int w, int h, short c = 0x2588, short col = 0x000F) {
		DrawLine(x, y, x + w, y, c, col);
		DrawLine(x, y, x, y + h, c, col);
		DrawLine(x, y + h, x + w, y + h, c, col);
		DrawLine(x + w, y, x + w, y + h, c, col);
	}
	void FillRect(int _x, int _y, int w, int h, short c = PIXEL_SOLID, short col = FG_BLACK) {
		for (signed int x = 0; x < w; x++)
		{
			for (signed int y = 0; y < h; y++)
			{
				Draw(_x + x, _y + y, c, col);
			}
		}
	}
	void CreateButton(int x, int y, int w, int h, short borderC = PIXEL_SOLID, short borderCol = FG_WHITE, short innerCol = L' ', short innerChar = PIXEL_SOLID, std::wstring text = L"Button", short textCol = FG_RED) {
		int tx = x + w / 2 - text.size() / 2;
		int ty = y + h / 2;

		FillRect(x, y, w, h, innerChar, innerCol);
		DrawRect(x,y,w,h,borderC,borderCol);
		DrawString(tx, ty, text, textCol);
	}

	~GameEngine() {}
private:
	void GameLoop() {

		if (!OnUserCreate())
			active = false;

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (active) {
			// Handle Timing
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			float fElapsedTime = elapsedTime.count();

			// Handle Keyboard Input
			for (int i = 0; i < 256; i++)
			{
				keyNewState[i] = GetAsyncKeyState(i);

				keys[i].bPressed = false;
				keys[i].bReleased = false;

				if (keyNewState[i] != keyOldState[i])
				{
					if (keyNewState[i] & 0x8000)
					{
						keys[i].bPressed = !keys[i].bHeld;
						keys[i].bHeld = true;
					}
					else
					{
						keys[i].bReleased = true;
						keys[i].bHeld = false;
					}
				}

				keyOldState[i] = keyNewState[i];
			}

			if (!OnUserUpdate(fElapsedTime))
				active = false;

			wchar_t s[256];
			swprintf_s(s, 256, L"Console Game Engine - %s - FPS: %3.2f", AppName.c_str(), 1.0f / fElapsedTime);
			SetConsoleTitle(s);

			//WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			WriteConsoleOutput(hConsole, screen, { (short)nScreenWidth, (short)nScreenHeight }, {0,0}, &rectWindow);
		}
	}
protected:
	//wchar_t* screen;
	HANDLE hConsole;
	HANDLE hConsoleIn;
	DWORD dwBytesWritten;
	int nScreenHeight; //120
	int nScreenWidth; //30
	SMALL_RECT rectWindow;
	bool active = false;

	CHAR_INFO* screen;
	std::wstring AppName;

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} keys[256];
	short keyOldState[256] = { 0 };
	short keyNewState[256] = { 0 };

public:
	// Virtual Constructors
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;
};