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
#include <ParticleEmitter/ParticleEmitter.h>

class SampleScene
{
private:
	WindowsAPI* winApp_ = nullptr;

	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<Object*, 7> object_{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
	// 3Dモデルデータ
	std::array<SRT, 7> objectTransform_;

	Emitter emitter_;
	AccelerationField accelerationField_;
	ParticleEmitter* particleEmitter_ = nullptr;

	//2Dモデル
	SpriteManager* spriteManager_ = nullptr;
	std::array<bool, 2> isSpriteDraw_ = { true,false };
	std::array<Sprite*,2> sprite_;

	//Grid* grid_ = nullptr;

	Audio* audio_ = nullptr;

	//AxisIndicator* axis_ = nullptr;
	
	// カメラ
	SRT cameraTransform_{};
	Camera* defaultCamera_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLightElement directionalLightElement_;
	DirectionalLight* directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
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

