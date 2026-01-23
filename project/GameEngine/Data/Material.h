#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <format>

enum REFLECTION {
	REFLECTION_None,		//反射しない
	REFLECTION_Lambert,		//ランバート反射。現実と同じ反射だがあまり使うのはお勧めしない
	REFLECTION_HalfLambert,	//ハーフランバート反射。現実に見た目が似るように工夫された反射
};

enum SHADING {
	SHADING_Phong,			
	SHADING_Blinn_Phong,
};

struct Material {
	Vector4 color;						//色
	int32_t reflection;					//反射方法
	int32_t enableDirectionalLighting;
	int32_t enablePointLighting;
	int32_t enableSpotLighting;
	Matrix4x4 uvTransform;				//UV行列
	float shininess = 0.0f;				//反射輝度
	int32_t shading;					//フォンシェーディング
	float padding[3];
};