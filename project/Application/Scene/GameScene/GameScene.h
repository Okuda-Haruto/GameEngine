#pragma once
#include "GameCamera/GameCamera.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "Character/Player/Player.h"
#include "ModelHolder/ModelHolder.h"

#include <memory>

class GameScene
{
private:

	//モデルを保持
	std::unique_ptr<ModelHolder> modelHolder_;

	//メインカメラ
	std::unique_ptr<GameCamera> gameCamera_;
	//背景
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	//キャラクター
	std::unique_ptr<Player> player_;

	//光源
	std::unique_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
};

