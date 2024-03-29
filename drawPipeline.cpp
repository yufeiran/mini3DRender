#include"drawPipeline.h"
#include<iostream>
#include<thread>
using namespace std;
int drawThreadSum=6;

void setDrawThreadSum(int n)
{
	drawThreadSum = n;
}

Mat makeWorldToCameraMat(const Vec& Forward, const Vec& side, const Vec& up, const Vec& eye)
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
	m.m[2][3] = f * n / (n - f); 
	m.m[3][2] = 1;

	return m;
}


Mat makeClipToScreenMat(const double x0, const double y0, const double width, const double height, const double Zmin, const double Zmax)
{
	Mat m;
	m.m[0][0] = 0.5 * width;
	m.m[1][1] = -0.5 * height;
	m.m[2][2] = Zmax - Zmin;
	m.m[3][3] = 1;

	m.m[0][3] = x0 + 0.5 * width;
	m.m[1][3] = y0 + 0.5 * height;
	m.m[2][3] = Zmin;
	return m;
}

Vec tannsformObjToWorld(const Vec& objVec, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz)
{
	if (debugMode)
		cout << "raw :" << objVec << endl << " - > ";
	// object to world
	Mat mObjectToWorld;
	mObjectToWorld = makeMoveMat(moveVec.x, moveVec.y, moveVec.z) * makeRotateByYMat(rotateYAng) * makeRotateByXMat(rotateXAng) * makeRotateByZMat(rotateZAng) * makeScaleMat(sx, sy, sz);
	Vec worldVec = mObjectToWorld * objVec;

	if (debugMode)
		cout << "world :" << worldVec << endl << " - > ";

	return worldVec;
}

Vec transformWorldToCamera(const Camera& camera, const Vec& worldVec)
{
	Mat mWorldToObject = makeWorldToCameraMat(camera.Forward, camera.side, camera.up, camera.eye);

	Vec cameraVec = mWorldToObject * worldVec;

	if (debugMode)
		cout << "camera :" << cameraVec << endl << " - > ";
	return cameraVec;

}

Vec transformCameraToClip(const Camera& camera, const Vec& cameraVec)
{
	// Camera to Clip 
	Mat mCameraToClip = makeCameraToClipMat(camera.viewAng, camera.aspect, camera.n, camera.f);
	Vec clipVec = mCameraToClip * cameraVec;

	if (debugMode)
		cout << "clip :" << clipVec << endl << " - > ";


	return clipVec;
}

Vec perspectiveDivision(const Vec& clipHomogeneousVec)
{
	Vec clipVec = clipHomogeneousVec;
	clipVec.x /= clipVec.w;
	clipVec.y /= clipVec.w;
	clipVec.z /= clipVec.w;
	clipVec.w = 1;
	if (debugMode)
		cout << "new clip by perspectiveDivision :" << clipVec << endl << " - > ";
	return clipVec;
}

Vec transformClipToScreen(const Camera& camera, const Vec& clipVec)
{
	// Clip to Screen
	Mat mClipToScreen = makeClipToScreenMat(camera.X0, camera.Y0, camera.width, camera.height, camera.Zmin, camera.Zmax);
	Vec ScreenVec = mClipToScreen * clipVec;

	if (debugMode)
		cout << "screen :" << ScreenVec << endl <<" - > ";
	return ScreenVec;

}

void drawModelSub(int threadID,int threadSum, const Camera& camera, const Model* model,
	const Vec& moveVec, const double rotateXAng, const double rotateYAng,
	const double rotateZAng, const double  sx, const double sy, const double sz)
{
	for (int i = threadID; i < model->vIndexList.size(); i+= threadSum)
	{
		Triangle uvTri{ -1,-1,-1 }, vnTri{ -1,-1,-1 };
		if (i < model->uvIndexList.size())
		{
			uvTri = model->uvIndexList[i];
		}
		else {
			cout<<"uv error"<<endl;
		}
		if (i < model->vnIndexList.size())
		{
			vnTri = model->vnIndexList[i];
		}
		else {
			cout<<"vn error"<<endl;
		}
		DrawTriangle(model->vIndexList[i], uvTri, vnTri,
			model, camera, moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz);
	}
}


