#pragma once
#ifndef MODEL_H
#define MODEL_H


#include"unit.h"
#include<vector>

struct Model
{
	std::vector<Vec>pointList;
	std::vector<Triangle>triangleList;
};

Model* loadModel(const char* filepath);



#endif // !MODEL_H
