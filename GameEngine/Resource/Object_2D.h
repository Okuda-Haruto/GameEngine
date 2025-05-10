#pragma once

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "ObjectData.h"
#include "Texture.h"

//3Dオブジェクト
class Object_2D {
private:
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
	//WVP用リソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResource_;
	//WVPデータ
	std::vector <TransformationMatrix*> wvpData_;

	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//Windowのサイズ
	uint32_t kWindowWidth_;
	uint32_t kWindowHeight_;

	Texture* texture_ = nullptr;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="kWindowWidth">ウィンドウの幅</param>
	/// <param name="kWindowHeight">ウィンドウの高さ</param>
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight);

	// Texture入力
	void SetTexture(Texture* texture) { texture_ = texture; };

	/// <summary>
	/// 2Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_2D_Data& data);

	//リソース初期化
	void Reset();

};