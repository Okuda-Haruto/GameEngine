#include "Object_3D.h"

#include <numbers>

#include "Matrix4x4_operation.h"
#include "Vector3_operation.h"
#include "Collision.h"

#include "CreateBufferResource.h"
#include "LoadObjFile.h"
#include "GetDescriptorHandle.h"

#include "GameEngine.h"

# pragma region Object_3D

Object_3D::~Object_3D() {
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}

void Object_3D::Initialize(const std::string& directoryPath, const std::string& filename) {

	device_ = GameEngine::GetDevice();

	//モデル読み込み
	modelData_ = LoadObjFile(directoryPath, filename);

	//マテリアルの数だけ行う
	for (ModelData modelDatum : modelData_) {

		ObjectData objectDatum{};

		//頂点リソースを作る
		objectDatum.vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelDatum.vertices.size());

		//頂点バッファビューを作成する
		//リソースの先頭のアドレスから使う
		objectDatum.vertexBufferView_.BufferLocation = objectDatum.vertexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点のサイズ
		objectDatum.vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelDatum.vertices.size());
		//1頂点あたりのサイズ
		objectDatum.vertexBufferView_.StrideInBytes = sizeof(VertexData);

		//頂点リソースにデータを書き込む
		objectDatum.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&objectDatum.vertexData_));	//書き込むためのアドレスを取得

		std::memcpy(objectDatum.vertexData_, modelDatum.vertices.data(), sizeof(VertexData) * modelDatum.vertices.size());	//頂点データにリソースにコピー

		objectDatum.vertexResource_->Unmap(0, nullptr);

		//Sprite用のインデックスリソースを作る
		objectDatum.indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * modelDatum.vertices.size());

		//インデックスバッファビューを作成する
		//リソースの先頭のアドレスから使う
		objectDatum.indexBufferView_.BufferLocation = objectDatum.indexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズはインデックスは3つサイズ
		objectDatum.indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelDatum.vertices.size());
		//インデックスはuint32_tとする
		objectDatum.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		//インデックスデータを書き込む
		objectDatum.indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&objectDatum.indexData_));

		//3角形2つを組合わせ4角形にする
		for (int i = 0; i < modelDatum.vertices.size(); i++) {
			objectDatum.indexData_[i] = i;
		}

		objectDatum.indexResource_->Unmap(0, nullptr);
		objectData_.push_back(objectDatum);
	}
	
	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Object_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_Multi_Data& data) {

	assert(light_ != nullptr || isLighting_ == false);	//Lightがセットされていない場合止める

	for (INT i = 0; i < objectData_.size();i++) {
		//WVPデータを更新
		wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

		Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
		wvpData_[index_]->World = worldMatrix;
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera_->GetViewMatrix(), camera_->GetProjectionMatrix()));
		wvpData_[index_]->WVP = worldViewProjectionMatrix;

		wvpResource_[index_]->Unmap(0, nullptr);

		//マテリアルデータを更新
		materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

		materialData_[index_]->color = data.material[i].color;
		materialData_[index_]->uvTransform = MakeAffineMatrix(data.material[i].uvTransform.scale, data.material[i].uvTransform.rotate, data.material[i].uvTransform.translate);
		materialData_[index_]->enableLighting = isLighting_;

		materialResource_[index_]->Unmap(0, nullptr);

		commandList->IASetVertexBuffers(0, 1, &objectData_[i].vertexBufferView_);	//VBVを設定
		commandList->IASetIndexBuffer(&objectData_[i].indexBufferView_);	//IBVを設定
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList->SetGraphicsRootDescriptorTable(2, GameEngine::TextureGet(data.material[i].textureIndex));
		if (isLighting_) {
			commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
		}

		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
		commandList->SetPipelineState(GameEngine::TrianglePSO());	//PSOを設定
		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//描画(DrawCall)
		commandList->DrawIndexedInstanced(UINT(modelData_[i].vertices.size()), 1, 0, 0, 0);

		//最大値を超えたら最初から
		index_++;
		if (index_ >= kMaxIndex_) {
			index_ = 0;
		}
	}
}

# pragma endregion

# pragma region Sprite_3D

Sprite_3D::~Sprite_3D() {
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
	vertexData_ = nullptr;
}

