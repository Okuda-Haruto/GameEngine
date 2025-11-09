#pragma once
#include "SRT.h"
#include "Material.h"
#include "Model/Model.h"

struct Parts {
	UINT textureIndex;
	Material* material = nullptr;
	SRT transform;
	SRT UVtransform;
};