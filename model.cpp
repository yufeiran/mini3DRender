#include"model.h"
#include<fstream>
#include<string>
#include<sstream>
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
		if (nowLineData.empty())break;
		if (nowLineData[0] == '#')continue;
		istringstream nowLineStream(nowLineData);
		while (nowLineStream) {
			string firstCmd;
			nowLineStream >> firstCmd;

			if (firstCmd == "v") {
				double x, y, z;
				nowLineStream >> x >> y >> z;
				m->pointList.push_back(Vec(x, y, z));

			}
			else if (firstCmd == "f")
			{
				int ia, ib, ic;
				nowLineStream >> ia >> ib >> ic;
				m->triangleList.push_back({ ia,ib,ic });
			}
		}
	}

	return m;
}
