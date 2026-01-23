#pragma once
#include <Windows.h>

#include <dxcapi.h>

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

#include <Xinput.h>
#include <random>
#include <iostream>
#include <algorithm>

#include "D3DResourceLeakChecker.h"
#include <Operation/Operation.h>

#include "Object/Object.h"
#include "Sprite/Sprite.h"
#include "TransformationMatrix.h"
#include "InstancingTransformationMatrix.h"
#include "BoneMatrix.h"
#include "Fog.h"

#include <Audio/Audio.h>
#include "Input/Input.h"
#include "DebugCamera.h"

#include "TextureData.h"

#include "WindowsAPI/WindowsAPI.h"
#include "DirectXCommon/DirectXCommon.h"
#include "ImGuiManager/ImGuiManager.h"
#include "ModelManager/ModelManager.h"
#include "SRVManager/SRVManager.h"
#include "ParticleManager/ParticleManager.h"
#include "PrimitiveManager/PrimitiveManager.h"
#include "AudioManager/AudioManager.h"

#include <vector>
#include <array>
#include <list>

using namespace std;

class GameEngine {
private:

	//ウィンドウの幅
	static int32_t kWindowWidth_;
	//ウィンドウの高さ
	static int32_t kWindowHeight_;

#ifdef _DEBUG
	//リソースチェック
	D3DResourceLeakChecker leakCheck_;
#endif
	unique_ptr<WindowsAPI> winApp_;

	unique_ptr<DirectXCommon> dxCommon_;

	unique_ptr<SRVManager> srvManager_;

	unique_ptr<ImGuiManager> imguiManager_;

	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;

	//RootSignature
	Microsoft::WRL::ComPtr <ID3D12RootSignature> objectRootSignature_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> instancingObjectRootSignature_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> spriteRootSignature_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> particleRootSignature_;

	//Windowのメッセージ
	MSG msg_{};

	//PSO
	Microsoft::WRL::ComPtr <ID3D12PipelineState> object3DPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> object2DPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> noDepthObjectPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> instancingObjectPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> particlePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> particleAddBrendPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> spritePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> linePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> noDepthLinePipelineState_ = nullptr;

public:
	//描画可能なモデルの数(通常)
	static const int16_t kMaxIndex = 1024;
	//描画可能なモデルの数(インスタシング)
	static const int16_t kMaxInstanceIndex = 64;
	//インスタンス数
	static const uint32_t kMaxNumInstance = 256;
private:
#pragma region object
	int16_t objectIndex_;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > objectMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxIndex> objectMaterialData_;
	//ボーンリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > objectBoneResource_;
	//ボーンデータ
	std::array <BoneMatrix*, kMaxIndex> objectBoneData_;
	//WVP用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > objectWvpResource_;
	//WVPデータ
	std::array <TransformationMatrix*, kMaxIndex> objectWvpData_;
#pragma endregion

#pragma region instancingObject
	int16_t instancingObjectIndex_;
	uint32_t startInstancingObjectIndex_;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingObjectMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxInstanceIndex> instancingObjectMaterialData_;
	//ボーンリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingObjectBoneResource_;
	//ボーンデータ
	std::array <BoneMatrix*, kMaxInstanceIndex> instancingObjectBoneData_;
	//インスタンス用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingObjectResource_;
	//インスタンスデータ
	std::array < std::array <InstancingTransformationMatrix*, kMaxNumInstance>, kMaxInstanceIndex> instancingObjectData_;
#pragma endregion

#pragma region Particle
	int16_t particleIndex_;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > particleMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxInstanceIndex> particleMaterialData_;
	//インスタンス用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > particleResource_;
	//インスタンスデータ
	std::array < std::array <InstancingTransformationMatrix*, kMaxNumInstance>, kMaxInstanceIndex> particleData_;
#pragma endregion

#pragma region sprite
	int16_t spriteIndex_;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > spriteMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxIndex> spriteMaterialData_;
	//WVP用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > spriteWvpResource_;
	//WVPデータ
	std::array <TransformationMatrix*, kMaxIndex> spriteWvpData_;
#pragma endregion

#pragma region instancingSprite
	int16_t instancingSpriteIndex_;
	uint32_t startInstancingSpriteIndex_;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingSpriteMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxInstanceIndex > instancingSpriteMaterialData_;
	//インスタンス用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingSpriteResource_;
	//インスタンスデータ
	std::array <std::array <InstancingTransformationMatrix*, kMaxNumInstance>, kMaxInstanceIndex > instancingSpriteData_;
#pragma endregion

#pragma region primitive
	//マテリアルリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, PrimitiveManager::SHAPE_count> primitiveMaterialResource_;
	//マテリアルデータ
	std::array<Material*, PrimitiveManager::SHAPE_count> primitiveMaterialData_;
	//インスタンス用リソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, PrimitiveManager::SHAPE_count> primitiveResource_;
	//インスタンスデータ
	std::array<std::array<InstancingTransformationMatrix*, PrimitiveManager::kMaxNumPrimitive>, PrimitiveManager::SHAPE_count> primitiveData_;
#pragma endregion

	Microsoft::WRL::ComPtr<ID3D12Resource> fogResource_;
	Fog* fogData_;
	
	//XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	//オーディオ宛先	生ポインタ以外はほぼ使えない
	IXAudio2MasteringVoice* masterVoice_;

	std::mt19937 randomEngine_;

