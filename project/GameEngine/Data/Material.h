#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <format>

struct Material {
	Vector4 color;
	int32_t reflection;
	int32_t enableDirectionalLighting;
	int32_t enablePointLighting;
	int32_t enableSpotLighting;
	Matrix4x4 uvTransform;
	float shininess = 0.0f;
};