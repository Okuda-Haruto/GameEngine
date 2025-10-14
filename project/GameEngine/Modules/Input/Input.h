#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>
#include <Xinput.h>
#include <Vector2.h>

#include "WindowsAPI/WindowsAPI.h"

//マウスクリック
enum PAD_BUTTON {
	PAD_BUTTON_UP,		//上ボタン
	PAD_BUTTON_DOWN,	//下ボタン
	PAD_BUTTON_LEFT,	//左ボタン
	PAD_BUTTON_RIGHT,	//右ボタン
	PAD_BUTTON_START,	//スタートボタン
	PAD_BUTTON_BACK,	//バックボタン
	PAD_BUTTON_LSTICK,	//Lスティック押込み
	PAD_BUTTON_RSTICK,	//Rスティック押込み
	PAD_BUTTON_LB,		//LBボタン
	PAD_BUTTON_RB,		//RBボタン
	PAD_BUTTON_LT,		//LTボタン
	PAD_BUTTON_RT,		//RTボタン
	PAD_BUTTON_A,		//Aボタン
	PAD_BUTTON_B,		//Bボタン
	PAD_BUTTON_X,		//Xボタン
	PAD_BUTTON_Y,		//Yボタン

};

//パッドボタン入力	単体では使用しないこと
struct PadTrigger {
	bool hold = {};	//押している
	bool idle = {};	//離している
	bool trigger = {};	//押した瞬間
	bool release = {};	//離した瞬間
};

//パッドスティック入力	単体では使用しないこと
struct PadStick {
	float magnitude;	//傾け度合い
	Vector2 vector;		//傾いている方向
};

//パッド入力
struct Pad {
	bool isConnected;	//接続されているか
	PadTrigger Button[16];	//パッドボタン	列挙型[PAD_BUTTON]の列挙子を使用すること
	PadStick LeftStick;		//Lスティック
	PadStick RightStick;	//Rスティック
};

class Input
{
public:
	//namespace省略
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;
private:

	//WindowsAPI
	WindowsAPI* winApp_ = nullptr;

	//DirectInputのインスタンス
	Comptr<IDirectInput8> directInput;
	//キーボードデバイス
	Comptr<IDirectInputDevice8W> keyboardDevice_;
	BYTE preKeys_[256]{};
	BYTE keys_[256]{};

	//パッド入力
	XINPUT_STATE pad_{};
	DWORD dwResult_{};
	XINPUT_STATE prePad_{};

	Pad padState_{};

public:
	//初期化
	void Initialize(WindowsAPI* winApp);
	//更新
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber) { return keys_[keyNumber]; }

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber) { return keys_[keyNumber] && !preKeys_[keyNumber]; }

	/// <summary>
	/// キーの引きをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>離しているか</returns>
	bool IdleKey(BYTE keyNumber) { return !keys_[keyNumber]; }

	/// <summary>
	/// キーのリリースをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>リリースか</returns>
	bool ReleaseKey(BYTE keyNumber) { return !keys_[keyNumber] && preKeys_[keyNumber]; }

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushPadButton(PAD_BUTTON button);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerPadButton(PAD_BUTTON button);

	PadStick PadLeftStick() { return padState_.LeftStick; }
	PadStick PadRightStick() { return padState_.RightStick; }
};

