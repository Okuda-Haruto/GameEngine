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

#include "Text.h"
#include "Audio.h"
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

#include <vector>
#include <array>
#include <list>

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
	WindowsAPI* winApp_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	SRVManager* srvManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	uint32_t StructuredBufferIndex_;

	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//RootSignature
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> instancingRootSignature_;

	//Windowのメッセージ
	MSG msg_{};

	//PSO
	Microsoft::WRL::ComPtr <ID3D12PipelineState> trianglePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> instancingTrianglePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> particlePipelineState_ = nullptr;
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
	int16_t objectIndex;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > objectMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxIndex> objectMaterialData_;
	//WVP用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxIndex > objectWvpResource_;
	//WVPデータ
	std::array <TransformationMatrix*, kMaxIndex> objectWvpData_;
#pragma endregion

#pragma region instancingObject
	int16_t instancingObjectIndex;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingObjectMaterialResource_;
	//マテリアルデータ
	std::array <Material*, kMaxInstanceIndex> instancingObjectMaterialData_;
	//インスタンス用リソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingObjectResource_;
	//インスタンスデータ
	std::array < std::array <InstancingTransformationMatrix*, kMaxNumInstance>, kMaxInstanceIndex> instancingObjectData_;
#pragma endregion

#pragma region sprite
	int16_t spriteIndex;
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
	int16_t instancingSpriteIndex;
	//マテリアルリソース
	std::array <Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxInstanceIndex > instancingSpriteMaterialResource_;
	//マテリアルデータ
	std::array <std::array <Material*, kMaxNumInstance>, kMaxInstanceIndex > instancingSpriteMaterialData_;
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

	//XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	//オーディオ宛先
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	//DirectInput
	IDirectInput8* directInput_ = nullptr;
	//キーボードデバイス
	IDirectInputDevice8* keyboardDevice_ = nullptr;
	//マウスデバイス
	IDirectInputDevice8* mouseDevice_ = nullptr;

	std::mt19937 randomEngine_;

#pragma region 入力関係

	//キー入力
	BYTE keys_[256]{};
	BYTE preKeys_[256]{};

	//マウス入力
	DIMOUSESTATE preMouse_;
	DIMOUSESTATE mouse_;

	//パッド入力
	XINPUT_STATE pad_[4];
	DWORD dwResult_[4];
	XINPUT_STATE prePad_[4];
#pragma endregion

	//テクスチャデータ
	std::vector<TextureData> textureData_;

	//コンストラクタ
	GameEngine();
	// デストラクタ
	~GameEngine();

	void Intialize_(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720);

	Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2_() { return xAudio2_; }

	float randomFloat_(float minFloat, float maxFloat);
	int32_t randomInt_(int32_t minInt,int32_t maxInt);

	bool StartFlame_();
	bool WindowState_();
	void PreDraw_();
	void PostDraw_();

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice_() { return dxCommon_->GetDevice(); }

	Microsoft::WRL::ComPtr <ID3D12RootSignature> RootSignature_() { return rootSignature_; }
	Microsoft::WRL::ComPtr <ID3D12RootSignature> InstancingRootSignature_() { return instancingRootSignature_; }

	ID3D12PipelineState* TrianglePSO_() { return trianglePipelineState_.Get(); }
	ID3D12PipelineState* InstancingTrianglePSO_() { return particlePipelineState_.Get(); }
	ID3D12PipelineState* ParticlePSO_() { return particlePipelineState_.Get(); }
	ID3D12PipelineState* LinePSO_() { return linePipelineState_.Get(); }

	void DrawObject_3D_(Object* object, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight);
	void DrawInstancingObject_3D_(std::list<Object*> objects, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight);
	void DrawParticle_(ParticleGroup particleGroup);
	/*void DrawSprite_3D_();
	void DrawInstancingSprite_3D_();
	void DrawBillbord_3D_();
	void DrawInstancingBillbord_3D_();
	void DrawLine_3D_();
	void DrawInstancingLine_3D_();
	void DrawAxisIndicator_3D_();
	void DrawGrid_3D_();
	void DrawObject_2D_();
	void DrawInstancingObject_2D_();*/
	void DrawSprite_2D_(Sprite* sprite);
	void DrawInstancingSprite_2D_(std::vector<Sprite*> sprits);

	void DrawLine_(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource);
	void DrawPoint_(std::list<Vector3> points, PrimitiveManager::PrimitiveResource primitiveResource);
	void DrawAABB_(std::list<AABB> aabbs, PrimitiveManager::PrimitiveResource primitiveResource);

	Keybord GetKeybord_();
	Mouse GetMouse_();
	Pad GetPad_(int usePadNum = 0);

	WindowsAPI* GetWindowsAPI_() { return winApp_; }

	// インスタンス生成
	static GameEngine* getInstance();

