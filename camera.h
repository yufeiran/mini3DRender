#pragma once
#include"unit.h"
struct Camera
{
	Vec eye=Vec(2.5, 1.2, 0);
	Vec lookat = Vec(0, 0, 0);
	Vec up = Vec(0, 1, 0);

	Vec Forward = lookat - eye;
	Vec side;

	double viewAng = 45;
	double aspect = 4.0 / 3.0;
	double n = 2;
	double f = 300;

	double X0 = 0;
	double Y0 = 0;

	double width = 800;
	double height = 600;

	double Zmin = 0;
	double Zmax = 1;

	void update();
};