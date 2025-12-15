#include "Input.h"
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>
#include <cassert>
#include <math.h>

#pragma comment(lib,"dinput8.lib")

using namespace Microsoft::WRL;

void Input::Initialize(WindowsAPI* winApp) {

	winApp_ = winApp;

	//DirectInputの初期化
	HRESULT hr = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboardDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	//マウスデバイスの生成
	hr = directInput->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = mouseDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Update() {

	//キー入力
	//前frame処理
	memcpy(preKeys_, keys_, sizeof(BYTE) * 256);
	//キーボード情報を入手
	keyboardDevice_->Acquire();
	//全キーの入力情報を取得する
	keyboardDevice_->GetDeviceState(sizeof(BYTE) * 256, keys_);

	for (int i = 0; i < 256; i++) {
		keybord_.hold[i] = keys_[i];
		keybord_.idle[i] = ~keys_[i];
		keybord_.trigger[i] = keys_[i] & ~preKeys_[i];
		keybord_.release[i] = ~keys_[i] & preKeys_[i];
	}

	//前frame処理
	memcpy(&preMouseState_, &mouseState_, sizeof(DIMOUSESTATE));
	//マウス情報を入手
	mouseDevice_->Acquire();
	//マウス入力
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);

	//マウス座標
	POINT p;
	GetCursorPos(&p);
	//スクリーン上からウィンドウ上へ
	ScreenToClient(winApp_->GetHwnd(), &p);

	mouse_.Position = { float(p.x),float(p.y) };
	mouse_.Movement = { float(mouseState_.lX),float(mouseState_.lY),float(mouseState_.lZ) };
	for (int i = 0; i < 3; i++) {
		mouse_.click[i] = mouseState_.rgbButtons[i];
	}

	//パッド入力
	for (auto i = 0; i < 4; i++) {
		//前frame処理
		memcpy(&prePadState_, &padState_, sizeof(XINPUT_STATE));
		ZeroMemory(&padState_, sizeof(XINPUT_STATE));

		//パッド入力を入手
		dwResult_[i] = XInputGetState(0, &padState_[i]);

		//接続されているか
		if (dwResult_[i] == ERROR_SUCCESS) {
			//接続されている
			pad_[i].isConnected = true;

			//スティックの傾きを得る
			//デッドゾーンチェック
			float LX = padState_[i].Gamepad.sThumbLX;
			float LY = padState_[i].Gamepad.sThumbLY;

			float magnitude = sqrtf(powf(LX, 2) + powf(LY, 2));

			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0;

			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				if (magnitude > 32767) {
					magnitude = 32767;
				}
				magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			} else {
				magnitude = 0.0f;
				normalizedMagnitude = 0.0f;
			}

			pad_[i].LeftStick = {
				normalizedMagnitude,
				{ normalizedLX, normalizedLY}
			};

			//Rスティックも
			float RX = padState_[i].Gamepad.sThumbRX;
			float RY = padState_[i].Gamepad.sThumbRY;

			magnitude = sqrtf(powf(RX, 2) + powf(RY, 2));

			float normalizedRX = RX / magnitude;
			float normalizedRY = RY / magnitude;

			normalizedMagnitude = 0;

			if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
				if (magnitude > 32767) {
					magnitude = 32767;
				}
				magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			} else {
				magnitude = 0.0f;
				normalizedMagnitude = 0.0f;
			}

			pad_[i].RightStick = {
				normalizedMagnitude,
				{ normalizedRX, normalizedRY}
			};

			//ボタンの入力変換
			for (int j = 0; j <= 16; j++) {
				int hold = 0x0001 << j;
				if (j != PAD_BUTTON_LT && j != PAD_BUTTON_RT) {
					pad_[i].Button[j].hold = padState_[i].Gamepad.wButtons & hold;
					pad_[i].Button[j].idle = ~(padState_[i].Gamepad.wButtons & hold);
					pad_[i].Button[j].trigger = (padState_[i].Gamepad.wButtons & hold) & ~(prePadState_[i].Gamepad.wButtons & hold);
					pad_[i].Button[j].release = ~(padState_[i].Gamepad.wButtons & hold) & (prePadState_[i].Gamepad.wButtons & hold);
				} else {
					if (j == PAD_BUTTON_LT) {
						pad_[i].Button[j].hold = padState_[i].Gamepad.bLeftTrigger >= 0x80;
						pad_[i].Button[j].idle = !(padState_[i].Gamepad.bLeftTrigger >= 0x80);
						pad_[i].Button[j].trigger = (padState_[i].Gamepad.bLeftTrigger >= 0x80) && !(prePadState_[i].Gamepad.bLeftTrigger >= 0x80);
						pad_[i].Button[j].release = !(padState_[i].Gamepad.bLeftTrigger >= 0x80) && (prePadState_[i].Gamepad.bLeftTrigger >= 0x80);
					} else {
						pad_[i].Button[j].hold = padState_[i].Gamepad.bRightTrigger >= 0x80;
						pad_[i].Button[j].idle = !(padState_[i].Gamepad.bRightTrigger >= 0x80);
						pad_[i].Button[j].trigger = (padState_[i].Gamepad.bRightTrigger >= 0x80) && !(prePadState_[i].Gamepad.bRightTrigger >= 0x80);
						pad_[i].Button[j].release = !(padState_[i].Gamepad.bRightTrigger >= 0x80) && (prePadState_[i].Gamepad.bRightTrigger >= 0x80);
					}
				}
			}
		} else {
			//接続されていない
			pad_[i].isConnected = false;
		}
	}

}