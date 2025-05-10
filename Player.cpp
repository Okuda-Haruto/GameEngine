#define NOMINMAX
#include "Player.h"
#include <numbers>
#include <algorithm>

void Player::Initialize(Object_3D* object,Texture* texture,Light* light, const Vector3& position) {
	//NULLポインタチェック
	assert(object);

	object_ = object;
	texture_ = texture;
	light_ = light;

	data_.transform.translate = position;
}

void Player::Update(Keybord keybord) {

	//着地フラグ
	bool landing = false;

	//地面との当たり判定
	//下降中か
	if (velocity_.y < 0) {
		//Y座標が地面以下になったら着地
		if (data_.transform.translate.y <= 2.0f) {
			landing = true;
		}
	}
	//接地判定
	if (onGround_) {
		//ジャンプ開始
		if (velocity_.y > 0.0f) {
			//空中状態に移行
			onGround_ = false;
		}
	} else {
		//着地
		if (landing) {
			//めり込み排除
			data_.transform.translate.y = 2.0f;
			//摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAcceleration);
			//下方向速度をリセット
			velocity_.y = 0.0f;
			//接地状態に移行
			onGround_ = true;
		}
	}

	//移動入力
	if (onGround_) {
		//左右移動操作
		if (keybord.trigger[DIK_RIGHT] || keybord.trigger[DIK_LEFT]) {
			//左右加速
			Vector3 acceleration = {};
			if (keybord.trigger[DIK_RIGHT]) {
				//左移動中の右入力
				if (velocity_.x < 0.0f) {
					//速度と逆方向に入力中はブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = data_.transform.rotate.y;
					turntimer_ = kTimeTurn - turntimer_;
				}
			} else if (keybord.trigger[DIK_LEFT]) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中はブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::KLeft) {
					lrDirection_ = LRDirection::KLeft;
					turnFirstRotationY_ = data_.transform.rotate.y;
					turntimer_ = kTimeTurn - turntimer_;
				}
			}
			//加速/減速
			velocity_.x += acceleration.x;

			//最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		//ジャンプ処理
		if (keybord.hit[DIK_UP]) {
			//ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}
		//空中
	} else {
		//落下速度
		velocity_.y += -kGravityAcceleration;
		//落下速度制限
		velocity_.y = std::max(velocity_.y,-kLimitFallSpeed);
	}

	data_.transform.translate.x += velocity_.x;
	data_.transform.translate.y += velocity_.y;

	//旋回制御
	if (turntimer_ > 0.0f) {

		turntimer_ -= 1.0f / 60.0f;

		//左右の自キャラ回転テーブル
		float destinationRotationYTable[] = {
			0,
			std::numbers::pi_v<float>,
		};

		//状況に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		data_.transform.rotate.y = turnFirstRotationY_ * (turntimer_ / kTimeTurn) + destinationRotationY * ((kTimeTurn - turntimer_) / kTimeTurn);
	}

	//行列の更新
	object_->SetLight(light_);
	object_->SetTexture(texture_);
}

void Player::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Camera camera) {

	data_.camera = camera;

	object_->Draw(commandList, data_);
}