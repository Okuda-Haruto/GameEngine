#pragma once
#include <GameCamera/GameCamera.h>
#include <Sprite/Sprite.h>

#include <memory>

class Target {
private:
	//カメラ
	std::weak_ptr<GameCamera> gameCamera_;

	//照準スプライト
	std::pair<bool,std::unique_ptr<Sprite>> sprite_;

public:

	//初期化
	void Initialize(std::weak_ptr<GameCamera> gameCamera);

	//更新処理
	void Update();

	//描画処理
	void Draw();
};