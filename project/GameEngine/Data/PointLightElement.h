#pragma once
#include <Vector3.h>
#include <Vector4.h>

struct PointLightElement {
	Vector4 color;	//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//輝度
	float radius;	//ライトの届く最大距離
	float decay;	//減衰率
	float padding[2];
};
