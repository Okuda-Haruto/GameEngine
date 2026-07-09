#include "PrimitiveRing.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "LoadGLTFFile.h"
#include "Animation/Animation.h"
#include <numbers>

PrimitiveRing::~PrimitiveRing() {

}

void PrimitiveRing::Initialize(uint32_t textureIndex, std::shared_ptr<Camera> camera, DirectXCommon* dxCommon) {

	textureIndex_ = textureIndex;

	camera_ = camera;

	const uint32_t kRingDivide = 32;
	const float kOuterRadius = 1.0f;
	const float kInnerRadius = 0.2f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * kRingDivide * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * kRingDivide * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Primitive用のインデックスリソースを作る
	indexCount_ = kRingDivide * 6;
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * indexCount_);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount_;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

#pragma region Ring

		for (uint32_t index = 0; index < kRingDivide; index++) {
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);
			float u = float(index) / float(kRingDivide);
			float uNext = float(index + 1) / float(kRingDivide);

			//positionとuv。normalは必要なら+zを設定する
			vertexData_[index * 4].position =     { -sin * kOuterRadius, cos * kOuterRadius, 0.0f, 1.0f };
			vertexData_[index * 4].texcoord =     { u ,0.0f };
			vertexData_[index * 4].normal =     { 0.0f,0.0f,1.0f };
			vertexData_[index * 4 + 1].position = { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f };
			vertexData_[index * 4 + 1].texcoord = { uNext ,0.0f };
			vertexData_[index * 4 + 1].normal = { 0.0f,0.0f,1.0f };
			vertexData_[index * 4 + 2].position = { -sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f };
			vertexData_[index * 4 + 2].texcoord = { u ,1.0f };
			vertexData_[index * 4 + 2].normal = { 0.0f,0.0f,1.0f };
			vertexData_[index * 4 + 3].position = { -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f, 1.0f };
			vertexData_[index * 4 + 3].texcoord = { uNext ,1.0f };
			vertexData_[index * 4 + 3].normal = { 0.0f,0.0f,1.0f };

			indexData_[index * 6 + 0] = index * 4 + 0; indexData_[index * 6 + 1] = index * 4 + 1; indexData_[index * 6 + 2] = index * 4 + 2;
			indexData_[index * 6 + 3] = index * 4 + 1; indexData_[index * 6 + 4] = index * 4 + 3; indexData_[index * 6 + 5] = index * 4 + 2;
		}

#pragma endregion

	vertexResource_->Unmap(0, nullptr);

	indexResource_->Unmap(0, nullptr);
}

void PrimitiveRing::Draw(SRT transform, Material material) {
	GameEngine::DrawPrimitiveRing(this, transform, material);
}

void PrimitiveRing::DrawBillBoard(SRT transform, Material material) {
	GameEngine::DrawPrimitiveRing_Billboard(this, transform, material);
}