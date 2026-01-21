#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "UINT4.h"

struct ObjectVertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	UINT4 boneID;
	Vector4 boneWeight;
};
