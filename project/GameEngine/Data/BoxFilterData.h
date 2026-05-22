#pragma once
#include <cstdint>

struct BoxFilterData {
	//ぼやけさせる範囲
	int32_t scale;

	float padding[3];
};