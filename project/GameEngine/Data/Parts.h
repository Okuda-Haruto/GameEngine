#pragma once
#include "SRT.h"
#include "Material.h"
#include "Model/Model.h"

using namespace std;

struct Parts {
	UINT textureIndex;
	shared_ptr<Material> material = nullptr;
	shared_ptr<SRT> parent = nullptr;
	shared_ptr<SRT> transform = nullptr;
	SRT UVtransform;
};