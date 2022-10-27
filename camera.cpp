#include"camera.h"


void Camera::update()
{

	Forward = lookat - eye;
	Forward = Forward.normal();

	up = up.normal();

	side = cross(Forward, up);

	up = cross(side, Forward);


}