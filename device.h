#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#include<Windows.h>
const int bits = 24;


extern int screenWidth;
extern int screenHeight;
extern double screenRatio;

void initWindow();
void updateFrame();
LRESULT CALLBACK WindowProc(
	_In_	HWND hwnd,
	_In_	UINT uMsg,
	_In_	WPARAM wParam,
	_In_	LPARAM lParam
);

class Color
{
public:
	Color(int R, int G, int B) :r(R), g(G), b(B) {};
	Color() :r(255), g(255), b(255) {};
	int r, g, b;
};


void CleanScreen();

void DrawPoint(int x, int y, Color color = Color(255, 255, 255));

void PutBufferToScreen();

void DrawPoint(int x, int y, const Color color);

void DrawLine(double x0, double y0, double x1, double y1,const Color color=Color(255,255,255));

void CleanScreen();


void GameLoop();

#endif // !DEVICE_H
