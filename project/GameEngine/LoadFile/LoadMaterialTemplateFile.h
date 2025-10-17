#pragma once
#include "MaterialData.h"
#include <list>

std::list<MaterialData> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);