#pragma once
#include <GameEngine.h>
#include <Line.h>
#include <array>
#include <Effect.h>
#include <Input/Input.h>
#include <PointLight.h>
#include <SpotLight.h>
#include <WindowsAPI/WindowsAPI.h>

#include <Object/Object.h>
#include <Sprite/Sprite.h>

class SampleScene
{
private:
	WindowsAPI* winApp_ = nullptr;

	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<Object*, 7> object_{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
	// 3Dモデルデータ
	std::array<SRT, 7> objectTransform_;

	//SRT EffectTransform;
	//Vector4 EffectColor = { 1,1,1,1 };
	//Effect* effect_ = nullptr;
	//bool isSpawnEffect_ = false;
	//bool isUseField = false;

	//2Dモデル
	SpriteManager* spriteManager_ = nullptr;
	bool isSpriteDraw_ = false;
	std::array<Sprite*,2> sprite_;
	// 2Dモデルデータ
	//Object_Single_Data spriteData_;

	//Grid* grid_ = nullptr;

	Audio* audio_ = nullptr;

	// カメラ
	SRT cameraTransform_{};
	Camera* camera_;

	//AxisIndicator* axis_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLightElement directionalLightElement_;
	DirectionalLight* directionalLight_ = nullptr;
	UINT isLighting_ = 2;
	float shininess_ = 40.0f;

	PointLightElement pointLightElement_;
	PointLight* pointLight_ = nullptr;

	SpotLightElement spotLightElement_;
	SpotLight* spotLight_ = nullptr;


	//インプット
	Input* input = nullptr;


	bool isDisplayUI = true;

public:
	//デストラクタ
	~SampleScene();
	//初期化
	void Initialize(WindowsAPI* winApp, DirectXCommon* dxCommon);
	//更新
	void Update();
	//描画
	void Draw();
};

