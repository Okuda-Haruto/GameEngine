#include "InstancingSprite.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "SpriteManager/SpriteManager.h"

void InstancingSprite::Initialize(const std::string& textureFilePath) {

	DirectXCommon* dxCommon = GameEngine::GetDirectXCommon();

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
}

void InstancingSprite::Update() {

	for (auto& data : dataMap_) {
		data.second.transform = {
			{ data.second.size.x,data.second.size.y,1.0f },
			{ 0.0f,0.0f,data.second.rotation },
			{ data.second.position.x - data.second.anchorPoint.x * data.second.size.x,data.second.position.y - data.second.anchorPoint.y * data.second.size.y,0.0f }
		};

		float scaleX = data.second.textureSize.x / metadata_.width;
		float scaleY = data.second.textureSize.y / metadata_.height;

		float offsetX = data.second.textuerLeftTop.x / metadata_.width;
		float offsetY = data.second.textuerLeftTop.y / metadata_.height;

		if (data.second.isFlipX) {
			scaleX = -scaleX;
			offsetX += data.second.textureSize.x / metadata_.width;
		}

		if (data.second.isFlipY) {
			scaleY = -scaleY;
			offsetY += data.second.textureSize.y / metadata_.height;
		}

		data.second.uvTransform = {
			{ scaleX,scaleY,1.0f },
			{ 0.0f,0.0f,0.0f },
			{ offsetX,offsetY,0.0f }
		};
	}

}

void InstancingSprite::Draw2D() {
	GameEngine::DrawInstancingSprite_2D(this);
}

void InstancingSprite::AddNewSprite(const std::string spriteName) {

	//同一名Spriteが存在しないなら生成
	auto [it, inserted] = dataMap_.try_emplace(spriteName);

	//すでに同一名Spriteが存在するならreturn
	if (!inserted) {
		return;
	}

	//Textureサイズで初期化
	AdjustTextureSize(it->second);
}

void InstancingSprite::DeleteSprite(const std::string spriteName) {
	//削除
	dataMap_.erase(spriteName);
}

void InstancingSprite::SetTextue(const std::string& textureFilePath) {
	textureIndex_ = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
}

void InstancingSprite::AdjustTextureSize(SpriteData& data) {

	data.textureSize.x = static_cast<float>(metadata_.width);
	data.textureSize.y = static_cast<float>(metadata_.height);
	//画像サイズをテクスチャサイズに合わせる
	data.size = data.textureSize;
}