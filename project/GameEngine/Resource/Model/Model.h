#pragma once
#include <wrl.h>
#include <vector>
#include <ModelData.h>
#include <d3d12.h>
#include <string>
#include <VertexData.h>
#include <Offset.h>
#include <DirectXCommon/DirectXCommon.h>
#include <TextureManager/TextureManager.h>

//モデル
class Model {
private:
	//頂点の数
	UINT vertexIndex_;

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

	//オフセット
	std::vector<Offset> offsets_;
	//テクスチャ
	std::vector<UINT> textureIndex_;
public:
	~Model();

	//初期値
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon);
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; }
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIBV() { return indexBufferView_; }
	//頂点の数
	UINT GetVertexIndex() { return vertexIndex_; }
	//オフセット
	std::vector<Offset> GetOffsets() { return offsets_; }
	//テクスチャ番号
	UINT GetTextureIndex(UINT offsetNum) { if (textureIndex_.empty()) return TextureManager::GetInstance()->GetWhite2x2(); return textureIndex_[offsetNum]; }
};