#pragma once
#include "ModelData.h"
#include <string>

//.objファイルからModelDataを構築する
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

