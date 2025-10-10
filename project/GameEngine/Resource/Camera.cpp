#include "Camera.h"
#include <GameEngine.h>
#include "Matrix4x4_operation.h"
#include <cassert>
#include <numbers>
#include <CreateBufferResource.h>

Camera::Camera() {
	kWindowWidth_ = GameEngine::GetWindowWidth();
	kWindowHeight_ = GameEngine::GetWindowHeight();
}

void Camera::Initialize() {
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-10.0f });
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);

	//光源用のリソースを作る
	cameraResource_ = CreateBufferResource(GameEngine::GetDevice(), sizeof(CameraForGPU));
	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = Vector3{ 0.0f,0.0f,0.0f };

	cameraResource_->Unmap(0, nullptr);
}

void Camera::Update(SRT transform) {

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, transform.rotate, transform.translate);
	viewMatrix_ = Inverse(worldMatrix);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = Vector3{ worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };

	cameraResource_->Unmap(0, nullptr);
}

void Camera::Update() {
	assert(debugCamera_ != nullptr);
	debugCamera_->UpdateCamera(this);

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