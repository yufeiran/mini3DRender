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
	memset(buffer, 0, sizeof(char)* screenHeight*screenWidth*3);
	memset(ZBuffer, 0, sizeof(double) * screenHeight * screenWidth );
	//for (int y = 0; y < screenHeight; y++) {
	//	for (int x = 0; x < screenWidth; x++) {
	//		ZBuffer[y * screenWidth + x] = 1;
	//	}
	//}
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

void DrawLine_DDA(double x0, double y0,double z0, double x1, double y1,double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}
	double k = (double)(y0 - y1) / (double)(x0 - x1);
	if (k < 1 && k>0) {
		//use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi, z0,color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(xi, yi, nowZ,color);
		}

	}
	else if (k >= 1) {
		//use y iter	
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi, z0,color);
		for (int y = y0; y < y1; y++) {
			yi = yi + 1;
			xi = xi + k_;
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(xi, yi,nowZ, color);
		}
	}
	else if (k < -1) {
		//use y iter
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi,z0, color);
		for (int y = y0; y > y1; y--) {
			yi = yi - 1;
			xi = xi - k_;
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(xi, yi,nowZ, color);
		}
	}
	else if (k < 0 && k >= -1) {
		// use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi,z0, color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(xi, yi,nowZ,color);
		}
	}
}

void DrawLine_mid(int x0, int y0,double z0, int x1, int y1,double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}

	double k =(double) (y0 - y1) /(double) (x0 - x1);

	// Ax+By+C
	// A=(y1-y0) B=(x0-x1)
	int A = y0 - y1;
	int B = x1 - x0;


	if (k > 0 && k <= 1)
	{
		int d = 2 * A + B;

		int x = x0;
		int y = y0;
		while (x != x1)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y,nowZ,color);
			if (B * d < 0) {
				y++;
				d += 2 * A + 2 * B;
			}
			else if (B * d > 0) {
				d += 2 * A;
			}
			x++;
		}
	}
	else if (k > 1) {
		int d = A+2*B;

		int y = y0;
		int x = x0;
		while (y != y1)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d < 0) {
				d += 2 * B;
			}
			else if (B * d > 0) {
				d += 2 * A+2*B;
				x++;
			}
			y++;
		}
	}
	else if (k >= -1&&k<0) {
		int d = 2 * A - B;

		int x = x0;
		int y = y0;
		while (x != x1)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d >0) {

				y--;
				d += 2*A;
			}
			else if (B * d < 0) {
				d +=2*( A - B);
			}
			x++;
		}
	}
	else if (k < -1) {
		int d = A-2*B;

		int y = y0;
		int x = x0;
		while (y != y1)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d > 0) {
				d += -2*B;
			}
			else if (B * d < 0) {
				x++;
				d +=2*( A - B);
			}
			y--;
		}
	}
}


//浮点运算版本Bresenham算法
void DrawLine_Bresenham_Raw(int x0, int y0,double z0, int x1, int y1,double z1, const Color& color)
{

	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}

	double k = (double)(y1 - y0) / (double)(x1 - x0);

	double d = 0;


	if (k >= 0 && k < 1) {
		int y = y0;
		for (int x = x0; x < x1; x++)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += k;
			if (d > 0.5) {
				y++;
				d -= 1;
			}
		}
	}
	else if (k >= 1) {
		int x = x0;
		double _k = 1.0 / k;
		for (int y = y0; y < y1; y++)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ,color);
			d += _k;
			if (d > 0.5) {
				x++;
				d -= 1;
			}
		}
	}
	else if (k<0 && k>-1) {
		int y = y0;
		double _k = -k;
		for (int x = x0; x < x1; x++)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ,color);
			d += _k;
			if (d > 0.5) {
				y--;
				d -= 1;
			}
		}
	}
	else if (k <= -1) {
		int x = x0;
		double _k = -1.0 / k;
		for (int y = y0; y > y1; y--)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y,nowZ, color);
			d += _k;
			if (d > 0.5) {
				x++;
				d -= 1;
			}
		}
	}
}

