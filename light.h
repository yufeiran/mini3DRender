#pragma once
#include<vector>

#include"unit.h"


struct Light
{
	double energy; //光强
	Color color;
	Vec posInWorld; //在世界空间中的位置
	Vec posInCamera; 
};

extern 	std::vector<Light> lightVec;