void calLightVecPosInCamera(const Camera&camera)
{

	for (int i = 0; i < lightVec.size(); i++)
	{
		const auto& lightPosWorld = lightVec[i].posInWorld;
		const auto& lightPosCamera = transformWorldToCamera(camera, lightPosWorld);
		lightVec[i].posInCamera = lightPosCamera;

	}

}

bool drawSprite(const Camera& camera, const Sprite& sprite)
{
	return drawModel(camera, sprite.model, sprite.moveVec,
		sprite.rotateXAng, sprite.rotateYAng, sprite.rotateZAng,
		sprite.scaleX, sprite.scaleY, sprite.scaleZ);

}

bool drawModel(const Camera& camera, const Model* model,
	const Vec& moveVec, const double rotateXAng, const double rotateYAng, 
	const double rotateZAng, const double  sx, const double sy, const double sz)
{

	calLightVecPosInCamera(camera);

	vector<thread> threadVec;
	for (int i = 0; i < drawThreadSum; i++)
	{

		threadVec.push_back(thread (drawModelSub ,i, drawThreadSum, camera, model,
			moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz ));
	}
	for (auto& s : threadVec)
	{
		s.join();
	}
	//for (int i = 0; i < model->vIndexList.size(); i++)
	//{
	//	Triangle uvTri{-1,-1,-1},vnTri{-1,-1,-1};
	//	if (i <model->uvIndexList.size())
	//	{
	//		uvTri = model->uvIndexList[i];
	//	}
	//	if (i < model->vnIndexList.size())
	//	{
	//		vnTri = model->vnIndexList[i];
	//	}
	//	DrawTriangle(model->vIndexList[i], uvTri, vnTri,
	//		model, camera, moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz);
	//}


	return true;
}


Vec getCameraPosFromScreenPoint(const Vec& pOnScreen, const Camera& camera)
{
	double OneOverPz = (pOnScreen.z - camera.f / (camera.f - camera.n)) * (camera.n - camera.f) / (camera.f * camera.n);

	double Pz = 1.0 / OneOverPz;


	double N = camera.n;

	double h = camera.n * tan(degToRad(camera.viewAng)); //裁剪空间前裁剪面的宽度
	double w = h * camera.aspect; //裁剪空间前裁剪面的高度


	//这里我们需要的是p在投影面上的坐标，所以要对p在屏幕上的坐标进行逆变换
	Vec pOnClipCuboid;
	Vec pOnClip;
	Vec pOnCamera;
	pOnClipCuboid.x = 2.0 * pOnScreen.x / camera.width - 1.0;
	pOnClipCuboid.y = -(2.0 * pOnScreen.y / camera.height - 1.0);
	pOnClipCuboid.z = (pOnScreen.z - camera.Zmin) / (camera.Zmax - camera.Zmin);

	pOnClip.x = pOnClipCuboid.x * w;
	pOnClip.y = pOnClipCuboid.y * h;
	pOnClip.z = camera.n;

	pOnCamera.x = pOnClip.x * Pz / N;
	pOnCamera.y = pOnClip.y * Pz / N;
	pOnCamera.z = Pz;
	return pOnCamera;
}

UVPair calUVByCameraPoint(const Vec& pOnCamera, const VPoint& vp1, const VPoint& vp2, const VPoint& vp3, const Camera& camera)
{



	Vec A = vp1.pointCamera, B = vp2.pointCamera, C = vp3.pointCamera;
	Vec P = pOnCamera;



	double i = 0, j = 0, k = 0;

	double i_result_up = (-(B.y - C.y) * (P.x - C.x) + (B.x - C.x) * (P.y - C.y));
	double i_result_bottom = ((B.y - C.y) * (C.x - A.x) - (B.x - C.x) * (C.y - A.y));;

	if (i_result_bottom == 0)
	{
		//cout << "devide by zero!" << endl;
		return UVPair{ 0,0 };
	}

	i = i_result_up /
		i_result_bottom;
	
	i = clamp(i, 0, 1);

	double j_result_up = (-(P.x - C.x) * (A.y - C.y) + (P.y - C.y) * (A.x - C.x));
	double j_result_bottom = ((C.x - B.x) * (A.y - C.y) - (C.y - B.y) * (A.x - C.x));

	j = j_result_up /
		j_result_bottom;

	j = clamp(j, 0, 1);

	if (j_result_bottom == 0)
	{
		//cout << "devide by zero!" << endl;
		return UVPair{ 0,0 };
	}


	k = 1.0 - i - j;


	UVPair uvP;
	uvP.first = i * vp1.uv.first + j * vp2.uv.first + k * vp3.uv.first;
	uvP.second = i * vp1.uv.second + j * vp2.uv.second + k * vp3.uv.second;
	return uvP;
}



