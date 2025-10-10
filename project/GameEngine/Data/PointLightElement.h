#pragma once
#include <Vector3.h>
#include <Vector4.h>

struct PointLightElement {
	Vector4 color;	//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//輝度
	float radius;
	float decay;
	float padding[2];
};
