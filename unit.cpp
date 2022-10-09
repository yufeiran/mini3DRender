#include"unit.h"

double degToRad(double ang)
{
	return ang * PI / 180.0;
}



Vec cross(const Vec& v1, const Vec& v2)
{
	return Vec(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

Vec operator*(double n, const Vec& v) {
	return v * n;
}

std::ostream& operator<<(std::ostream& os, const Vec& vec)
{
	os << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
	return os;
}


Vec Mat::operator*(const Vec& vec)
{
	Vec ans;
	for (int i = 0; i < 4; i++)
	{
		auto& ans_i = ans.get(i);
		ans_i = vec.x * m[i][0] + vec.y * m[i][1] + vec.z * m[i][2] + vec.w * m[i][3];
	}
	return ans;
}


Mat Mat::operator*(const Mat& mat)
{
	Mat ans;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				ans.m[i][j] += this->m[i][k] * mat.m[k][j];
			}
		}
	}

	return ans;
}

Mat makeRotateByXMat(double theta)
{
	double ang = degToRad(theta);
	Mat m;
	m.m[0][0] = 1;
	m.m[1][1] = cos(ang);
	m.m[1][2] = -sin(ang);
	m.m[2][1] = sin(ang);
	m.m[2][2] = cos(ang);
	m.m[3][3] = 1;
	return m;
}

Mat makeRotateByYMat(double theta)
{
	double ang = degToRad(theta);
	Mat m;
	m.m[0][0] = cos(ang);
	m.m[0][2] = sin(ang);
	m.m[1][1] = 1;
	m.m[2][0] = -sin(ang);
	m.m[2][2] = cos(ang);
	m.m[3][3] = 1;
	return m;
}

Mat makeRotateByZMat(double theta)
{
	double ang = degToRad(theta);
	Mat m;
	m.m[0][0] = cos(ang);
	m.m[0][1] = -sin(ang);
	m.m[1][0] = sin(ang);
	m.m[1][1] = cos(ang);
	m.m[2][2] = 1;
	m.m[3][3] = 1;
	return m;
}

Mat makeMoveMat(double x, double y, double z)
{
	Mat m;
	m.m[0][3] = x;
	m.m[1][3] = y;
	m.m[2][3] = z;
	m.m[0][0] = 1;
	m.m[1][1] = 1;
	m.m[2][2] = 1;
	m.m[3][3] = 1;
	return m;
}

Mat makeScaleMat(double sx, double sy, double sz)
{
	Mat m;
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	m.m[3][3] = 1;
	return m;
}

std::ostream& operator<<(std::ostream& os, const Mat& mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			os << mat.m[i][j] << " ";
		}
		os << std::endl;
	}

	return os;
}
