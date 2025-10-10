#include "Player.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"
#include "Math/Matrix4x4_operation.h"
#include <numbers>
#include "Math/Lerp.h"

void Player::Initialize() {
	//モデルの生成
	model_ = std::make_unique<Object_3D>();
	model_->Initialize("resources/Caracter/Player", "Player.obj");
	data_.SetMaterial(model_->GetModelData());
	data_.transform.translate.y = 1.0f;

	targetTransform_ = std::make_unique<SRT>();
	targetTransform_->scale = { 1.0f,1.0f,1.0f };
	targetTransform_->rotate = { 0.0f,0.0f,0.0f };
	targetTransform_->translate = { 0.0f,1.0f,0.0f };
}

void Player::Update() {
	Pad pad = GameEngine::GetPad();
	if (pad.isConnected) {
		const float deadZone = 0.7f;
		bool isMove = false;

		//速さ
		const float speed = 0.3f;

		//移動量
		Vector3 move = {
			pad.LeftStick.vector.x,0.0f,pad.LeftStick.vector.y
		};

		if (Length(move) > deadZone) {
			isMove = true;
		}

		if (isMove) {
			//移動量に速さを反映
			move = Normalize(move) * speed;

			Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(cameraTransform_->rotate.x), Multiply(MakeRotateYMatrix(cameraTransform_->rotate.y), MakeRotateZMatrix(cameraTransform_->rotate.z)));
			move = rotateMatrix * move;

			//移動
			data_.transform.translate += move;

			angle = std::atan2(move.x, move.z);

		}
	}
			
	//最短角度補完
	float diff = angle - data_.transform.rotate.y;
	
	diff = std::fmodf(diff, std::numbers::pi_v<float> * 2);

	if (diff > std::numbers::pi_v<float>) {
		diff -= std::numbers::pi_v<float> * 2;
	} else if (diff < -std::numbers::pi_v<float>) {
		diff += std::numbers::pi_v<float> * 2;
	}
	data_.transform.rotate.y = Lerp<float>(data_.transform.rotate.y, diff, 0.2f);

	ImGui::Begin("プレイヤー");
	ImGui::DragFloat3("translate", &data_.transform.translate.x,0.1f);
	ImGui::End();

	*targetTransform_ = data_.transform;
}

void Player::Draw() {
	model_->Draw(GameEngine::GetCommandList(),data_);
}