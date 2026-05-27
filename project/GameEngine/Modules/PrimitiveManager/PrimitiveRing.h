#pragma once
#include <wrl.h>
#include <vector>
#include <ModelData.h>
#include <d3d12.h>
#include <string>
#include <VertexData.h>
#include <AnimationInterpolation.h>
#include <Offset.h>
#include <DirectXCommon/DirectXCommon.h>
#include <TextureManager/TextureManager.h>
#include <Camera/Camera.h>
#include <Material.h>

//モデル
class PrimitiveRing {
private:
	static unique_ptr<PrimitiveRing> instance;

	DirectXCommon* dxCommon_ = nullptr;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	ObjectVertexData* vertexData_ = nullptr;

	//インデックス数
	uint32_t indexCount_;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	//テクスチャ番号
	uint32_t textureIndex_;

	std::shared_ptr<Camera> camera_;
public:
	~PrimitiveRing();

	//初期値
	void Initialize(uint32_t textureIndex, std::shared_ptr<Camera> camera, DirectXCommon* dxCommon);

	//描画処理
	void Draw(SRT transform, Material material);
	void DrawBillBoard(SRT transform, Material material);

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; }
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIBV() { return indexBufferView_; }
	//テクスチャ番号
	uint32_t GetTextureIndex() { return textureIndex_; }

	//インデックス数
	uint32_t GetIndexCount() { return indexCount_; }

	std::shared_ptr<Camera> GetCamera() { return camera_; }

};