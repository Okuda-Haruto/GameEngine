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

	DebugCamera* debugCamera_ = nullptr;
	Vector3 centerPoint_{};
public:
	Camera(GameEngine* gameEngine);

	void Initialize();
	
	void Update(SRT transform);
	void Update();

	void SetViewMatrix(Matrix4x4 viewMatrix) { viewMatrix_ = viewMatrix; }
	void SetProjectionMatrix(Matrix4x4 projectionMatrix) { projectionMatrix_ = projectionMatrix; }

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	void SetDebugCamera(DebugCamera* debugCamera) { debugCamera = debugCamera_; }
	DebugCamera* GetDebugCamera() { return debugCamera_; }

	void SetCenterPoint(Vector3 centerPoint) { centerPoint_ = centerPoint; }
	Vector3 GetCenterPoint() { return centerPoint_; }
};