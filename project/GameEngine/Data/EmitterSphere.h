#pragma once
#include <Vector3.h>
#include <stdint.h>

struct EmitterSphere {
	//位置
	Vector3 translate;
	//射出半径
	float radius;
	//射出数
	uint32_t count;
	//射出間隔
	float frequency;
	//射出間隔調整時間
	float frequencyTime;
	//射出許可
	uint32_t emit;
};