#include"device.h"

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

void DrawPoint(int x, int y, const Color color)
{
	if (x < 0 || x >= screenWidth)return;
	if (y < 0 || y >= screenHeight)return;

	buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 3] = color.r;
	buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 2] = color.g;
	buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 1] = color.b;
}


void CleanScreen()
{
	for (int y = 0; y < screenHeight; y++)
	{
		for (int x = 0; x < screenWidth; x++)
		{

			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 1] = 0;
			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 2] = 0;
			buffer[int(y) * screenWidth * 3 + (int(x) + 1) * 3 - 3] = 0;
		}
	}
}