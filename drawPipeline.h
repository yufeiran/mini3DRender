#pragma once
#include"mini3DRender.h"

struct VPoint //用于最终绘制到屏幕上点的顶点结构
{
	Vec pointScreen;
	Vec pointCamera;
	Vec pointClipHomogeneous;
	UVPair uv;
};



Mat makeWorldToCameraMat(const Vec& Forward, const Vec& side, const Vec& up, const Vec& eye);
Mat makeCameraToClipMat(const double viewAng, const double aspect, double n, double f);
Mat makeClipToScreenMat(const double x0, const double y0, const double width, const double height, const double Zmin, const double Zmax);

Vec tannsformObjToWorld(const Vec& objVec, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz);

Vec transformWorldToCamera(const Camera&camera, const Vec& worldVec);

Vec transformCameraToClip(const Camera& camera, const Vec& cameraVec);

Vec perspectiveDivision(const Vec& clipHomogeneousVec);

Vec transformClipToScreen(const Camera& camera, const Vec& clipVec);


UVPair calUVByScreenPoint(const Vec& pOnScreen, const VPoint& vp1, const VPoint& vp2, const VPoint& vp3, const Camera& camera);



void DrawLine(double x0, double y0, double z0, double x1, double y1, double z1, const Color& color = Color(255, 255, 255));

void DrawScanLine(double x0, double z0, double x1, double z1, double y,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Model* model,const Camera&camera);

void DrawTopFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Model* model, const Camera& camera);
void DrawBottomFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3,
	const Model* model, const Camera& camera);

void DrawTriangle(const Triangle& vTri, const Triangle& uvTri, const Triangle& vnTri,//对应的在模型中的坐标
	const Model* model, const Camera& camera, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz);

bool drawModel(const Camera& camera, const Model* model, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz);

