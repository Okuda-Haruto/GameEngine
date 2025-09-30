#include "LoadObjFile.h"
#include "LoadMaterialTemplateFile.h"
#include <cassert>
#include "LoadObjFile.h"
#include "LoadMaterialTemplateFile.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <list>

//.objファイルからModelDataを構築する
std::vector<ModelData> LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	std::vector<ModelData> modelData;	//構築するModelData
	ModelData modelDatum;				//単体のModelData

	std::list<MaterialDatum> materialData;	//全てのMaterialDataを格納したリスト

	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcoords;	//テクスチャ座標

	std::string line;	//ファイルから読み込んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//開けていない場合止める

	//テキストファイルを1行づつ読み込む
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			materialData = LoadMaterialTemplateFile(directoryPath, materialFilename);
		} else if (identifier == "o") {	//モデル名。次のモデルが始まる合図なので、モデルデータを格納しておく

			//そのままでは最初のモデル名に反応してしまうので、中身のないモデルデータは無視する
			if (modelDatum.vertices.size() > 0 && modelDatum.material.textureFilePath.size() > 0) {
				modelData.push_back(modelDatum);
				//初期化
				modelDatum = {};
			}
		} else if (identifier == "v") {	//頂点位置
			Vector4 position;
			s >> position.x >> position.y >> position.z;	//	>>は空白を意味する
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {	//頂点テクスチャ座標
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {	//頂点法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {	//面
			VertexData triangle[3];
			//面は三角形限定。その他は未定義
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');	//	/区切りでインデックスを読んでいく

					//ない場合は通さない
					if (index != "") {
						elementIndices[element] = std::stoi(index);
					}
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];	//1始まりなので添字には-1をつける
				Vector2 texcoord;
				if (texcoords.size() > 0) {
					texcoord = texcoords[elementIndices[1] - 1];
				} else {
					texcoord = {};
				}
				Vector3 normal;
				if (normals.size() > 0) {
					normal = normals[elementIndices[2] - 1];
				} else {
					normal = {};
				}
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順に登録することで、周り順を逆にする
			modelDatum.vertices.push_back(triangle[2]);
			modelDatum.vertices.push_back(triangle[1]);
			modelDatum.vertices.push_back(triangle[0]);
		} else if (identifier == "usemtl") {
			//使用するMaterialの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//マテリアルデータのリストから同じ名称のマテリアルのデータを取得する
			for (const MaterialDatum& materialDatum : materialData) {
				if (materialDatum.materialName == materialFilename) {	//マテリアルの名称は必要ないので、それ以外を移す
					modelDatum.material.materialName.clear();
					modelDatum.material.textureFilePath = materialDatum.textureFilePath;
				}
			}
		}
	}
	//ファイルの終わりでモデルデータを格納する
	modelData.push_back(modelDatum);

	return modelData;
}