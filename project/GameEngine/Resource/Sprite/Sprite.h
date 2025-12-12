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

class Sprite {
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

	//マテリアルリソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource_;
	//マテリアルデータ
	std::vector <Material*> materialData_;

	//座標
	Vector2 position_;
	//回転
	float rotation_;
	//サイズ
	Vector2 size_;

	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };

	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	// トランスフォーム
	SRT transform_;

	//テクスチャ左上座標
	Vector2 textuerLeftTop_ = { 0.0f,0.0f };

	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };

	// UVトランスフォーム
	SRT uvTransform_;
	// 色
	Vector4 color_;

	//マテリアル
	Material material_;

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

	const Vector2& GetPosition() const { return position_; }
	void SetPosition(const Vector2& position) { position_ = position; }
	const float& GetRotation() const { return rotation_; }
	void SetRotation(const float& rotation) { rotation_ = rotation; }
	const Vector2& GetSize() const { return size_; }
	void SetSize(const Vector2& size) { size_ = size; }

	void SetTextue(const std::string& textureFilePath);

	const SRT& GetTransform() const { return transform_; }

	const SRT& GetUVTransform() const { return uvTransform_; }
	void SetUVTransform(const SRT& uvTransform) { uvTransform_ = uvTransform; }

	const Vector4& GetColor() const { return color_; }
	void SetColor(const Vector4& color) { color_ = color; }

	const Vector2& GetAnchorPoint()const { return anchorPoint_; }
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

	const bool& GetIsFlipX()const { return isFlipX_; }
	void SetIsFlipX(const bool& isFlip) { isFlipX_ = isFlip; }
	const bool& GetIsFlipY()const { return isFlipY_; }
	void SetIsFlipY(const bool& isFlip) { isFlipY_ = isFlip; }

	const Vector2& GetTextureLeftTop()const { return textuerLeftTop_; }
	void SetTextureLeftTop(const Vector2& textuerLeftTop) { textuerLeftTop_ = textuerLeftTop; }
	const Vector2& GetTextureSize()const { return textureSize_; }
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }

	const Material& GetMaterial()const { return material_; }

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() const { return indexBufferView_; }
	UINT GetTextureIndex() { return textureIndex_; }
private:
	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
};