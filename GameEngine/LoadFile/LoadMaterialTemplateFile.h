#pragma once
#include "MaterialData.h"
#include <list>

std::list<MaterialDatum> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);