#pragma once
#include <vector>
#include "VertexData.h"
#include <windows.h>

struct ModelData {
	std::vector<VertexData> vertices;
	int16_t textureIndex = -1;
};