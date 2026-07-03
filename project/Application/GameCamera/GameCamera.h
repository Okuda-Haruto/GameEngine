#pragma once
#include <Camera/Camera.h>
#include <DebugCamera.h>
#include <memory>
#include <optional>
#include <numbers>
#include <Input/Input.h>
#include <Shape/Sphere.h>

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
	bool offsetDirection_;

	//注目対象
	std::weak_ptr<Sphere> targetSpehre_;
	//注目対象を見る角度
	Vector3 targetAngle_;
	//角移動速度
	const float kAngleSpeed = std::numbers::pi_v<float> / 180 * 2;
	//角度を0に近づける補完の強さ
	const float kAngleLerpLate = 0.2f;
	//ローカル角度
	Vector3 localAngle_;
	//ターゲットからから外れる時間
	const float kMaxPushRStickTime = 0.25f;
	//右スティックを完全に傾けている時間
	float pushRStickTime_;

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
	//角移動速度
	const float kAngleSpeed = std::numbers::pi_v<float> / 180 * 2;
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

	//始点Transform
	SRT* observerTransform_ = nullptr;
	//終点Sphere
	std::vector<std::weak_ptr<Sphere>> targetSpheres_;

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

	SRT* GetObserverTransform(){ return observerTransform_; }
	std::vector<std::weak_ptr<Sphere>> GetTargetSpheres() { return targetSpheres_; }

	BaseCamera* GetNowCamera() { return nowCamera_.get(); }
	shared_ptr<Camera> GetCamera() { return camera_; }
	float GetSepiaTone() { return sepiaTone_; }

	//追従対象を指定
	void SetObserverTransform(SRT* observerTransform) { observerTransform_ = observerTransform; }
	void SetTargetSphere(std::weak_ptr<Sphere> targetSphere) { targetSpheres_.push_back(targetSphere); }

	void SetShakeTime(float shakeTime) { shakeTime_ = shakeTime; }

	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { debugCamera_ = debugCamera; camera_->SetDebugCamera(debugCamera_);
	}
};