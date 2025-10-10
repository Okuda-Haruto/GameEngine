#pragma once
#include <GameEngine.h>
#include <Line.h>
#include <array>
#include <Effect.h>
#include <Input/Input.h>
#include <PointLight.h>

class SampleScene
{
private:
	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<Object_3D*, 7> object_{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
	// 3Dモデルデータ
	std::array<Object_Multi_Data, 7> objectData_;

	SRT EffectTransform;
	Vector4 EffectColor = { 1,1,1,1 };
	Effect* effect_ = nullptr;
	bool isSpawnEffect_ = false;
	bool isUseField = false;

	//2Dモデル
	bool isSpriteDraw_ = false;
	Sprite_2D* sprite_ = nullptr;
	// 2Dモデルデータ
	Object_Single_Data spriteData_;

	Grid* grid_ = nullptr;

	Audio* audio_ = nullptr;

	// カメラ
	SRT cameraTransform_{};
	Camera* camera_;

	AxisIndicator* axis_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLightElement directionalLightElement_;
	DirectionalLight* directionalLight_ = nullptr;
	UINT isLighting_ = 2;

	PointLightElement pointLightElement_;
	PointLight* pointLight_ = nullptr;


	//インプット
	Input* input = nullptr;


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

