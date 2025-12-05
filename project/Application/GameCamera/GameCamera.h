#pragma once
#include <Camera.h>
#include <memory>

class GameCamera
{
private:
	//カメラ
	Camera* camera_;

	const float kMaxShiftTime = 0.5f;
	float shiftTime_ = 0.0f;

	float shakeTime_ = 0.0f;

	std::unique_ptr<SRT> transform_;
	SRT lockonTransform_;
	SRT normalTransform_;

	const SRT* player_ = nullptr;
	const SRT* target_ = nullptr;

	const float kMaxCameraPos = 2.0f;
	float cameraPos_ = 1.0f;
	float velocity_ = 0.0f;

	//追従対象からカメラ位置までのオフセット
	Vector3 offset_;

	bool isTargeted_ = false;
public:
	//初期化
	void Initialize();
	//更新
	void Update();

	//Transformのゲッター
	SRT* GetTransform() { return transform_.get(); }

	//オフセット
	void SetOffset(Vector3 offset) { offset_ = offset; }
	void SetRotate(Vector3 rotate) { normalTransform_.rotate = rotate; }

	//追従対象を指定
	void SetPlayer(const SRT* player) { player_ = player; }
	void SetTarget(const SRT* target) { target_ = target; }

	void SetShakeTime(float shakeTime) { shakeTime_ = shakeTime; }

	Camera* GetCamera() { return camera_; }

	void SetIsTargeted(bool isTargeted) { isTargeted_ = isTargeted; }
	void SetMoveVelocity(float velocity) { if(fabsf(velocity) > 0.5f)velocity_ = velocity; }
};