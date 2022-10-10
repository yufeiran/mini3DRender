
#include"mini3DRender.h"

#include<vector>

using namespace std;

bool debugMode = false;
//=======================================================

//======================================================

Vec eye(5, 5, 0);
Vec lookat(0, 5, 0);
Vec up(0, 1, 0);

Vec Forward = lookat - eye;
Vec side;

double viewAng = 45;
double aspect = 4.0 / 3.0;
double n = 1;
double f = 30;

double X0 = 0;
double Y0 = 0;

double width = 800;
double height = 600;

double Zmin = 0;
double Zmax = 1;

Model* model;
vector<Vec>ansList;
vector<pair<int, int>>lineList;


double rotateXAng = 0;
double rotateYAng = 0;
double rotateZAng = 0;

double dAng = 1;
double dMoveSize = 0.05;

Mat makeWorldToCameraMat(const Vec& Forward, const Vec& side, const Vec& up,const Vec&eye)
{
	//这里直接取“从世界到相机空间变换矩阵”的逆矩阵，推导请看https://yufeiran.com/cao-gao-zhi-shang-tui/ 2.3.2节
	Mat mMoveR = makeMoveMat(-eye.x, -eye.y, -eye.z);
	Mat mRotateR;
	mRotateR.m[3][3] = 1;
	for (int i = 0; i < 3; i++)
	{
		mRotateR.m[0][i] = side[i];
		mRotateR.m[1][i] = up[i];
		mRotateR.m[2][i] = -Forward[i];
	}
	return mRotateR * mMoveR;
}

Mat makeCameraToClipMat(const double viewAng, const double aspect, double n, double f)
{
	Mat m;
	double radViewAng = degToRad(viewAng);
	double cotViewAng = 1.0 / tan(radViewAng);
	m.m[0][0] = cotViewAng / aspect;
	m.m[1][1] = cotViewAng;
	m.m[2][2] = f / (f - n);
	m.m[2][3] = 1;
	m.m[3][2] = f * n / (n - f);
	return m;
}


Mat makeClipToScreenMat(const double x0, const double y0, const double width, const double height, const double Zmin, const double Zmax)
{
	Mat m;
	m.m[0][0] = 0.5*width;
	m.m[1][1] = -0.5*height;
	m.m[2][2] = Zmax - Zmin;
	m.m[3][3] = 1;
	
	m.m[0][3] = x0 + 0.5 * width;
	m.m[1][3] = y0 + 0.5 * height;
	m.m[2][3] = Zmin;
	return m;
}


Vec transform(const Vec& objVec)
{
	if(debugMode)
		cout << "raw :" << objVec <<endl<< " - > ";
	// object to world
	Mat mObjectToWorld;
	mObjectToWorld = makeMoveMat(0, 4, 0)*makeRotateByYMat(rotateYAng)* makeRotateByXMat(rotateXAng)*makeRotateByZMat(rotateZAng);
	Vec worldVec = mObjectToWorld * objVec;

	if (debugMode)
		cout << "world :" << worldVec << endl << " - > ";
	// world to Camera

	Forward = lookat - eye;
	Forward = Forward.normal();

	up = up.normal();
	
	side = cross(Forward, up);

	up = cross(side, Forward);

	Mat mWorldToObject = makeWorldToCameraMat(Forward, side, up, eye);

	Vec CameraVec = mWorldToObject * worldVec;

	if (debugMode)
		cout << "camera :" << CameraVec << endl << " - > ";

	// Camera to Clip 
	Mat mCameraToClip = makeCameraToClipMat(viewAng, aspect, n, f);
	Vec ClipVec = mCameraToClip * CameraVec;

	if (debugMode)
		cout << "clip :" << ClipVec << endl << " - > ";

	ClipVec.z = -ClipVec.z;
	ClipVec.x /= ClipVec.w;
	ClipVec.y /= ClipVec.w;
	ClipVec.z /= ClipVec.w;
	ClipVec.w = 1;

	if (debugMode)
		cout << "new clip :" << ClipVec << endl << " - > ";
	// Clip to Screen
	Mat mClipToScreen = makeClipToScreenMat(X0, Y0, width, height, Zmin, Zmax);
	Vec ScreenVec = mClipToScreen * ClipVec;

	if (debugMode)
		cout << "screen :" << ScreenVec << endl;

	return ScreenVec;

}