void Sprite_3D::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight) {

	device_ = device;

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Sprite_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_Multi_Data& data) {

	assert(light_ != nullptr || isLighting_ == true);	//Lightがセットされていない場合止める

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { -spriteWidth_ / 2 / kWindowWidth_,-spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { -spriteWidth_ / 2 / kWindowWidth_,spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { spriteWidth_ / 2 / kWindowWidth_,-spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { spriteWidth_ / 2 / kWindowWidth_,spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	Matrix4x4 inverseMatrix = Inverse(camera_->GetViewMatrix());

	//カメラからスプライトへの距離
	Vector3 CameraToSprite{
		data.transform.translate.x - inverseMatrix.m[3][0],
		0,
		data.transform.translate.z - inverseMatrix.m[3][2],
	};
	//正規化
	CameraToSprite = Normalize(CameraToSprite);

	Vector3 rotate{};
	rotate = data.transform.rotate;
	if (CameraToSprite.z > 0) {
		rotate.y = sinf(CameraToSprite.x) * std::numbers::pi_v<float> / 2;
	} else {
		rotate.y = sinf(CameraToSprite.x) * -std::numbers::pi_v<float> / 2 + std::numbers::pi_v<float>;
	}
	data.transform.rotate = rotate;

	//WVPデータを更新
	wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, rotate, data.transform.translate);
	wvpData_[index_]->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera_->GetViewMatrix(), camera_->GetProjectionMatrix()));
	wvpData_[index_]->WVP = worldViewProjectionMatrix;

	wvpResource_[index_]->Unmap(0, nullptr);
	//マテリアルデータを更新

	materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

	materialData_[index_]->color = data.material[0].color;
	materialData_[index_]->uvTransform = MakeAffineMatrix(data.material[0].uvTransform.scale, data.material[0].uvTransform.rotate, data.material[0].uvTransform.translate);
	materialData_[index_]->enableLighting = isLighting_;

	materialResource_[index_]->Unmap(0, nullptr);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, GameEngine::TextureGet(data.material[0].textureIndex));
	if (isLighting_) {
		commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
	}

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
	commandList->SetPipelineState(GameEngine::TrianglePSO());	//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画(DrawCall)
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex_) {
		index_ = 0;
	}
}

void Sprite_3D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

# pragma endregion

# pragma region Line_3D

Line_3D::~Line_3D() {
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
	vertexData_ = nullptr;
}

void Line_3D::Initialize(const Vector3& start, const Vector3& end, Microsoft::WRL::ComPtr<ID3D12Device> device) {

	device_ = device;

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 2);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * 2);
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));	//書き込むためのアドレスを取得

	//始点
	vertexData_[0].position = { start.x, start.y, start.z,1.0f };
	vertexData_[0].texcoord = { 0.0f,0.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//終点
	vertexData_[1].position = { end.x, end.y, end.z,1.0f };
	vertexData_[1].texcoord = { 1.0f,1.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * 2);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは3つサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * 2);
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//2頂点の組み合わせ
	indexData_[0] = 0; indexData_[1] = 1;

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Line_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_Single_Data& data) {

	//WVPデータを更新
	wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
	wvpData_[index_]->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera_->GetViewMatrix(), camera_->GetProjectionMatrix()));
	wvpData_[index_]->WVP = worldViewProjectionMatrix;

	wvpResource_[index_]->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

	materialData_[index_]->color = data.material.color;
	materialData_[index_]->uvTransform = MakeAffineMatrix(data.material.uvTransform.scale, data.material.uvTransform.rotate, data.material.uvTransform.translate);
	materialData_[index_]->enableLighting = false;

	materialResource_[index_]->Unmap(0, nullptr);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, GameEngine::TextureGet(data.material.textureIndex));

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());


	commandList->SetPipelineState(GameEngine::LinePSO());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//描画(DrawCall)
	commandList->DrawIndexedInstanced(2, 1, 0, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex_) {
		index_ = 0;
	}
}

void Line_3D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}

# pragma endregion

# pragma region AxisIndicator

AxisIndicator::~AxisIndicator() {
	wvpData_ = nullptr;
	materialData_ = nullptr;
}