void DrawLine_DDA(double x0, double y0, double z0, double x1, double y1, double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}
	double k = (double)(y0 - y1) / (double)(x0 - x1);
	if (k < 1 && k>0) {
		//use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi, z0, color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(xi, yi, nowZ, color);
		}

	}
	else if (k >= 1) {
		//use y iter	
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi, z0, color);
		for (int y = y0; y < y1; y++) {
			yi = yi + 1;
			xi = xi + k_;
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(xi, yi, nowZ, color);
		}
	}
	else if (k < -1) {
		//use y iter
		double k_ = 1.0 / k;
		double yi = y0, xi = x0;
		DrawPoint(xi, yi, z0, color);
		for (int y = y0; y > y1; y--) {
			yi = yi - 1;
			xi = xi - k_;
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(xi, yi, nowZ, color);
		}
	}
	else if (k < 0 && k >= -1) {
		// use x iter
		double xi = x0, yi = y0;
		DrawPoint(xi, yi, z0, color);
		for (int x = x0; x < x1; x++) {
			xi = xi + 1;
			yi = yi + k;
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(xi, yi, nowZ, color);
		}
	}
}

void DrawLine_mid(int x0, int y0, double z0, int x1, int y1, double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}

	double k = (double)(y0 - y1) / (double)(x0 - x1);

	// Ax+By+C
	// A=(y1-y0) B=(x0-x1)
	int A = y0 - y1;
	int B = x1 - x0;


	if (k > 0 && k <= 1)
	{
		int d = 2 * A + B;

		int x = x0;
		int y = y0;
		while (x != x1)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d < 0) {
				y++;
				d += 2 * A + 2 * B;
			}
			else if (B * d > 0) {
				d += 2 * A;
			}
			x++;
		}
	}
	else if (k > 1) {
		int d = A + 2 * B;

		int y = y0;
		int x = x0;
		while (y != y1)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d < 0) {
				d += 2 * B;
			}
			else if (B * d > 0) {
				d += 2 * A + 2 * B;
				x++;
			}
			y++;
		}
	}
	else if (k >= -1 && k < 0) {
		int d = 2 * A - B;

		int x = x0;
		int y = y0;
		while (x != x1)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d > 0) {

				y--;
				d += 2 * A;
			}
			else if (B * d < 0) {
				d += 2 * (A - B);
			}
			x++;
		}
	}
	else if (k < -1) {
		int d = A - 2 * B;

		int y = y0;
		int x = x0;
		while (y != y1)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			if (B * d > 0) {
				d += -2 * B;
			}
			else if (B * d < 0) {
				x++;
				d += 2 * (A - B);
			}
			y--;
		}
	}
}


//浮点运算版本Bresenham算法
void DrawLine_Bresenham_Raw(int x0, int y0, double z0, int x1, int y1, double z1, const Color& color)
{

	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}

	double k = (double)(y1 - y0) / (double)(x1 - x0);

	double d = 0;


	if (k >= 0 && k < 1) {
		int y = y0;
		for (int x = x0; x < x1; x++)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += k;
			if (d > 0.5) {
				y++;
				d -= 1;
			}
		}
	}
	else if (k >= 1) {
		int x = x0;
		double _k = 1.0 / k;
		for (int y = y0; y < y1; y++)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += _k;
			if (d > 0.5) {
				x++;
				d -= 1;
			}
		}
	}
	else if (k<0 && k>-1) {
		int y = y0;
		double _k = -k;
		for (int x = x0; x < x1; x++)
		{
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += _k;
			if (d > 0.5) {
				y--;
				d -= 1;
			}
		}
	}
	else if (k <= -1) {
		int x = x0;
		double _k = -1.0 / k;
		for (int y = y0; y > y1; y--)
		{
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += _k;
			if (d > 0.5) {
				x++;
				d -= 1;
			}
		}
	}
}

