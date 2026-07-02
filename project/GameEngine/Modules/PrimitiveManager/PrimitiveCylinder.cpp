#include "PrimitiveCylinder.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "LoadGLTFFile.h"
#include "Animation/Animation.h"
#include <numbers>

PrimitiveCylinder::~PrimitiveCylinder() {

}

void PrimitiveCylinder::Initialize(uint32_t textureIndex, std::shared_ptr<Camera> camera, DirectXCommon* dxCommon) {

	textureIndex_ = textureIndex;

	camera_ = camera;

	const uint32_t kCylinderDivide = 32;
	const float kTopRadius = 1.0f;
	const float kBottomRadius = 1.0f;
	const float kHeight = 3.0f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / kCylinderDivide;

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(ObjectVertexData) * kCylinderDivide * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(ObjectVertexData) * kCylinderDivide * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(ObjectVertexData);

	//Primitive用のインデックスリソースを作る
	indexCount_ = kCylinderDivide * 6;
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

#pragma region Cylinder

	for (uint32_t index = 0; index < kCylinderDivide; index++) {
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float sinNext = std::sin((index + 1) * radianPerDivide);
		float cosNext = std::cos((index + 1) * radianPerDivide);
		float u = float(index) / float(kCylinderDivide);
		float uNext = float(index + 1) / float(kCylinderDivide);

		//positionとuv。normalは必要なら+zを設定する
		vertexData_[index * 4].position = { -sin * kTopRadius, kHeight, cos * kTopRadius, 1.0f };
		vertexData_[index * 4].texcoord = { u ,1.0f - 0.0f };
		vertexData_[index * 4].normal = { -sin,0.0f,cos };
		vertexData_[index * 4 + 1].position = { -sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f };
		vertexData_[index * 4 + 1].texcoord = { uNext ,1.0f - 0.0f };
		vertexData_[index * 4 + 1].normal = { -sinNext,0.0f,cosNext };
		vertexData_[index * 4 + 2].position = { -sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f };
		vertexData_[index * 4 + 2].texcoord = { u ,1.0f - 1.0f };
		vertexData_[index * 4 + 2].normal = { -sin,0.0f,cos };
		vertexData_[index * 4 + 3].position = { -sinNext * kBottomRadius, 0.0f, cosNext * kBottomRadius, 1.0f };
		vertexData_[index * 4 + 3].texcoord = { uNext ,1.0f - 1.0f };
		vertexData_[index * 4 + 3].normal = { -sinNext,0.0f,cosNext };

		indexData_[index * 6 + 0] = index * 4 + 0; indexData_[index * 6 + 1] = index * 4 + 1; indexData_[index * 6 + 2] = index * 4 + 2;
		indexData_[index * 6 + 3] = index * 4 + 1; indexData_[index * 6 + 4] = index * 4 + 3; indexData_[index * 6 + 5] = index * 4 + 2;
	}

#pragma endregion

	vertexResource_->Unmap(0, nullptr);

	indexResource_->Unmap(0, nullptr);
}

void PrimitiveCylinder::Draw(SRT transform, Material material) {
	GameEngine::DrawPrimitiveCylinder(this, transform, material);
}

void PrimitiveCylinder::DrawBillBoard(SRT transform, Material material) {
	GameEngine::DrawPrimitiveCylinder_Billboard(this, transform, material);
}