#pragma once
#include <string>
#include "DirectXCommon/DirectXCommon.h"
#include "SpriteManager/SpriteManager.h"
#include "TextureManager/TextureManager.h"
#include "TransformationMatrix.h"
#include "Material.h"

#include "VertexData.h"
#include "Vector2.h"
#include "SRT.h"

class SpriteManager;

class InstancingSprite {
private:
	//スプライト共通部
	SpriteManager* spriteManager_ = nullptr;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	VertexData* vertexData_ = nullptr;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	//Sprite一つ一つに持たせないといけないデータ
	struct SpriteData {
		//有効化
		bool isDraw = true;

		//座標
		Vector2 position{};
		//回転
		float rotation{};
		//サイズ
		Vector2 size{};

		// 色
		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };

		//アンカーポイント
		Vector2 anchorPoint = { 0.0f,0.0f };

		//左右フリップ
		bool isFlipX = false;
		//上下フリップ
		bool isFlipY = false;

		//テクスチャ左上座標
		Vector2 textuerLeftTop = { 0.0f,0.0f };

		//テクスチャ切り出しサイズ
		Vector2 textureSize = { 100.0f,100.0f };

		// トランスフォーム
		SRT transform{};
		// UVトランスフォーム
		SRT uvTransform{};
	};

	//Spriteデータ
	std::unordered_map<std::string, SpriteData> dataMap_;

	// テクスチャ番号
	uint32_t textureIndex_ = 0;

	//メタデータ
	DirectX::TexMetadata metadata_;
public:

	//初期化
	void Initialize(const std::string& textureFilePath);
	//更新
	void Update();
	//描画
	void Draw2D();

	//Sprite追加
	void AddNewSprite(const std::string spriteName);

	//Sprite削除
	void DeleteSprite(const std::string spriteName);

	//テスクチャ変更
	void SetTextue(const std::string& textureFilePath);
	//テクスチャインデックス
	UINT GetTextureIndex() { return textureIndex_; }

	//頂点バッファビュー
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; }
	//インデックスバッファビュー
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() const { return indexBufferView_; }

private:

	//getterのテンプレート
	template<typename T>
	const T& GetData(const std::string& spriteName, T SpriteData::* member) const {
		auto it = dataMap_.find(spriteName);

		//指定したSpriteが存在しない場合assert
		assert(it != dataMap_.end());

		return it->second.*member;
	}
	//setterのテンプレート
	template<typename T>
	void SetData(const std::string& spriteName, const T& value, T SpriteData::* member) {
		auto it = dataMap_.find(spriteName);

		//指定したSpriteが存在しない場合新しく生成
		if (it == dataMap_.end()) {
			AddNewSprite(spriteName);
			it = dataMap_.find(spriteName);
		}

		it->second.*member = value;
	}
	//Listのgetterのテンプレート
	template<typename T>
	std::list<T> GetData(T SpriteData::* member) const {
		std::list<T> dataList;
		for (const auto& data : dataMap_) {
			dataList.push_back(data.second.*member);
		}
		return dataList;
	}

public:

	//IsDrawGetter
	const bool& GetIsDraw(const std::string spriteName) const { return GetData(spriteName, &SpriteData::isDraw); }
	//IsDrawSetter
	void SetIsDraw(const std::string spriteName, const bool& isDraw) { return SetData(spriteName, isDraw, &SpriteData::isDraw); }

	//PositionGetter
	const Vector2& GetPosition(const std::string spriteName) const { return GetData(spriteName, &SpriteData::position); }
	//PositionSetter
	void SetPosition(const std::string spriteName, const Vector2& position) { return SetData(spriteName, position, &SpriteData::position); }
	//RotationGetter
	const float& GetRotation(const std::string spriteName) const { return GetData(spriteName, &SpriteData::rotation); }
	//RotationSetter
	void SetRotation(const std::string spriteName, const float& rotation) { return SetData(spriteName, rotation, &SpriteData::rotation); }
	//SizeGetter
	const Vector2& GetSize(const std::string spriteName) const { return GetData(spriteName, &SpriteData::size); }
	//SizeSetter
	void SetSize(const std::string spriteName, const Vector2& size) { return SetData(spriteName, size, &SpriteData::size); }

	//ColorGetter
	const Vector4& GetColor(const std::string spriteName) const { return GetData(spriteName, &SpriteData::color); }
	//ColorSetter
	void SetColor(const std::string spriteName, const Vector4& color) { return SetData(spriteName, color, &SpriteData::color); }

	//AnchorPointGetter
	const Vector2& GetAnchorPoint(const std::string spriteName) const { return GetData(spriteName, &SpriteData::anchorPoint); }
	//AnchorPointSetter
	void SetAnchorPoint(const std::string spriteName, const Vector2& anchorPoint) { return SetData(spriteName, anchorPoint, &SpriteData::anchorPoint); }

	//IsFlipXGetter
	const bool& GetIsFlipX(const std::string spriteName) const { return GetData(spriteName, &SpriteData::isFlipX); }
	//IsFlipXSetter
	void SetIsFlipX(const std::string spriteName, const bool& isFlip) { return SetData(spriteName, isFlip, &SpriteData::isFlipX); }
	//IsFlipYGetter
	const bool& GetIsFlipY(const std::string spriteName) const { return GetData(spriteName, &SpriteData::isFlipY); }
	//IsFlipYSetter
	void SetIsFlipY(const std::string spriteName, const bool& isFlip) { return SetData(spriteName, isFlip, &SpriteData::isFlipY); }

	//TextureLeftTopGetter
	const Vector2& GetTextureLeftTop(const std::string spriteName) const { return GetData(spriteName, &SpriteData::textuerLeftTop); }
	//TextureLeftTopSetter
	void SetTextureLeftTop(const std::string spriteName, const Vector2& textuerLeftTop) { return SetData(spriteName, textuerLeftTop, &SpriteData::textuerLeftTop); }
	//TextureSizeGetter
	const Vector2& GetTextureSize(const std::string spriteName) const { return GetData(spriteName, &SpriteData::textureSize); }
	//TextureSizeSetter
	void SetTextureSize(const std::string spriteName, const Vector2& textureSize) { return SetData(spriteName, textureSize, &SpriteData::textureSize); }

	//TransformGetter
	const SRT& GetTransform(const std::string spriteName) const { return GetData(spriteName, &SpriteData::transform); }
	//TransformListGetter
	std::list<SRT> GetTransformList() const { return GetData(&SpriteData::transform); }
	//UVTransformGetter
	const SRT& GetUVTransform(const std::string spriteName) const { return GetData(spriteName, &SpriteData::uvTransform); }
	//UVTransformListGetter
	std::list<SRT> GetUVTransformList() const { return GetData(&SpriteData::uvTransform); }

	//NameListGetter
	std::list<std::string> GetSpriteNames() const {
		std::list<std::string> names;

		for (const auto& [name, data] : dataMap_) {
			names.push_back(name);
		}

		return names;
	}

private:

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize(SpriteData& data);
};