void DrawLine_Bresenham(int x0, int y0, double z0, int x1, int y1, double z1, const Color& color)
{
	if (x1 < x0) {
		int tempX, tempY;
		double tempZ;
		tempX = x0;
		x0 = x1;
		x1 = tempX;
		tempY = y0;
		y0 = y1;
		y1 = tempY;
		tempZ = z0;
		z0 = z1;
		z1 = tempZ;

	}
	if (x1 == x0) {
		for (int y = min(y0, y1); y <= max(y0, y1); y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x0, y, nowZ, color);
		}
		return;
	}

	//k=0
	if (y1 == y0) {
		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y0, nowZ, color);
		}
		return;
	}
	int dx = x1 - x0, dy = y1 - y0;
	bool kSign = ((y1 - y0) >= 0 && (x1 - x0) >= 0) ? true : false; //true => positive false =>negative
	bool kGreaterThenOne = (abs(y1 - y0) > abs(x1 - x0)) ? true : false;

	int d = 0;
	// 0<k<=1
	if (kSign == true && kGreaterThenOne == false) {
		int y = y0;

		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += 2 * dy;
			if (d > dx) {
				d -= 2 * dx;
				y++;
			}
		}
	}
	//k>1
	else if (kSign == true && kGreaterThenOne == true) {
		int x = x0;

		for (int y = y0; y < y1; y++) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += 2 * dx;
			if (d > dy) {
				d -= 2 * dy;
				x++;
			}
		}
	}
	//-1<k<=0
	else if (kSign == false && kGreaterThenOne == false) {
		int y = y0;

		for (int x = x0; x < x1; x++) {
			double nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += -2 * dy;
			if (d > dx) {
				d -= 2 * dx;
				y--;
			}
		}
	}
	//k<-1
	else if (kSign == false && kGreaterThenOne == true) {
		int x = x0;

		for (int y = y0; y > y1; y--) {
			double nowZ = z0 + (double)(y - y0) / (double)(y1 - y0) * (z1 - z0);
			DrawPoint(x, y, nowZ, color);
			d += 2 * dx;
			if (d > -dy) {
				d -= -2 * dy;
				x++;
			}
		}
	}

}



void DrawLine(double x0, double y0, double z0, double x1, double y1, double z1, const Color& color)
{
	DrawLine_Bresenham(x0, y0, z0, x1, y1, z1, color);
	//DrawLine_Bresenham_Raw(x0, y0,z0, x1, y1,z1, color);
	//DrawLine_mid(x0, y0,z0, x1, y1,z1, color);
	//DrawLine_DDA(x0, y0,z0, x1, y1,z1, color);
}

