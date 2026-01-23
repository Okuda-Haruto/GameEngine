#pragma once
#include <GameEngine.h>
#include <Line.h>
#include <array>
#include <Input/Input.h>
#include <PointLight/PointLight.h>
#include <SpotLight/SpotLight.h>
#include <WindowsAPI/WindowsAPI.h>

#include <Object/Object.h>
#include <Sprite/Sprite.h>
#include <ParticleEmitter/ParticleEmitter.h>

using namespace std;

class SampleScene
{
private:

	shared_ptr<Input> input_;

	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<unique_ptr<Object>, 7> object_;
	// 3Dモデルデータ
	std::array<SRT, 7> objectTransform_;

	Emitter emitter_;
	AccelerationField accelerationField_;
	unique_ptr<ParticleEmitter> particleEmitter_ = nullptr;

	//2Dモデル
	std::array<bool, 2> isSpriteDraw_ = { true,false };
	std::array<unique_ptr<Sprite>,2> sprite_;

	unique_ptr<Audio> audio_ = nullptr;
	
	// カメラ
	SRT cameraTransform_{};
	shared_ptr<Camera> defaultCamera_ = nullptr;

	//デバッグカメラ
	shared_ptr<DebugCamera> debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLightElement directionalLightElement_;
	shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	PointLightElement pointLightElement_;
	shared_ptr<PointLight> pointLight_ = nullptr;

	SpotLightElement spotLightElement_;
	shared_ptr<SpotLight> spotLight_ = nullptr;

	bool isDisplayUI = true;

public:
	//デストラクタ
	~SampleScene();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
};

