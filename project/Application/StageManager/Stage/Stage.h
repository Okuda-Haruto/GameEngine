#pragma once
#include <string>
#include <GameCamera/GameCamera.h>
#include <DebugCamera.h>
#include <BackGround/BackGround.h>
#include <Entity/Player/Player.h>
#include <Entity/Boss/Boss.h>
#include <Entity/Bullet/Bullet.h>
#include <Entity/Bomb/Bomb.h>
#include <Entity/ShockWave/ShockWave.h>
#include <Entity/Item/Item.h>
#include <Entity/BreakObject/BreakObject.h>
#include <Entity/ColliderObject/ColliderObject.h>
#include <Event/Event.h>
#include <HUD/HUD.h>
#include "../StageManager.h"

#include <PrimitiveManager/PrimitiveCylinder.h>
#include <ParticleEmitter/ParticleEmitter.h>
#include <PrimitiveManager/PrimitiveRing.h>

//ステージ
class Stage {
protected:
	//メインカメラ
	std::shared_ptr<GameCamera> gameCamera_;
	std::shared_ptr<DebugCamera> debugCamera_;

	//背景
	std::unique_ptr<BackGround> backGround_;
	//接地、壁判定
	std::vector<Colliders> groundCollider_;
	//キャラクター
	std::unique_ptr<Player> player_;
	std::unique_ptr<Boss> boss_;
	//弾
	std::list<std::unique_ptr<Bullet>> bullets_;
	std::list<std::unique_ptr<Bomb>> bombs_;
	std::list<std::unique_ptr<ShockWave>> shockWaves_;

	//アイテム
	std::list<std::unique_ptr<Item>> items_;

	//破壊可能オブジェクト
	std::list<std::unique_ptr<BreakObject>> breakObjects_;
	//接触判定オブジェクト
	std::vector<std::unique_ptr<ColliderObject>> colliderObjects_;

	//情報表示
	std::unique_ptr<HUD> hud_;

	//入力
	std::shared_ptr<Input> input_;


	//光源	いつかは外部で管理したい
	shared_ptr<DirectionalLight> directionalLight_;
	DirectionalLightElement directionalLightElement_;
	shared_ptr<PointLight> pointLight_;
	PointLightElement pointLightElement_;

	std::unique_ptr<ParticleEmitter> particle_;
	std::unique_ptr<ParticleEmitter> particle_2;
	std::unique_ptr<ParticleEmitter> particle_3;
	std::unique_ptr<ParticleEmitter> particle_4;

	std::unique_ptr<PrimitiveRing> ring_;
	SRT ringTransform_;
	Material ringMaterial_;

	std::unique_ptr<PrimitiveCylinder> cylider_;
	SRT cylinderTransform_;
	Material cylinderMaterial_;
	const float kMaxCyliderTime_ = 0.6f;
	float cylinderTime_;

	bool isClear_ = false;
	bool isEnd_ = false;
	bool isTutorial_ = false;
public:

	~Stage();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="stageData">選択したステージ</param>
	/// <param name="input">入力</param>
	void Initialize(bool isTutorial, StageData stageData, std::shared_ptr<Input> input);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	//Spriteのみ
	void DrawSprite();

	//衝突
	void Collision();

	bool BossObstructed(const Capsule& capsule);

	//コライダーを考慮した移動
	Vector3 MoveWithCollision(SphereCollider& collider, Vector3 velocity);

	//後でクォータニオンにしたい
	void AddBullet(SRT transform, float speed, CollisionID id, Bullet::BulletMove bulletMove, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> event = nullptr);
	void AddBomb(SRT transform, float range, float maxLifeTime, CollisionID id, std::shared_ptr<Model> model);
	void AddShockWave(SRT transform, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder);
	void AddItem(SRT transform);
	void AddBreakObject(SRT transform, float maxHP, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> event);

	void Explosion(Vector3 position, float range, float maxLifeTime, CollisionID id, float damage);

	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { debugCamera_ = debugCamera; gameCamera_->SetDebugCamera(debugCamera_); }

	std::shared_ptr<GameCamera> GetGameCamera() { return gameCamera_; }
	std::shared_ptr<DirectionalLight> GetDirectionalLight() { return directionalLight_; }
	std::shared_ptr<PointLight> GetPointLight() { return pointLight_; }
	Player* GetPlayer() { return player_.get(); }
	Boss* GetBoss() { return boss_.get(); }
	bool IsClear() { return isClear_; }
	bool IsEnd() { return isEnd_; }

	void ClearColliderObjects() { colliderObjects_.clear(); }
	void ClearBreakObjects() { breakObjects_.clear(); }

	void SetRingColorA(float a) { ringMaterial_.color.w = a; }
};