void AxisIndicator::Initialize(Camera* camera) {

	device_ = GameEngine::GetDevice();

	camera_ = camera;

	//モデル読み込み
	modelData_ = LoadObjFile("resources/DebugResources/axisIndicator", "axisIndicator.obj");

	//マテリアルの数だけ行う
	for (ModelData modelDatum : modelData_) {

		ObjectData objectDatum{};

		//頂点リソースを作る
		objectDatum.vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelDatum.vertices.size());

		//頂点バッファビューを作成する
		//リソースの先頭のアドレスから使う
		objectDatum.vertexBufferView_.BufferLocation = objectDatum.vertexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点のサイズ
		objectDatum.vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelDatum.vertices.size());
		//1頂点あたりのサイズ
		objectDatum.vertexBufferView_.StrideInBytes = sizeof(VertexData);

		//頂点リソースにデータを書き込む
		objectDatum.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&objectDatum.vertexData_));	//書き込むためのアドレスを取得

		std::memcpy(objectDatum.vertexData_, modelDatum.vertices.data(), sizeof(VertexData) * modelDatum.vertices.size());	//頂点データにリソースにコピー

		objectDatum.vertexResource_->Unmap(0, nullptr);

		//Sprite用のインデックスリソースを作る
		objectDatum.indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * modelDatum.vertices.size());

		//インデックスバッファビューを作成する
		//リソースの先頭のアドレスから使う
		objectDatum.indexBufferView_.BufferLocation = objectDatum.indexResource_->GetGPUVirtualAddress();
		//使用するリソースのサイズはインデックスは3つサイズ
		objectDatum.indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelDatum.vertices.size());
		//インデックスはuint32_tとする
		objectDatum.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		//インデックスデータを書き込む
		objectDatum.indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&objectDatum.indexData_));

		//3角形2つを組合わせ4角形にする
		for (int i = 0; i < modelDatum.vertices.size(); i++) {
			objectDatum.indexData_[i] = i;
		}

		objectDatum.indexResource_->Unmap(0, nullptr);

		textureIndex_ = GameEngine::TextureLoad(modelDatum.material.textureFilePath);
		objectData_.push_back(objectDatum);
	}

	wvpResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	wvpData_ = nullptr;
	materialResource_ = CreateBufferResource(device_, sizeof(Material));
	materialData_ = nullptr;
}

void AxisIndicator::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {

	//WVPデータを更新
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

	Matrix4x4 CameraMatrix = Inverse(camera_->GetViewMatrix());
	Vector3 size{ 0.01f,0.01f,0.01f };
	Matrix4x4 worldMatrix;
	worldMatrix.m[0][0] = CameraMatrix.m[0][0] * size.x	; worldMatrix.m[0][1] = CameraMatrix.m[0][1] * size.x	; worldMatrix.m[0][2] = CameraMatrix.m[0][2] * size.x	; worldMatrix.m[0][3] = 0;
	worldMatrix.m[1][0] = CameraMatrix.m[1][0] * size.y	; worldMatrix.m[1][1] = CameraMatrix.m[1][1] * size.y	; worldMatrix.m[1][2] = CameraMatrix.m[1][2] * size.y	; worldMatrix.m[0][3] = 0;
	worldMatrix.m[2][0] = CameraMatrix.m[2][0] * size.z	; worldMatrix.m[2][1] = CameraMatrix.m[2][1] * size.z	; worldMatrix.m[2][2] = CameraMatrix.m[2][2] * size.z	; worldMatrix.m[0][3] = 0;
	worldMatrix.m[3][0] = 0.375f							; worldMatrix.m[3][1] = 0.2f							; worldMatrix.m[3][2] = 1								; worldMatrix.m[3][3] = 1;

	wvpData_->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(MakeIdentity4x4(), camera_->GetProjectionMatrix()));
	wvpData_->WVP = worldViewProjectionMatrix;

	wvpResource_->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	materialData_->color = {1.0f,1.0f,1.0f,1.0f};
	materialData_->uvTransform = MakeIdentity4x4();

	materialResource_->Unmap(0, nullptr);

	for (INT i = 0; i < objectData_.size(); i++) {
		commandList->IASetVertexBuffers(0, 1, &objectData_[i].vertexBufferView_);	//VBVを設定
		commandList->IASetIndexBuffer(&objectData_[i].indexBufferView_);	//IBVを設定
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList->SetGraphicsRootDescriptorTable(2, GameEngine::TextureGet(textureIndex_));

		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		commandList->SetPipelineState(GameEngine::TrianglePSO());	//PSOを設定
		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//描画(DrawCall)
		commandList->DrawIndexedInstanced(UINT(modelData_[i].vertices.size()), 1, 0, 0, 0);
	}
}

# pragma endregion

# pragma region Grid

Grid::~Grid() {
	delete lineX_;
	delete lineY_;
	delete lineZ_;
}

void Grid::Initialize(Camera* camera) {

	camera_ = camera;

	lineX_ = new Line_3D;
	lineX_->Initialize({ 0.0f,0.0f,0.0f }, { float(kGridLength_ * 2),0.0f,0.0f }, GameEngine::GetDevice());
	lineY_ = new Line_3D;
	lineY_->Initialize({ 0.0f,0.0f,0.0f }, { 0.0f,float(kGridLength_ * 2),0.0f }, GameEngine::GetDevice());
	lineZ_ = new Line_3D;
	lineZ_->Initialize({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,float(kGridLength_ * 2) }, GameEngine::GetDevice());

}