void DrawLine_Bresenham(int x0, int y0,double z0, int x1, int y1,double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ,color);
		}
		return;
	}

	//k=0
	if (y1==y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0,nowZ, color);
		}
		return;
	}
	int dx = x1 - x0, dy = y1 - y0;
	bool kSign = ((y1 - y0) >= 0 && (x1 - x0) >= 0) ? true : false; //true => positive false =>negative
	bool kGreaterThenOne = (abs(y1 - y0) > abs(x1 - x0)) ? true : false;

	int d = 0;
	// 0<k<=1
	if (kSign == true && kGreaterThenOne == false) {
		int y = y0;

		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ,color);
			d += 2 * dy;
			if (d > dx) {
				d -= 2 * dx;
				y++;
			}
		}
	}
	//k>1
	else if (kSign == true && kGreaterThenOne == true) {
		int x = x0;

		for (int y = y0; y < y1; y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ,color);
			d += 2 * dx;
			if (d > dy) {
				d -= 2 * dy;
				x++;
			}
		}
	}
	//-1<k<=0
	else if(kSign==false&&kGreaterThenOne==false){
		int y = y0;

		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y,nowZ, color);
			d += -2 * dy;
			if (d > dx) {
				d -= 2 * dx;
				y--;
			}
		}
	}
	//k<-1
	else if (kSign == false && kGreaterThenOne == true) {
		int x = x0;

		for (int y = y0; y > y1; y--) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y,nowZ, color);
			d += 2 * dx;
			if (d > -dy) {
				d -= -2 * dy;
				x++;
			}
		}
	}
	
}



void DrawLine(double x0, double y0,double z0, double x1, double y1,double z1,const Color& color)
{
	DrawLine_Bresenham(x0, y0,z0, x1, y1,z1, color);
	//DrawLine_Bresenham_Raw(x0, y0,z0, x1, y1,z1, color);
	//DrawLine_mid(x0, y0,z0, x1, y1,z1, color);
	//DrawLine_DDA(x0, y0,z0, x1, y1,z1, color);
}

void DrawScanLine(double x0, double z0, double x1, double z1, double y, const Color& color)
{
	if (x0 > x1) {
		swap(x0, x1);
		swap(z0, z1);
	}
	for (int x = x0; x < x1; x++) {
		double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
		DrawPoint(x, y, nowZ, color);
	}
	return;
}

void DrawTopFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3, const Color& color)
{
	double XL = v1.x, XR = v1.x;
	double dX1 = -(v2.x - v1.x) / (v2.y - v1.y), dX2 = -(v3.x - v1.x) / (v3.y - v1.y);
	double ZL = v1.z, ZR = v1.z;
	double dZ1 =- (v2.z - v1.z) / (v2.y - v1.y), dZ2 =- (v3.z - v1.z) / (v3.y - v1.y);
	for (int y = v1.y; y > v2.y; y--)
	{
		DrawScanLine(XL, ZL, XR, ZR, y, color);
		XL += dX1;
		XR += dX2;
		ZL += dZ1;
		ZR += dZ2; 
	}
}

void DrawBottomFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3, const Color& color)
{


	double XL = v1.x, XR = v1.x;
	double dX1 = (v2.x - v1.x) / (v2.y - v1.y), dX2 = (v3.x - v1.x) / (v3.y - v1.y);
	double ZL = v1.z, ZR = v1.z;
	double dZ1 = (v2.z - v1.z) / (v2.y - v1.y), dZ2 = (v3.z - v1.z) / (v3.y - v1.y);
	for (int y = v1.y; y < v2.y; y++)
	{
		DrawScanLine(XL, ZL, XR, ZR, y, color);
		XL += dX1;
		XR += dX2;
		ZL += dZ1;
		ZR += dZ2;
	}
}

void DrawTriangle(const Vec& v1, const Vec& v2, const Vec& v3, const Color& color)
{
	//按y值从小到大排序3个顶点
	Vec V[3] = { v1,v2,v3 };
	int maxIndex=0, minIndex = 0;
	Vec maxV = v1, minV = v1,midV=v1;
	for (int i = 0; i < 3; i++)
	{
		if (V[i].y < minV.y)
		{
			minIndex = i;
			minV = V[i];
		}
		if (V[i].y > maxV.y)
		{
			maxIndex = i;
			maxV = V[i];
		}
	}
	int midIndex;
	for (int i = 0; i < 3; i++) {
		if (i == maxIndex || i == minIndex) {
			continue;
		}
		else {
			midIndex = i;
			midV = V[i];
			
		}
	}
	Vec V1 = minV, V2 = midV, V3 = maxV;

	if (V1.y == V2.y) {
		 DrawTopFlatTriangle(V3, V1, V2, color);
	}
	else if (V2.y == V3.y) {
		DrawBottomFlatTriangle(V1, V3, V2, color);
	}
	else {
		Vec V4;
		V4.y = V2.y;
		V4.x = V1.x + (V4.y - V1.y) / (V3.y - V1.y) * (V3.x - V1.x);
		V4.z = V1.z + (V4.y - V1.y) / (V3.y - V1.y) * (V3.z - V1.z);

		 DrawBottomFlatTriangle(V1, V2, V4, color);
		 DrawTopFlatTriangle(V3, V2, V4, color);

	}

}