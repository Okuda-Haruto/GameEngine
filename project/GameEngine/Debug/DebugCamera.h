#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include <cstdint>

#include "Input.h"

//カメラモード
enum class DebugCameraMode {
	PlayerCamera,			//プレイヤー移動風カメラ
	SphericalCoordinates,	//球面座標系カメラ
};

class BaseCameraMode {
public: // 純粋仮想関数	＊派生クラスに実装を強要する
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update(DebugCamera* debugCamera);
};

class CameraModePlayerCamera : public BaseCameraMode {
private:
	//注目地点
	Vector3 centerPoint_{ 0.0f,0.0f,0.0f };
	//球面座標系
	Vector3 sphericalCoordinates_{ 10.0f,0.0f,0.0f };

public:
	void Initialize();
	void Update(DebugCamera* debugCamera);
};

class CameraModeSphericalCoordinates : public BaseCameraMode {
private:
	//注目地点
	Vector3 centerPoint_{ 0.0f,0.0f,0.0f };
	//球面座標系
	Vector3 sphericalCoordinates_{ 10.0f,0.0f,0.0f };

public:
	void Initialize();
	void Update(DebugCamera* debugCamera);
};

//デバッグカメラ
class DebugCamera
{
private:
	//注目地点
	Vector3 centerPoint_{0.0f,0.0f,0.0f};
	//球面座標系
	Vector3 sphericalCoordinates_{ 10.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translation_ = { 0,0,-10 };
	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;
	//回転行列
	Matrix4x4 rotateMatrix_;
	//ビュー行列
	Matrix4x4 viewMatrix_;
	//射影行列
	Matrix4x4 projectionMatrix_;

	BaseCameraMode* cameraMode_;

	void SetDebugCameraType(BaseCameraMode* cameraMode) { cameraMode_ = cameraMode; }
public:

	DebugCamera();

	//初期化
	void Initialize();

	//更新
	void Update();

	//初期化
	void Reset();

	//出力
	void UpdateCamera(Camera* camera);

	void ChangeCameraMode(BaseCameraMode* newCameraMode);

	void ChangeCameraMode(DebugCameraMode debugCameraMode);

	Vector3 GetAttentionPoint() { return centerPoint_; }

	Vector3 GetSphericalCoordinates() { return sphericalCoordinates_; }
	void SetSphericalCoordinates(const Vector3& sphericalCoordinates) { sphericalCoordinates_ = sphericalCoordinates; }

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	void SetViewMatrix(Matrix4x4 viewMatrix) { viewMatrix_ = viewMatrix; }

	Matrix4x4 GetRotateMatrix() { return rotateMatrix_; }
	void SetRotateMatrix(Matrix4x4 rotateMatrix) { rotateMatrix_ = rotateMatrix; }
};