void Grid::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {
	lineX_->SetCamera(camera_);
	lineY_->SetCamera(camera_);
	lineZ_->SetCamera(camera_);
	Object_Single_Data data;
	Matrix4x4 CameraMatrix = Inverse(camera_->GetViewMatrix());
	centerPoint_ = camera_->GetCenterPoint();
	centerPoint_ = {
		std::round(centerPoint_.x),
		std::round(centerPoint_.y),
		std::round(centerPoint_.z)
	};

	if (xy_ && std::abs(CameraMatrix.m[3][2]) > 0.01f) {
		for (int i = 0; i <= kGridLength_ * 2; i++) {
			data.transform.translate = { float(-kGridLength_ + centerPoint_.x),float(-kGridLength_ + centerPoint_.y) + i,0.0f };
			data.material.color = {0.2f,0.2f,0.2f,1.0f};
			if (data.transform.translate.y == 0.0f) {
				data.material.color = { 0.8f,0.0f,0.0f,1.0f };
				data.transform.translate.z = 0.0004f;
			} else if (int(data.transform.translate.y) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.z = 0.0002f;
			} else if (int(data.transform.translate.y) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.z = 0.0001f;
			}
			lineX_->Draw(commandList, data);
			data.transform.translate = { float(-kGridLength_ + centerPoint_.x) + i,float(-kGridLength_ + centerPoint_.y),0.0f };
			data.material.color = { 0.2f,0.2f,0.2f,1.0f };
			if (data.transform.translate.x == 0.0f) {
				data.material.color = { 0.0f,0.8f,0.0f,1.0f };
				data.transform.translate.z = 0.0003f;
			} else if (int(data.transform.translate.x) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.z = 0.0002f;
			} else if (int(data.transform.translate.x) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.z = 0.0001f;
			}
			lineY_->Draw(commandList, data);
		}
	}
	if (xz_ && std::abs(CameraMatrix.m[3][1]) > 0.01f) {
		for (int i = 0; i <= kGridLength_ * 2; i++) {
			data.transform.translate = { float(-kGridLength_ + centerPoint_.x),0.0f,float(-kGridLength_ + centerPoint_.z) + i };
			data.material.color = { 0.2f,0.2f,0.2f,1.0f };
			if (data.transform.translate.z == 0.0f) {
				data.material.color = { 0.8f,0.0f,0.0f,1.0f };
				data.transform.translate.y = 0.0004f;
			} else if (int(data.transform.translate.z) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.y = 0.0002f;
			} else if (int(data.transform.translate.z) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.y = 0.0001f;
			}
			lineX_->Draw(commandList, data);
			data.transform.translate = { float(-kGridLength_ + centerPoint_.x) + i,0.0f,float(-kGridLength_ + centerPoint_.z) };
			data.material.color = { 0.2f,0.2f,0.2f,1.0f };
			if (data.transform.translate.x == 0.0f) {
				data.material.color = { 0.0f,0.0f,0.8f,1.0f };
				data.transform.translate.y = 0.0003f;
			} else if (int(data.transform.translate.x) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.y = 0.0002f;
			} else if (int(data.transform.translate.x) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.y = 0.0001f;
			}
			lineZ_->Draw(commandList, data);
		}
	}
	if (yz_ && std::abs(CameraMatrix.m[3][0]) > 0.01f) {
		for (int i = 0; i <= kGridLength_ * 2; i++) {
			data.transform.translate = { 0.0f,float(-kGridLength_ + centerPoint_.y),float(-kGridLength_ + centerPoint_.z) + i };
			data.material.color = { 0.2f,0.2f,0.2f,1.0f };
			if (data.transform.translate.z == 0.0f) {
				data.material.color = { 0.0f,0.8f,0.0f,1.0f };
				data.transform.translate.x = 0.0004f;
			} else if (int(data.transform.translate.z) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.x = 0.0002f;
			} else if (int(data.transform.translate.z) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.x = 0.0001f;
			}
			lineY_->Draw(commandList, data);
			data.transform.translate = { 0.0f,float(-kGridLength_ + centerPoint_.y) + i,float(-kGridLength_ + centerPoint_.z) };
			data.material.color = { 0.2f,0.2f,0.2f,1.0f };
			if (data.transform.translate.y == 0.0f) {
				data.material.color = { 0.0f,0.0f,0.8f,1.0f };
				data.transform.translate.x = 0.0003f;
			} else if (int(data.transform.translate.y) % 100 == 0) {
				data.material.color = { 1.0f,1.0f,1.0f,1.0f };
				data.transform.translate.x = 0.0002f;
			} else if (int(data.transform.translate.y) % 10 == 0) {
				data.material.color = { 0.5f,0.5f,0.5f,1.0f };
				data.transform.translate.x = 0.0001f;
			}
			lineZ_->Draw(commandList, data);
		}
	}
}

# pragma endregion