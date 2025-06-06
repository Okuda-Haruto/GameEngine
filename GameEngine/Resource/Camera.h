#pragma once
class GameEngine;
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
public:
	Camera(GameEngine* gameEngine);

	void Initialize();
	
	void Update(SRT transform);

	void SetViewMatrix(Matrix4x4 viewMatrix) { viewMatrix_ = viewMatrix; }
	void SetProjectionMatrix(Matrix4x4 projectionMatrix) { projectionMatrix_ = projectionMatrix; }

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }
};