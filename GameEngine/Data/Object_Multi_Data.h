#pragma once
#include <ObjectMaterial.h>
#include <vector>

struct Object_Multi_Data {
	SRT transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	std::vector<ObjectMaterial> material;
};