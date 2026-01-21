#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "UINT4.h"

struct ObjectVertexData {
	Vector4 position;
	Vector2 texcoord;
	float _pad0[2];
	Vector3 normal;
	float _pad1;
	UINT4 boneID;
	Vector4 boneWeight;
};
