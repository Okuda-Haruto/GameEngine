#include "LoadMaterialTemplateFile.h"
#include <sstream>
#include <fstream>
#include <cassert>

std::list<MaterialDatum> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//変数の宣言
	std::list<MaterialDatum> materialData;	//構築するMaterialData
	MaterialDatum materialDatum;	//単体のマテリアルデータ
	std::string line;	//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//開けていない場合止める

	//MaterialDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "newmtl") {			//newmtlにマテリアル名が格納されている
			std::string materialName;
			s >> materialName;

			materialDatum.materialName = materialName;

		} else if (identifier == "map_Kd") {	//map_Kdにはtextureのファイル名が記載されている
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialDatum.textureFilePath = directoryPath + "/" + textureFilename;

			//materialDataの最後尾に格納する
			materialData.push_back(materialDatum);
		}
	}
	return materialData;
}