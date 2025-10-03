#pragma once
#include <SRT.h>
#include <stdint.h>

struct Emitter {
	SRT transform;		//エミッターのTransform
	Vector3 velocity;
	uint32_t count;		//発生数
	Vector4 beforeColor;
	Vector4 afterColor;
	float lifeTime;
	float frequency;	//発生頻度
	float frequencyTime;//発生頻度用時刻

};