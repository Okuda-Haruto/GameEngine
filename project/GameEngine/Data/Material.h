#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <format>

struct Material {
	Vector4 color;						//色
	int32_t reflection;					//反射方法
	int32_t enableDirectionalLighting;
	int32_t enablePointLighting;
	int32_t enableSpotLighting;
	Matrix4x4 uvTransform;				//UV行列
	float shininess = 0.0f;				//反射輝度
};