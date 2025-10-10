#pragma once
#include "Windows.h"
#include "Vector2.h"
#include "Vector3.h"

//キーボード入力
struct Keybord {
	BYTE hold[256] = {};	//押している
	BYTE idle[256] = {};	//離している
	BYTE trigger[256] = {};	//押した瞬間
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