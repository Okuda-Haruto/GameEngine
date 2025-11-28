#pragma once
#include "../BaseScene.h"
#include "Sprite/Sprite.h"

class TitleScene : public BaseScene
{
private:
	//モデルを保持
	ModelHolder* modelHolder_;

	std::unique_ptr<Sprite>fadeSprite_;
	std::unique_ptr<Sprite>titleSprite_;

public:

	~TitleScene();

	//初期化
	void Initialize(ModelHolder* modelHolder, SpriteManager* spriteManager);
	//更新
	void Update();
	//描画
	void Draw();
};

