#pragma once
#include <string>
#include <GameCamera/GameCamera.h>
#include <Character/Player/Player.h>
#include <Character/Boss/Boss.h>
#include <Bullet/PlayerBullet/PlayerBullet.h>
#include <Bullet/BossBullet/BossBullet.h>

//ステージ
class Stage {
private:
	//メインカメラ
	std::unique_ptr<GameCamera> gameCamera_;

	//背景
	std::vector<std::unique_ptr<Object>> backGroundObject_;
	//接地、壁判定
	std::vector<Colliders> groundCollider_;
	//キャラクター
	std::unique_ptr<Player> player_;
	std::unique_ptr<Boss> boss_;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullet_;
	std::list<std::unique_ptr<BossBullet>> bossBullet_;

	//入力
	std::shared_ptr<Input> input_;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="stageName">選択したステージ</param>
	/// <param name="input">入力</param>
	void Initialize(std::string stageName, std::shared_ptr<Input> input);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};