#pragma once
#include "../BaseScene.h"
#include "Sprite/Sprite.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "BackGround/Fence/Fence.h"
#include "GameCamera/GameCamera.h"
#include "DirectionalLight.h"

class TitleScene : public BaseScene
{
private:
	//モデルを保持
	ModelHolder* modelHolder_;

	std::unique_ptr<Sprite>fadeSprite_;

	//メインカメラ
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Fence> fence_;

	std::unique_ptr<Sprite>titleSprite_;
	std::unique_ptr<Sprite>pless_B_Start_Sprite_;

	float animationTime = 0.0f;

public:

	~TitleScene();

	//初期化
	void Initialize(ModelHolder* modelHolder, SpriteManager* spriteManager);
	//更新
	void Update();
	//描画
	void Draw();
};

