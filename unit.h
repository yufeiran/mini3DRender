#pragma once
#ifndef UNIT_H
#define UNIT_H

#include<math.h>
#include<iostream>

//该头文件用来放置向量和矩阵的定义以及运算
const double PI = 3.14159265359;

double degToRad(double ang);

//默认向量都是竖着的，左乘矩阵进行变换
//默认加减乘除是作用于x/y/z三维中的，w维要自己再写
struct Vec
{
	double x, y, z, w;
	Vec() :x(0), y(0), z(0), w(0) {}

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
	Vec operator+(const Vec& v)const {
		return Vec(x + v.x, y + v.y, z + v.z, w);
	}
	Vec operator-(const Vec& v)const {
		return Vec(x - v.x, y - v.y, z - v.z, w);
	}
	Vec operator-() const {
		return Vec(-x, -y, -z, w);
	}
	Vec operator*(double n) const {
		return Vec(x * n, y * n, z * n, w);
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

Vec cross(const Vec& v1, const Vec& v2);

Vec operator*(double n, const Vec& v);

std::ostream& operator<<(std::ostream& os, const Vec& vec);

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


Mat makeRotateByXMat(double theta);

Mat makeRotateByYMat(double theta);

Mat makeRotateByZMat(double theta);

Mat makeMoveMat(double x, double y, double z);

Mat makeScaleMat(double sx, double sy, double sz);

std::ostream& operator<<(std::ostream& os, const Mat& mat);



struct Triangle
{
	int pointIndex[3];
	Triangle(int ia, int ib, int ic) {
		pointIndex[0] = ia;
		pointIndex[1] = ib;
		pointIndex[2] = ic;
	}
};
#endif // !UNIT_H
