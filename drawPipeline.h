#pragma once
#include"mini3DRender.h"

struct VPoint //�������ջ��Ƶ���Ļ�ϵ�Ķ���ṹ
{
	Vec pointScreen;
	Vec pointCamera;
	UVPair uv;
};



void DrawLine(double x0, double y0, double z0, double x1, double y1, double z1, const Color& color = Color(255, 255, 255));

void DrawScanLine(double x0, double z0, double x1, double z1, double y,
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3, 
	const Model* model);

void DrawTopFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3, 
	const VPoint& vp1, const VPoint& vp2, const VPoint& vp3, 
	const Model* model);
void DrawBottomFlatTriangle(const Vec& v1, const Vec& v2, const Vec& v3,
	const VPoint&vp1,const VPoint&vp2,const VPoint&vp3,
	const Model* model);

void DrawTriangle(const Triangle&vTri,const Triangle&uvTri,const Triangle&vnTri,//��Ӧ����ģ���е�����
	const Model* model,const Camera&camera, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng,const double rotateZAng,
	const double  sx, const double sy, const double sz);



Mat makeWorldToCameraMat(const Vec& Forward, const Vec& side, const Vec& up, const Vec& eye);
Mat makeCameraToClipMat(const double viewAng, const double aspect, double n, double f);
Mat makeClipToScreenMat(const double x0, const double y0, const double width, const double height, const double Zmin, const double Zmax);

Vec tannsformObjToWorld(const Vec& objVec, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng,
	const double  sx, const double sy, const double sz);

Vec transformWorldToCamera(const Camera&camera, const Vec& worldVec);

Vec transformCameraToClip(const Camera& camera, const Vec& cameraVec);

Vec transformClipToScreen(const Camera& camera, const Vec& clipVec);

bool drawModel(const Camera& camera, const Model* model, const Vec& moveVec,
	const double rotateXAng, const double rotateYAng, const double rotateZAng, 
	const double  sx, const double sy, const double sz);



UVPair calUVByScreenPoint(const Vec& P, const Vec& A, const Vec& B, const Vec& C,
	const UVPair& uvA, const UVPair& uvB, const UVPair& uvC);