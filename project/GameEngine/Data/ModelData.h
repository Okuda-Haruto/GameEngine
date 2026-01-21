#pragma once
#include <vector>
#include <string>
#include "ObjectVertexData.h"
#include "Offset.h"
#include "Bone.h"
#include "AnimationData.h"
#include "MaterialData.h"
#include <Node.h>

struct ModelData {
	std::vector<ObjectVertexData> vertices;
	std::vector<uint32_t> indexes;
	std::vector<Offset> offset;
	std::vector<UINT> textureIndex;
	std::vector<AnimationData> animations;
	std::vector<Bone> bones;
	MaterialData material;
	std::shared_ptr<Node> rootNode;
};