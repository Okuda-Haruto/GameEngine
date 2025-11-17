#include "Player.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"

Player::~Player() {

}

void Player::Initialize(ModelHolder* modelHolder) {
	modelHolder_ = modelHolder;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder_->GetModel(ModelIndex::Player));
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,1.0f,-20.0f };
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;
	dodgeCoolTime = 0.0f;
	dodgeActiveTime = kMaxDodgeActiveTime;
}

void Player::Update() {
	Pad pad = GameEngine::GetPad();
	Keybord keys = GameEngine::GetKeybord();
	const float deadZone = 0.7f;
	bool isMove = false;

	//速さ
	const float speed = 0.3f;

	Vector3 move{};

	//移動量
	if (pad.isConnected) {
		move = {
			pad.LeftStick.vector.x * pad.LeftStick.magnitude,0.0f,pad.LeftStick.vector.y * pad.LeftStick.magnitude
		};
	}
	if (Vector3::Length(move) > deadZone) {
		isMove = true;
	}

	if (!isMove) {
		if (keys.hold[DIK_W] || keys.hold[DIK_UP]) {
			move.z = 1.0f;
		}
		if (keys.hold[DIK_S] || keys.hold[DIK_DOWN]) {
			move.z = -1.0f;
		}
		if (keys.hold[DIK_D] || keys.hold[DIK_RIGHT]) {
			move.x = 1.0f;
		}
		if (keys.hold[DIK_A] || keys.hold[DIK_LEFT]) {
			move.x = -1.0f;
		}
		if (Vector3::Length(move) > deadZone) {
			isMove = true;
		}
	}

#pragma region 移動処理

	//回避中は移動させないこと
	if (isMove && dodgeActiveTime >= kMaxDodgeActiveTime) {
		//移動量に速さを反映
		velocity_ = Vector3::Normalize(move) * speed;

		Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateYMatrix(cameraTransform_->rotate.y);
		velocity_ = rotateMatrix * velocity_;

		//移動
		transform_.translate += velocity_;

		angle = std::atan2(velocity_.x, velocity_.z);

	}

	//最短角度補完
	float diff = angle - transform_.rotate.y ;

	if (diff >= std::numbers::pi_v<float>*2 ) {
		diff = angle - transform_.rotate.y;
	}

	diff = std::fmodf(diff, std::numbers::pi_v<float> * 2);

	if (diff > std::numbers::pi_v<float>) {
		diff -= std::numbers::pi_v<float> * 2;
	} else if (diff < -std::numbers::pi_v<float>) {
		diff += std::numbers::pi_v<float> * 2;
	}

	if (diff > std::numbers::pi_v<float>) {
		diff -= std::numbers::pi_v<float> *2;
	} else if (diff < -std::numbers::pi_v<float>) {
		diff += std::numbers::pi_v<float> *2;
	}

	//正直あまりやりたくはない方法だが2πを超えると遠回りで回転してしまうので致し方無い
	transform_.rotate.y = (transform_.rotate.y + diff * 0.1f) ;
	transform_.rotate.y = std::fmodf(transform_.rotate.y, std::numbers::pi_v<float> * 2);

#pragma endregion

#pragma region 回避行動

	const float dodgeSpeed = 2.0f;

	//回避中
	if(dodgeActiveTime < kMaxDodgeActiveTime) {
		dodgeActiveTime += 1.0f / 60.0f;
		//kMaxDodgeActiveTimeを超えると角度がおかしくなる気がするので戻す
		if (dodgeActiveTime >= kMaxDodgeActiveTime) {
			//dodgeActiveTime = kMaxDodgeActiveTime;
		}
		velocity_ = Vector3{ 0.0f,0.0f,1.0f } * Lerp(0.2f,dodgeSpeed,powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
		//前転
		transform_.rotate.x = Lerp(0.0f, std::numbers::pi_v<float> * 2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime,3));

		Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateYMatrix(transform_.rotate.y);
		velocity_ = rotateMatrix * velocity_;

		//移動
		transform_.translate += velocity_;
	//回避中じゃない
	} else {
		//回避インターバル
		if (dodgeCoolTime < kMaxDodgeCoolTime) {
			dodgeCoolTime += 1.0f / 60.0f;
		}
	}

	//回避インターバル
	if (dodgeCoolTime >= kMaxDodgeCoolTime) {
		if (keys.trigger[DIK_C] && isMove) {
			dodgeActiveTime = 0.0f;
			dodgeCoolTime = 0.0f;
		}
	}

#pragma endregion

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", transform_.rotate.y / std::numbers::pi_v<float> * 180.0f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x);
	ImGui::End();
#endif

	*targetTransform_ = transform_;

	object_->SetTransform(transform_);
}

void Player::Draw() {
	object_->Draw3D();
}