#include"device.h"
#include"model.h"
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
double* ZBuffer;

double oneFrameTime;

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
	ZBuffer = new double[screenHeight * screenWidth];
	for (int i = 0; i < screenHeight; i++) {
		for (int j = 0; j < screenWidth; j++) {
			ZBuffer[i * screenWidth + j] = 1;
		}
	}
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
	static LARGE_INTEGER lastFrameTime = { 0 };
	LARGE_INTEGER nowTime={0};

	if (nFreq.QuadPart==0) {
		QueryPerformanceFrequency(&nFreq);
	}
	if (lastSecondTime.QuadPart == 0) {
		QueryPerformanceCounter(&lastSecondTime);
	}
	if (lastSecondTime.QuadPart == 0) {
		QueryPerformanceCounter(&lastFrameTime);
	}

	QueryPerformanceCounter(&nowTime);


	FrameCount++;
	double time = (double)(nowTime.QuadPart - lastSecondTime.QuadPart)  / (double)nFreq.QuadPart;
	oneFrameTime = (double)(nowTime.QuadPart - lastFrameTime.QuadPart) / (double)nFreq.QuadPart;
	QueryPerformanceCounter(&lastFrameTime);
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


 void DrawPoint(int x, int y,double z, const Color &color)
{
	if (x < 0 || x >= screenWidth)return;
	if (y < 0 || y >= screenHeight)return;
	

	int pos = y * screenWidth + (x + 1) ;
	double screenZ = 1 - ZBuffer[pos - 1];
	if (z > screenZ)
	{
		return;
	}

	buffer[pos*3 - 1] = color.r;
	buffer[pos*3 - 2] = color.g;
	buffer[pos *3- 3] = color.b;
	ZBuffer[pos - 1] = 1-z;
}


void CleanScreen()
{
	//memset(buffer, 0, sizeof(char)* screenHeight*screenWidth*3);
	memset(ZBuffer, 0, sizeof(double) * screenHeight * screenWidth );

	for (int y = 0; y < screenHeight; y++)
	{
		for (int x = 0; x < screenWidth; x++)
		{
			double nowPart = 1-(double)y / (double)screenHeight;
			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 3] =177;
			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 2] = 150;
			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 1] = nowPart*147;
		}
	}
}
