
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
int rotateXStatus = 0;
int rotateYStatus = 0;
int rotateZStatus = 0;

double dAng = 20;
double dMoveSize = 0.5;
int cameraMoveForward = 0;
int cameraMoveUp = 0;
int cameraMoveSide = 0;

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
	if (rotateYStatus == -1)
		rotateYAng -= dAng*oneFrameTime;
	if (rotateYStatus == 1)
		rotateYAng += dAng * oneFrameTime;
	if (rotateXStatus == -1)
		rotateXAng -= dAng * oneFrameTime;
	if (rotateXStatus == 1)
		rotateXAng += dAng * oneFrameTime;
	if (rotateZStatus == -1)
		rotateZAng -= dAng * oneFrameTime;
	if (rotateZStatus == 1)
		rotateZAng += dAng * oneFrameTime;
	if (cameraMoveForward==1) {
		eye = eye + Forward * dMoveSize * oneFrameTime;
		lookat = lookat + Forward * dMoveSize * oneFrameTime;
	}
	if (cameraMoveForward == -1) {
		eye = eye - Forward * dMoveSize * oneFrameTime;
		lookat = lookat - Forward * dMoveSize * oneFrameTime;
	}
	if (cameraMoveSide == 1) 
	{
		eye = eye - side * dMoveSize * oneFrameTime;
		lookat = lookat - side * dMoveSize * oneFrameTime;
	}
	if (cameraMoveSide == -1) {
		eye = eye + side * dMoveSize * oneFrameTime;
		lookat = lookat + side * dMoveSize * oneFrameTime;
	}
	if (cameraMoveUp == 1) {
		eye = eye + up * dMoveSize * oneFrameTime;
		lookat = lookat + up * dMoveSize * oneFrameTime;
	}
	if (cameraMoveUp == -1) {
		eye = eye - up * dMoveSize * oneFrameTime;
		lookat = lookat - up * dMoveSize * oneFrameTime;
	}

	CleanScreen();

	Vec v1 = { 50,150,0.5 }, v2 = { 10,50,0.5 }, v3 = { 100,50,0.4 },v4={10,10,0.3};
	// DrawTopFlatTriangle(v1, v2, v3, Color(203, 64, 66));
	// DrawBottomFlatTriangle(v4, v2, v3, Color(88, 178, 220));

	//DrawTriangle(v1, v2, v3,Color(203,64,66));
	//DrawTriangle(v4, v2, v3, Color(88, 178, 220));
	//DrawTriangle(v1, v4, v2, Color(88, 178, 220));

	for (const auto& tri : model->triangleList)
	{
		const auto& p0 = model->pointList[tri.pointIndex[0]-1];
		const auto& p1 = model->pointList[tri.pointIndex[1]-1];
		const auto& p2 = model->pointList[tri.pointIndex[2]-1];
		Vec P0 = transform(p0);
		Vec P1 = transform(p1);
		Vec P2 = transform(p2);

		DrawTriangle(P0, P1, P2, Color(88, 178, 220));
		DrawLine(P0.x, P0.y, P0.z, P1.x, P1.y, P1.z);
		DrawLine(P1.x, P1.y, P1.z, P2.x, P2.y, P2.z);
		DrawLine(P2.x, P2.y, P2.z, P0.x, P0.y, P0.z);
	}
	

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
	case WM_KEYUP:
	{
		if (wParam == 'A')
			rotateYStatus = 0;
		if (wParam == 'D')
			rotateYStatus = 0;
		if (wParam == 'W')
			rotateXStatus = 0;
		if (wParam == 'S')
			rotateXStatus = 0;
		if (wParam == 'Q')
			rotateZStatus = 0;
		if (wParam == 'E')
			rotateZStatus = 0;

		if (wParam == VK_UP) {
			cameraMoveForward = 0;
		}
		else if (wParam == VK_DOWN) {
			cameraMoveForward = 0;
		}
		if (wParam == VK_LEFT)
		{
			cameraMoveSide = 0;

		}
		else if (wParam == VK_RIGHT) {
			cameraMoveSide = 0;
		}
		if (wParam == VK_PRIOR) {
			cameraMoveUp = 0;
		}
		else if (wParam == VK_NEXT) {
			cameraMoveUp = 0;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			exit(0);
			
		if (wParam == 'A')
			rotateYStatus = -1;
		if (wParam == 'D')
			rotateYStatus = 1;
		if (wParam == 'W')
			rotateXStatus =-1;
		if (wParam == 'S')
			rotateXStatus = 1;
		if (wParam == 'Q')
			rotateZStatus = -1;
		if (wParam == 'E')
			rotateZStatus = 1;

		if (wParam == VK_UP) {
			cameraMoveForward = 1;
		}
		else if (wParam == VK_DOWN) {
			cameraMoveForward = -1;
		}
		if (wParam == VK_LEFT)
		{
			cameraMoveSide = 1;
		}
		else if (wParam == VK_RIGHT) {
			cameraMoveSide = -1;
		}
		if (wParam == VK_PRIOR) {
			cameraMoveUp = 1;
		}
		if (wParam == VK_NEXT) {
			cameraMoveUp = -1;
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

	if (1)
		model = loadModel("model/bunny.obj");
	else
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