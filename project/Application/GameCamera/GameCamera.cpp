#include "GameCamera.h"
#include "Math/Vector3_operation.h"
#include "GameEngine.h"
#include "Math/Matrix4x4_operation.h"


void GameCamera::Initialize() {
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	transform_ = std::make_unique<SRT>();
	transform_->scale = { 1.0f,1.0f,1.0f };
	transform_->rotate = { 0.0f,0.0f,0.0f };
	transform_->translate = { 0.0f,1.0f,0.0f };
}

void GameCamera::Update() {
	
	Pad pad = GameEngine::GetPad();

	if (pad.isConnected) {

		const float rotateSpeed = 0.2f;

		Vector3 rotate = Normalize(Vector3(pad.RightStick.vector.x, pad.RightStick.vector.y, 0.0f)) * pad.RightStick.magnitude;

		transform_->rotate.y += float(pad.RightStick.vector.x);

	}
	//追従対象がいれば
	if (target_) {
		//追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f,2.0f,-20.0f };

		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(transform_->rotate.x), Multiply(MakeRotateYMatrix(transform_->rotate.y), MakeRotateZMatrix(transform_->rotate.z)));

		offset = TransformNormal(offset, rotateMatrix);

		transform_->translate = target_->translate + offset;

		//座標をコピーしてオフセット分ずらす
		camera_->Update(*transform_);
	}
}