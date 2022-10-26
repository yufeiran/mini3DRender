#pragma once
#ifndef MODEL_H
#define MODEL_H


#include"unit.h"
#include<vector>


using UVPair = std::pair<double, double>;

struct Model
{
	std::vector<Vec>pointList;
	std::vector<Triangle>triangleList;
	std::vector<UVPair>uvList;
	
};

Model* loadModel(const char* filepath);



#endif // !MODEL_H
