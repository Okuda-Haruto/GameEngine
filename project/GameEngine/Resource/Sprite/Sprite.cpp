#include "Sprite.h"
#include <GameEngine.h>
#include <LoadObjFile.h>
#include <SpriteManager/SpriteManager.h>

void Sprite::Initialize(const std::string& textureFilePath) {

	DirectXCommon* dxCommon = SpriteManager::GetInstance()->GetDirectXCommon();

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	//Sprite用のインデックスリソースを作る
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * 6);

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

	textureIndex_ = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);

	metadata_ = TextureManager::GetInstance()->GetMetaData(textureFilePath);

	size_ = { 1.0f,1.0f };
	position_ = {};
	rotation_ = 0.0f;

	textuerLeftTop_ = {};
	AdjustTextureSize();

	uvTransform_ = {};
	uvTransform_.scale = { 1.0f,1.0f,1.0f };

	color_ = { 1.0f,1.0f,1.0f,1.0f };

	material_ = {};
}

void Sprite::Update() {
	transform_ = {
		{size_.x,size_.y,1.0f},
		{0.0f,0.0f,rotation_ },
		{position_.x,position_.y,0.0f}
	};

	material_.color = color_;
	material_.uvTransform = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);

	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	float tex_left = textuerLeftTop_.x / metadata_.width;
	float tex_right = (textuerLeftTop_.x + textureSize_.x) / metadata_.width;
	float tex_top = textuerLeftTop_.y / metadata_.height;
	float tex_bottom = (textuerLeftTop_.y + textureSize_.y) / metadata_.height;

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = { left,bottom,0.0f,1.0f };
	vertexData_[1].position = { left,top,0.0f,1.0f };
	vertexData_[2].position = { right,bottom,0.0f,1.0f };
	vertexData_[3].position = { right,top,0.0f,1.0f };

	//頂点リソースにデータを書き込む
	vertexData_[0].texcoord = { tex_left,tex_bottom };
	vertexData_[1].texcoord = { tex_left,tex_top };
	vertexData_[2].texcoord = { tex_right,tex_bottom };
	vertexData_[3].texcoord = { tex_right,tex_top };

	vertexResource_->Unmap(0, nullptr);

}

void Sprite::Draw2D() {
	GameEngine::DrawSprite_2D(this);
}

void Sprite::SetTextue(const std::string& textureFilePath) {
	textureIndex_ = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
}

void Sprite::AdjustTextureSize() {

	textureSize_.x = static_cast<float>(metadata_.width);
	textureSize_.y = static_cast<float>(metadata_.height);
	//画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}