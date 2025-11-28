#pragma once
#include <SRT.h>
#include <stdint.h>
#include <AABB.h>

struct Emitter {
	SRT transform;		//エミッターのTransform
	AABB spawnRange;	//出現範囲
	Vector3 angleBase;	//基礎方向
	Vector3 angleRange;	//方向範囲
	float speedBase;	//基礎速度
	float speedRange;	//速度範囲
	uint32_t count;		//発生数
	Vector4 beforeColor;
	Vector4 afterColor;
	float lifeTime;
	float frequency;	//発生頻度
	float frequencyTime;//発生頻度用時刻

};