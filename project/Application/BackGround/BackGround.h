#pragma once
#include <memory>
#include <Object/Object.h>
#include <GameCamera/GameCamera.h>
#include <ModelHolder/ModelHolder.h>
#include "Fence/Fence.h"
#include "Ground/Ground.h"
#include "Skydome/Skydome.h"
#include "Tumbleweed/Tumbleweed.h"

class BackGround
{
private:
	//背景モデル
	std::vector<std::unique_ptr<Object>> objects_;
	std::vector<int> indexes_;
	std::unique_ptr<Fence> fence_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Tumbleweed> tumbleweed_;

	std::shared_ptr<DirectionalLight> directionalLight_;
	std::shared_ptr<PointLight> pointLight_;
	std::shared_ptr<GameCamera> gameCamera_;

public:
	//初期化
	void Initialize(std::string filePath, std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<PointLight> pointLight);
	//更新処理
	void Update();
	//描画
	void Draw();

	void LoadBackGroundObject(std::string filePath);
	void SaveBackGround(std::string filePath);
};
