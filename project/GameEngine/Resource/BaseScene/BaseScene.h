#pragma once
#include "ModelHolder/ModelHolder.h"
#include "SpriteManager/SpriteManager.h"
#include <memory>
#include <Input/Input.h>

class SceneManager;

enum class Fade {
	None,
	FadeIn,
	FadeOut,
};

class BaseScene {
protected:
	SceneManager* sceneManager_ = nullptr;

	const float kMaxFadeTime = 0.5f;
	float fadeTime_;

	Fade fade_ = Fade::FadeIn;
public:
	virtual ~BaseScene() = default;

	virtual void Initialize(shared_ptr<Input> input) {}
	virtual void Finalize() {}
	virtual void Update() {}
	virtual void Draw() {}

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };
};