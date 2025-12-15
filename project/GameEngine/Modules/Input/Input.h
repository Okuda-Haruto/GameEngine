#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>
#include <Xinput.h>
#include <Vector2.h>
#include <Vector3.h>

#include <memory>
#include <array>

#include "WindowsAPI/WindowsAPI.h"

using namespace std;

#pragma region Keybord

//キーボード入力
struct Keybord {
	BYTE hold[256] = {};	//押している
	BYTE idle[256] = {};	//離している
	BYTE trigger[256] = {};	//押した瞬間
	BYTE release[256] = {};	//離した瞬間
};

#pragma endregion

#pragma region Mouse

//マウスクリック
enum MOUSE_BUTTON {
	MOUSE_BOTTON_LEFT,	//左クリック
	MOUSE_BOTTON_RIGHT,	//右スリック
	MOUSE_BOTTON_WHEEL	//ホイールクリック
};

//マウス入力
struct Mouse {
	Vector2 Position;	//マウス座標
	Vector3 Movement;	//マウス移動量
	bool click[3];		//マウスクリック	列挙型[MOUSE_BOTTON]の列挙子を使用すること
};

#pragma endregion

#pragma region Pad

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

#pragma endregion

class Input
{
public:
	//namespace省略
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;
private:

	//WindowsAPI
	WindowsAPI* winApp_;

	//DirectInputのインスタンス
	Comptr<IDirectInput8> directInput;
	//キーボードデバイス
	Comptr<IDirectInputDevice8W> keyboardDevice_;
	//マウスデバイス
	Comptr<IDirectInputDevice8> mouseDevice_;


	//キー入力
	BYTE keys_[256]{};
	BYTE preKeys_[256]{};

	//マウス入力
	DIMOUSESTATE preMouseState_;
	DIMOUSESTATE mouseState_;

	//パッド入力
	XINPUT_STATE padState_[4];
	DWORD dwResult_[4];
	XINPUT_STATE prePadState_[4];

	Keybord keybord_{};
	Mouse mouse_{};
	std::array<Pad, 4> pad_{};

public:
	//初期化
	void Initialize(WindowsAPI* winApp);
	//更新
	void Update();

	/// <summary>
	/// キーボード入力
	/// </summary>
	/// <returns>キーボード入力</returns>
	Keybord GetKeyBord() { return keybord_; }

	/// <summary>
	/// マウス入力
	/// </summary>
	/// <returns>マウス入力</returns>
	Mouse GetMouse() { return mouse_; }

	/// <summary>
	/// パッド入力
	/// </summary>
	/// <param name="index">パッド番号(0~4まで)</param>
	/// <returns>パッド入力</returns>
	Pad GetPad(int index) { if (index >= 0 && index < pad_.size()) { return pad_[index]; } else { return pad_[0]; } }

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
};

