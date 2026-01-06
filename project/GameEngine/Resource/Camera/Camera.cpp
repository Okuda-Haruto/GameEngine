#include "Camera.h"
#include <GameEngine.h>
#include <cassert>
#include <numbers>

Camera::Camera() {
	kWindowWidth_ = GameEngine::GetWindowWidth();
	kWindowHeight_ = GameEngine::GetWindowHeight();
}

void Camera::Initialize(DirectXCommon* dxCommon) {
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-10.0f });
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 1000.0f);

	//カメラ座標用のリソースを作る
	cameraResource_ = dxCommon->CreateBufferResources(sizeof(CameraForGPU));
	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = Vector3{ 0.0f,0.0f,0.0f };
	cameraData_->nearDist = 7.0f;
	cameraData_->farDist = 300.0f;
	cameraData_->nearTransparentDist = 2.0f;
	cameraData_->farTransparentDist = 50.0f;

	cameraResource_->Unmap(0, nullptr);
}

void Camera::Update(SRT transform) {

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, transform.rotate, transform.translate);
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 1000.0f);

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = Vector3{ worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };

	cameraResource_->Unmap(0, nullptr);
}

void Camera::Update() {
	assert(debugCamera_.lock() != nullptr);
	debugCamera_.lock()->UpdateCamera(this);

	Matrix4x4 worldMatrix = Inverse(viewMatrix_);

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = Vector3{ worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };

	cameraResource_->Unmap(0, nullptr);
}

void Camera::SetParent(SRT* transform) {
	parentTransform_ = transform;
}

void Camera::ResetParent() {
	parentTransform_ = nullptr;
}