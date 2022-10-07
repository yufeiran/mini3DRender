#pragma once
#include<math.h>
#include<iostream>

//该头文件用来放置向量和矩阵的定义以及运算
const double PI = 3.14159265359;

double degToRad(double ang)
{
	return ang * PI / 180.0;
}


//默认向量都是竖着的，左乘矩阵进行变换
//默认加减乘除是作用于x/y/z三维中的，w维要自己再写
struct Vec
{
	double x, y, z, w;
	Vec() :x(0), y(0), z(0),w(0) {}
	
	Vec(double X, double Y, double Z, double W) :x(X), y(Y), z(Z), w(W) {};
	Vec(double X, double Y, double Z) :x(X), y(Y), z(Z), w(1) {};
	double& get(int index) {
		switch (index)
		{
			case 0:return x;
			case 1:return y;
			case 2:return z;
			case 3:return w;
		default:
			return w;
		}
	}
	Vec normal() {
		double diva = 1.0 / sqrt(x * x + y * y + z * z);
		return Vec(x * diva, y * diva, z * diva, w);
	}
	Vec operator-(const Vec& v) {
		return Vec(x - v.x, y - v.y, z - v.z, w);
	}
	Vec operator-() {
		return Vec(-x, -y, -z, w);
	}
	double& operator[](int index) 
	{
		switch (index)
		{
		case 0:return x;
		case 1:return y;
		case 2:return z;
		default:
		case 3:return w;
			break;
		}
	}
	const double operator[](int index) const
	{
		switch (index)
		{
		case 0:return x;
		case 1:return y;
		case 2:return z;
		default:
		case 3:return w;
			break;
		}
	}
};

Vec cross(const Vec& v1, const Vec& v2)
{
	return Vec(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z, 
		v1.x * v2.y - v1.y * v2.x);
}

std::ostream& operator<<(std::ostream& os, const Vec& vec)
{
	os << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
	return os;
}

struct Mat
{
	double m[4][4];
	Mat()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m[i][j] = 0;
			}
		}
	}


	Vec operator*(const Vec& vec);
	Mat operator*(const Mat& mat);

};

Vec Mat::operator*(const Vec& vec)
{
	Vec ans;
	for (int i = 0; i < 4; i++)
	{
		auto& ans_i = ans.get(i);
		ans_i= vec.x * m[i][0] + vec.y * m[i][1] + vec.z * m[i][2] + vec.w * m[i][3];
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

std::ostream& operator<<(std::ostream& os,const Mat&mat)
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

