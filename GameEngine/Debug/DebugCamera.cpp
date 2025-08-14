#define NOMINMAX
#include "DebugCamera.h"
#include "Vector3_operation.h"
#include "Matrix4x4_operation.h"
#include <GameEngine.h>

#include <algorithm>
#include <numbers>
#include <cmath>

DebugCamera::DebugCamera() {
	kWindowWidth_ = GameEngine::GetWindowWidth();
	kWindowHeight_ = GameEngine::GetWindowHeight();
}

void DebugCamera::Initialize() {

	ChangeCameraMode(new CameraModeSphericalCoordinates);

	rotateMatrix_ = MakeIdentity4x4();
	//正面状態
	sphericalCoordinates_ = { -10.0f,0.0f,std::numbers::pi_v<float> / 4 * 3 };
	viewMatrix_ = MakeIdentity4x4();
}

void DebugCamera::Update() {

	cameraMode_->Update(this);
	
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.1f, 2000.0f);
}

void DebugCamera::Reset() {
	translation_ = { 0,0,0 };
	centerPoint_ = { 0,0,0 };
	Initialize();
}

void DebugCamera::UpdateCamera(Camera* camera) {
	assert(cameraMode_ != nullptr);
	Update();
	camera->SetViewMatrix(viewMatrix_);
};

void BaseCameraMode::Update(DebugCamera* debugCamera) {}

void CameraModePlayerCamera::Update(DebugCamera* debugCamera) {
	Keybord key = GameEngine::GetKeybord();
	Mouse mouse = GameEngine::GetMouse();

	Matrix4x4 rotateMatrix = debugCamera->GetRotateMatrix();
	Vector3 sphericalCoordinates = debugCamera->GetSphericalCoordinates();

	//ズームは0
	sphericalCoordinates.x = 0.0f;

	const float speed = 0.3f;
	//移動
	if (key.hold[DIK_W] || key.hold[DIK_S] || key.hold[DIK_A] || key.hold[DIK_D]) {
		Vector3 move{};
		if (key.hold[DIK_W]) {
			move.z = 1.0f;
		} else if (key.hold[DIK_S]) {
			move.z = -1.0f;
		}
		if (key.hold[DIK_D]) {
			move.x = 1.0f;
		} else if (key.hold[DIK_A]) {
			move.x = -1.0f;
		}

		//カメラの向きに合わせて変換
		move = Transform(move, rotateMatrix);
		move = Normalize(Vector3{ move.x,0.0f,move.z }) * speed;

		centerPoint_ = { centerPoint_.x + move.x,centerPoint_.y + move.y,centerPoint_.z + move.z };
	}
	if (key.hold[DIK_SPACE] || key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) {
		Vector3 move{};
		if (key.hold[DIK_SPACE]) {
			move.y = 1.0f;
		} else if (key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) {
			move.y = -1.0f;
		}

		move *= speed;

		centerPoint_ = { centerPoint_.x + move.x,centerPoint_.y + move.y,centerPoint_.z + move.z };
	}
	//回転
	if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
		const float speedX = mouse.Movement.y / (90.0f * std::numbers::pi_v<float>);
		const float speedY = -mouse.Movement.x / (90.0f * std::numbers::pi_v<float>);

		sphericalCoordinates.z += speedX;	//0~πの範囲
		sphericalCoordinates.z = std::max(std::min(sphericalCoordinates.z, std::numbers::pi_v<float>), 0.0f);
		sphericalCoordinates.y += speedY;	//1周したら0
		if (sphericalCoordinates.y > std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y -= std::numbers::pi_v<float> *2;
		} else if (sphericalCoordinates.y < -std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y += std::numbers::pi_v<float> *2;
		}
	}

	Vector3 translate{};

	//球面座標系から直交座標系へ
	translate.x = centerPoint_.x + sphericalCoordinates.x * std::sinf(sphericalCoordinates.z) * std::cosf(sphericalCoordinates.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正
	translate.y = centerPoint_.y + sphericalCoordinates.x * std::cosf(sphericalCoordinates.z);
	translate.z = centerPoint_.z + sphericalCoordinates.x * std::sinf(sphericalCoordinates.z) * std::sinf(sphericalCoordinates.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正

	//正面状態の初期値を基に、二つの角度を補正
	rotateMatrix = Multiply(MakeRotateXMatrix(sphericalCoordinates.z - std::numbers::pi_v<float> / 2), MakeRotateYMatrix(-sphericalCoordinates.y));

	Matrix4x4 cameraMatrix = Multiply(rotateMatrix, MakeTranslateMatrix(translate));
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);

	debugCamera->SetSphericalCoordinates(sphericalCoordinates);
	debugCamera->SetRotateMatrix(rotateMatrix);
	debugCamera->SetViewMatrix(viewMatrix);
}

