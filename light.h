#pragma once
#include<vector>

#include"unit.h"


struct Light
{
	double energy; //��ǿ
	Color color;
	Vec posInWorld; //������ռ��е�λ��
	Vec posInCamera; 
};

extern 	std::vector<Light> lightVec;