void DrawScanLine(double x0, double z0, double x1, double z1, double y,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Vec& n, //三角形面法向量
	const Model* model, const Camera& camera)
{

	if (y < 0 || y > screenHeight)return;
	if (x0 > x1) {
		swap(x0, x1);
		swap(z0, z1);
	}

	for (int x = max(0,x0); x <= min(x1,screenWidth); x++) {
		double nowZ;
		if ((int)x0 == (int)x1) {
			nowZ = z0;
		}
		else {
			nowZ = z0 + (double)(x - x0) / (double)(x1 - x0) * (z1 - z0);
		}
		Color nowColor;
		const Vec& pOnCamera = getCameraPosFromScreenPoint({ (double)x,y,(double)nowZ }, camera);
		if (model->drawMode == TextureColorWithLine|| model->drawMode== TextureColor) {
		
			UVPair nowUV = calUVByCameraPoint(pOnCamera, vp1, vp2, vp3,camera);
			nowColor = model->texture->getColor(nowUV.first, nowUV.second);
			


		}
		else {
			nowColor = model->filColor;
		}
		if (lightVec.empty() == false)
		{
			//开始计算光照
			//L=La+Ld+Ls
			// 这部分详情请参考 https://yufeiran.com/mini3drender-2/
			// 
			//light diffuse 漫反射

			Vec I = (pOnCamera - lightVec[0].posInCamera).normal();
			double kd = (double)(nowColor.r + nowColor.g + nowColor.b) / (double)(3 * 255);
			double r = (pOnCamera - lightVec[0].posInCamera).len();
			double Ld = kd * (lightVec[0].energy / (r * r)) * max(0, n * I);


			//light specular 镜面反射
			Vec viewerPos = { 0,0,0 };
			Vec V = (viewerPos - pOnCamera).normal();
			Vec h = (I + V).normal();

			double ks = kd;
			double p = 32;

			double Ls = ks * (lightVec[0].energy / (r * r)) * pow(max(0, n * h), p);

			//light Amibient 环境光
			double ka = kd;
			double Ia = 0.05;
			double La = ka * Ia;

			double L = min(1, Ls + Ld + La);

			Color lightColor(L * lightVec[0].color.r, L * lightVec[0].color.g, L * lightVec[0].color.b);
			nowColor = nowColor + lightColor;
		}

		DrawPoint(x, y, nowZ, nowColor);
	}
	return;
}

void DrawTopFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Vec&n, //三角形面法向量
	const Model* model, const Camera& camera
	)
{
	double XL = v1.x, XR = v1.x;
	double dX1 = -(v2.x - v1.x) / (v2.y - v1.y), dX2 = -(v3.x - v1.x) / (v3.y - v1.y);
	double ZL = v1.z, ZR = v1.z;
	double dZ1 = -(v2.z - v1.z) / (v2.y - v1.y), dZ2 = -(v3.z - v1.z) / (v3.y - v1.y);
	if (round(v1.y) - round(v2.y) <= 1) {
		DrawScanLine(XL, ZL, XR, ZR, round(v1.y), vp1, vp2, vp3,n, model,camera);
		return;
	}
	for (int y = round(v1.y); y >= v2.y; y--)
	{
		DrawScanLine(XL, ZL, XR, ZR, y, vp1, vp2, vp3,n, model, camera);
		XL += dX1;
		XR += dX2;
		ZL += dZ1;
		ZR += dZ2;
	}
}

void DrawBottomFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Vec& n, //三角形面法向量
	const Model* model, const Camera& camera)
{


	double XL = v1.x, XR = v1.x;
	double dX1 = (v2.x - v1.x) / (v2.y - v1.y), dX2 = (v3.x - v1.x) / (v3.y - v1.y);
	double ZL = v1.z, ZR = v1.z;
	double dZ1 = (v2.z - v1.z) / (v2.y - v1.y), dZ2 = (v3.z - v1.z) / (v3.y - v1.y);
	if (round(v2.y) - round(v1.y) <= 1) {
		DrawScanLine(XL, ZL, XR, ZR, round(v1.y), vp1, vp2, vp3,n, model, camera);
		return;
	}
	for (int y = round(v1.y); y <= v2.y; y++)
	{
		DrawScanLine(XL, ZL, XR, ZR, y, vp1, vp2, vp3,n, model, camera);
		XL += dX1;
		XR += dX2;
		ZL += dZ1;
		ZR += dZ2;
	}
}

void SwapByYValue(VPoint& v1, VPoint& v2, VPoint& v3)
{
	//按y值从小到大排序3个顶点
	VPoint V[3] = { v1,v2,v3 };
	int maxIndex = 0, minIndex = 0;
	VPoint maxV = v1, minV = v1, midV;
	for (int i = 0; i < 3; i++)
	{
		if (V[i].pointScreen.y < minV.pointScreen.y)
		{
			minIndex = i;
			minV = V[i];
		}
		if (V[i].pointScreen.y >= maxV.pointScreen.y)
		{
			maxIndex = i;
			maxV = V[i];
		}
	}
	int midIndex;
	for (int i = 0; i < 3; i++) {
		if (i == maxIndex || i == minIndex) {
			continue;
		}
		else {
			midIndex = i;
			midV = V[i];

		}
	}
	v1 = minV, v2 = midV, v3 = maxV;
	v1.pointScreen.x = (int)v1.pointScreen.x; v1.pointScreen.y = (int)v1.pointScreen.y;
	v2.pointScreen.x = (int)v2.pointScreen.x; v2.pointScreen.y = (int)v2.pointScreen.y;
	v3.pointScreen.x = (int)v3.pointScreen.x; v3.pointScreen.y = (int)v3.pointScreen.y;
}

