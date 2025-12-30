#pragma once
#include <Vector3.h>

struct CameraForGPU {
	Vector3 worldPosition;
	float nearDist; // 透明が始まる距離
	float farDist;  // 完全に透明になる距離
};