public:

	// コピー、代入を禁止する
	GameEngine(const GameEngine*) = delete;
	GameEngine* operator=(const GameEngine*) = delete;

	static void Delete() { delete getInstance(); }

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名 (例:L"LE2A_00_ミョウジ_ナマエ")</param>
	/// <param name="kWindowWidth">ウィンドウの幅 (例:1280)</param>
	/// <param name="kWindowHeight">ウィンドウの高さ (例:720)</param>
	static void Intialize(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720) { getInstance()->Intialize_(WindowName, kWindowWidth, kWindowHeight); }

	[[nodiscard]]
	static Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2() { return getInstance()->GetXAudio2_(); }

	static float randomFloat(float minFloat, float maxFloat) { return getInstance()->randomFloat_(minFloat, maxFloat); }
	static int32_t randomInt(int32_t minInt, int32_t maxInt) { return getInstance()->randomInt_(minInt, maxInt); }

	/// <summary>
	/// フレームの開始
	/// </summary>
	/// <returns>Windowsのメッセージがあるか</returns>
	[[nodiscard]]
	static bool StartFlame() { return getInstance()->StartFlame_(); }

	/// <summary>
	/// ウィンドウ状態
	/// </summary>
	/// <returns>ウィンドウを閉じているか</returns>
	[[nodiscard]]
	static bool WindowState() { return getInstance()->WindowState_(); }

	//描画前処理
	static void PreDraw() { getInstance()->PreDraw_(); }

	//描画後処理
	static void PostDraw() { getInstance()->PostDraw_(); }

	static Microsoft::WRL::ComPtr <ID3D12RootSignature> RootSignature() { return getInstance()->RootSignature_(); }
	static Microsoft::WRL::ComPtr <ID3D12RootSignature> InstancingRootSignature() { return getInstance()->InstancingRootSignature_(); }

	[[nodiscard]]
	static ID3D12PipelineState* TrianglePSO() { return getInstance()->TrianglePSO_(); }

	[[nodiscard]]
	static ID3D12PipelineState* ParticlePSO() { return getInstance()->ParticlePSO_(); }

	[[nodiscard]]
	static ID3D12PipelineState* LinePSO() { return getInstance()->LinePSO_(); }

	//ウィンドウ幅
	[[nodiscard]]
	static int32_t GetWindowWidth() { return kWindowWidth_; }

	//ウィンドウ高さ
	[[nodiscard]]
	static int32_t GetWindowHeight() { return kWindowHeight_; }

	//デバイス
	[[nodiscard]]
	static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return getInstance()->GetDevice_(); }


	static void DrawObject_3D(Object* object, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) { return getInstance()->DrawObject_3D_(object, directionalLight, pointLight, spotLight); }
	static void DrawInstancingObject_3D(std::list<Object*> objects, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) { return getInstance()->DrawInstancingObject_3D_(objects, directionalLight, pointLight, spotLight); }
	static void DrawParticle(ParticleGroup particleGroup) { return getInstance()->DrawParticle_(particleGroup); }
	
	static void DrawSprite_2D(Sprite* sprite) { return getInstance()->DrawSprite_2D_(sprite); }
	static void DrawInstancingSprite_2D(std::vector<Sprite*> sprits) { return getInstance()->DrawInstancingSprite_2D_(sprits); }

	static void DrawLine(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource) { return getInstance()->DrawLine_(lines, primitiveResource); }
	static void DrawPoint(std::list<Vector3> points, PrimitiveManager::PrimitiveResource primitiveResource) { return getInstance()->DrawPoint_(points, primitiveResource); }
	static void DrawAABB(std::list<AABB> aabbs, PrimitiveManager::PrimitiveResource primitiveResource) { return getInstance()->DrawAABB_(aabbs, primitiveResource); }
	
	//キーボード入力
	[[nodiscard]]
	static Keybord GetKeybord() { return getInstance()->GetKeybord_(); }

	//マウス入力
	[[nodiscard]]
	static Mouse GetMouse() { return getInstance()->GetMouse_(); }

	/// <summary>
	/// パッド入力
	/// </summary>
	/// <param name="usePadNum">参照するパッドの番号。1つ目なら0を入力</param>
	[[nodiscard]]
	static Pad GetPad(int usePadNum = 0) { return getInstance()->GetPad_(usePadNum); }

	[[nodiscard]]
	static WindowsAPI* GetWindowsAPI() { return getInstance()->GetWindowsAPI_(); }

	static DirectXCommon* GetDirectXCommon() { return getInstance()->dxCommon_; }
};