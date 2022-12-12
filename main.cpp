#include<vector>
#include"mini3DRender.h"



using namespace std;

bool debugMode = false;
//=======================================================

//======================================================

Camera camera;
Model* model;
vector<Vec>ansList;
vector<pair<int, int>>lineList;


Vec moveVec = { 4.5,5,0 };
double rotateXAng = 0;
double rotateYAng = 0;
double rotateZAng = 0;
int rotateXStatus = 0;
int rotateYStatus = 0;
int rotateZStatus = 0;
double scaleX = 1;
double scaleY = 1;
double scaleZ = 1;

double dAng = 20;
double dMoveSize = 0.5;
int cameraMoveForward = 0;
int cameraMoveUp = 0;
int cameraMoveSide = 0;

double tempLightAng = 0;

Vec Cube[8];
const int randomColorSum = 1000;
Color randColorList[randomColorSum];

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
		camera.eye = camera.eye + camera.Forward * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat + camera.Forward * dMoveSize * oneFrameTime;
	}
	if (cameraMoveForward == -1) {
		camera.eye = camera.eye - camera.Forward * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat - camera.Forward * dMoveSize * oneFrameTime;
	}
	if (cameraMoveSide == 1) 
	{
		camera.eye = camera.eye - camera.side * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat - camera.side * dMoveSize * oneFrameTime;
	}
	if (cameraMoveSide == -1) {
		camera.eye = camera.eye + camera.side * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat + camera.side * dMoveSize * oneFrameTime;
	}
	if (cameraMoveUp == 1) {
		camera.eye = camera.eye + camera.up * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat + camera.up * dMoveSize * oneFrameTime;
	}
	if (cameraMoveUp == -1) {
		camera.eye = camera.eye - camera.up * dMoveSize * oneFrameTime;
		camera.lookat = camera.lookat - camera.up * dMoveSize * oneFrameTime;
	}
	//每次改变完camera都要重新计算camera的数据
	camera.update();
	CleanScreen();


	//围绕(0,y,0)旋转点光源

	tempLightAng+=0.1;

	double LightR = 2;
	lightVec[0].posInWorld.y = cos(tempLightAng) * LightR;
	lightVec[0].posInWorld.z = sin(tempLightAng) * LightR;


	Vec v1 = { 50,150,0.5 }, v2 = { 10,50,0.5 }, v3 = { 100,50,0.4 },v4={10,10,0.3};
	// DrawTopFlatTriangle(v1, v2, v3, Color(203, 64, 66));
	// DrawBottomFlatTriangle(v4, v2, v3, Color(88, 178, 220));

	//DrawTriangle(v1, v2, v3,Color(203,64,66));
	//DrawTriangle(v4, v2, v3, Color(88, 178, 220));
	//DrawTriangle(v1, v4, v2, Color(88, 178, 220));
	//drawModel(camera, model, moveVec, rotateXAng, rotateYAng, rotateZAng, scaleX, scaleY, scaleZ);
	for (const auto& s : spriteVec)
	{
		drawSprite(camera, s);
	}

	PutBufferToScreen();

	int nowFps= CalFPS();
	char titleStr[50];
	sprintf_s(titleStr, "FPS:%d camera pos (%.2f,%.2f,%.2f)", nowFps,camera.eye.x, camera.eye.y, camera.eye.z);
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
		if (wParam == 'M')
		{
			cout << "Enter Render Thread Sum:" << endl;
			cout << ">";
			int temp;
			cin >> temp;
			drawThreadSum = temp;
		}

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

	setDrawThreadSum(6);

	Light L;
	L.color = { 255,255,255 };
	L.energy = 30;
	L.posInWorld = { 10,0,0 };
	lightVec.push_back(L);

	
	for (int i = 0; i < randomColorSum; i++)
	{
		randColorList[i] = Color(rand() % 256, rand() % 256, rand() % 256);
	}

	Model* mBunny = loadModel("model/bunny.obj");
	mBunny->drawMode = FillColor;
	Model* mCube = loadModel("model/cube.obj");
	mCube->texture=loadTexture("model/wood.jpg");
	mCube->drawMode= TextureColor;
	Model* mGround = loadModel("model/Ground.obj");
	mGround->texture = loadTexture("model/green.png");
	mGround->drawMode = TextureColor;
	mGround->isNeedCVVCut = false;


	modelVec.push_back(mBunny);
	modelVec.push_back(mCube);
	modelVec.push_back(mGround);

	Sprite sBunny;
	sBunny.model = mBunny;
	sBunny.moveVec = { 0,0,0 };

	Sprite sBunny1 = sBunny;
	sBunny1.moveVec = { 1,0,0 };

	Sprite sCube;
	sCube.model = mCube;
	sCube.moveVec = { 0,0,0 };

	Sprite sGround;
	sGround.model = mGround;
	sGround.moveVec = { -0.2,0,0 };
	sGround.scaleX = 0.1;
	sGround.scaleY = 0.1;
	sGround.scaleZ= 0.1;
	spriteVec.push_back(sBunny);
	//spriteVec.push_back(sBunny1);
	//spriteVec.push_back(sCube);
	spriteVec.push_back(sGround);

	int loadModelId =4;
	switch (loadModelId)
	{
	case 0:
		moveVec = { 0,0,0 };
		model = loadModel("model/bunny.obj");
		model->drawMode = FillColor;

		break;
	case 1:
		moveVec = { 0,0,0 };
		model = loadModel("model/cube.obj");
		model->texture = loadTexture("model/wood.jpg");
		model->drawMode = TextureColor;
		break;
	case 2:
		moveVec = { 2,0,0 };
		model = loadModel("model/panel.obj");
		model->texture = loadTexture("model/yuki.jpg");
		model->drawMode = TextureColorWithLine;
		rotateYAng = 90;
		break;
	case 3:
		moveVec = { 70,0,0 };
		model = loadModel("model/chair.obj");
		break;
	case 4:
		moveVec = { -2,-3,0 };
		model = loadModel("model/Ground.obj");
		model->texture = loadTexture("model/green.png");
		model->drawMode = TextureColorWithLine;
		break;

	default:
		break;
	}
	while (1)
	{
		updateFrame();
	}


	return 0;
}