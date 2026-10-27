#pragma once
#include "Object/Object.h"

class Player;

class Hat {
private:
	//体力
	std::unique_ptr<Object> object_;
	SRT transform_;
	//ハット用アニメーション時間
	const float kMaxAnimationTime = 2.0f;
	float animationTime_ = 0.0f;

	Player* player_;
public:

	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player);

	void Update();

	void Draw();
};