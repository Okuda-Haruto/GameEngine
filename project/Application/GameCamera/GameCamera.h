#pragma once
#include <Camera/Camera.h>
#include <DebugCamera.h>
#include <memory>
#include <optional>
#include <Input/Input.h>

using namespace std;

class GameCamera;

#pragma region BaseCamera

class BaseCamera {
protected:
	SRT transform_;
	GameCamera* gameCamera_;
	std::shared_ptr<Input> input_;
public:
	virtual void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) = 0;
	virtual void Update() = 0;
	SRT GetTransform() { return transform_; }
};

//注目カメラ
class LockOnCamera : public BaseCamera {
private:
	//プレイヤーから前後方向にどれだけ離れるか
	const float kCameraOffsetZ = -10.0f;
	//肩越しカメラの左右位置
	const float kMaxCameraOffsetX = 2.0f;
	float cameraOffsetX_ = 3.0f;
	//地面からどれだけ浮かせた位置にあるか
	const float kCameraOffsetY = 2.5f;
	//offset方向
	bool offsetDirection;
	//プレイヤーからカメラへのローカル座標
	Vector3 offset_;
public:
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) override;
	void Update() override;
	SRT GetTransform() { return transform_; }
};

//広域カメラ
class WideViewCamera : public BaseCamera {
private:
	//プレイヤーから前後方向にどれだけ離れるか
	const float kCameraOffsetZ = -60.0f;
	//基礎角回転速度
	const float kCameraRotateSpeed = 15.0f;
	//プレイヤーからカメラへのローカル座標
	Vector3 offset_;
public:
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input) override;
	void Update() override;
	SRT GetTransform() { return transform_; }
};

#pragma endregion



class GameCamera
{
private:
	//カメラ
	shared_ptr<Camera> camera_;
	shared_ptr<DebugCamera> debugCamera_;

	float shakeTime_ = 0.0f;

	//セピア調の強さ
	float sepiaTone_ = 0.0f;

	shared_ptr<SRT> transform_;

	//カメラ
	std::unique_ptr<BaseCamera> nowCamera_;
	std::unique_ptr<BaseCamera> nextCamera_;
	//カメラ遷移時間
	float maxLerpTime_;
	float lerpTime_;
	//カメラ遷移中のTransform
	std::optional<SRT> betweenTransform_;

	SRT* player_ = nullptr;
	SRT* target_ = nullptr;

	shared_ptr<Input> input_;
public:
	~GameCamera();

	//初期化
	void Initialize(shared_ptr<Input> input);
	//更新
	void Update();

	void ChangeCamera(std::unique_ptr<BaseCamera> newCamera, float lerpTime);

	//Transformのゲッター
	SRT* GetTransform() { return transform_.get(); }

	SRT* GetPlayerTransform(){ return player_; }
	SRT* GetTargetTransform() { return target_; }

	BaseCamera* GetNowCamera() { return nowCamera_.get(); }
	shared_ptr<Camera> GetCamera() { return camera_; }
	float GetSepiaTone() { return sepiaTone_; }

	//追従対象を指定
	void SetPlayer(SRT* player) { player_ = player; }
	void SetTarget(SRT* target) { target_ = target; }

	void SetShakeTime(float shakeTime) { shakeTime_ = shakeTime; }

	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { debugCamera_ = debugCamera; }
};