Vec Cube[8];

void GameLoop()
{
	CleanScreen();

	DrawLine(100, 100, 200, 100);
	//for (const auto& tri : model->triangleList)
	//{
	//	const auto& p0 = model->pointList[tri.pointIndex[0]-1];
	//	const auto& p1 = model->pointList[tri.pointIndex[1]-1];
	//	const auto& p2 = model->pointList[tri.pointIndex[2]-1];
	//	Vec P0 = transform(p0);
	//	Vec P1 = transform(p1);
	//	Vec P2 = transform(p2);
	//	DrawLine(P0.x, P0.y, P1.x, P1.y);
	//	DrawLine(P1.x, P1.y, P2.x, P2.y);
	//	DrawLine(P2.x, P2.y, P0.x, P0.y);
	//}
	//

	PutBufferToScreen();

	int nowFps= CalFPS();
	char titleStr[50];
	sprintf_s(titleStr, "FPS:%d", nowFps);
	SetTitle(titleStr);
}


LRESULT CALLBACK WindowProc(
	_In_	HWND hwnd,
	_In_	UINT uMsg,
	_In_	WPARAM wParam,
	_In_	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		exit(0);

		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			exit(0);
			
		if (wParam == 'A')
			rotateYAng -= dAng;
		if (wParam == 'D')
			rotateYAng += dAng;
		if (wParam == 'W')
			rotateXAng -= dAng;
		if (wParam == 'S')
			rotateXAng += dAng;
		if (wParam == 'Q')
			rotateZAng -= dAng;
		if (wParam == 'E')
			rotateZAng += dAng;

		if (wParam == VK_UP) {
			eye =eye +Forward * dMoveSize;
			lookat=lookat+ Forward * dMoveSize;
		}
		else if (wParam == VK_DOWN) {
			eye = eye - Forward * dMoveSize;
			lookat = lookat - Forward * dMoveSize;
		}
		if (wParam == VK_LEFT)
		{
			eye = eye - side * dMoveSize;
			lookat = lookat - side * dMoveSize;
		}
		else if (wParam == VK_RIGHT) {
			eye = eye + side * dMoveSize;
			lookat = lookat + side * dMoveSize;
		}
		else if (wParam == VK_PRIOR) {
			eye = eye + up * dMoveSize;
			lookat = lookat + up * dMoveSize;
		}
		else if (wParam == VK_NEXT) {
			eye = eye - up * dMoveSize;
			lookat = lookat - up * dMoveSize;
		}
			break;
	}
	case WM_SIZE:
	{
		int nowWidth = LOWORD(lParam);
		int nowHeight = LOWORD(lParam);
		//cout << nowWidth << " " << nowHeight << endl;
		break;
	}
	case WM_MOUSEMOVE:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int main()
{
	initWindow();

	model = loadModel("model/cube.obj");

	Cube[0] = Vec(1, -1, 1);
	Cube[1] = Vec(1, 1, 1);
	Cube[2] = Vec(-1, 1, 1);
	Cube[3] = Vec(-1, -1, 1);
	Cube[4] = Vec(1, -1, -1);
	Cube[5] = Vec(1, 1, -1);
	Cube[6] = Vec(-1, 1, -1);
	Cube[7] = Vec(-1, -1, -1);

	lineList.push_back(pair<int,int>(0, 1));
	lineList.push_back(pair<int, int>(1, 2));
	lineList.push_back(pair<int, int>(2, 3));
	lineList.push_back(pair<int, int>(3, 0));

	lineList.push_back(pair<int, int>(4, 5));
	lineList.push_back(pair<int, int>(5, 6));
	lineList.push_back(pair<int, int>(6, 7));
	lineList.push_back(pair<int, int>(7, 4));

	lineList.push_back(pair<int, int>(0, 4));
	lineList.push_back(pair<int, int>(1, 5));
	lineList.push_back(pair<int, int>(2, 6));
	lineList.push_back(pair<int, int>(3, 7));


	while (1)
	{
		updateFrame();
	}


	return 0;
}