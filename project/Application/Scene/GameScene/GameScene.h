#pragma once
#include "GameCamera/GameCamera.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "Caracter/Player/Player.h"

#include <memory>

class GameScene
{
private:
	//メインカメラ
	std::unique_ptr<GameCamera> gameCamera_;
	//背景
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	//キャラクター
	std::unique_ptr<Player> player_;
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
};

