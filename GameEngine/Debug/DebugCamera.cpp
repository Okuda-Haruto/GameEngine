#include "DebugCamera.h"
#include "Matrix4x4_operation.h"

#define _USE_MATH_DEFINES
#include <cmath>

void DebugCamera::Initialize(uint32_t width, uint32_t height) {
	kWindowWidth_ = width;
	kWindowHeight_ = height;

	matRot_ = MakeIdentity4x4();
}

void DebugCamera::Update(Mouse mouse) {

	mouse_ = mouse;

	if (mouse_.click[MOUSE_BOTTON_LEFT]) {
		if (mouse_.Movement.x != 0.0f) {
			const float speed = -mouse_.Movement.x / 100.0f;

			//camera移動ベクトル
			Vector3 move = { speed,0,0 };

			move = Transform(move, matRot_);

			translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

		}
		if (mouse_.Movement.y != 0.0f) {
			const float speed = mouse_.Movement.y / 100.0f;

			//camera移動ベクトル
			Vector3 move = { 0,speed,0 };

			move = Transform(move, matRot_);

			translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

		}
	} else if (mouse_.click[MOUSE_BOTTON_RIGHT]) {
		if (mouse_.Movement.x != 0.0f || mouse_.Movement.y != 0.0f) {
			const float speedX = -mouse_.Movement.y / (180.0f * float(M_PI));
			const float speedY = -mouse_.Movement.x / (180.0f * float(M_PI));

			Matrix4x4 matRotDelta = MakeIdentity4x4();
			matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speedX));
			matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speedY));

			matRot_ = Multiply(matRotDelta, matRot_);

		}
	} else if (mouse_.Movement.z != 0.0f) {
		const float speed = mouse_.Movement.z / 100.0f;

		//camera移動ベクトル
		Vector3 move = { 0,0,speed };

		move = Transform(move, matRot_);

		translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

	}

	Matrix4x4 cameraMatrix = Multiply(matRot_, MakeTranslateMatrix(translation_));
	viewMatrix_ = Inverse(cameraMatrix);


	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);
}

void DebugCamera::Reset() {
	translation_ = { 0,0,-10 };
	matRot_ = MakeIdentity4x4();
}

Camera DebugCamera::GetCamera() { 
	Camera camera;
	camera.viewMatrix = viewMatrix_;
	camera.projectionMatrix = projectionMatrix_;
	return camera; 
};