#pragma once
#include "SRT.h"
#include "Material.h"
#include "Model/Model.h"

struct Parts {
	Material material;
	Model model;
	UINT textureIndex = 0;
	SRT transform;
};