#include"camera.h"


void Camera::update()
{
	if (cameraMode == SurroundCameraMode)
	{
		updateSurroundCameraMode();
	}
	

	Forward = lookat - eye;
	Forward = Forward.normal();

	up = up.normal();

	side = cross(Forward, up);

	up = cross(side, Forward);

}

void Camera::updateSurroundCameraMode()
{
	const Vec offset = { 0,0.1,0 };
	side = { 1.0,-1.0 / (tan(degToRad(surroundCameraPhi))),0 };
	eye = { surroundCameraR * sin(degToRad(surroundCameraTheta)) * cos(degToRad(surroundCameraPhi)),
		surroundCameraR * sin(degToRad(surroundCameraTheta)) * sin(degToRad(surroundCameraPhi)),
		surroundCameraR * cos(degToRad(surroundCameraTheta))
	};

	lookat = { 0,0,0 };

	eye += offset;
	lookat += offset;

}