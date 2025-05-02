#pragma once

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "SRT.h"

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
	//WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	//WVPデータ
	TransformationMatrix* transformationMatrixData_ = nullptr;
	//マテリアル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルデータ
	Material* materialData_ = nullptr;

	//Transform変数
	Transform transform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	//UVTransform変数
	Transform uvTransform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	//Color変数
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
public:

	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);

	// Transform入力
	void SetTransform(Transform transform);
	// UVTransform入力
	void SetUVTransform(Transform uvTransform);
	// Color入力
	void SetColor(Vector4 color);

	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

};