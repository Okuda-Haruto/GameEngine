#pragma once
#include "ModelData.h"
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//.gltfファイルからModelDataを構築する
ModelData LoadGLTFFile(const std::string& directoryPath, const std::string& filename);

std::shared_ptr<Node> ReadNode(aiNode* node);

std::shared_ptr<Node>& FindNode(std::shared_ptr<Node>& node, const std::string& name);

void SetVertexWeight(UINT4& ids, Vector4& weights, UINT id, float weight);