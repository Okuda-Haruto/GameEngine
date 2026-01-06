#pragma once
#include "BaseScene/BaseScene.h"
#include "Sprite/Sprite.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "BackGround/Fence/Fence.h"
#include "GameCamera/GameCamera.h"
#include "DirectionalLight/DirectionalLight.h"

using namespace std;

class ClearScene : public BaseScene
{
private:

	shared_ptr<Input> input_;

	std::unique_ptr<Sprite>fadeSprite_;

	//メインカメラ
	shared_ptr<Camera> camera_;

	shared_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Fence> fence_;
	std::unique_ptr<Object> player_;

	std::unique_ptr<Sprite>titleSprite_;

	float animationTime = 0.0f;

public:

	~ClearScene();

	//初期化
	void Initialize(shared_ptr<Input> input) override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

