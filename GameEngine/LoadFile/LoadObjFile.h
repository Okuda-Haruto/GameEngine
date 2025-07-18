#pragma once
#include "ModelData.h"

//.objファイルからModelDataを構築する
std::vector<ModelData> LoadObjFile(const std::string& directoryPath, const std::string& filename);

