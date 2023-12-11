#include"model.h"
#include<fstream>
#include<string>
#include<sstream>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;

vector<Model*>modelVec;

Model* getModelByName(const char* name)
{
	for (auto& m : modelVec)
	{
		if (m->name == name)
		{
			return m;
		}
	}
}

Model* loadModel(const char* filepath,const char*name)
{
	ifstream infile(filepath);
	auto m = new Model;

	if (infile.is_open() == false)
		throw("loadModel error");

	if (name != NULL)
	{
		m->name = name;
	}

	string nowLineData;

	while (infile) {
		getline(infile, nowLineData);
		if (nowLineData.empty())continue;
		if (nowLineData[0] == '#')continue;
		if (nowLineData == "mtllib")continue;
		if (nowLineData == "usemtl")continue;
		if (nowLineData == "g")continue;
		if (nowLineData == "o")continue;
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
				vector<string>paramList;
				while (nowLineStream) {
					string temp;
					nowLineStream >> temp;
					if (temp == "")break;
					paramList.push_back(temp);
				}


				//解析到的f 对于每个点的索引 
				//paramIndex[0] 代表 v index ,[1] 代表 vt index ,[2]代表 vn index
				int paramIndex[3][4];
				bool hasVtIndex = false;
				bool hasVnIndex = false;


				for (int i = 0; i < paramList.size(); i++)
				{
					string& nowStr = paramList[i];
					istringstream nowPointStr(nowStr);
					int j = 0;
					string tempStr;
					while (getline(nowPointStr,tempStr,'/'))
					{
						if (j == 1)hasVtIndex = true;
						if (j == 2)hasVnIndex = true;
						paramIndex[j][i] = stoi(tempStr);
						j++;
					}
				}

				int pointSum = paramList.size();

				if (pointSum == 3)
				{
					m->vIndexList.push_back({ paramIndex[0][0],paramIndex[0][1],paramIndex[0][2] });
					if (hasVtIndex) {
						m->uvIndexList.push_back({ paramIndex[1][0],paramIndex[1][1],paramIndex[1][2] });
					}
					if (hasVnIndex) {
						m->vnIndexList.push_back({ paramIndex[2][0],paramIndex[2][1],paramIndex[2][2] });
					}
				}
				else if (pointSum == 4)
				{
					m->vIndexList.push_back({ paramIndex[0][0],paramIndex[0][1],paramIndex[0][2] });
					m->vIndexList.push_back({ paramIndex[0][2],paramIndex[0][3],paramIndex[0][0] });
					if (hasVtIndex) {
						m->uvIndexList.push_back({ paramIndex[1][0],paramIndex[1][1],paramIndex[1][2] });
						m->uvIndexList.push_back({ paramIndex[1][2],paramIndex[1][3],paramIndex[1][0] });
					}
					if (hasVnIndex) {
						m->vnIndexList.push_back({ paramIndex[2][0],paramIndex[2][1],paramIndex[2][2] });
						m->vnIndexList.push_back({ paramIndex[2][2],paramIndex[2][3],paramIndex[2][0] });
					}
				}

				//string nowStr;
				//nowLineStream >> nowStr;
				//int IndexData[12];
				//int loadParamCount = 0;

				//if (nowStr.find('/') == std::string::npos)
				//{
				//	IndexData[0] = stoi(nowStr);
				//	nowLineStream >> nowStr;
				//	IndexData[1] = stoi(nowStr);
				//	nowLineStream >> nowStr;
				//	IndexData[2] = stoi(nowStr);
				//	loadParamCount = 3;

				//}
				//else {
				//	while (nowLineStream)
				//	{
				//		istringstream nowStrStream(nowStr);

				//		string tempStr;

				//		// 面 按照 v/vt/vn 这样的排列方式给出
				//		while (getline(nowStrStream, tempStr, '/'))
				//		{
				//			IndexData[loadParamCount] = stoi(tempStr);
				//			loadParamCount++;
				//		}
				//		nowLineStream >> nowStr;
				//	}

				//}



				//if (loadParamCount >= 3) {
				//	m->vIndexList.push_back({ IndexData[0],IndexData[1],IndexData[2] });
				//}
				//if (loadParamCount >= 6) {
				//	m->uvIndexList.push_back({ IndexData[3], IndexData[4], IndexData[5] });
				//}
				//if (loadParamCount == 9) {
				//	m->vnIndexList.push_back({ IndexData[6],IndexData[7],IndexData[8] });
				//}
			}
			else break;
			if (!nowLineStream)break;
		}
	}

	return m;
}



Texture *loadTexture(const char* filepath, bool flipTexture)
{
	Texture *t=new Texture;
	stbi_set_flip_vertically_on_load(flipTexture);

	t->data = stbi_load(filepath, &t->width, &t->height, &t->channel, 0);

	return t;
}

Color Texture::getColor(double u, double v)
{
	//if (v < 0)v = 0;
	//if (u < 0)u = 0;
	//if (v > 1)v = 1;
	//if (u > 1)u = 1;
	//u = 1.0 - u;
	//v = 1.0 - v;
	u = std::max(std::min(u,1.0),0.0);
	v = std::max(std::min(v,1.0),0.0);

	int x = u * (width-1);
	int y = v * (height-1);
	int index = y * width + x;
	return Color(data[index * channel], data[index * channel + 1], data[index * channel + 2]);


}