bool checkIsPointInCuboid(const Vec& p)
{
	if (p.x < -1.2 || p.x>1.2)return false;
	if (p.y < -1.2 || p.y>1.2)return false;
	if (p.z < 0 || p.z >1)return false;
	return true;
}

void DrawTriangle(const Triangle& vTri, const Triangle& uvTri, const Triangle& vnTri,//对应的在模型中的坐标
	const Model* model, const Camera& camera, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz)
{

	if (debugMode == true)
	{
		cout << "=========================DrawTriangle=========================" << endl << " - > ";
	}
	if (vTri.pointIndex[0] - 1 >= model->vList.size())return;
	if (vTri.pointIndex[1] - 1 >= model->vList.size())return;
	if (vTri.pointIndex[2] - 1 >= model->vList.size())return;
	const auto& p0Object = model->vList[vTri.pointIndex[0] - 1];
	const auto& p1Object = model->vList[vTri.pointIndex[1] - 1];
	const auto& p2Object = model->vList[vTri.pointIndex[2] - 1];

	const auto& p0World = tannsformObjToWorld(p0Object, moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz);
	const auto& p1World = tannsformObjToWorld(p1Object, moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz);
	const auto& p2World = tannsformObjToWorld(p2Object, moveVec, rotateXAng, rotateYAng, rotateZAng, sx, sy, sz);

	 auto p0Camera = transformWorldToCamera(camera, p0World);
	 auto p1Camera = transformWorldToCamera(camera, p1World);
	 auto p2Camera = transformWorldToCamera(camera, p2World);


	p0Camera.z = -p0Camera.z;
	p1Camera.z = -p1Camera.z;
	p2Camera.z = -p2Camera.z;
	if (debugMode == true)
	{
		cout << "camera Z neg p0:" << p0Camera << endl << " - > ";
		cout << "camera Z neg p1:" << p0Camera << endl << " - > ";
		cout << "camera Z neg p2:" << p0Camera << endl << " - > ";
	}

	if ((p0Camera.z < camera.n )&& 
		(p1Camera.z < camera.n) && 
		(p2Camera.z < camera.n)) {
		return;
	}

	//背面消隐
	Vec AB = p1Camera - p0Camera;
	Vec BC = p2Camera - p1Camera;
	Vec triNormalVec = cross(AB, BC);
	const Vec cameraDirection = p0Camera;
	double backsideBlankingResult = triNormalVec * cameraDirection;
	const double bias = 0.05;
	if (backsideBlankingResult < 0- bias) {
		return;
	}



	const auto& p0ClipHomogeneousVec = transformCameraToClip(camera, p0Camera);
	const auto& p1ClipHomogeneousVec = transformCameraToClip(camera, p1Camera);
	const auto& p2ClipHomogeneousVec = transformCameraToClip(camera, p2Camera);




	const auto& p0Clip = perspectiveDivision(p0ClipHomogeneousVec);
	const auto& p1Clip = perspectiveDivision(p1ClipHomogeneousVec);
	const auto& p2Clip = perspectiveDivision(p2ClipHomogeneousVec);

	if (model->isNeedCVVCut == true)
	{
		if ((checkIsPointInCuboid(p0Clip) == false) &&
			(checkIsPointInCuboid(p1Clip) == false) &&
			(checkIsPointInCuboid(p2Clip) == false)) {
			return;
		}
	}


	const auto& p0Screen = transformClipToScreen(camera, p0Clip);
	const auto& p1Screen = transformClipToScreen(camera, p1Clip);
	const auto& p2Screen = transformClipToScreen(camera, p2Clip);

	//遮挡剔除
	//如果三角形的三个顶点的z值都大于屏幕相应点位的z值就剔除
	double screenZInP0 = getZBufferByPos(p0Screen.x, p0Screen.y);
	double screenZInP1 = getZBufferByPos(p1Screen.x, p1Screen.y);
	double screenZInP2 = getZBufferByPos(p2Screen.x, p2Screen.y);
	const double delta = 0.02;
	if ((p0Screen.z > (screenZInP0+ delta))&& (p1Screen.z > (screenZInP1+delta ))&&( p2Screen.z > (screenZInP2+ delta))) {
		return;
	}

	//if ((p0Screen.z <screenZInP0) && (p1Screen.z < screenZInP1) && (p2Screen.z < screenZInP2)) {
	//	return;
	//}

	if (model->drawMode == TextureColorWithLine || model->drawMode == FillColorWithLine || model->drawMode == LineColor)
	{
		DrawLine(p0Screen.x, p0Screen.y, p0Screen.z, p1Screen.x, p1Screen.y, p1Screen.z, model->lineColor);
		DrawLine(p1Screen.x, p1Screen.y, p1Screen.z, p2Screen.x, p2Screen.y, p2Screen.z, model->lineColor);
		DrawLine(p2Screen.x, p2Screen.y, p2Screen.z, p0Screen.x, p0Screen.y, p0Screen.z, model->lineColor);
	}

	//计算三角形面的法线
	Vec v1 = p0Camera - p1Camera;
	Vec v2 = p2Camera - p1Camera;
	Vec n = cross(v1, v2).normal();


	VPoint P[3];
	P[0].pointCamera = p0Camera;
	P[0].pointClipHomogeneous = p0ClipHomogeneousVec;
	P[0].pointClipHomogeneous.w = p0ClipHomogeneousVec.w;
	P[0].pointScreen = p0Screen;
	if (uvTri.pointIndex[0] != -1) {
		P[0].uv = model->uvList[uvTri.pointIndex[0] - 1];
	}

	P[1].pointCamera = p1Camera;
	P[1].pointClipHomogeneous = p1ClipHomogeneousVec;
	P[1].pointClipHomogeneous.w = p1ClipHomogeneousVec.w;
	P[1].pointScreen = p1Screen;
	if (uvTri.pointIndex[1] != -1) {
		P[1].uv = model->uvList[uvTri.pointIndex[1] - 1];
	}

	P[2].pointCamera = p2Camera;
	P[2].pointClipHomogeneous = p2ClipHomogeneousVec;
	P[2].pointClipHomogeneous.w = p2ClipHomogeneousVec.w;
	P[2].pointScreen = p2Screen;
	if (uvTri.pointIndex[2] != -1) {
		P[2].uv = model->uvList[uvTri.pointIndex[2] - 1];
	}


	//按y值从小到大排序3个顶点
	SwapByYValue(P[0], P[1], P[2]);
	Vec V1 = P[0].pointScreen, V2 = P[1].pointScreen, V3 = P[2].pointScreen;

	if (P[0].pointScreen.y == P[1].pointScreen.y) {
		DrawTopFlatTriangle(V3, V1, V2, P[2], P[0], P[1],n, model, camera);
	}
	else if (P[1].pointScreen.y == P[2].pointScreen.y) {
		DrawBottomFlatTriangle(V1, V3, V2, P[0], P[2], P[1],n, model, camera);
	}
	else {
		Vec V4;
		V4.y = V2.y;
		V4.x = V1.x + (V4.y - V1.y) / (V3.y - V1.y) * (V3.x - V1.x);
		V4.z = V1.z + (V4.y - V1.y) / (V3.y - V1.y) * (V3.z - V1.z);

		DrawBottomFlatTriangle(V1, V2, V4, P[0], P[1], P[2],n, model, camera);
		DrawScanLine(V2.x, V2.z, V4.x, V4.z, V2.y, P[0], P[1], P[2],n, model, camera);
		DrawTopFlatTriangle(V3, V2, V4, P[2], P[1], P[0],n, model, camera);

	}

}