#pragma once

#include "DirectXTex/DirectXTex.h"

#include <d3d12.h>

#include <wrl.h>
#include <vector>
#include <ObjectData.h>
#include "Material.h"
#include "TransformationMatrix.h"
#include "Object_Multi_Data.h"
#include "Object_Single_Data.h"
#include "Camera.h"
#include "Light.h"
#include "Particle.h"
#include "ParticleForGPU.h"

#include "Line.h"

# pragma region Object_3D

//3Dオブジェクト
class Object_3D {
private:
	//ロードしたモデルのデータ
	std::vector<ModelData> modelData_;
	//描画用に変換したモデルのデータ
	std::vector<ObjectData> objectData_;

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
	int isLighting_ = 0;

	//カメラ
	Camera* camera_ = nullptr;

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
	void Initialize(const std::string& directoryPath, const std::string& filename);

	// Light入力
	void SetLight(Light* light) { light_ = light; }
	// Lightを使用するか
	void isLighting(int isLighting) { isLighting_ = isLighting; }
	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const Object_Multi_Data& data);

	std::vector<ModelData> GetModelData() { return modelData_; }
};

# pragma endregion

# pragma region Instance_3D

//3Dオブジェクト
class Instance_3D {
private:
	//ロードしたモデルのデータ
	std::vector<ModelData> modelData_;
	//描画用に変換したモデルのデータ
	std::vector<ObjectData> objectData_;

	//マテリアルリソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource_;
	//マテリアルデータ
	std::vector <Material*> materialData_;
	//インスタンス数
	static const uint32_t kMaxNumInstance = 32;
	//インスタンス用リソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResource_;
	//インスタンスデータ
	std::vector <ParticleForGPU*> instancingData_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//光源データ
	Light* light_ = nullptr;
	//ライティングを使用するか
	int isLighting_ = 0;

	//カメラ
	Camera* camera_ = nullptr;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Instance_3D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	void Initialize(const std::string& directoryPath, const std::string& filename);

	// Light入力
	void SetLight(Light* light) { light_ = light; }
	// Lightを使用するか
	void isLighting(int isLighting) { isLighting_ = isLighting; }
	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, std::list< Object_Multi_Data> data);

	std::vector<ModelData> GetModelData() { return modelData_; }
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

	//カメラ
	Camera* camera_ = nullptr;

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
	void Initialize();

	// Light入力
	void SetLight(Light* light) { light_ = light; }
	// Lightを使用するか
	void isLighting(bool isLighting) { isLighting_ = isLighting; }
	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }
	// スプライトの表示サイズ入力
	void SetSpriteSize(float width, float height) { spriteWidth_ = width; spriteHeight_ = height; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_Multi_Data& data);

	//リソース初期化
	void Reset();
};

# pragma endregion

# pragma region Particle_3D

//3Dパーティクル　(ビルボード)
class Particle_3D {
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

	//インスタンス数
	static const uint32_t kMaxNumInstance = 128;
	//インスタンス用リソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResource_;
	//インスタンスデータ
	std::vector <ParticleForGPU*> instancingData_;

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

	//カメラ
	Camera* camera_ = nullptr;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Particle_3D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize();

	// Light入力
	void SetLight(Light* light) { light_ = light; }
	// Lightを使用するか
	void isLighting(bool isLighting) { isLighting_ = isLighting; }
	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }
	// スプライトの表示サイズ入力
	void SetSpriteSize(float width, float height) { spriteWidth_ = width; spriteHeight_ = height; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Particles);

	//リソース初期化
	void Reset();
};

# pragma endregion

# pragma region Line_3D

//3Dオブジェクト
class Line_3D {
private:
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_{};
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

	//カメラ
	Camera* camera_ = nullptr;

	//リソース番号の最大値
	static const int kMaxIndex_ = 1024;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Line_3D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	/// <param name="device">デバイス</param>
	void Initialize();

	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const Segment& segment, const Vector4& color, Camera* camera_);

	//リソース初期化
	void Reset();
};

# pragma endregion

# pragma region AxisIndicator

//軸表示
class AxisIndicator {
private:
	//ロードしたモデルのデータ
	std::vector<ModelData> modelData_;
	//描画用に変換したモデルのデータ
	std::vector<ObjectData> objectData_;

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルデータ
	Material* materialData_;
	//WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	//WVPデータ
	TransformationMatrix* wvpData_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//カメラ
	Camera* camera_ = nullptr;

	UINT textureIndex_ = 0;

public:

	~AxisIndicator();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(Camera* camera);

	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	//リソース初期化
	void Reset();
};

# pragma endregion

# pragma region Grid

//3Dオブジェクト
class Grid {
private:
	//Gridの中心からの範囲
	const int kGridLength_ = 100;

	//3D軸線
	Line_3D* lineX_ = nullptr;
	Line_3D* lineY_ = nullptr;
	Line_3D* lineZ_ = nullptr;

	//平面
	bool xy_ = false;
	bool xz_ = true;
	bool yz_ = false;

	//描画中心点
	Vector3 centerPoint_;

	//カメラ
	Camera* camera_ = nullptr;

public:

	~Grid();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);

	// Camera入力
	void SetCamera(Camera* camera) { camera_ = camera; }

	void IsDrawXY(bool xy) { xy_ = xy; }
	void IsDrawXZ(bool xz) { xz_ = xz; }
	void IsDrawYZ(bool yz) { yz_ = yz; }

	/// <summary>
	/// 3Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
};

# pragma endregion