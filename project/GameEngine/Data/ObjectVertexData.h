#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "UINT4.h"

struct ObjectVertexData {
	Vector4 position;
	Vector2 texcoord;
	float padding[2];
	Vector3 normal;
	float paddingNormal;  // 4byte padding → 16byte境界に揃える
	UINT4 boneID;
	Vector4 boneWeight;
};
static_assert(sizeof(ObjectVertexData) % 16 == 0, "Vertex struct not 16-byte aligned");