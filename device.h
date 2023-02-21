#pragma once
#ifndef DEVICE_H
#define DEVICE_H
#include"unit.h"
#include"model.h"
#include<Windows.h>
const int bits = 24;



extern int screenWidth;
extern int screenHeight;
extern double screenRatio;
extern double oneFrameTime;

void initWindow();
void updateFrame();
LRESULT CALLBACK WindowProc(
	_In_	HWND hwnd,
	_In_	UINT uMsg,
	_In_	WPARAM wParam,
	_In_	LPARAM lParam
);


void CleanScreen();

void PutBufferToScreen();

void DrawPoint(int x, int y,double z, const Color& color = Color(255, 255, 255));
double getZBufferByPos(int x, int y);


void CleanScreen();


void GameLoopOnce();


int CalFPS();

void SetTitle(const char* title);

#endif // !DEVICE_H
