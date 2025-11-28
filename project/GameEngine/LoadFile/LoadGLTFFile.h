#pragma once
#include "ModelData.h"
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//.gltfファイルからModelDataを構築する
ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

Node ReadNode(aiNode* node);