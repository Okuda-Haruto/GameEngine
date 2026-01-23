#pragma once
#include "Quaternion.h"
#include <string>

struct QuaternionKeyFlame {
	double time;	//キーフレームの入っている時間
	Quaternion quaternion;	//キーフレーム中のクォータニオン
};