#pragma once
#include "SRT.h"
#include "Material.h"
#include "Model/Model.h"



struct Parts {
	UINT textureIndex;
	std::shared_ptr<Material> material = nullptr;
	std::shared_ptr<SRT> parent = nullptr;
	std::shared_ptr<SRT> transform = nullptr;
	SRT UVtransform;
};