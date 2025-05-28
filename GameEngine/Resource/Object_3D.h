#pragma once

#include "DirectXTex/DirectXTex.h"

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include <vector>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Object_3D_Data.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

# pragma region Object_3D

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

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//光源データ
	Light* light_ = nullptr;
	//ライティングを使用するか
	bool isLighting_ = true;

	//テクスチャデータ
	Texture* texture_ = nullptr;

	//カメラ
	Camera* camera_;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Object_3D();
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	/// <param name="device">デバイス</param>
	void Initialize(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device> device);
 
	// Light入力
	void SetLight(Light* light) { light_ = light; }
	// Texture入力
	void SetTexture(Texture* texture) { texture_ = texture; }
	// Lightを使用するか
	void isLighting(bool isLighting) { isLighting_ = isLighting; }
	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data);

	//リソース初期化
	void Reset();

	// モデルデータ
	[[nodiscard]]
	ModelData ModelData();
};

# pragma endregion

# pragma region Sprite_3D

//3Dスプライト　(ビルボード)
class Sprite_3D {
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

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;

	//Spriteの表示サイズ
	float spriteWidth_ = 1280;
	float spriteHeight_ = 720;

	//光源データ
	Light* light_ = nullptr;
	//ライティングを使用するか
	bool isLighting_ = true;

	//テクスチャデータ
	Texture* texture_ = nullptr;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Sprite_3D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight);

	// Light入力
	void SetLight(Light* light) { light_ = light; };
	// Texture入力
	void SetTexture(Texture* texture) { texture_ = texture; };
	// Lightを使用するか
	void isLighting(bool isLighting) { isLighting_ = isLighting; };
	// スプライトの表示サイズ入力
	void SetSpriteSize(float width, float height) { spriteWidth_ = width; spriteHeight_ = height; };

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data);

	//リソース初期化
	void Reset();
};

# pragma endregion