void CameraModeSphericalCoordinates::Update(DebugCamera* debugCamera) {
	Keybord key = GameEngine::GetKeybord();
	Mouse mouse = GameEngine::GetMouse();

	Matrix4x4 rotateMatrix = debugCamera->GetRotateMatrix();
	Vector3 sphericalCoordinates = debugCamera->GetSphericalCoordinates();

	//移動
	if (mouse.click[MOUSE_BOTTON_WHEEL] && (key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT])) {
		if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
			Vector3 move{};
			move.x = -mouse.Movement.x / 100.0f;
			move.y = mouse.Movement.y / 100.0f;

			//カメラの向きに合わせて変換
			move = Transform(move, rotateMatrix);

			centerPoint_ = { centerPoint_.x + move.x,centerPoint_.y + move.y,centerPoint_.z + move.z };
		}
	}
	//ズーム
	if (mouse.Movement.z != 0.0f) {
		const float speed = mouse.Movement.z / 100.0f;
		//sphericalCoordinates.x は注目位置とカメラの距離
		sphericalCoordinates.x += speed;
		sphericalCoordinates.x = std::min(sphericalCoordinates.x, 0.0f);
	}
	//回転
	if (mouse.click[MOUSE_BOTTON_WHEEL] && !(key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT])) {
		if (mouse.Movement.x != 0.0f || mouse.Movement.y != 0.0f) {
			const float speedX = mouse.Movement.y / (90.0f * std::numbers::pi_v<float>);
			const float speedY = -mouse.Movement.x / (90.0f * std::numbers::pi_v<float>);

			sphericalCoordinates.z += speedX;	//0~πの範囲
			sphericalCoordinates.z = std::max(std::min(sphericalCoordinates.z, std::numbers::pi_v<float>), 0.0f);
			sphericalCoordinates.y += speedY;	//1周したら0
			if (sphericalCoordinates.y > std::numbers::pi_v<float> *2) {
				sphericalCoordinates.y -= std::numbers::pi_v<float> *2;
			} else if (sphericalCoordinates.y < -std::numbers::pi_v<float> *2) {
				sphericalCoordinates.y += std::numbers::pi_v<float> *2;
			}
		}
	}

	Vector3 translate{};

	//球面座標系から直交座標系へ
	translate.x = centerPoint_.x + sphericalCoordinates.x * std::sinf(sphericalCoordinates.z) * std::cosf(sphericalCoordinates.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正
	translate.y = centerPoint_.y + sphericalCoordinates.x * std::cosf(sphericalCoordinates.z);
	translate.z = centerPoint_.z + sphericalCoordinates.x * std::sinf(sphericalCoordinates.z) * std::sinf(sphericalCoordinates.y + std::numbers::pi_v<float> / 2);	//-π~πの範囲にするために補正

	//正面状態の初期値を基に、二つの角度を補正
	rotateMatrix = Multiply(MakeRotateXMatrix(sphericalCoordinates.z - std::numbers::pi_v<float> / 2), MakeRotateYMatrix(-sphericalCoordinates.y));

	Matrix4x4 cameraMatrix = Multiply(rotateMatrix, MakeTranslateMatrix(translate));
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);

	debugCamera->SetSphericalCoordinates(sphericalCoordinates);
	debugCamera->SetRotateMatrix(rotateMatrix);
	debugCamera->SetViewMatrix(viewMatrix);
}

void DebugCamera::ChangeCameraMode(BaseCameraMode* newCameraMode) {
	delete cameraMode_;
	cameraMode_ = newCameraMode;
}