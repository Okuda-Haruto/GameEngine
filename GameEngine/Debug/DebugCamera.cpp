#define NOMINMAX
#include "DebugCamera.h"
#include "Matrix4x4_operation.h"
#include <GameEngine.h>

#include <algorithm>
#include <numbers>
#include <cmath>

DebugCamera::DebugCamera(GameEngine* gameEngine) {
	kWindowWidth_ = gameEngine->GetWindowWidth();
	kWindowHeight_ = gameEngine->GetWindowHeight();
}

void DebugCamera::Initialize() {

	rotateMatrix_ = MakeIdentity4x4();
	//正面状態
	sphericalCoordinates_ = { -10.0f,0,std::numbers::pi_v<float> / 2 };
}

void DebugCamera::Update(Mouse mouse, Keybord keybord) {

	/*if (mouse.click[MOUSE_BOTTON_LEFT]) {
		if (mouse.Movement.x != 0.0f) {
			const float speed = -mouse.Movement.x / 100.0f;

			//camera移動ベクトル
			Vector3 move = { speed,0,0 };

			move = Transform(move, matRot_);

			translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

		}
		if (mouse.Movement.y != 0.0f) {
			const float speed = mouse.Movement.y / 100.0f;

			//camera移動ベクトル
			Vector3 move = { 0,speed,0 };

			move = Transform(move, matRot_);

			translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

		}
	} else if (mouse.click[MOUSE_BOTTON_RIGHT]) {
		if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
			const float speedX = -mouse.Movement.y / (180.0f * std::numbers::pi_v<float>);
			const float speedY = -mouse.Movement.x / (180.0f * std::numbers::pi_v<float>);

			Matrix4x4 matRotDelta = MakeIdentity4x4();
			matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speedX));
			matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speedY));

			matRot_ = Multiply(matRotDelta, matRot_);

		}
	} else if (mouse.Movement.z != 0.0f) {
		const float speed = mouse.Movement.z / 100.0f;

		//camera移動ベクトル
		Vector3 move = { 0,0,speed };

		move = Transform(move, matRot_);

		translation_ = { translation_.x + move.x,translation_.y + move.y,translation_.z + move.z };

	}*/

	//球面座標系のデバッグカメラ
	//移動
	if (mouse.click[MOUSE_BOTTON_WHEEL] && (keybord.hold[DIK_LSHIFT] || keybord.hold[DIK_RSHIFT])) {
		if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
			Vector3 move{};
			move.x = -mouse.Movement.x / 100.0f;
			move.y = mouse.Movement.y / 100.0f;

			//カメラの向きに合わせて変換
			move = Transform(move, rotateMatrix_);

			AttentionPoint = { AttentionPoint.x + move.x,AttentionPoint.y + move.y,AttentionPoint.z + move.z };
		}
	}
	//ズーム
	if (mouse.Movement.z != 0.0f) {
		const float speed = mouse.Movement.z / 100.0f;
		//sphericalCoordinates_.x は注目位置とカメラの距離
		sphericalCoordinates_.x += speed;
	}
	//回転
	if (mouse.click[MOUSE_BOTTON_WHEEL] && !(keybord.hold[DIK_LSHIFT] || keybord.hold[DIK_RSHIFT])) {
		if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
			const float speedX = mouse.Movement.y / (90.0f * std::numbers::pi_v<float>);
			const float speedY = -mouse.Movement.x / (90.0f * std::numbers::pi_v<float>);

			sphericalCoordinates_.z += speedX;	//0~πの範囲
			sphericalCoordinates_.z = std::max(std::min(sphericalCoordinates_.z, std::numbers::pi_v<float>), 0.0f);
			sphericalCoordinates_.y += speedY;	//-π~πの範囲
			sphericalCoordinates_.y = std::max(std::min(sphericalCoordinates_.y, std::numbers::pi_v<float>), -std::numbers::pi_v<float>);
		}
	}

	//球面座標系から直交座標系へ
	translation_.x = AttentionPoint.x + sphericalCoordinates_.x * std::sinf(sphericalCoordinates_.z) * std::cosf(sphericalCoordinates_.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正
	translation_.y = AttentionPoint.y + sphericalCoordinates_.x * std::cosf(sphericalCoordinates_.z);
	translation_.z = AttentionPoint.z + sphericalCoordinates_.x * std::sinf(sphericalCoordinates_.z) * std::sinf(sphericalCoordinates_.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正

	//正面状態の初期値を基に、二つの角度を補正
	rotateMatrix_ = Multiply(MakeRotateXMatrix(sphericalCoordinates_.z - std::numbers::pi_v<float> / 2), MakeRotateYMatrix(-sphericalCoordinates_.y));

	Matrix4x4 cameraMatrix = Multiply(rotateMatrix_, MakeTranslateMatrix(translation_));
	viewMatrix_ = Inverse(cameraMatrix);
	
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 100.0f);
}

void DebugCamera::Reset() {
	translation_ = { 0,0,0 };
	AttentionPoint = { 0,0,0 };
	sphericalCoordinates_ = { 0,0,0 };
	rotateMatrix_ = MakeIdentity4x4();
}

void DebugCamera::UpdateCamera(Camera* camera) {
	camera->SetViewMatrix(viewMatrix_);
	camera->SetProjectionMatrix(projectionMatrix_);
};