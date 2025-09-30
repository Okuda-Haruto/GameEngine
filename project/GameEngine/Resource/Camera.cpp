#include "Camera.h"
#include <GameEngine.h>
#include "Matrix4x4_operation.h"
#include <cassert>
#include <numbers>

Camera::Camera() {
	kWindowWidth_ = GameEngine::GetWindowWidth();
	kWindowHeight_ = GameEngine::GetWindowHeight();
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

void Camera::Update() {
	assert(debugCamera_ != nullptr);
	debugCamera_->UpdateCamera(this);
}

void Camera::UpdateSphericalCoordinatesCamera() {
	Vector3 translation;
	//球面座標系から直交座標系へ
	translation.x = centerPoint_.x + sphericalCoordinates_.x * std::sinf(sphericalCoordinates_.z) * std::cosf(sphericalCoordinates_.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正
	translation.y = centerPoint_.y + sphericalCoordinates_.x * std::cosf(sphericalCoordinates_.z);
	translation.z = centerPoint_.z + sphericalCoordinates_.x * std::sinf(sphericalCoordinates_.z) * std::sinf(sphericalCoordinates_.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正

	//正面状態の初期値を基に、二つの角度を補正
	Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(sphericalCoordinates_.z - std::numbers::pi_v<float> / 2), MakeRotateYMatrix(-sphericalCoordinates_.y));

	Matrix4x4 cameraMatrix = Multiply(rotateMatrix, MakeTranslateMatrix(translation));
	viewMatrix_ = Inverse(cameraMatrix);

	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 2000.0f);
}