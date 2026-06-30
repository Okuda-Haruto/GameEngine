#pragma once
#include <Object/Object.h>
#include <Sprite/Sprite.h>

class Player;

class HUD {
private:
	//装弾数
	std::unique_ptr<Object> cylinder_;
	SRT cylinderTransform_;
	//体力
	std::unique_ptr<Object> hat_;
	SRT hatTransform_;
	//ハット用アニメーション時間
	const float kMaxAnimationTime = 2.0f;
	float animationTime_ = 0.0f;

	//操作説明
	std::array<std::unique_ptr<Sprite>, 4> sprite_;
	std::array<SRT, 4> spriteTransform;

	Player* player_;
public:

	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player);

	void Update();

	void Draw();
	void DrawSprite();
};