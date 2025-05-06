#pragma once

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "SRT.h"
#include "Camera.h"

//3Dオブジェクト
class Object_3D {
private:
	//ロードしたモデルのデータ
	ModelData modelData_;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	VertexData* vertexData_ = nullptr;
	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルデータ
	Material* materialData_ = nullptr;
	//WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	//WVPデータ
	TransformationMatrix* wvpData_ = nullptr;

	//Transform変数
	Transform transform_;
	//UVTransform変数
	Transform uvTransform_;
	//Camera変数
	Camera camera_;
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	/// <param name="device">デバイス</param>
	void Initialize(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device> device);

	// Transform入力
	void SetTransform(Transform transform) { transform_ = transform; };
	// UVTransform入力
	void SetUVTransform(Transform uvTransform) { uvTransform_ = uvTransform; };
	// Color入力
	void SetColor(Vector4 color) { materialData_->color = color; };
	// Camera入力
	void SetCamera(Camera camera) { camera_ = camera; };

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="directionalLightResource">オブジェクトを照らす照明 (例:directionalLightResource)</param>
	/// <param name="textureSrvHandleGPU">オブジェクトに貼り付けるテクスチャのGPUデスクリプタハンドル (例:object3DTexture->textureSrvHandleGPU())</param>
	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Microsoft::WRL::ComPtr<ID3D12Resource>& directionalLightResource, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	// モデルデータ
	[[nodiscard]]
	ModelData ModelData();
};