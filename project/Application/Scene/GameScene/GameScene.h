#pragma once
#include <BaseScene/BaseScene.h>
#include "GameCamera/GameCamera.h"
#include "BackGround/Skydome/Skydome.h"
#include "BackGround/Ground/Ground.h"
#include "BackGround/Fence/Fence.h"
#include "Bullet/PlayerBullet/PlayerBullet.h"
#include "Bullet/BossBullet/BossBullet.h"
#include "Character/Player/Player.h"
#include "Character/Boss/Boss.h"
#include "Sprite/Sprite.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include <array>

class GameScene : public BaseScene
{
private:

	//メインカメラ
	std::unique_ptr<GameCamera> gameCamera_;
	//背景
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Fence> fence_;
	//キャラクター
	std::unique_ptr<Player> player_;
	std::unique_ptr<Boss> boss_;

	std::list<PlayerBullet*> playerBullet_;
	std::list<BossBullet*> bossBullet_;

	std::unique_ptr<Object> cylinder_;
	SRT cylinderTransform_;
	std::unique_ptr<Object> hat_;
	SRT hatTransform_;
	const float kMaxAnimationTime = 2.0f;
	float animationTime_ = 0.0f;

	std::unique_ptr<Sprite>fadeSprite_;

	std::unique_ptr<ParticleEmitter> particle_;
	std::unique_ptr<ParticleEditor> editor_;
	std::unique_ptr<ParticleEmitter> particle_2;
	std::unique_ptr<ParticleEditor> editor_2;
	std::unique_ptr<ParticleEmitter> particle_3;
	std::unique_ptr<ParticleEditor> editor_3;
	std::unique_ptr<ParticleEmitter> particle_4;
	std::unique_ptr<ParticleEditor> editor_4;

	std::array<Sprite*, 4> sprite_;
	std::array<SRT,4> spriteTransform;


	//光源
	std::unique_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;
	std::unique_ptr<PointLight> pointLight_;
	PointLightElement pointLightElement_;
public:

	~GameScene();

	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	void Collision();

	void AddPlayerBullet(Vector3 translate, Vector3 rotate);
	void AddBossBullet(Vector3 translate, Vector3 rotate);
};

