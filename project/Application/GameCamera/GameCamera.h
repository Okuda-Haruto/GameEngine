#pragma once
#include <Camera/Camera.h>
#include <DebugCamera.h>
#include <memory>
#include <Input/Input.h>

using namespace std;

class GameCamera;

class BaseCamera {
protected:
	SRT transform_;
	GameCamera* gameCamera_;
public:
	virtual void Initialize(GameCamera* gameCamera) = 0;
	virtual void Update() = 0;
	SRT GetTransform() { return transform_; }
};

class LockOnCamera : public BaseCamera {
private:
	//プレイヤーから前後方向にどれだけ離れるか
	const float kCameraOffsetZ = -12.0f;
	//肩越しカメラの最大左右位置
	const float kMaxCameraOffsetX = 3.0f;
	//プレイヤーからカメラへのローカル座標
	Vector3 offset_;
public:
	void Initialize(GameCamera* gameCamera) override;
	void Update() override;
	SRT GetTransform() { return transform_; }
};

class GameCamera
{
private:
	//カメラ
	shared_ptr<Camera> camera_;
	shared_ptr<DebugCamera> debugCamera_;

	const float kMaxShiftTime = 0.25f;
	float shiftTime_ = 0.0f;

	const float kMaxEventShiftTime = 0.25f;
	float eventShiftTime_ = 0.0f;
	bool isEvent_ = false;

	float shakeTime_ = 0.0f;

	//セピア調の強さ
	float sepiaTone_ = 0.0f;

	shared_ptr<SRT> transform_;
	SRT lockonTransform_;
	SRT normalTransform_;
	SRT eventTransform_;

	const SRT* player_ = nullptr;
	const SRT* target_ = nullptr;

	const float kMaxCameraPos = 3.0f;
	//肩越しカメラ水平位置
	float cameraHorizontalOffset_ = 3.0f;
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

	SRT GetPlayerTransform(){ return *player_; }
	SRT GetTargetTransform() { return *target_; }
	float GetCameraHorizontalOffset() { return cameraHorizontalOffset_; }

	shared_ptr<Camera> GetCamera() { return camera_; }
	float GetSepiaTone() { return sepiaTone_; }

	//オフセット
	void SetOffset(Vector3 offset) { offset_ = offset; }
	void SetRotate(Vector3 rotate) { normalTransform_.rotate = rotate; }

	//追従対象を指定
	void SetPlayer(const SRT* player) { player_ = player; }
	void SetTarget(const SRT* target) { target_ = target; }

	void SetEventTransform(const SRT& event) { eventTransform_ = event; }
	void SetIsEvent(bool IsEvent) { isEvent_ = IsEvent; if (isEvent_)eventShiftTime_ = kMaxEventShiftTime; }

	void SetShakeTime(float shakeTime) { shakeTime_ = shakeTime; }

	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { debugCamera_ = debugCamera; }

	void SetIsTargeted(bool isTargeted) { isTargeted_ = isTargeted; }
	void SetMoveVelocity(float velocity) { if(fabsf(velocity) > 0.5f)velocity_ = velocity; }
};