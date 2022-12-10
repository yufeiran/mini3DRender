#include"model.h"
#include<fstream>
#include<string>
#include<sstream>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;



Model* loadModel(const char* filepath)
{
	ifstream infile(filepath);
	auto m = new Model;

	if (infile.is_open() == false)
		throw("loadModel error");

	
	string nowLineData;

	while (infile) {
		getline(infile, nowLineData);
		if (nowLineData.empty())continue;
		if (nowLineData[0] == '#')continue;
		istringstream nowLineStream(nowLineData);
		while (nowLineStream) {
			string firstCmd;
			nowLineStream >> firstCmd;

			//顶点
			if (firstCmd == "v") {
				double x, y, z;
				nowLineStream >> x >> y >> z;
				m->vList.push_back(Vec(x, y, z));

			}
			//纹理uv坐标
			else if (firstCmd == "vt") {
				double u, v;
				nowLineStream >> u >> v;
				m->uvList.push_back({ u,v });
			}
			//面
			else if (firstCmd == "f")
			{
				string nowStr;
				nowLineStream >> nowStr;
				int IndexData[9];
				int loadParamCount = 0;

				if (nowStr.find('/') == std::string::npos)
				{
					IndexData[0] = stoi(nowStr);
					nowLineStream >> nowStr;
					IndexData[1] = stoi(nowStr);
					nowLineStream >> nowStr;
					IndexData[2] = stoi(nowStr);
					loadParamCount = 3;

				}
				else {
					while (nowLineStream)
					{
						istringstream nowStrStream(nowStr);

						string tempStr;

						// 面 按照 v/vt/vn 这样的排列方式给出
						while (getline(nowStrStream, tempStr, '/'))
						{
							IndexData[loadParamCount] = stoi(tempStr);
							loadParamCount++;
						}
						nowLineStream >> nowStr;
					}

				}



				if (loadParamCount >= 3) {
					m->vIndexList.push_back({ IndexData[0],IndexData[1],IndexData[2] });
				}
				if (loadParamCount >= 6) {
					m->uvIndexList.push_back({ IndexData[3], IndexData[4], IndexData[5] });
				}
				if (loadParamCount == 9) {
					m->vnIndexList.push_back({ IndexData[6],IndexData[7],IndexData[8] });
				}
			}
			else continue;
			if (!nowLineStream)break;
		}
	}

	return m;
}



Texture *loadTexture(const char* filepath)
{
	Texture *t=new Texture;
	t->data = stbi_load(filepath, &t->width, &t->height, &t->channel, 0);

	return t;
}

Color Texture::getColor(double u, double v)
{
	if (v < 0)v = 0;
	if (u < 0)u = 0;
	if (v > 1)v = 1;
	if (u > 1)u = 1;
	//u = 1.0 - u;
	//v = 1.0 - v;
	int x = u * width, y = v * height;
	int index = y * width + x;
	return Color(data[index * channel], data[index * channel + 1], data[index * channel + 2]);


}