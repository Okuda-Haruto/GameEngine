#pragma once
#include "../BaseScene.h"
#include "Sprite/Sprite.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "BackGround/Fence/Fence.h"
#include "GameCamera/GameCamera.h"
#include "Light/DirectionalLight/DirectionalLight.h"



class TitleScene : public BaseScene
{
private:

	std::shared_ptr<Input> input_;

	std::unique_ptr<Sprite>fadeSprite_;

	//メインカメラ
	std::shared_ptr<Camera> camera_;

	std::shared_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Fence> fence_;
	std::unique_ptr<Object> player_;

	std::unique_ptr<Sprite>titleSprite_;
	std::unique_ptr<Sprite>press_B_Start_Sprite_;
	std::unique_ptr<Sprite>credit_Sprite_;

	float animationTime = 0.0f;

public:

	~TitleScene();

	//初期化
	void Initialize(std::shared_ptr<Input> input) override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

