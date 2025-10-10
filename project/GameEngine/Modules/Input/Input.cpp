#include "Input.h"
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>
#include <cassert>
#include <math.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

void Input::Initialize(HINSTANCE hInstance,HWND hwnd) {

	//DirectInputの初期化
	HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboardDevice_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

void Input::Update() {

	//キー入力
	//前frame処理
	memcpy(preKeys_, keys_, sizeof(BYTE) * 256);
	//キーボード情報を入手
	keyboardDevice_->Acquire();
	//全キーの入力情報を取得する
	keyboardDevice_->GetDeviceState(sizeof(BYTE) * 256, keys_);

	//パッド入力
	//前frame処理
	memcpy(&prePad_, &pad_, sizeof(XINPUT_STATE));
	ZeroMemory(&pad_, sizeof(XINPUT_STATE));

	//パッド入力を入手
	dwResult_ = XInputGetState(0, &pad_);

	//接続されているか
	if (dwResult_ == ERROR_SUCCESS) {
		//接続されている
		padState_.isConnected = true;

		//スティックの傾きを得る
		//デッドゾーンチェック
		float LX = pad_.Gamepad.sThumbLX;
		float LY = pad_.Gamepad.sThumbLY;

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

		padState_.LeftStick = {
			normalizedMagnitude,
			{ normalizedLX, normalizedLY}
		};

		//Rスティックも
		float RX = pad_.Gamepad.sThumbRX;
		float RY = pad_.Gamepad.sThumbRY;

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

		padState_.RightStick = {
			normalizedMagnitude,
			{ normalizedRX, normalizedRY}
		};

		//ボタンの入力変換
		for (int i = 0; i <= 16; i++) {
			int hold = 0x0001 << i;
			if (i != PAD_BUTTON_LT && i != PAD_BUTTON_RT) {
				padState_.Button[i].hold = pad_.Gamepad.wButtons & hold;
				padState_.Button[i].idle = ~(pad_.Gamepad.wButtons & hold);
				padState_.Button[i].trigger = (pad_.Gamepad.wButtons & hold) & ~(prePad_.Gamepad.wButtons & hold);
				padState_.Button[i].release = ~(pad_.Gamepad.wButtons & hold) & (prePad_.Gamepad.wButtons & hold);
			} else {
				if (i == PAD_BUTTON_LT) {
					padState_.Button[i].hold = pad_.Gamepad.bLeftTrigger >= 0x80;
					padState_.Button[i].idle = !(pad_.Gamepad.bLeftTrigger >= 0x80);
					padState_.Button[i].trigger = (pad_.Gamepad.bLeftTrigger >= 0x80) && !(prePad_.Gamepad.bLeftTrigger >= 0x80);
					padState_.Button[i].release = !(pad_.Gamepad.bLeftTrigger >= 0x80) && (prePad_.Gamepad.bLeftTrigger >= 0x80);
				} else {
					padState_.Button[i].hold = pad_.Gamepad.bRightTrigger >= 0x80;
					padState_.Button[i].idle = !(pad_.Gamepad.bRightTrigger >= 0x80);
					padState_.Button[i].trigger = (pad_.Gamepad.bRightTrigger >= 0x80) && !(prePad_.Gamepad.bRightTrigger >= 0x80);
					padState_.Button[i].release = !(pad_.Gamepad.bRightTrigger >= 0x80) && (prePad_.Gamepad.bRightTrigger >= 0x80);
				}
			}
		}
	} else {
		//接続されていない
		padState_.isConnected = false;
	}

}

bool Input::PushPadButton(PAD_BUTTON button) {
	return padState_.Button[button].hold;
}

bool Input::TriggerPadButton(PAD_BUTTON button) {
	return padState_.Button[button].trigger;
}