#include "LoadObjFile.h"
#include "LoadMaterialTemplateFile.h"
#include "TextureManager/TextureManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <list>
#include <GameEngine.h>

//.objファイルからModelDataを構築する
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	ModelData modelData;	//構築するModelData

	std::list<MaterialData> materialData;	//全てのMaterialDataを格納したリスト

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
			if (modelData.vertices.size() > 0 && !modelData.textureIndex.empty()) {
				Offset offset{};	//オフセット

				//開始地点
				if (modelData.offset.size() > 0) {
					size_t offsetIndex = modelData.offset.size() - 1;
					offset.vertexStart = modelData.offset[offsetIndex].vertexStart + modelData.offset[offsetIndex].vertexCount;
				} else {
					offset.vertexStart = 0;
				}

				//頂点数
				offset.vertexCount = UINT(modelData.vertices.size()) - offset.vertexStart;

				//インデックスと頂点の要素は同一
				offset.indexStart = offset.vertexStart;
				offset.indexCount = offset.vertexCount;

				modelData.offset.push_back(offset);
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
			ObjectVertexData triangle[3]{};
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
				triangle[faceVertex].position = position;
				triangle[faceVertex].texcoord = texcoord;
				triangle[faceVertex].normal = normal;
			}
			//頂点を逆順に登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if (identifier == "usemtl") {
			//使用するMaterialの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//マテリアルデータのリストから同じ名称のマテリアルのデータを取得する
			for (const MaterialData& MaterialData : materialData) {
				if (MaterialData.materialName == materialFilename) {	//マテリアルの名称は必要ないので、それ以外を移す
					modelData.textureIndex.push_back(TextureManager::GetInstance()->GetSrvIndex(MaterialData.textureFilepath));
				}
			}
		}
	}

	Offset offset{};	//オフセット

	//開始地点
	if (modelData.offset.size() > 0) {
		size_t offsetIndex = modelData.offset.size() - 1;
		offset.vertexStart = modelData.offset[offsetIndex].vertexStart + modelData.offset[offsetIndex].vertexCount;
	} else {
		offset.vertexStart = 0;
	}

	//頂点数
	offset.vertexCount = UINT(modelData.vertices.size()) - offset.vertexStart;

	//インデックスと頂点の要素は同一
	offset.indexStart = offset.vertexStart;
	offset.indexCount = offset.vertexCount;

	modelData.offset.push_back(offset);

	//3角形2つを組合わせ4角形にする
	for (uint32_t i = 0; i < modelData.vertices.size(); i++) {
		modelData.indexes.push_back(i);
	}

	return modelData;
}