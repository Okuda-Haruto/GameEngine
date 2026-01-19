#pragma once
#include "SRT.h"
#include <string>

struct KeyFrame{
	double time;	//キーフレームの入っている時間
	Vector3 vector;	//キーフレーム中のベクトル
};