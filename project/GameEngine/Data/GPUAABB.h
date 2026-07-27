#pragma once
#include <Vector3.h>

struct GPUAABB {
	Vector3 min;
	float padding0;
	Vector3 max;
	float padding1;
};