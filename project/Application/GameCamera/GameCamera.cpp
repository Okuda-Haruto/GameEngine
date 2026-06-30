#include "GameCamera.h"
#include <Vector3.h>
#include "GameEngine.h"
#include <Matrix4x4.h>
#include <numbers>

#pragma region LockOnCamera

void LockOnCamera::Initialize(GameCamera* gameCamera) {
	gameCamera_ = gameCamera;
}

void LockOnCamera::Update() {
	Vector3 diff = gameCamera_->GetTargetTransform().translate - gameCamera_->GetPlayerTransform().translate;
	if (Length(diff) <= 0.0f) {
		diff = { 0.0f,0.0f,1.0f };
	}

	//  Y軸回り回転(θy)
	transform_.rotate.y = std::atan2(diff.x, diff.z);
	float length = Length(Vector3{ diff.x, 0.0f, diff.z });
	// X軸回り回転(θx)
	transform_.rotate.x = 0;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotate.y);
	transform_.translate = gameCamera_->GetPlayerTransform().translate + rotateMatrix * Vector3{ gameCamera_->GetCameraHorizontalOffset(),0.0f,0.0f} + Normalize(diff) * -12.0f;
	transform_.translate.y = 3.0f;
}

#pragma endregion


GameCamera::~GameCamera(){

}

void GameCamera::Initialize(shared_ptr<Input> input) {
	camera_ = make_shared<Camera>();
	camera_= Object::GetDefaultCamera();
	transform_ = make_shared<SRT>();
	transform_->scale = { 1.0f,1.0f,1.0f };
	transform_->rotate = { 0.0f,0.0f,0.0f };
	transform_->translate = { 0.0f,1.0f,0.0f };
	shiftTime_ = 0.0f;
	shakeTime_ = 0.0f;
	lockonTransform_ = *transform_;
	normalTransform_ = *transform_;

	input_ = input;

}

