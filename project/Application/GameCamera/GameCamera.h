#pragma once
#include <Camera/Camera.h>
#include <memory>
#include <Input/Input.h>

using namespace std;

class GameCamera
{
private:
	//カメラ
	shared_ptr<Camera> camera_;

	const float kMaxShiftTime = 0.25f;
	float shiftTime_ = 0.0f;

	const float kMaxEventShiftTime = 0.25f;
	float eventShiftTime_ = 0.0f;
	bool isEvent_ = false;

	float shakeTime_ = 0.0f;

	shared_ptr<SRT> transform_;
	SRT lockonTransform_;
	SRT normalTransform_;
	SRT eventTransform_;

	const SRT* player_ = nullptr;
	const SRT* target_ = nullptr;

	const float kMaxCameraPos = 2.0f;
	float cameraPos_ = 1.0f;
	float velocity_ = 0.0f;

	//追従対象からカメラ位置までのオフセット
	Vector3 offset_;

	bool isTargeted_ = false;

	shared_ptr<Input> input_;
public:
	~GameCamera();

	//初期化
	void Initialize(shared_ptr<Input> input);
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

	void SetEventTransform(const SRT& event) { eventTransform_ = event; }
	void SetIsEvent(bool IsEvent) { isEvent_ = IsEvent; if (isEvent_)eventShiftTime_ = kMaxEventShiftTime; }

	void SetShakeTime(float shakeTime) { shakeTime_ = shakeTime; }

	shared_ptr<Camera> GetCamera() { return camera_; }

	void SetIsTargeted(bool isTargeted) { isTargeted_ = isTargeted; }
	void SetMoveVelocity(float velocity) { if(fabsf(velocity) > 0.5f)velocity_ = velocity; }
};