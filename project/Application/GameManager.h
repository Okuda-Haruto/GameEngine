#pragma once
#include "Scene/GameScene/GameScene.h"
#include "Scene/TitleScene/TitleScene.h"
#include "ModelHolder/ModelHolder.h"

class GameManager {
private:
	BaseScene* scene_ = nullptr;
	std::unique_ptr<ModelHolder> modelHolder_;
	std::unique_ptr<SpriteManager> spriteManager_;
public:

	~GameManager();

	void Initialize();

	void Update();

	void Draw();;

	void ChangeScene(BaseScene* nextScene);
};