void GameCamera::Update() {

	Pad pad = input_->GetPad(0);

	if (shakeTime_ > 0.0f) {
		shakeTime_ -= 1.0f / 60.0f;
		if (shakeTime_ < 0.0f) {
			shakeTime_ = 0.0f;
		}
	}

	//追従対象がいれば
	if (isTargeted_) {
		if (shiftTime_ < kMaxShiftTime) {
			shiftTime_ += 1.0f / 60.0f;
			if (shiftTime_ > kMaxShiftTime) {
				shiftTime_ = kMaxShiftTime;
			}
		}
	} else {
		if (shiftTime_ > 0.0f) {
			shiftTime_ -= 1.0f / 60.0f;
			if (shiftTime_ < 0.0f) {
				shiftTime_ = 0.0f;
			}
		}
	}

	if (!isEvent_) {
		if (eventShiftTime_ > 0.0f) {
			eventShiftTime_ -= 1.0f / 60.0f;
			if (eventShiftTime_ < 0.0f) {
				eventShiftTime_ = 0.0f;
			}
		}
	}

	if (fabsf(velocity_) > 0.5f) {
		if (velocity_ > 0.0f) {
			cameraHorizontalOffset_ += (kMaxCameraPos - cameraHorizontalOffset_) / 3;
			if (cameraHorizontalOffset_ > kMaxCameraPos) {
				cameraHorizontalOffset_ = kMaxCameraPos;
			}
		} else {
			cameraHorizontalOffset_ += (-kMaxCameraPos - cameraHorizontalOffset_) / 3;
			if (cameraHorizontalOffset_ < -kMaxCameraPos) {
				cameraHorizontalOffset_ = -kMaxCameraPos;
			}
		}
	} else {
		velocity_ = 0.0f;
	}


	Vector3 diff = target_->translate - player_->translate;
	if (Length(diff) <= 0.0f) {
		diff = { 0.0f,0.0f,1.0f };
	}

	//  Y軸回り回転(θy)
	lockonTransform_.rotate.y = std::atan2(diff.x, diff.z);
	float length = Length(Vector3{ diff.x, 0.0f, diff.z });
	// X軸回り回転(θx)
	lockonTransform_.rotate.x = 0;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(lockonTransform_.rotate.y);
	lockonTransform_.translate = player_->translate + rotateMatrix * Vector3{ cameraHorizontalOffset_,0.0f,0.0f } + Normalize(diff) * -12.0f;
	lockonTransform_.translate.y = 3.0f;

	lockonTransform_.scale = {};

	if (pad.isConnected) {

		const float rotateSpeed = 0.2f;

		Vector3 rotate = Normalize(Vector3(pad.RightStick.vector.x, pad.RightStick.vector.y, 0.0f));

		normalTransform_.rotate.y += rotate.x * std::numbers::pi_v<float> *0.01f * pad.RightStick.magnitude;
		normalTransform_.rotate.x += -rotate.y * std::numbers::pi_v<float> *0.01f * pad.RightStick.magnitude;

	}
	//追従対象がいれば
	if (player_) {

		Matrix4x4 rotateMatrix = MakeRotateMatrix(normalTransform_.rotate);

		Vector3 position = TransformNormal(offset_, rotateMatrix);

		normalTransform_.translate = player_->translate + position;

		//座標をコピーしてオフセット分ずらす
		camera_->Update(*transform_);
	}

	if (fabsf(lockonTransform_.rotate.x - normalTransform_.rotate.x) > std::numbers::pi_v<float>) {
		if (lockonTransform_.rotate.x >= normalTransform_.rotate.x) {
			if (lockonTransform_.rotate.x > std::numbers::pi_v<float> *2) {
				lockonTransform_.rotate.x -= std::numbers::pi_v<float> *2;
			} else {
				normalTransform_.rotate.x += std::numbers::pi_v<float> *2;
			}
		} else {
			if (normalTransform_.rotate.x > std::numbers::pi_v<float> *2) {
				normalTransform_.rotate.x -= std::numbers::pi_v<float> *2;
			} else {
				lockonTransform_.rotate.x += std::numbers::pi_v<float> *2;
			}
		}
	}

	if (fabsf(lockonTransform_.rotate.y - normalTransform_.rotate.y) > std::numbers::pi_v<float>) {
		if (lockonTransform_.rotate.y >= normalTransform_.rotate.y) {
			if (lockonTransform_.rotate.y > std::numbers::pi_v<float> *2) {
				lockonTransform_.rotate.y -= std::numbers::pi_v<float> *2;
			} else {
				normalTransform_.rotate.y += std::numbers::pi_v<float> *2;
			}
		} else {
			if (normalTransform_.rotate.y > std::numbers::pi_v<float> *2) {
				normalTransform_.rotate.y -= std::numbers::pi_v<float> *2;
			} else {
				lockonTransform_.rotate.y += std::numbers::pi_v<float> *2;
			}
		}
	}

	//通常の変換
	transform_->translate = lockonTransform_.translate * (1.0f - powf(1.0f - shiftTime_ / kMaxShiftTime, 2)) 
		+ normalTransform_.translate * (1.0f - (1.0f - powf(1.0f - shiftTime_ / kMaxShiftTime, 2)));
	//eventから戻る
	if (eventShiftTime_ > 0.0f) {
		transform_->translate = eventTransform_.translate * (1.0f - powf(1.0f - eventShiftTime_ / kMaxEventShiftTime, 2))
			+ transform_->translate * (1.0f - (1.0f - powf(1.0f - eventShiftTime_ / kMaxEventShiftTime, 2)));
	}

	if (shakeTime_ > 0.0f) {
		transform_->translate.x += GameEngine::randomFloat(-shakeTime_, shakeTime_);
		transform_->translate.y += GameEngine::randomFloat(-shakeTime_, shakeTime_);
		transform_->translate.z += GameEngine::randomFloat(-shakeTime_, shakeTime_);
	}
	//通常の変換
	transform_->rotate = lockonTransform_.rotate * (1.0f - powf(1.0f - shiftTime_ / kMaxShiftTime, 2)) 
		+ normalTransform_.rotate * (1.0f - (1.0f - powf(1.0f - shiftTime_ / kMaxShiftTime, 2)));
	//eventから戻る
	if (eventShiftTime_ > 0.0f) {
		transform_->rotate = eventTransform_.rotate * (1.0f - powf(1.0f - eventShiftTime_ / kMaxEventShiftTime, 2))
			+ transform_->rotate * (1.0f - (1.0f - powf(1.0f - eventShiftTime_ / kMaxEventShiftTime, 2)));
	}

	if (debugCamera_) {
		debugCamera_->UpdateCamera(camera_.get());
	}

	sepiaTone_ = eventShiftTime_ / kMaxEventShiftTime;
}