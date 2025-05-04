#pragma once
#include "Windows.h"
#include "Vector2.h"
#include "Vector3.h"

//キーボード入力
struct Keybord {
	BYTE trigger[256] = {};	//押している
	BYTE leave[256] = {};	//離している
	BYTE hit[256] = {};		//押した瞬間
	BYTE release[256] = {};	//離した瞬間
};

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

//マウスクリック
enum PAD_BUTTON {
	PAD_BOTTON_UP,		//上ボタン
	PAD_BOTTON_DOWN,	//下ボタン
	PAD_BOTTON_LEFT,	//左ボタン
	PAD_BOTTON_RIGHT,	//右ボタン
	PAD_BOTTON_START,	//スタートボタン
	PAD_BOTTON_BACK,	//バックボタン
	PAD_BOTTON_LSTICK,	//Lスティック押込み
	PAD_BOTTON_RSTICK,	//Rスティック押込み
	PAD_BOTTON_LB,		//LBボタン
	PAD_BOTTON_RB,		//RBボタン
	PAD_BOTTON_LT,		//LTボタン
	PAD_BOTTON_RT,		//RTボタン
	PAD_BOTTON_A,		//Aボタン
	PAD_BOTTON_B,		//Bボタン
	PAD_BOTTON_X,		//Xボタン
	PAD_BOTTON_Y,		//Yボタン

};

//パッドボタン入力	単体では使用しないこと
struct PadTrigger {
	bool trigger = {};	//押している
	bool leave = {};	//離している
	bool hit = {};		//押した瞬間
	bool release = {};	//離した瞬間
};

//パッドスティック入力	単体では使用しないこと
struct PadStick {
	float magnitude;
	Vector2 vector;
};

//パッド入力
struct Pad {
	bool isConnected;	//接続されているか
	PadTrigger Button[16];	//パッドボタン	列挙型[PAD_BOTTON]の列挙子を使用すること
	PadStick LeftStick;		//Lスティック
	PadStick RightStick;	//Rスティック
};