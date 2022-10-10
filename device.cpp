#include"device.h"
#include<stdio.h>
#include<string>
#include<iostream>
using namespace std;

BYTE* buffer;

HDC screen_hdc;
HDC hCompatibleDC;
HBITMAP hCompatibleBitmap;
HBITMAP hOldBitmap;
BITMAPINFO binfo;

HINSTANCE hInstance;
WNDCLASS Draw;
HWND hwnd;
MSG msg;

int screenWidth=800;
int screenHeight=600;
double screenRatio;

void PutBufferToScreen()
{
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, screenHeight, buffer, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, -1, -1, screenWidth, screenHeight, hCompatibleDC, 0, 0, SRCCOPY);
	//SetStretchBltMode(screen_hdc, HALFTONE);
	//StretchBlt(screen_hdc,0, 0, windowWidth, windowHeight, screen_hdc, 0, 0, picWidth, picHeight, SRCCOPY);
}

void initWindow()
{

	screenRatio = static_cast<double>(screenHeight) / static_cast<double>(screenWidth);

	//buffer = (BYTE*)malloc(sizeof(BYTE) * picWidth * picHeight * bits / 8);
	buffer = new BYTE[sizeof(BYTE) * screenWidth * screenHeight * bits / 8];
	hInstance = GetModuleHandle(NULL);

	Draw.cbClsExtra = 0;
	Draw.cbWndExtra = 0;
	Draw.hCursor = LoadCursor(hInstance, IDC_ARROW);
	Draw.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	Draw.lpszMenuName = NULL;
	Draw.style = WS_MINIMIZEBOX | CS_HREDRAW | CS_VREDRAW;
	Draw.hbrBackground = (HBRUSH)COLOR_WINDOW;
	Draw.lpfnWndProc = WindowProc;
	Draw.lpszClassName = L"DDraw";
	Draw.hInstance = hInstance;

	RegisterClass(&Draw);

	hwnd = CreateWindow(
		L"DDraw",
		L"Draw",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		screenWidth + 17,
		screenHeight + 40,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//init bitbuffer
	ZeroMemory(&binfo, sizeof(BITMAPINFO));
	binfo.bmiHeader.biBitCount = bits;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = -screenHeight;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSizeImage = 0;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = screenWidth;

	screen_hdc = GetDC(hwnd);
	hCompatibleDC = CreateCompatibleDC(screen_hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, screenWidth, screenHeight);
	hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap);
}

void updateFrame()
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GameLoop();
}

int CalFPS()
{
	static LARGE_INTEGER nFreq = {0};
	static int FrameCount = 0;
	static int Fps = 0;
	static LARGE_INTEGER lastSecondTime = { 0 };
	LARGE_INTEGER nowTime={0};

	if (nFreq.QuadPart==0) {
		QueryPerformanceFrequency(&nFreq);
	}
	if (lastSecondTime.QuadPart == 0) {
		QueryPerformanceCounter(&lastSecondTime);
	}

	QueryPerformanceCounter(&nowTime);


	FrameCount++;
	double time = (double)(nowTime.QuadPart - lastSecondTime.QuadPart)  / (double)nFreq.QuadPart;
	//cout << time << " ";
	//cout << "nowTime " << nowTime.QuadPart << " lastSecondTime " << lastSecondTime.QuadPart << endl;
	if (time >= 1) {
		//cout << "tick!" << endl;
		Fps = FrameCount;
		FrameCount = 0;
		lastSecondTime = nowTime;
	}
	return Fps;
}

void SetTitle(const char* title)
{
	SetWindowTextA(hwnd, title);
}


 void DrawPoint(int x, int y, const Color &color)
{
	if (x < 0 || x >= screenWidth)return;
	if (y < 0 || y >= screenHeight)return;

	int pos = y * screenWidth * 3 + (x + 1) * 3;

	buffer[pos - 3] = color.r;
	buffer[pos - 2] = color.g;
	buffer[pos - 1] = color.b;
}


void CleanScreen()
{
	memset(buffer, 0, sizeof(char)* screenHeight*screenWidth*3);
	//for (int y = 0; y < screenHeight; y++)
	//{
	//	for (int x = 0; x < screenWidth; x++)
	//	{
	//		buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 3] =0;
	//		buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 2] = 0;
	//		buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 1] = 0;
	//	}
	//}
}

void DrawLine_DDA(double x0, double y0, double x1, double y1, const Color& color)
{
	if (x1 < x0) {
		double tempX, tempY;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			DrawPoint(x0, y, color);
		}
		return;
	}

	double k = (y1 - y0) / (x1 - x0);

	if (k == 0) {
		for (int x = x0; x < x1; x++) {
			DrawPoint(x, y0, color);
		}
	}

	if (k < 1 && k>0) {
		//use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi, color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			DrawPoint(xi, yi, color);
		}

	}
	else if (k >= 1) {
		//use y iter	
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi, color);
		for (int y = y0; y < y1; y++) {
			yi = yi + 1;
			xi = xi + k_;
			DrawPoint(xi, yi, color);
		}
	}
	else if (k < -1) {
		//use y iter
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi, color);
		for (int y = y0; y > y1; y--) {
			yi = yi - 1;
			xi = xi - k_;
			DrawPoint(xi, yi, color);
		}
	}
	else if (k < 0 && k >= -1) {
		// use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi, color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			DrawPoint(xi, yi);
		}
	}
}

void DrawLine_mid(int x0, int y0, int x1, int y1, const Color& color)
{
	if (x1 < x0) {
		double tempX, tempY;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			DrawPoint(x0, y, color);
		}
		return;
	}

	double k =(double) (y1 - y0) /(double) (x1 - x0);

	if (k == 0) {
		for (int x = x0; x < x1; x++) {
			DrawPoint(x, y0, color);
		}
	}

	if (k > 0 && k <= 1)
	{
		int d = 2 * (y0 - y1) + (x1 - x0);

		int x = x0;
		int y = y0;
		while (x != x1)
		{
			DrawPoint(x, y,color);
			if ((x1 - x0) * d < 0) {
				y++;
				d += 2 * (y0 - y1) + 2 * (x1 - x0);
			}
			else if ((x1 - x0) * d > 0) {
				d += 2 * (y0 - y1);
			}
			x++;
		}
	}
	else if (k > 1) {
		int d = 2 * (x0 - x1) + (y1 - y0);

		int y = y0;
		int x = x0;
		while (y != y1)
		{
			DrawPoint(x, y, color);
			if ((y1 - y0) * d < 0) {
				x++;
				d += 2 * (x0 - x1) + 2 * (y1 - y0);
			}
			else if ((y1 - y0) * d > 0) {
				d += 2 * (x0 - x1);
			}
			y++;
		}
	}



}

void DrawLine(double x0, double y0, double x1, double y1,const Color& color)
{
	
	DrawLine_mid(x0, y0, x1, y1, color);
	//DrawLine_DDA(x0, y0, x1, y1, color);
}