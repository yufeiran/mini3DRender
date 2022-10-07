
#include"device.h"

#include"mini3DRender.h"
#include<vector>
using namespace std;

bool debugMode = false;
//=======================================================

//======================================================

Vec eye(5, 5, 0);
Vec lookat(0, 5, 0);
Vec up(0, 1, 0);

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

vector<Vec>ansList;


double rotateXAng = 0;
double rotateYAng = 45;
double rotateZAng = 0;

Mat makeWorldToCameraMat(const Vec& forward, const Vec& side, const Vec& up,const Vec&eye)
{
	//这里直接取“从世界到相机空间变换矩阵”的逆矩阵，推导请看https://yufeiran.com/cao-gao-zhi-shang-tui/ 2.3.2节
	Mat mMoveR = makeMoveMat(-eye.x, -eye.y, -eye.z);
	Mat mRotateR;
	mRotateR.m[3][3] = 1;
	for (int i = 0; i < 3; i++)
	{
		mRotateR.m[0][i] = side[i];
		mRotateR.m[1][i] = up[i];
		mRotateR.m[2][i] = -forward[i];
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
	mObjectToWorld = makeMoveMat(0, 5, 0)*makeRotateByYMat(rotateYAng)* makeRotateByXMat(rotateXAng)*makeRotateByZMat(rotateZAng);
	Vec worldVec = mObjectToWorld * objVec;

	if (debugMode)
		cout << "world :" << worldVec << endl << " - > ";
	// world to Camera

	Vec forward = lookat - eye;
	forward = forward.normal();

	up = up.normal();
	Vec side;
	side = cross(forward, up);

	up = cross(side, forward);

	Mat mWorldToObject = makeWorldToCameraMat(forward, side, up, eye);

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
	ansList.clear();
	for (int i = 0; i < 8; i++)
	{
		auto ans = transform(Cube[i]);
		//cout << ans << endl;
		ansList.push_back(ans);
	}

	CleanScreen();
	for (int y = 0; y < screenHeight; y++)
		for (int x = 0; x < screenWidth; x++)
		{
			Color color = Color(0, 0, 0);
			DrawPoint(x, screenHeight - y, color);
		}

	for (auto& v : ansList)
	{
		Color color = Color(255, 255, 255);
		DrawPoint(v.x, screenHeight - v.y, color);
	}

	PutBufferToScreen();
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
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			exit(0);
			
		if (wParam == 'A')
			rotateYAng -= 0.5;
		if (wParam == 'D')
			rotateYAng += 0.5;
		if (wParam == 'W')
			rotateXAng -= 0.5;
		if (wParam == 'S')
			rotateXAng += 0.5;
		if (wParam == 'Q')
			rotateZAng -= 0.5;
		if (wParam == 'E')
			rotateZAng += 0.5;
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



	Cube[0] = Vec(1, -1, 1);
	Cube[1] = Vec(1, 1, 1);
	Cube[2] = Vec(-1, 1, 1);
	Cube[3] = Vec(-1, -1, 1);
	Cube[4] = Vec(1, -1, -1);
	Cube[5] = Vec(1, 1, -1);
	Cube[6] = Vec(-1, 1, -1);
	Cube[7] = Vec(-1, -1, -1);








	cout << "-------" << endl;

	while (1)
	{
		updateFrame();
	}


	return 0;
}