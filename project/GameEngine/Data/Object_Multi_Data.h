#pragma once
#include <ModelData.h>
#include <ObjectMaterial.h>
#include <vector>

struct Object_Multi_Data {
	SRT transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	std::vector<ObjectMaterial> material;

	void SetMaterial(ModelData modelData);
	void SetMaterial(std::vector<ModelData> modelData);
};