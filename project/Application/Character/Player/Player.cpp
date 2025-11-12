#include "Player.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"

void Player::Initialize(ModelHolder* modelHolder) {
	modelHolder_ = modelHolder;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder_->GetModel(ModelIndex::Player));
	transform_.translate.y += 1.0f;
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	targetTransform_->scale = { 1.0f,1.0f,1.0f };
	targetTransform_->rotate = { 0.0f,0.0f,0.0f };
	targetTransform_->translate = { 0.0f,1.0f,0.0f };
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
		if (keys.hold[DIK_W]) {
			move.z = 1.0f;
		}
		if (keys.hold[DIK_S]) {
			move.z = -1.0f;
		}
		if (keys.hold[DIK_D]) {
			move.x = 1.0f;
		}
		if (keys.hold[DIK_A]) {
			move.x = -1.0f;
		}
		if (Vector3::Length(move) > deadZone) {
			isMove = true;
		}
	}

	if (isMove) {
		//移動量に速さを反映
		move = Vector3::Normalize(move) * speed;

		Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateYMatrix(cameraTransform_->rotate.y);
		move = rotateMatrix * move;

		//移動
		transform_.translate += move;

		angle = std::atan2(move.x, move.z);

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

	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", transform_.rotate.y / std::numbers::pi_v<float> * 180.0f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::End();

	*targetTransform_ = transform_;
}

void Player::Draw() {
	object_->Draw3D();
}