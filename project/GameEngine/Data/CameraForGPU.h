#pragma once
#include <Vector3.h>

struct CameraForGPU {
	Vector3 worldPosition;
	float nearDist;
	float farDist;
	float nearTransparentDist;
	float farTransparentDist;
};