	//テクスチャデータ
	std::vector<TextureData> textureData_;

	void Initialize_(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720);

	float randomFloat_(float minFloat, float maxFloat);
	int32_t randomInt_(int32_t minInt,int32_t maxInt);

	bool StartFlame_();
	bool WindowState_();
	void PreDraw_();
	void PostDraw_();

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice_() { return dxCommon_->GetDevice(); }

	void DrawObject_3D_(Object* object, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight, UINT animationIndex, float time);
	void DrawParts_3D_(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight);
	void DrawObject_2D_(Object* object, shared_ptr<DirectionalLight> directionalLight);
	void DrawParts_2D_(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight);
	void DrawInstancingObject_3D_(std::list<Object*> objects, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight);
	void DrawParticle_(ParticleGroup particleGroup);

	void DrawSprite_2D_(Sprite* sprite);
	void DrawInstancingSprite_2D_(std::list<Sprite*> sprits);

	void DrawLine_(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource);
	void DrawPoint_(std::list<Vector3> points, PrimitiveManager::PrimitiveResource primitiveResource);
	void DrawAABB_(std::list<AABB> aabbs, PrimitiveManager::PrimitiveResource primitiveResource);

	WindowsAPI* GetWindowsAPI_() { return winApp_.get(); }

	// インスタンス生成
	static GameEngine* GetInstance();

	static unique_ptr<GameEngine> instance;

	void Finalize_();

public:

	GameEngine() = default;
	~GameEngine() = default;
	GameEngine(GameEngine&) = delete;
	GameEngine& operator=(GameEngine&) = delete;


	// コピー、代入を禁止する
	GameEngine(const unique_ptr<GameEngine>) = delete;
	unique_ptr<GameEngine> operator=(const unique_ptr<GameEngine>) = delete;

	static void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名 (例:L"LE2A_00_ミョウジ_ナマエ")</param>
	/// <param name="kWindowWidth">ウィンドウの幅 (例:1280)</param>
	/// <param name="kWindowHeight">ウィンドウの高さ (例:720)</param>
	static void Initialize(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720) { GetInstance()->Initialize_(WindowName, kWindowWidth, kWindowHeight); }

	static float randomFloat(float minFloat, float maxFloat) { return GetInstance()->randomFloat_(minFloat, maxFloat); }
	static int32_t randomInt(int32_t minInt, int32_t maxInt) { return GetInstance()->randomInt_(minInt, maxInt); }

	/// <summary>
	/// フレームの開始
	/// </summary>
	/// <returns>Windowsのメッセージがあるか</returns>
	[[nodiscard]]
	static bool StartFlame() { return GetInstance()->StartFlame_(); }

	/// <summary>
	/// ウィンドウ状態
	/// </summary>
	/// <returns>ウィンドウを閉じているか</returns>
	[[nodiscard]]
	static bool WindowState() { return GetInstance()->WindowState_(); }

	//描画前処理
	static void PreDraw() { GetInstance()->PreDraw_(); }

	//描画後処理
	static void PostDraw() { GetInstance()->PostDraw_(); }

	//ウィンドウ幅
	[[nodiscard]]
	static int32_t GetWindowWidth() { return kWindowWidth_; }

	//ウィンドウ高さ
	[[nodiscard]]
	static int32_t GetWindowHeight() { return kWindowHeight_; }

	//デバイス
	[[nodiscard]]
	static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return GetInstance()->GetDevice_(); }


	static void DrawObject_3D(Object* object, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight, UINT animationIndex = 0, float time = 0.0f) { return GetInstance()->DrawObject_3D_(object, directionalLight, pointLight, spotLight, animationIndex, time); }
	static void DrawParts_3D(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight) { return GetInstance()->DrawParts_3D_(object, partsIndex, directionalLight, pointLight, spotLight); }
	static void DrawObject_2D(Object* object, shared_ptr<DirectionalLight> directionalLight) { return GetInstance()->DrawObject_2D_(object, directionalLight); }
	static void DrawParts_2D(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight) { return GetInstance()->DrawParts_2D_(object, partsIndex, directionalLight); }
	static void DrawInstancingObject_3D(std::list<Object*> objects, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight) { return GetInstance()->DrawInstancingObject_3D_(objects, directionalLight, pointLight, spotLight); }
	static void DrawParticle(ParticleGroup particleGroup) { return GetInstance()->DrawParticle_(particleGroup); }
	
	static void DrawSprite_2D(Sprite* sprite) { return GetInstance()->DrawSprite_2D_(sprite); }
	static void DrawInstancingSprite_2D(std::list<Sprite*> sprits) { return GetInstance()->DrawInstancingSprite_2D_(sprits); }

	static void DrawLine(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource) { return GetInstance()->DrawLine_(lines, primitiveResource); }
	static void DrawPoint(std::list<Vector3> points, PrimitiveManager::PrimitiveResource primitiveResource) { return GetInstance()->DrawPoint_(points, primitiveResource); }
	static void DrawAABB(std::list<AABB> aabbs, PrimitiveManager::PrimitiveResource primitiveResource) { return GetInstance()->DrawAABB_(aabbs, primitiveResource); }

	[[nodiscard]]
	static WindowsAPI* GetWindowsAPI() { return GetInstance()->GetWindowsAPI_(); }

	static DirectXCommon* GetDirectXCommon() { return GetInstance()->dxCommon_.get(); }
};