#pragma once
#include <string>
#include <GameCamera/GameCamera.h>
#include <DebugCamera.h>
#include <BackGround/BackGround.h>
#include <Character/Player/Player.h>
#include <Character/Boss/Boss.h>
#include <Bullet/PlayerBullet/PlayerBullet.h>
#include <Bullet/BossBullet/BossBullet.h>
#include <ColliderObject/ColliderObject.h>
#include <HUD/HUD.h>
#include "../StageManager.h"

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
	std::list<std::unique_ptr<PlayerBullet>> playerBullet_;
	std::list<std::unique_ptr<BossBullet>> bossBullet_;

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

	bool isClear_ = false;
	bool isEnd_ = false;
public:

	~Stage();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="stageData">選択したステージ</param>
	/// <param name="input">入力</param>
	void Initialize(StageData stageData, std::shared_ptr<Input> input);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	void DrawSprite();

	void Collision();

	//後でクォータニオンにしたい
	void AddPlayerBullet(Vector3 translate, Vector3 rotate);
	void AddBossBullet(Vector3 translate, Vector3 rotate);

	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { debugCamera_ = debugCamera; gameCamera_->SetDebugCamera(debugCamera_); }

	std::shared_ptr<GameCamera> GetGameCamera() { return gameCamera_; }
	std::shared_ptr<DirectionalLight> GetDirectionalLight() { return directionalLight_; }
	std::shared_ptr<PointLight> GetPointLight() { return pointLight_; }
	Player* GetPlayer() { return player_.get(); }
	Boss* GetBoss() { return boss_.get(); }
	bool IsClear() { return isClear_; }
	bool IsEnd() { return isEnd_; }

	void ClearColliderObjects() { colliderObjects_.clear(); }
};