#include "LoadMaterialTemplateFile.h"
#include <sstream>
#include <fstream>
#include <cassert>

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//変数の宣言

	MaterialData materialData;	//構築するMaterialData
	std::string line;	//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//開けていない場合止める

	//MaterialDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {	//map_Kdにはtextureのファイル名が記載されている
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}