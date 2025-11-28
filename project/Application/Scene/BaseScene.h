#pragma once
#include "ModelHolder/ModelHolder.h"
#include "SpriteManager/SpriteManager.h"
#include <memory>

enum class Fade {
	None,
	FadeIn,
	FadeOut,
};

class BaseScene {
protected:
	const float kMaxFadeTime = 0.5f;
	float fadeTime_;

	bool isfinished_ = false;
	BaseScene* nextScene_ = nullptr;

	Fade fade_ = Fade::FadeIn;
public:
	virtual ~BaseScene() {}

	virtual void Initialize(ModelHolder* modelHolder, SpriteManager* spriteManager) {}
	virtual void Update() {}
	virtual void Draw() {}

	bool IsFinished() { return isfinished_; }
	BaseScene* NextScene() { return nextScene_; }
};