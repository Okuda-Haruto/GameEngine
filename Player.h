#pragma once
#include <GameEngine.h>

//自キャラ
class Player {
private:
	//加速度
	static inline const float kAcceleration = 0.05f;
	//減速度
	static inline const float kAttenuation = 0.1f;
	//最高速度
	static inline const float kLimitRunSpeed = 1.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.1f;
	//最高落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	//ジャンプ初速
	static inline const float kJumpAcceleration = 1.0f;

	// 左右
	enum class LRDirection {
		kRight,
		KLeft,
	};
	//向き
	LRDirection lrDirection_ = LRDirection::kRight;

	//接地状態
	bool onGround_ = true;

	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turntimer_ = 0.0f;

	//ワールド変換データ
	Object_3D_Data data_;
	//モデル
	Object_3D* object_ = nullptr;
	
	Texture* texture_ = nullptr;

	Light* light_ = nullptr;

	//速度
	Vector3 velocity_ = {};
public:
	//初期化
	void Initialize(Object_3D* object, Texture* texture, Light* light, const Vector3& position);
	//更新
	void Update(Keybord keybord);
	//描画
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Camera camera);

	void Reset() { object_->Reset(); };
};