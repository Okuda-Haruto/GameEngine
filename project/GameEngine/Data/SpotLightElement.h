#pragma once
#include <Vector3.h>
#include <Vector4.h>

struct SpotLight {
	Vector4 Color;	//ライトの色
	Vector3	position;	//ライトに位置
	float intensity;	//輝度
	Vector3 direction;	//スポットライトの方向
	float distance;	//ライトの届く最大距離
	float decay;	//減衰率
	float cosAngle;	//スポットライトの余弦
	float cosFalloutStart;	//fallOut開始角度
	float padding;
};