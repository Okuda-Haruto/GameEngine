#include "Camera.h"
#include <GameEngine.h>
#include "Matrix4x4_operation.h"

Camera::Camera(GameEngine* gameEngine) {
	kWindowWidth_ = gameEngine->GetWindowWidth();
	kWindowHeight_ = gameEngine->GetWindowHeight();
}

void Camera::Initialize() {
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-10.0f });
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);
}

void Camera::Update(SRT transform) {

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, transform.rotate, transform.translate);
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);
}