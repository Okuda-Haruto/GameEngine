#include "Object.h"
#include <GameEngine.h>
#include <LoadObjFile.h>
#include <CreateBufferResource.h>
#include <Matrix4x4_operation.h>

void Object::Initialize(const std::string& directoryPath, const std::string& filename) {
	device_ = GameEngine::GetDevice();

	//モデル読み込み
	std::vector<ModelData> modelData_ = LoadObjFile(directoryPath, filename);

	//モデルの数だけ行う
	for (ModelData modelDatum : modelData_) {
		Parts part;

		//頂点リソースを作る
		part.model.vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelDatum.vertices.size());

		//頂点バッファビューを作成する
		//リソースの先頭のアドレスから使う
		part.model.vertexBufferView_.BufferLocation = part.model.vertexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点のサイズ
		part.model.vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelDatum.vertices.size());
		//1頂点あたりのサイズ
		part.model.vertexBufferView_.StrideInBytes = sizeof(VertexData);

		//頂点リソースにデータを書き込む
		part.model.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&part.model.vertexData_));	//書き込むためのアドレスを取得

		std::memcpy(part.model.vertexData_, modelDatum.vertices.data(), sizeof(VertexData) * modelDatum.vertices.size());	//頂点データにリソースにコピー

		part.model.vertexResource_->Unmap(0, nullptr);

		//Sprite用のインデックスリソースを作る
		part.model.indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * modelDatum.vertices.size());

		//インデックスバッファビューを作成する
		//リソースの先頭のアドレスから使う
		part.model.indexBufferView_.BufferLocation = part.model.indexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズはインデックスは3つサイズ
		part.model.indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelDatum.vertices.size());
		//インデックスはuint32_tとする
		part.model.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		//インデックスデータを書き込む
		part.model.indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&part.model.indexData_));

		//3角形2つを組合わせ4角形にする
		for (int i = 0; i < modelDatum.vertices.size(); i++) {
			part.model.indexData_[i] = i;
		}

		part.model.indexResource_->Unmap(0, nullptr);

		part.model.vertexIndex_ = UINT(modelDatum.vertices.size());

		if (modelDatum.textureIndex != -1) {
			part.textureIndex = modelDatum.textureIndex;
		}

		part.transform = {};
		part.transform.scale = { 1.0f,1.0f,1.0f };
		part.material.uvTransform = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });

		parts_.push_back(part);
	}

	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
}

void Object::Draw3D(Camera* camera, int reflection, float shininess, DirectionalLight* directionalLight, PointLight* pointLight) {
	GameEngine::DrawObject_3D(this, camera, reflection, shininess, directionalLight, pointLight);
}