#include "GameCamera.h"
#include <Vector3.h>
#include "GameEngine.h"
#include <Matrix4x4.h>
#include <numbers>


void GameCamera::Initialize() {
	camera_ = new Camera;
	camera_= Object::GetDefaultCamera();
	transform_ = std::make_unique<SRT>();
	transform_->scale = { 1.0f,1.0f,1.0f };
	transform_->rotate = { 0.0f,0.0f,0.0f };
	transform_->translate = { 0.0f,1.0f,0.0f };
}

void GameCamera::Update() {
	
	Pad pad = GameEngine::GetPad();

	if (pad.isConnected) {

		const float rotateSpeed = 0.2f;

		Vector3 rotate = Normalize(Vector3(pad.RightStick.vector.x, pad.RightStick.vector.y, 0.0f));

		transform_->rotate.y += rotate.x * std::numbers::pi_v<float> * 0.01f * pad.RightStick.magnitude;
		transform_->rotate.x += -rotate.y * std::numbers::pi_v<float> * 0.01f * pad.RightStick.magnitude;

	}
	//追従対象がいれば
	if (target_) {

		Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_->rotate);

		Vector3 position = TransformNormal(offset_, rotateMatrix);

		transform_->translate = target_->translate + position;

		//座標をコピーしてオフセット分ずらす
		camera_->Update(*transform_);
	}
}