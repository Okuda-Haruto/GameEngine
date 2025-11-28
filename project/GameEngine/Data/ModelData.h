#pragma once
#include <vector>
#include <string>
#include "VertexData.h"
#include "Offset.h"
#include "MaterialData.h"
#include <Node.h>

struct ModelData {
	std::vector<VertexData> vertices;
	std::vector<Offset> offset;
	std::vector<UINT> textureIndex;
	MaterialData material;
	Node rootNode;
};