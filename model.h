#pragma once
#ifndef MODEL_H
#define MODEL_H
#include<vector>
#include<string>
#include"unit.h"



struct Texture;
Texture* loadTexture(const char* filepath);
struct Texture
{
	int width, height;
	int channel;
	unsigned char* data;
	unsigned char getData(int x, int y) const {
		return data[y * width + x];
	}
	Texture(const char* filepath)
	{
		auto t = loadTexture(filepath);
		width = t->width;
		height = t->height;
		channel = t->channel;
		data = t->data;
	}
	Texture()
	{
		width = 0;
		height = 0;
		channel = 0;
		data = nullptr;
	}
	Texture(const Texture& t) {
		width = t.width;
		height = t.height;
		channel = t.channel;

		data = new unsigned char[width * height * channel];
	}

	~Texture()
	{
		if (data != nullptr) {
			delete data;
		}
	}
	Color getColor(double u, double v);
};




using UVPair = std::pair<double, double>;


//模型绘制模式 分别是用纹理绘制(带线框)、只用纹理绘制、用纯色填充(也绘制线框)、只绘制线框
enum ModelDrawMode {
	TextureColorWithLine, TextureColor,FillColorWithLine, FillColor,LineColor
};
struct Model
{
	std::vector<Vec>vList;
	std::vector<UVPair>uvList;
	std::vector<Vec>vnList; //顶点法向量
	std::vector<Triangle>vIndexList;
	std::vector<Triangle>uvIndexList;
	std::vector<Triangle>vnIndexList;



	ModelDrawMode drawMode = FillColorWithLine;
	Texture* texture;
	Color filColor{ 88, 178, 220 };
	Color lineColor{ 255, 255, 255 };

	bool isNeedCVVCut = true;
	int id;
	std::string name;
};

Model* loadModel(const char* filepath, const char* name = 0);

Model* getModelByName(const char* name);
extern std::vector<Model*>modelVec;
#endif // !MODEL_H
