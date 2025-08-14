#pragma once
class GameEngine;
class DebugCamera;
#include "Matrix4x4.h"
#include "SRT.h"
#include <format>

//カメラ
class Camera {
private:
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;

	//注目地点
	Vector3 centerPoint_{ 0.0f,0.0f,0.0f };
	//球面座標系
	Vector3 sphericalCoordinates_{ 10.0f,0.0f,0.0f };

	DebugCamera* debugCamera_ = nullptr;
public:
	Camera();

	void Initialize();
	
	void Update(SRT transform);
	void Update();

	void UpdateSphericalCoordinatesCamera();

	void SetViewMatrix(Matrix4x4 viewMatrix) { viewMatrix_ = viewMatrix; }
	void SetProjectionMatrix(Matrix4x4 projectionMatrix) { projectionMatrix_ = projectionMatrix; }

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	void SetSphericalCoordinates(Vector3 sphericalCoordinates) { sphericalCoordinates_ = sphericalCoordinates; }
	Vector3 GetSphericalCoordinates() { return sphericalCoordinates_; }

	void SetCenterPoint(Vector3 centerPoint) { centerPoint_ = centerPoint; }
	Vector3 GetCenterPoint() { return centerPoint_; }

	void setDebugCamera(DebugCamera* debugCamera) { debugCamera_ = debugCamera; }
};