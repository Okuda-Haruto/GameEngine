#include "GameCamera.h"
#include <Vector3.h>
#include "GameEngine.h"
#include <Matrix4x4.h>
#include <numbers>
#include <Math/Easing.h>

#pragma region LockOnCamera

void LockOnCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) {
	gameCamera_ = gameCamera;
	input_ = input;

	transform_ = {};
}

void LockOnCamera::Update() {
	Keyboard keyboard = input_->GetKeyboard();
	Pad pad = input_->GetPad();

	//進行方向に合わせる
	if (pad.LeftStick.magnitude > 0.5f || keyboard.keys[DIK_A].hold || keyboard.keys[DIK_D].hold) {
		if (pad.LeftStick.vector.x > 0.5f || keyboard.keys[DIK_D].hold) {
			offsetDirection = true;
		} else if (pad.LeftStick.vector.x < -0.5f || keyboard.keys[DIK_A].hold) {
			offsetDirection = false;
		}
	}

	if (offsetDirection) {
		cameraOffsetX_ = Easing::Lerp(cameraOffsetX_, kMaxCameraOffsetX, 0.2f);
	}else{
		cameraOffsetX_ = Easing::Lerp(cameraOffsetX_, -kMaxCameraOffsetX, 0.2f);
	}

	//プレイヤーが存在している場合
	if (gameCamera_->GetPlayerTransform()) {
		//注目対象が存在している場合
		if (gameCamera_->GetTargetTransform()) {
			Vector3 diff = gameCamera_->GetTargetTransform()->translate - gameCamera_->GetPlayerTransform()->translate;
			//必ずいずれかの向きを向くように
			if (Length(diff) <= 0.0f) {
				diff = { 0.0f,0.0f,1.0f };
			}

			//  Y軸回り回転(θy)
			transform_.rotate.y = std::atan2(diff.x, diff.z);
			float length = Length(Vector3{ diff.x, 0.0f, diff.z });
			// X軸回り回転(θx)
			transform_.rotate.x = 0;

			Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotate.y);
			transform_.translate = gameCamera_->GetPlayerTransform()->translate + rotateMatrix * Vector3{ cameraOffsetX_,0.0f,0.0f } + Normalize(diff) * kCameraOffsetZ;
			transform_.translate.y = kCameraOffsetY;
		} else {
			//存在していないなら角度は変えない
			Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotate.y);
			transform_.translate = gameCamera_->GetPlayerTransform()->translate + rotateMatrix * Vector3{ cameraOffsetX_,0.0f,kCameraOffsetZ };
			transform_.translate.y = kCameraOffsetY;
		}
	}
}

#pragma endregion


#pragma region WideViewCamera

void WideViewCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) {
	gameCamera_ = gameCamera;
	input_ = input;

	transform_ = {};
	if (gameCamera_->GetNowCamera()) {
		transform_.rotate = gameCamera_->GetNowCamera()->GetTransform().rotate;
		transform_.rotate.x = 15.0f * std::numbers::pi_v<float> / 180;
	}
}

void WideViewCamera::Update() {
	Pad pad = input_->GetPad();

	if (pad.isConnected) {
		Vector3 rotate = Normalize(Vector3(pad.RightStick.vector.x, pad.RightStick.vector.y, 0.0f));

		transform_.rotate.y += (rotate.x * std::numbers::pi_v<float> / 180) * kCameraRotateSpeed * pad.RightStick.magnitude;
		transform_.rotate.x += (-rotate.y * std::numbers::pi_v<float> / 180) * kCameraRotateSpeed * pad.RightStick.magnitude;
	}

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	offset_ = { 0.0f,0.0f,kCameraOffsetZ };

	Vector3 position = TransformNormal(offset_, rotateMatrix);

	//プレイヤーが存在している場合
	if (gameCamera_->GetPlayerTransform()) {
		transform_.translate = gameCamera_->GetPlayerTransform()->translate + position;
	} else {
		//存在しない場合原点中心
		transform_.translate = position;
	}
}

#pragma endregion



GameCamera::~GameCamera(){

}

void GameCamera::Initialize(shared_ptr<Input> input) {
	camera_ = make_shared<Camera>();
	camera_= Object::GetDefaultCamera();
	transform_ = make_shared<SRT>();
	*transform_ = {};
	shakeTime_ = 0.0f;

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

	if (lerpTime_ > 0.0f) {
		lerpTime_ -= 1.0f / 60.0f;
		if (lerpTime_ < 0.0f) {
			lerpTime_ = 0.0f;
			nowCamera_.release();
			nowCamera_ = move(nextCamera_);
			betweenTransform_.reset();
		}
	}

	if (nowCamera_) {
		nowCamera_->Update();
	}
	if (nextCamera_) {
		nextCamera_->Update();
	}

	if (nextCamera_) {
		//遷移途中で別の遷移に移った場合
		if (betweenTransform_) {
			*transform_ = Easing::Lerp(nextCamera_->GetTransform(), betweenTransform_.value(), lerpTime_ / maxLerpTime_);
		} else {
			*transform_ = Easing::Lerp(nextCamera_->GetTransform(), nowCamera_->GetTransform(), lerpTime_ / maxLerpTime_);
		}
	} else if (nowCamera_) {
		*transform_ = nowCamera_->GetTransform();
	}
	//scaleで乱されないように
	transform_->scale = { 1.0f,1.0f,1.0f };

	if (shakeTime_ > 0.0f) {
		transform_->translate.x += GameEngine::randomFloat(-shakeTime_, shakeTime_);
		transform_->translate.y += GameEngine::randomFloat(-shakeTime_, shakeTime_);
		transform_->translate.z += GameEngine::randomFloat(-shakeTime_, shakeTime_);
	}

	camera_->Update(*transform_);
}

void GameCamera::ChangeCamera(std::unique_ptr<BaseCamera> newCamera, float lerpTime) {
	if (!nowCamera_) {
		nowCamera_ = move(newCamera);
		nowCamera_->Initialize(this, input_);
		return;
	}
	//同じカメラの場合無視
	if (nextCamera_) {
		if (typeid(*newCamera) == typeid(*nextCamera_)) {
			return;
		}
		betweenTransform_ = *transform_;
	} else {
		if (typeid(*newCamera) == typeid(*nowCamera_)) {
			return;
		}
	}

	nextCamera_ = move(newCamera);
	nextCamera_->Initialize(this, input_);
	maxLerpTime_ = lerpTime;
	lerpTime_ = maxLerpTime_;
}