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

class SampleScene
{
private:

	std::shared_ptr<Input> input_;

	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<std::unique_ptr<Object>, 7> object_;

	std::vector<std::unique_ptr<Object>> gltfs_;
	int gltfIndex_ = 0;

	// 3Dモデルデータ
	std::array<SRT, 7> objectTransform_;

	Emitter emitter_;
	AccelerationField accelerationField_;
	std::unique_ptr<ParticleEmitter> particleEmitter_ = nullptr;

	//2Dモデル
	std::array<bool, 2> isSpriteDraw_ = { true,false };
	std::array<std::unique_ptr<Sprite>,2> sprite_;

	std::unique_ptr<Audio> audio_ = nullptr;
	
	// カメラ
	SRT cameraTransform_{};
	std::shared_ptr<Camera> defaultCamera_ = nullptr;

	//デバッグカメラ
	std::shared_ptr<DebugCamera> debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLightElement directionalLightElement_;
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	PointLightElement pointLightElement_;
	std::shared_ptr<PointLight> pointLight_ = nullptr;

	SpotLightElement spotLightElement_;
	std::shared_ptr<SpotLight> spotLight_ = nullptr;

	bool isDisplayUI = false;

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

