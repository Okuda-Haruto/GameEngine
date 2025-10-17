#include "Player.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"
#include "Math/Matrix4x4_operation.h"
#include <numbers>
#include "Math/Lerp.h"

void Player::Initialize() {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize("resources/Caracter/Player", "Player.obj");
	objectTransform.translate.y += 1.0f;
	object_->SetTransform(objectTransform);

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
	if (Length(move) > deadZone) {
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
		if (Length(move) > deadZone) {
			isMove = true;
		}
	}

	if (isMove) {
		//移動量に速さを反映
		move = Normalize(move) * speed;

		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(0.0f), Multiply(MakeRotateYMatrix(cameraTransform_->rotate.y), MakeRotateZMatrix(0.0f)));
		move = rotateMatrix * move;

		//移動
		objectTransform.translate += move;

		angle = std::atan2(move.x, move.z);

	}

	//最短角度補完
	float diff = angle - objectTransform.rotate.y ;

	if (diff >= std::numbers::pi_v<float>*2 ) {
		diff = angle - objectTransform.rotate.y;
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
	objectTransform.rotate.y = (objectTransform.rotate.y + diff * 0.1f) ;
	objectTransform.rotate.y = std::fmodf(objectTransform.rotate.y, std::numbers::pi_v<float> * 2);

	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", objectTransform.rotate.y / std::numbers::pi_v<float> * 180.0f);
	ImGui::DragFloat3("translate", &objectTransform.translate.x, 0.1f);
	ImGui::End();

	*targetTransform_ = objectTransform;
}

void Player::Draw() {
	object_->Draw3D(camera_, 0, 40, nullptr, nullptr);
}