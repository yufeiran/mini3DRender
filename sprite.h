#pragma once
#include<vector>
#include"unit.h"
#include"model.h"
struct Sprite
{
	Model* model;
	double rotateXAng=0, rotateYAng=0, rotateZAng=0;
	double scaleX=1, scaleY=1, scaleZ=1;
	Vec moveVec= { 0,0,0 };

	bool isVisable = true;

};


extern std::vector<Sprite>spriteVec;