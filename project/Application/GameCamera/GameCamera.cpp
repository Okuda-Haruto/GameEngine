#include "GameCamera.h"
#include <Vector3.h>
#include "GameEngine.h"
#include <Matrix4x4.h>
#include <numbers>
#include <Easing.h>
#include <Collision.h>

#pragma region LockOnCamera

void LockOnCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) {
	gameCamera_ = gameCamera;
	input_ = input;

	transform_ = {};
	pushRStickTime_ = 0;
	if (gameCamera_->GetObserverTransform()) {
		cameraAngle_ = gameCamera_->GetObserverTransform()->rotate;
	} else {
		cameraAngle_ = {};
	}
}

void LockOnCamera::Update() {
	Keyboard keyboard = input_->GetKeyboard();
	Pad pad = input_->GetPad();

	//進行方向に合わせる
	if (pad.LeftStick.magnitude > 0.5f || keyboard.keys[DIK_A].hold || keyboard.keys[DIK_D].hold) {
		if (pad.LeftStick.vector.x > 0.5f || keyboard.keys[DIK_D].hold) {
			offsetDirection_ = true;
		} else if (pad.LeftStick.vector.x < -0.5f || keyboard.keys[DIK_A].hold) {
			offsetDirection_ = false;
		}
	}

	//カメラを左右どちらに寄せるか
	if (offsetDirection_) {
		cameraOffsetX_ = Easing::Lerp(cameraOffsetX_, kMaxCameraOffsetX, 0.2f);
	}else{
		cameraOffsetX_ = Easing::Lerp(cameraOffsetX_, -kMaxCameraOffsetX, 0.2f);
	}

	//プレイヤーが存在している場合
	if (gameCamera_->GetObserverTransform()) {
		//スティック傾けで角度変更
		if (pad.RightStick.magnitude > 0.01f) {
			rotateVelocity_ += {
				kAngleSpeed * -pad.RightStick.vector.y * pad.RightStick.magnitude ,
				kAngleSpeed * pad.RightStick.vector.x * pad.RightStick.magnitude, 
				0.0f
			};
		}
		if (Length(rotateVelocity_) > 0.01f) {
			rotateVelocity_ = Easing::Lerp(rotateVelocity_, {0.0f,0.0f,0.0f}, kRotateVelocityLerpLate);
		} else {
			rotateVelocity_ = {};
		}
		cameraAngle_ += rotateVelocity_;

		//角度範囲
		if (cameraAngle_.x > std::numbers::pi_v<float> / 2) {
			cameraAngle_.x = std::numbers::pi_v<float> / 2;
		} else if (cameraAngle_.x < -std::numbers::pi_v<float> / 2) {
			cameraAngle_.x = -std::numbers::pi_v<float> / 2;
		}
		if (cameraAngle_.y > std::numbers::pi_v<float>) {
			cameraAngle_.y -= std::numbers::pi_v<float> *2;
		} else if (cameraAngle_.y < -std::numbers::pi_v<float>) {
			cameraAngle_.y += std::numbers::pi_v<float> *2;
		}

		//注目対象が存在している場合
		if (targetSpehre_.lock()) {
			//角度範囲
			if (fabsf(targetAngle_.y - cameraAngle_.y) > std::numbers::pi_v<float>) {
				if (targetAngle_.y - cameraAngle_.y > 0.0f) {
					targetAngle_.y -= std::numbers::pi_v<float> * 2;
				} else {
					targetAngle_.y += std::numbers::pi_v<float> * 2;
				}
			}

			//傾けた分をある程度戻す
			if (Length(cameraAngle_ - targetAngle_) > 0.01f) {
				cameraAngle_ = Easing::Lerp(cameraAngle_, targetAngle_, kAngleLerpLate);
			} else {
				cameraAngle_ = targetAngle_;
			}

			Vector3 diff = targetSpehre_.lock()->center - gameCamera_->GetObserverTransform()->translate;
			//必ずいずれかの向きを向くように
			if (Length(diff) <= 0.0f) {
				diff = { 0.0f,0.0f,1.0f };
			}

			//  Y軸回り回転(θy)
			targetAngle_.y = std::atan2(diff.x, diff.z);
			float length = Length(Vector3{ diff.x, 0.0f, diff.z });
			// X軸回り回転(θx)
			targetAngle_.x = 0;
			transform_.rotate = cameraAngle_;

			Matrix4x4 rotateMatrix = MakeRotateXMatrix(transform_.rotate.x) * MakeRotateYMatrix(transform_.rotate.y);
			transform_.translate = gameCamera_->GetObserverTransform()->translate + rotateMatrix * Vector3{ cameraOffsetX_,kCameraOffsetY,kCameraOffsetZ };

			//傾けている時間がある程度を過ぎたらターゲットを外す
			if (pad.RightStick.magnitude >= 1.0f && targetSpehre_.lock()) {
				pushRStickTime_ += 1.0f / 60.0f;
				if (pushRStickTime_ > kMaxPushRStickTime) {
					targetSpehre_.reset();
					pushRStickTime_ = 0.0f;
				}
			} else {
				pushRStickTime_ = 0.0f;
			}
		} else {
			//存在していないなら角度はそのまま
			transform_.rotate = cameraAngle_;
			Matrix4x4 rotateMatrix = MakeRotateXMatrix(transform_.rotate.x) * MakeRotateYMatrix(transform_.rotate.y);
			transform_.translate = gameCamera_->GetObserverTransform()->translate + rotateMatrix * Vector3{ cameraOffsetX_,kCameraOffsetY,kCameraOffsetZ };

			//視線にロックオン対象が接触しているか
			std::vector<std::weak_ptr<Sphere>> targetSpheres = gameCamera_->GetTargetSpheres();
			std::erase_if(targetSpheres,
				[](const std::weak_ptr<Sphere>& sphere) {
					return sphere.expired();
				});

			Ray playerRay = {
				.origin = transform_.translate,
				.diff = rotateMatrix * Vector3{0,0,1}
			};
#ifdef USE_IMGUI
			PrimitiveManager::GetInstance()->AddRay(playerRay);
#endif // USE_IMGUI

			//ターゲット候補
			std::weak_ptr<Sphere> keepTargetSphere;
			float observerDistance = 0;
			for (auto& target : targetSpheres) {
				auto sphere = target.lock();
				if (!sphere) {
					continue;
				}

				if (IsCollision(playerRay, *sphere)) {
					float length = Length(sphere->center - transform_.translate);

					if (auto keep = keepTargetSphere.lock()) {
						if (observerDistance > length) {
							keepTargetSphere = target;
							observerDistance = length;
						}
					} else {
						keepTargetSphere = target;
						observerDistance = length;
					}
				}
			}
			if (keepTargetSphere.lock()) {
				targetSpehre_ = keepTargetSphere;
			}
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
		//スティック傾けで角度変更
		if (pad.RightStick.magnitude > 0.01f) {
			rotateVelocity_ += {
				kAngleSpeed * -pad.RightStick.vector.y * pad.RightStick.magnitude,
					kAngleSpeed* pad.RightStick.vector.x* pad.RightStick.magnitude,
					0.0f
			};
		}
		if (Length(rotateVelocity_) > 0.01f) {
			rotateVelocity_ = Easing::Lerp(rotateVelocity_, { 0.0f,0.0f,0.0f }, kRotateVelocityLerpLate);
		} else {
			rotateVelocity_ = {};
		}
		transform_.rotate += rotateVelocity_;
	}

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	offset_ = { 0.0f,0.0f,kCameraOffsetZ };

	Vector3 position = TransformNormal(offset_, rotateMatrix);

	//プレイヤーが存在している場合
	if (gameCamera_->GetObserverTransform()) {
		transform_.translate = gameCamera_->GetObserverTransform()->translate + position;
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

	if (debugCamera_) {
		camera_->Update();
		return;
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

	if (fabsf(transform_->rotate.x) > std::numbers::pi_v<float>) {
		if (transform_->rotate.x > std::numbers::pi_v<float> *2) {
			transform_->rotate.x -= std::numbers::pi_v<float> *2;
		} else {
			transform_->rotate.x += std::numbers::pi_v<float> *2;
		}
	}
	if (fabsf(transform_->rotate.y) > std::numbers::pi_v<float>) {
		if (transform_->rotate.y > std::numbers::pi_v<float> *2) {
			transform_->rotate.y -= std::numbers::pi_v<float> *2;
		} else {
			transform_->rotate.y += std::numbers::pi_v<float> *2;
		}
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